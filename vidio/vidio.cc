// -*- C++ -*-

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz 
// or its licensors, as applicable.
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
// Project: vidio -- reading and writing video streams
// File: vidio.cc
// Purpose: reading and writing video files in XViD format
// Responsible: ulges
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de


extern "C" {
#include <assert.h>
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
};

#include "colib/colib.h"
#include "vidio.h"
#include "imgmap.h"
#include "io_png.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

int img_convert(AVPicture*, int, const AVPicture*, int, int, int) {
    throw "img_convert is not available anymore (FIXME)";
}

namespace iulib {

    class VidInImpl:public VideoInput {

      private:
        const char            *inputFile;          

        //libavcodec specific parameters
        AVFormatContext *pFormatCtx;
        AVCodecContext  *pCodecCtx;
        AVCodec         *pCodec;
        AVFrame         *pFrame; 
        AVFrame         *pFrameRGB;
        AVPacket        packet;
        int             framesRead;
        int             videoStream;
        int             frameFinished;
        int             numBytes;
        uint8_t         *buffer;

        void init();
        void cleanup();

      public:
        
        void open(const char *);
        ~VidInImpl();

        void rewind();
        bool readFrameGray(bytearray &frame);
        bool readFrame(bytearray &red,
                       bytearray &green,
                       bytearray &blue);
        bool readFrame(bytearray &rgb);
    
    }; // end class VidInImpl

    class VidOutImpl:public VideoOutput {
        
      private:
        
        int frameRate;
        int frameWidth;
        int frameHeight;
        int compression;
        char *outputFile;
        
        //libavcodec specific parameters
        AVOutputFormat   *fmt;
        AVFormatContext  *oc;
        AVStream         *videoStream;
        double           videoPts;
        AVFrame          *picture, *tmpPicture;
        uint8_t          *videoOutbuf;
        int              frameCount;
        int              videoOutbufSize;
        

        void init();
        void cleanup();

        AVStream *addOutputVideoStream(AVFormatContext *oc, 
                                       int codec_id,
                                       int framerate,
                                       int framewidth,
                                       int frameheight);
        void openOutputVideo(AVFormatContext *oc, 
                             AVStream *st);
        void closeOutputVideo(AVFormatContext *oc, 
                              AVStream *st);
        AVFrame *allocOutputPicture(int pix_fmt, int width, int height);

        void copyToYUV(bytearray &red,
                       bytearray &green,
                       bytearray &blue,
                       AVFrame *pict, 
                       int frame_index);

      public:
        
        // compr determines coding quality
        //  1 for very high, 
        // 10 for very low 
        void open(char *filename,
                  int framerate,
                  int framewidth,
                  int frameheight,
                  int compr);
        ~VidOutImpl();
        bool writeFrame(bytearray &red,
                        bytearray &green,
                        bytearray &blue);
            
    }; // end class VidOutImpl


    /***********************************
      class VidInImpl
    ***********************************/

    void VidInImpl::open(const char *filename) {
        inputFile = filename;
        init();
    }

    VidInImpl::~VidInImpl() {
        cleanup();
    }


    void VidInImpl::rewind() {
        cleanup();
        init();
    }

    void VidInImpl::init() {

        // Register all formats and codecs
        av_register_all();

        // Open video file
        if(av_open_input_file(&pFormatCtx, inputFile, NULL, 0, NULL)!=0)
            throw "Couldn not open file!";

        // Retrieve stream information
        if(av_find_stream_info(pFormatCtx)<0)
            throw "Couldn not find stream information!";

        // Dump information about file onto standard error
        dump_format(pFormatCtx, 0, inputFile, false);

        // Find the first video stream
        videoStream=-1;
        for(int i=0; i<(int)pFormatCtx->nb_streams; i++)
            if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO)
                {
                    videoStream=i;
                    break;
                }

        if(videoStream==-1)
            throw "Didn't find a video stream!";

        // Get a pointer to the codec context for the video stream
        pCodecCtx=pFormatCtx->streams[videoStream]->codec;

        // Find the decoder for the video stream
        pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
        if(pCodec==NULL)
            throw "Unknown Codec!";

        // Open codec
        if(avcodec_open(pCodecCtx, pCodec)<0)
            throw "Could not open Codec!";

        // Allocate video frame
        pFrame=avcodec_alloc_frame();

        // Allocate an AVFrame structure
        pFrameRGB=avcodec_alloc_frame();
        if(pFrameRGB==NULL)
            throw "Could not allocate frame structure!";

        // Determine required buffer size and allocate buffer
        numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
                                    pCodecCtx->height);
        buffer=new uint8_t[numBytes];

        // Assign appropriate parts of buffer to image planes in pFrameRGB
        avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
                       pCodecCtx->width, pCodecCtx->height);
        
    } // end init()



    void VidInImpl::cleanup() {

        // Free the RGB image
        delete [] buffer;
        av_free(pFrameRGB);

        // Free the YUV frame
        av_free(pFrame);

        // Close the codec
        avcodec_close(pCodecCtx);
        
        // Close the video file
        av_close_input_file(pFormatCtx);
        
    }


    bool VidInImpl::readFrameGray(bytearray &frame) {

        bool success = false;

        while(av_read_frame(pFormatCtx, &packet)>=0) {
            
            // Is this a packet from the video stream?
            if(packet.stream_index==videoStream) {
                
                // Decode video frame
                avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, 
                                     packet.data, packet.size);
                
                // Did we get a video frame?
                if(frameFinished) {

                        // Convert the image from its native format to RGB
                        img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, 
                                    (AVPicture*)pFrame, pCodecCtx->pix_fmt, pCodecCtx->width, 
                                    pCodecCtx->height);
                        
                        // Convert to IUPR format
                        frame.resize(pCodecCtx->width,
                                     pCodecCtx->height);
                        unsigned char *data = pFrameRGB->data[0];
                        for (int y=0; y<frame.dim(1); ++y)
                            for (int x=0; x<frame.dim(0); ++x) {
                                    unsigned char red = *data++;
                                    unsigned char green = *data++;
                                    unsigned char blue = *data++;
                                    // swap coordinates
                                    frame(x,frame.dim(1)-1-y) = (red+green+blue)/3;
                                }

                        success = true;
                        break;
                    }
            }

            // Free the packet that was allocated by av_read_frame
            av_free_packet(&packet);
        }
        
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);


        return success;
    } // end readFrameGray()



    bool VidInImpl::readFrame(bytearray &red,
                              bytearray &green,
                              bytearray &blue) {

        bool success = false;

        while(av_read_frame(pFormatCtx, &packet)>=0) {
            
            // Is this a packet from the video stream?
            if(packet.stream_index==videoStream) {
                
                // Decode video frame
                avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, 
                                     packet.data, packet.size);
                
                // Did we get a video frame?
                if(frameFinished) {

                        // Convert the image from its native format to RGB
                        img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, 
                                    (AVPicture*)pFrame, pCodecCtx->pix_fmt, pCodecCtx->width, 
                                    pCodecCtx->height);
                        
                        // Convert to IUPR format
                        red.resize(pCodecCtx->width,
                                   pCodecCtx->height);
                        green.resize(pCodecCtx->width,
                                   pCodecCtx->height);
                        blue.resize(pCodecCtx->width,
                                   pCodecCtx->height);
                        unsigned char *data = pFrameRGB->data[0];
                        for (int y=0; y<red.dim(1); ++y)
                            for (int x=0; x<red.dim(0); ++x) {
                                    // swap coordinates
                                    red(x,red.dim(1)-1-y) = *data++;
                                    green(x,green.dim(1)-1-y) = *data++;
                                    blue(x,blue.dim(1)-1-y) = *data++;
                                }

                        success = true;
                        break;
                    }
            }

            // Free the packet that was allocated by av_read_frame
            av_free_packet(&packet);
        }
        
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);


        return success;
    } // end readFrame()

    bool VidInImpl::readFrame(bytearray &rgb) {
        bool success = false;
        while(av_read_frame(pFormatCtx, &packet)>=0) {
            // Is this a packet from the video stream?
            if(packet.stream_index==videoStream) {
                // Decode video frame
                avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, 
                                     packet.data, packet.size);
                // Did we get a video frame?
                if(frameFinished) {
                    // Convert the image from its native format to RGB
                    img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, 
                                (AVPicture*)pFrame, pCodecCtx->pix_fmt,
                                pCodecCtx->width, 
                                pCodecCtx->height);
                    // Convert to IUPR format
                    rgb.resize(pCodecCtx->width, pCodecCtx->height,3);
                    unsigned char *data = pFrameRGB->data[0];
                    for (int y=0; y<rgb.dim(1); ++y)
                        for (int x=0; x<rgb.dim(0); ++x) {
                            // swap coordinates
                            rgb(x,rgb.dim(1)-1-y,0) = *data++;
                            rgb(x,rgb.dim(1)-1-y,1) = *data++;
                            rgb(x,rgb.dim(1)-1-y,2) = *data++;
                        }
                    success = true;
                    break;
                }
            }
            // Free the packet that was allocated by av_read_frame
            av_free_packet(&packet);
        }
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);


        return success;
    } // end readFrame()


    /***********************************
      class VidOutImpl
    ***********************************/


    void VidOutImpl::open(char *filename,
              int framerate,
              int framewidth,
              int frameheight,
              int compr) {
        outputFile = filename;
        frameRate = framerate;
        frameWidth = framewidth;
        frameHeight = frameheight;
        compression = compr;
        init();
    }


    VidOutImpl::~VidOutImpl() {
        cleanup();
    }



    void VidOutImpl::init() {

        av_register_all();

        // auto detect the output format from the name. default is
        // mpeg.
        fmt = guess_format(NULL, outputFile, NULL);
        if (!fmt) {
            printf("Could not deduce output format from file extension: using MPEG.\n");
            fmt = guess_format("mpeg", NULL, NULL);
        }
        if (!fmt) 
            throw "Could not find suitable output format\n";

        // allocate and format the output media context
        oc = av_alloc_format_context();
        if (!oc)
            throw "Memory error\n";
        oc->oformat = fmt;
        snprintf(oc->filename, sizeof(oc->filename), "%s", outputFile);

        // add the video streams using the default format codecs
        // and initialize the codecs
        videoStream = NULL;
        if (fmt->video_codec != CODEC_ID_NONE) {
            videoStream = addOutputVideoStream(oc, 
                                            fmt->video_codec, 
                                            frameRate,
                                            frameWidth,
                                            frameHeight);
        }

        // set the output parameters (must be done even if
        // no parameters
        if (av_set_parameters(oc, NULL) < 0)
            throw "Invalid output format parameters\n";

        dump_format(oc, 0, outputFile, 1);

        // now that all the parameters are set, we can open the
        // video codecs and allocate the necessary encode buffers
        if (videoStream)
            openOutputVideo(oc, videoStream);

        // open output file
        if (url_fopen(&oc->pb, outputFile, URL_WRONLY) < 0)
            throw "Could not open output video file\n";

        // write the stream header, if any
        av_write_header(oc);

    } // end init()

    

    void VidOutImpl::cleanup() {

        // close each codec
        if (videoStream)
            closeOutputVideo(oc, videoStream);

        // write the trailer, if any
        av_write_trailer(oc);
    
        // free the streams
        for(int i = 0; i < (int)oc->nb_streams; i++) {
            av_freep(&oc->streams[i]->codec);
            av_freep(&oc->streams[i]);
        }

        #ifdef HAVE_OLD_AVFORMAT
            url_fclose(&oc->pb);
        #else
            url_fclose(oc->pb);
        #endif

        // free the stream
        av_free(oc);

    } // end cleanup()



    AVStream *VidOutImpl::addOutputVideoStream(AVFormatContext *oc, 
                                           int codec_id,
                                           int frameRate,
                                           int width,
                                           int height) {

        AVCodecContext *c;
        AVStream *st;

        st = av_new_stream(oc, 0);
        if (!st) 
            throw "Could not alloc stream\n";

        c = st->codec;
        c->codec_id = (CodecID)codec_id;
        c->codec_type = CODEC_TYPE_VIDEO;

        // set encoding quality
        c->qmin = c->qmax = compression;

        if (width%2)
            c->width = width+1;
        else 
            c->width = width;
        if (height%2)
            c->height = height+1;
        else 
            c->height = height;

        if (width%2||height%2)
            fprintf(stderr, 
                    "Warning: Resolution is not a multiple of two "
                    "(adding one row / column)!\n");

        // time base: this is the fundamental unit of time (in seconds) in terms
        // of which frame timestamps are represented. for fixed-fps content,
        // timebase should be 1/framerate and timestamp increments should be
        // identically 1.
        c->time_base.den = frameRate;
        c->time_base.num = 1;
        c->gop_size = 12; // emit one intra frame every twelve frames at most
        c->pix_fmt = PIX_FMT_YUV420P;
        if (c->codec_id == CODEC_ID_MPEG1VIDEO){
            // needed to avoid using macroblocks in which some coeffs overflow
            // this doesnt happen with normal video, it just happens here as the
            // motion of the chroma plane doesnt match the luma plane 
            c->mb_decision=2;
        }

        // some formats want stream headers to be separate
        if(!strcmp(oc->oformat->name, "mp4") || 
           !strcmp(oc->oformat->name, "mov") || 
           !strcmp(oc->oformat->name, "3gp"))
            c->flags |= CODEC_FLAG_GLOBAL_HEADER;

        return st;

    } // end addVideoStream()





    void VidOutImpl::openOutputVideo(AVFormatContext *oc, 
                                 AVStream *st) {

        AVCodec *codec;
        AVCodecContext *c;
        
        c = st->codec;

        // find the video encoder
        codec = avcodec_find_encoder(c->codec_id);
        if (!codec)
            throw "codec not found\n";

        // open the codec
        if (avcodec_open(c, codec) < 0)
            throw "could not open codec\n";

        videoOutbuf = NULL;
        if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
            /* allocate output buffer */
            /* XXX: API change will be done */
            /* buffers passed into lav* can be allocated any way you prefer,
               as long as they're aligned enough for the architecture, and
               they're freed appropriately (such as using av_free for buffers
               allocated with av_malloc) */
            videoOutbufSize = 200000;
            videoOutbuf = (uint8_t*)av_malloc(videoOutbufSize);
        }

        // allocate the encoded raw picture
        picture = allocOutputPicture(c->pix_fmt, c->width, c->height);
        if (!picture)
            throw "Could not allocate picture (only supports even frame width and frame height)\n";

        // if the output format is not YUV420P, then a temporary YUV420P
        // picture is needed too. It is then converted to the required
        // output format
        tmpPicture = NULL;
        if (c->pix_fmt != PIX_FMT_YUV420P) {
            tmpPicture = allocOutputPicture(PIX_FMT_YUV420P, c->width, c->height);
            if (!tmpPicture)
                throw "Could not allocate temporary picture\n";
        }

    } // end openVideo()



    void VidOutImpl::closeOutputVideo(AVFormatContext *oc, AVStream *st) {

        avcodec_close(st->codec);
        av_free(picture->data[0]);
        av_free(picture);
        if (tmpPicture) {
            av_free(tmpPicture->data[0]);
            av_free(tmpPicture);
        }
        av_free(videoOutbuf);

    } // end closeVideo()



    AVFrame *VidOutImpl::allocOutputPicture(int pix_fmt, int width, int height) {

        AVFrame *picture;
        uint8_t *picture_buf;
        int size;

        picture = avcodec_alloc_frame();
        if (!picture)
            return NULL;
        size = avpicture_get_size(pix_fmt, width, height);
        picture_buf = (uint8_t*)av_malloc(size);
        if (!picture_buf) {
            av_free(picture);
            return NULL;
        }
        avpicture_fill((AVPicture *)picture, picture_buf,
                       pix_fmt, width, height);
        return picture;
    } // end allocOutputPicture




    void VidOutImpl::copyToYUV(bytearray &red,
                           bytearray &green,
                           bytearray &blue,
                           AVFrame *pict, 
                           int frame_index) {


        if (red.dim(0)!=green.dim(0) ||
            red.dim(1)!=green.dim(1) ||
            red.dim(0)!=blue.dim(0) ||
            red.dim(1)!=blue.dim(1))
            throw "dimensions for color channels differ!\n";

        // scale frame to output size wanted, 
        // and call recursively, if necessary
        if (frameWidth !=red.dim(0) || 
            frameHeight!=red.dim(1)) {

            bytearray tmp_red,tmp_green,tmp_blue;
            scale_interpolate(tmp_red,red,frameWidth,frameHeight);
            scale_interpolate(tmp_green,green,frameWidth,frameHeight);
            scale_interpolate(tmp_blue,blue,frameWidth,frameHeight);

            copyToYUV(tmp_red,tmp_green,tmp_blue,pict,frame_index);

        } else {

            // fill Y component
            for(int y=0;y<frameHeight;y++) {
                for(int x=0;x<frameWidth;x++) {
                    int vred = red(x,red.dim(1)-1-y);
                    int vgreen = green(x,green.dim(1)-1-y);
                    int vblue = blue(x,blue.dim(1)-1-y);
                    int val = ((66*vred+129*vgreen+25*vblue+128)>>8)+16;
                    pict->data[0][y * pict->linesize[0] + x] = val;
                }
            }
            
            // fill Chroma components Cb and Cr
            for(int y=0;y<frameHeight/2;y++) {
                for(int x=0;x<frameWidth/2;x++) {
                    int vred = red(2*x,red.dim(1)-1-2*y);
                    int vgreen = green(2*x,green.dim(1)-1-2*y);
                    int vblue = blue(2*x,blue.dim(1)-1-2*y);
                    int cb = ((-38*vred-74*vgreen+112*vblue+128)>>8)+128;
                    int cr = ((112*vred-94*vgreen-18*vblue+128)>>8)+128;       
                    pict->data[1][y * pict->linesize[1] + x] = cb;
                    pict->data[2][y * pict->linesize[2] + x] = cr;
                }
            }
        }
        
    } // end copyToYUV



    bool VidOutImpl::writeFrame(bytearray &red,
                                bytearray &green,
                                bytearray &blue) {

        int out_size, ret;
        AVCodecContext *c;

        c = videoStream->codec;

        if (c->pix_fmt != PIX_FMT_YUV420P) {
            throw "Only pix format YUV420P is supported so far!";
        } else {
            copyToYUV(red, green, blue, picture, frameCount);
        }


        if (oc->oformat->flags & AVFMT_RAWPICTURE) {
            // raw video case. The API will change slightly in the near
            // future for that
            AVPacket pkt;
            av_init_packet(&pkt);
            
            pkt.flags |= PKT_FLAG_KEY;
            pkt.stream_index= videoStream->index;
            pkt.data= (uint8_t *)picture;
            pkt.size= sizeof(AVPicture);
            
            ret = av_write_frame(oc, &pkt);
        } else {
            // encode the image
            out_size = avcodec_encode_video(c, videoOutbuf, videoOutbufSize, picture);
            // if zero size, it means the image was buffered
            if (out_size > 0) {
                AVPacket pkt;
                av_init_packet(&pkt);
                
                pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, videoStream->time_base);
                if(c->coded_frame->key_frame)
                    pkt.flags |= PKT_FLAG_KEY;
                pkt.stream_index= videoStream->index;
                pkt.data= videoOutbuf;
                pkt.size= out_size;

                // write the compressed frame in the media file
                ret = av_write_frame(oc, &pkt);
            } else {
                ret = 0;
            }
        }

        if (ret != 0) {
            fprintf(stderr,  "Error while writing video frame\n");
            return false;
        }
        frameCount++;

        return true;
    } // end writeFrame()

    VideoInput *make_VideoInput() {
        return new VidInImpl();
    }

    VideoOutput *make_VideoOutput() {
        return new VidOutImpl();
    }
}
