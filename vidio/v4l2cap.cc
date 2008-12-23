// -*- C++ -*-

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz 
// or its licensors, as applicable.
// Copyright 1995-2005 Thomas M. Breuel
// 
// You may not use this file except under the terms of the accompanying license.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You may
// obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// 
// Project: iulib -- image understanding library
// File: v4lcap.cc
// Purpose:
// Responsible: ulges
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

// FIXME implement frame rate setting
// FIXME clean up this code, remove redundant code

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>
}

#include "colib/colib.h"
#include "vidio.h"
using namespace colib;
using namespace iulib;

#define CLEAR(x) memset (&(x), 0, sizeof (x))

namespace iulib {

    int xioctl(int fd, int request, void *arg) {
        int r;
        do
            r = ioctl(fd, request, arg);
        while (-1 == r && EINTR == errno);
        return r;
    }

    typedef enum {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
    } io_method;

    struct VideoCapture:VideoInput {
        int width;
        int height;
        int format;
        v4l2_field field;
        const char *dev_name;
        io_method io;
        int fd;
        struct buffer {
            void *start;
            size_t length;
        };
        struct buffer *buffers;
        unsigned int n_buffers;
        const void *current_buffer;
        VideoCapture() {
            dev_name = 0;
            fd = -1;
            io = IO_METHOD_MMAP;
            buffers = 0;
            n_buffers = 0;
            width = 640;
            height = 480;
            format = V4L2_PIX_FMT_YUYV;
            field = V4L2_FIELD_INTERLACED;
        }
        void set(const char *key,const char *value) {
            if(!strcmp(key,"method")) {
                if(!strcmp(value,"mmap")) {
                    io = IO_METHOD_MMAP;
                } else if(!strcmp(value,"read")) {
                    io = IO_METHOD_READ;
                } else if(!strcmp(value,"userptr")) {
                    io = IO_METHOD_USERPTR;
                } else throw "unknown video capture method";
            } else if(!strcmp(key,"format")) {
                if(!strcmp(value,"yuyv")) {
                    this->format = V4L2_PIX_FMT_YUYV;
		} else if(!strcmp(value,"rgb24")) {
                    this->format = V4L2_PIX_FMT_RGB24;
		} else if(!strcmp(value,"rgb32")) {
                    this->format = V4L2_PIX_FMT_RGB32;
                } else throw "unknown device video format";
            } else throw "unknown key";
        }
        void set(const char *key,double value) {
            if(!strcmp(key,"w")) width = int(value);
            else if(!strcmp(key,"h")) height = int(value);
            else throw "unknown key";
        }
        void open(const char *dev_name) {
            this->dev_name = strdup(dev_name);
            open_device();
            init_device();
            start_capturing();
        }
        void close() {
	    //stop_capturing();
            uninit_device();
            close_device();
            free((void*)this->dev_name);
        }
    
	void start_capturing() {
	    unsigned int i;
	    enum v4l2_buf_type type;
	    switch (io) {
	    case IO_METHOD_READ:
		/* Nothing to do. */
		break;
	    case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i) {
		    struct v4l2_buffer buf;
		    CLEAR (buf);
		    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		    buf.memory = V4L2_MEMORY_MMAP;
		    buf.index = i;

		    if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
			throw "VIDIOC_QBUF";
		}
		
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
		    throw "VIDIOC_STREAMON";

		break;

	    case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i) {
		    struct v4l2_buffer buf;

		    CLEAR (buf);

		    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		    buf.memory = V4L2_MEMORY_USERPTR;
		    buf.index = i;
		    buf.m.userptr	= (unsigned long) buffers[i].start;
		    buf.length = buffers[i].length;

		    if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
			throw "VIDIOC_QBUF";
		}

		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
		    throw "VIDIOC_STREAMON";

		break;
	    }
	}


        int read_frame() {
            struct v4l2_buffer buf;
            unsigned int i;
            switch (io) {
            case IO_METHOD_READ:
                if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
                    switch (errno) {
                    case EAGAIN:
                        return 0;
                    case EIO:
                        /* Could ignore EIO, see spec. */
                        /* fall through */
                    default:
                        throw "read failed";
                    }
                }
                process_image(buffers[0].start);
                break;
            case IO_METHOD_MMAP:
                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
                    switch (errno) {
                    case EAGAIN:
                        return 0;
                    case EIO:
                        /* Could ignore EIO, see spec. */
                        /* fall through */
                    default:
                        throw "VIDIOC_DQBUF";
                    }
                }
                assert(buf.index < n_buffers);
                process_image(buffers[buf.index].start);
                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                    throw "VIDIOC_QBUF";
                break;
            case IO_METHOD_USERPTR:
                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_USERPTR;
                if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
                    switch (errno) {
                    case EAGAIN:
                        return 0;
                    case EIO:
                        /* Could ignore EIO, see spec. */
                        /* fall through */
                    default:
                        throw "VIDIOC_DQBUF";
                    }
                }
                for (i = 0; i < n_buffers; ++i)
                    if (buf.m.userptr == (unsigned long) buffers[i].start
                        && buf.length == buffers[i].length)
                        break;
                assert(i < n_buffers);
                process_image((void *) buf.m.userptr);
                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                    throw "VIDIOC_QBUF";
                break;
            default:
                throw "unknown I/O method";
            }
            return 1;
        }
        void capture() {
            for (;;) {
                fd_set fds;
                struct timeval tv;
                int r;
                FD_ZERO(&fds);
                FD_SET(fd, &fds);
                /* Timeout. */
                tv.tv_sec = 20;
                tv.tv_usec = 0;
                r = select(fd + 1, &fds, NULL, NULL, &tv);
                if (-1 == r) {
                    if (EINTR == errno)
                        continue;
                    throw "select";
                }
                if (0 == r) {
                    throw "select timeout";
                }
                if (read_frame())
                    break;

                /* EAGAIN - continue select loop. */
            }
        }
        void process_image(const void *p) {
            current_buffer = p;
            // fprintf(stderr,"process_image\n");
        }
        static inline int fclamp(float x) {
            if(x<0) return 0;
            if(x>255) return 255;
            return int(x);
        }
        bool readFrameGray(bytearray &gray) {
            gray.resize(width,height);
            readFrameAny(gray);
            return 1;
        }
        bool readFrame(bytearray &rgb) {
            rgb.resize(width,height,3);
            readFrameAny(rgb);
            return 1;
        }
        void readFrameAny(bytearray &image) {
            ASSERT(image.dim(0)==width && image.dim(1)==height);
            capture();
            ASSERT(current_buffer!=0);
            if(format==V4L2_PIX_FMT_YUYV) {
		for(int row=0;row<height;row++) {
		    unsigned char *in = width*row*2+(unsigned char *)current_buffer;
		    for(int col=0;col<width;col++) {
			int i = 2*col;
			int ii = 4*(col/2);
			int y = 255*(in[i]-16)/219;
			int cb = in[ii+1];
			int cr = in[ii+3];
			//int u = 127*(in[ii+1]-128)/112;
			//int v = 127*(in[ii+3]-128)/112;
			float r = y + 1.402*(127.0/112.0)*(cr-128);
			float g = y - 0.344*(127.0/112.0)*(cb-128)
			    - 0.714*(127.0/112.0)*(cr-128);
			float b = y + 1.772*(127.0/112.0)*(cb-128);
			int rrow = height-row-1;
			if(image.rank()==2) {
			    float value = fclamp((r+g+b)/3.0);
			    image.unsafe_at(col,rrow) = int(value);
			} else {
			    image.unsafe_at(col,rrow,0) = fclamp(r);
			    image.unsafe_at(col,rrow,1) = fclamp(g);
			    image.unsafe_at(col,rrow,2) = fclamp(b);
			}
		    }
		}
            } else {
                throw "cannot decode this format";
            }
        }
        bool readFrame(bytearray &red,bytearray &green,bytearray &blue) {
            bytearray temp;
            readFrame(temp);
            red.resize(width,height);
            green.resize(width,height);
            blue.resize(width,height);
            for(int i=0;i<width;i++) {
                for(int j=0;j<height;j++) {
                    red(i,j) = temp(i,j,0);
                    green(i,j) = temp(i,j,1);
                    blue(i,j) = temp(i,j,2);
                }
            }
            return 1;
        }
        void stop() {
            enum v4l2_buf_type type;
            switch (io) {
            case IO_METHOD_READ:
                /* Nothing to do. */
                break;
            case IO_METHOD_MMAP:
            case IO_METHOD_USERPTR:
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
                    throw "VIDIOC_STREAMOFF";
                break;
            }
        }
        void start() {
            unsigned int i;
            enum v4l2_buf_type type;
            switch (io) {
            case IO_METHOD_READ:
                /* Nothing to do. */
                break;
            case IO_METHOD_MMAP:
                for (i = 0; i < n_buffers; ++i) {
                    struct v4l2_buffer buf;
                    CLEAR(buf);
                    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    buf.memory = V4L2_MEMORY_MMAP;
                    buf.index = i;
                    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                        throw "VIDIOC_QBUF";
                }

                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                    throw "VIDIOC_STREAMON";
                break;
            case IO_METHOD_USERPTR:
                for (i = 0; i < n_buffers; ++i) {
                    struct v4l2_buffer buf;
                    CLEAR(buf);
                    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    buf.memory = V4L2_MEMORY_USERPTR;
                    buf.index = i;
                    buf.m.userptr = (unsigned long) buffers[i].start;
                    buf.length = buffers[i].length;
                    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                        throw "VIDIOC_QBUF";
                }
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                    throw "VIDIOC_STREAMON";
                break;
            }
        }
        void uninit_device() {
            unsigned int i;
            switch (io) {
            case IO_METHOD_READ:
                free(buffers[0].start);
                break;
            case IO_METHOD_MMAP:
                for (i = 0; i < n_buffers; ++i)
                    if (-1 == munmap(buffers[i].start, buffers[i].length))
                        throw "munmap";
                break;
            case IO_METHOD_USERPTR:
                for (i = 0; i < n_buffers; ++i)
                    free(buffers[i].start);
                break;
            }
            free(buffers);
        }
        void init_read(unsigned int buffer_size) {
            buffers = (buffer*) calloc(1, sizeof(*buffers));
            if (!buffers) {
                throw "out of memory";
            }
            buffers[0].length = buffer_size;
            buffers[0].start = malloc(buffer_size);
            if (!buffers[0].start) {
                throw "out of memory";
            }
        }
        void init_mmap() {
            struct v4l2_requestbuffers req;
            CLEAR(req);
            req.count = 4;
            req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            req.memory = V4L2_MEMORY_MMAP;
            if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                    throw "mmap not supported by device";
                } else {
                    throw "VIDIOC_REQBUFS";
                }
            }
            if (req.count < 2) {
                throw "insufficient buffer memory";
            }
            buffers = (buffer*)calloc(req.count, sizeof(*buffers));
            if (!buffers) {
                throw "out of memory";
            }
            for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;
                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = n_buffers;
                if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
                    throw "VIDIOC_QUERYBUF";
                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start = mmap(NULL /* start anywhere */ ,
                                                buf.length, PROT_READ | PROT_WRITE
                                                /* required */ ,
                                                MAP_SHARED /* recommended */ ,
                                                fd, buf.m.offset);
                if (MAP_FAILED == buffers[n_buffers].start)
                    throw "mmap";
            }
        }
        void init_userp(unsigned int buffer_size) {
            struct v4l2_requestbuffers req;
            unsigned int page_size;
            page_size = getpagesize();
            buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);
            CLEAR(req);
            req.count = 4;
            req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            req.memory = V4L2_MEMORY_USERPTR;
            if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                    throw "device does not support user pointer I/O";
                } else {
                    throw "VIDIOC_REQBUFS";
                }
            }
            buffers = (buffer*)calloc(4, sizeof(*buffers));
            if (!buffers) {
                throw "out of memory";
            }
            for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
                buffers[n_buffers].length = buffer_size;
                buffers[n_buffers].start = memalign( /* boundary */ page_size,
                                                     buffer_size);
                if (!buffers[n_buffers].start) {
                    throw "out of memory";
                }
            }
        }
        void init_device() {
            struct v4l2_capability cap;
            struct v4l2_cropcap cropcap;
            struct v4l2_crop crop;
            struct v4l2_format fmt;
            unsigned int min;
            if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                    throw "device is not a V4L2 device";
                } else {
                    throw "VIDIOC_QUERYCAP";
                }
            }
            if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                throw "device is not a video capture device";
            }
            switch (io) {
            case IO_METHOD_READ:
                if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
                    throw "device does not support read I/O";
                }
                break;
            case IO_METHOD_MMAP:
            case IO_METHOD_USERPTR:
                if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                    throw "device does not support streaming I/O";
                }
                break;
            }
            /* Select video input, video standard and tune here. */
            CLEAR(cropcap);
            cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect;       /* reset to default */
                if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
                    switch (errno) {
                    case EINVAL:
                        /* Cropping not supported. */
                        break;
                    default:
                        /* Errors ignored. */
                        break;
                    }
                }
            } else {
                /* Errors ignored. */
            }
            CLEAR(fmt);
            fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            fmt.fmt.pix.width = width;
            fmt.fmt.pix.height = height; 
            fmt.fmt.pix.pixelformat = format;
            fmt.fmt.pix.field = field;
            if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
                throw "driver does not accept the selected video format";
            // Note in buggy drivers, VIDIOC_S_FMT may change width and height...
            min = fmt.fmt.pix.width * 2;
            if (fmt.fmt.pix.bytesperline < min)
                fmt.fmt.pix.bytesperline = min;
            min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
            if (fmt.fmt.pix.sizeimage < min)
                fmt.fmt.pix.sizeimage = min;
            switch (io) {
            case IO_METHOD_READ:
                init_read(fmt.fmt.pix.sizeimage);
                break;
            case IO_METHOD_MMAP:
                init_mmap();
                break;
            case IO_METHOD_USERPTR:
                init_userp(fmt.fmt.pix.sizeimage);
                break;
            }
        }
        void close_device() {
            if (-1 == ::close(fd))
                throw "close failed";
            fd = -1;
        }
        void open_device() {
            struct stat st;
            if (-1 == stat(dev_name, &st)) {
                throw "cannot identify device";
            }
            if (!S_ISCHR(st.st_mode)) {
                throw "no such device";
            }
            fd = ::open(dev_name, O_RDWR /* required */  | O_NONBLOCK, 0);
            if (-1 == fd) {
                throw "cannot open device";
            }
        }
    };

    VideoInput *make_V4L2Capture() {
        return new VideoCapture();
    }

}

