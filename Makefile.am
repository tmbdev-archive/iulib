# Copyright 2008 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
# or its licensors, as applicable.
#
# You may not use this file except under the terms of the accompanying license.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you
# may not use this file except in compliance with the License. You may
# obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Project: iulib -- image understanding library
# File: Makefile.am
# Purpose: building iulib
# Responsible: kofler
# Reviewer:
# Primary Repository: http://ocropus.googlecode.com/svn/trunk/
# Web Sites: www.iupr.org, www.dfki.de

AM_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib

includedir = ${prefix}/include/iulib
colibdir = ${prefix}/include/colib

lib_LIBRARIES = libiulib.a

libiulib_a_SOURCES =  \
$(srcdir)/imgio/imgio.cc $(srcdir)/imgio/autoinvert.cc $(srcdir)/imgio/io_jpeg.cc $(srcdir)/imgio/io_pbm.cc $(srcdir)/imgio/io_png.cc \
$(srcdir)/imglib/imglabels.cc $(srcdir)/imglib/imgmisc.cc $(srcdir)/imglib/imgrescale.cc $(srcdir)/imglib/imggraymorph.cc $(srcdir)/imglib/imgops.cc $(srcdir)/imglib/imggauss.cc $(srcdir)/imglib/imgfilters.cc $(srcdir)/imglib/imgedges.cc $(srcdir)/imglib/imgbrushfire.cc $(srcdir)/imglib/imgtrace.cc $(srcdir)/imglib/imgmap.cc $(srcdir)/imglib/imgthin.cc $(srcdir)/imglib/imgmorph.cc \
$(srcdir)/imgbits/imgblit_c.cc $(srcdir)/imgbits/imgbsmp.cc $(srcdir)/imgbits/imgbits.cc $(srcdir)/imgbits/imgblit.cc $(srcdir)/imgbits/imgrle.cc $(srcdir)/imgbits/imgbthin.cc

include_HEADERS =  \
$(srcdir)/imgio/io_jpeg.h $(srcdir)/imgio/autoinvert.h $(srcdir)/imgio/io_pbm.h $(srcdir)/imgio/io_png.h $(srcdir)/imgio/imgio.h \
$(srcdir)/imglib/imgops.h $(srcdir)/imglib/iulib.h $(srcdir)/imglib/imgbrushfire.h $(srcdir)/imglib/imglib.h $(srcdir)/imglib/imgthin.h $(srcdir)/imglib/imgrescale.h $(srcdir)/imglib/imglabels.h $(srcdir)/imglib/imgmorph.h $(srcdir)/imglib/imgmisc.h $(srcdir)/imglib/imggraymorph.h $(srcdir)/imglib/imgfilters.h $(srcdir)/imglib/imgmap.h $(srcdir)/imglib/imgedges.h $(srcdir)/imglib/imgtrace.h $(srcdir)/imglib/imggauss.h \
$(srcdir)/imgbits/imgrle.h $(srcdir)/imgbits/imgbits.h $(srcdir)/imgbits/bithacks.h $(srcdir)/imgbits/imgbitptr.h
include_HEADERS += $(srcdir)/utils/dgraphics.h

colib_HEADERS = \
$(srcdir)/colib/objlist.h $(srcdir)/colib/nustring.h $(srcdir)/colib/colib.h $(srcdir)/colib/misc.h $(srcdir)/colib/vec2.h $(srcdir)/colib/heap.h $(srcdir)/colib/coords.h $(srcdir)/colib/env.h $(srcdir)/colib/narray.h $(srcdir)/colib/rowarrays.h $(srcdir)/colib/clib.h $(srcdir)/colib/classifier.h $(srcdir)/colib/ocrinterfaces.h $(srcdir)/colib/quicksort.h $(srcdir)/colib/iarith.h $(srcdir)/colib/unionfind.h $(srcdir)/colib/strbuf.h $(srcdir)/colib/compat.h $(srcdir)/colib/deprecation.h $(srcdir)/colib/narray-ops.h $(srcdir)/colib/checks.h $(srcdir)/colib/nbest.h $(srcdir)/colib/narray-util.h $(srcdir)/colib/smartptr.h $(srcdir)/colib/hash.h $(srcdir)/colib/debugf.h

check_PROGRAMS =  test-quicksort  test-coords  test-rowarrays  test-smartptr  test-heap  test-strbuf  test-misc  test-nbest  test-hash  test-narray  test-io_png  test-read_write_png  test-pbmpng  test-imgbrushfire  test-imggauss  test-imgmap  test-imgmisc  test-imglabels  test-simple
test_quicksort_SOURCES = $(srcdir)/colib/tests/test-quicksort.cc
test_quicksort_LDADD = libiulib.a
test_quicksort_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_coords_SOURCES = $(srcdir)/colib/tests/test-coords.cc
test_coords_LDADD = libiulib.a
test_coords_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_rowarrays_SOURCES = $(srcdir)/colib/tests/test-rowarrays.cc
test_rowarrays_LDADD = libiulib.a
test_rowarrays_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_smartptr_SOURCES = $(srcdir)/colib/tests/test-smartptr.cc
test_smartptr_LDADD = libiulib.a
test_smartptr_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_heap_SOURCES = $(srcdir)/colib/tests/test-heap.cc
test_heap_LDADD = libiulib.a
test_heap_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_strbuf_SOURCES = $(srcdir)/colib/tests/test-strbuf.cc
test_strbuf_LDADD = libiulib.a
test_strbuf_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_misc_SOURCES = $(srcdir)/colib/tests/test-misc.cc
test_misc_LDADD = libiulib.a
test_misc_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_nbest_SOURCES = $(srcdir)/colib/tests/test-nbest.cc
test_nbest_LDADD = libiulib.a
test_nbest_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_hash_SOURCES = $(srcdir)/colib/tests/test-hash.cc
test_hash_LDADD = libiulib.a
test_hash_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_narray_SOURCES = $(srcdir)/colib/tests/test-narray.cc
test_narray_LDADD = libiulib.a
test_narray_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_io_png_SOURCES = $(srcdir)/imgio/tests/test-io_png.cc
test_io_png_LDADD = libiulib.a
test_io_png_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_read_write_png_SOURCES = $(srcdir)/imgio/tests/test-read_write_png.cc
test_read_write_png_LDADD = libiulib.a
test_read_write_png_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_pbmpng_SOURCES = $(srcdir)/imgio/tests/test-pbmpng.cc
test_pbmpng_LDADD = libiulib.a
test_pbmpng_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_imgbrushfire_SOURCES = $(srcdir)/imglib/tests/test-imgbrushfire.cc
test_imgbrushfire_LDADD = libiulib.a
test_imgbrushfire_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_imggauss_SOURCES = $(srcdir)/imglib/tests/test-imggauss.cc
test_imggauss_LDADD = libiulib.a
test_imggauss_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_imgmap_SOURCES = $(srcdir)/imglib/tests/test-imgmap.cc
test_imgmap_LDADD = libiulib.a
test_imgmap_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_imgmisc_SOURCES = $(srcdir)/imglib/tests/test-imgmisc.cc
test_imgmisc_LDADD = libiulib.a
test_imgmisc_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_imglabels_SOURCES = $(srcdir)/imglib/tests/test-imglabels.cc
test_imglabels_LDADD = libiulib.a
test_imglabels_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_simple_SOURCES = $(srcdir)/imgbits/test-simple.cc
test_simple_LDADD = libiulib.a
test_simple_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib

# conditionals
if have_sdl
    libiulib_a_SOURCES += $(srcdir)/utils/dgraphics.cc
    libiulib_a_SOURCES += $(srcdir)/utils/SDL_lines.cc
    include_HEADERS += $(srcdir)/utils/SDL_lines.h
else
    libiulib_a_SOURCES += $(srcdir)/utils/dgraphics_nosdl.cc
endif

if have_vidio
    libiulib_a_SOURCES += $(srcdir)/vidio/vidio.cc
endif

# We install it always because iulib.h always includes it.
include_HEADERS += $(srcdir)/vidio/vidio.h

if have_v4l2
    libiulib_a_SOURCES += $(srcdir)/vidio/v4l2cap.cc
endif

# make installation of colib a separate target

install-colib:
	install -d $(colibdir)
	install $(colib_HEADERS) $(colibdir)

install: all install-colib
	install -d $(includedir)
	install -d $(libdir)
	install $(include_HEADERS) $(includedir)
	install $(lib_LIBRARIES) $(libdir)


check:
	@echo "# running tests"
	$(srcdir)/test-quicksort
	$(srcdir)/test-coords
	$(srcdir)/test-rowarrays
	$(srcdir)/test-smartptr
	$(srcdir)/test-heap
	$(srcdir)/test-strbuf
	$(srcdir)/test-misc
	$(srcdir)/test-nbest
	$(srcdir)/test-hash
	$(srcdir)/test-narray
	$(srcdir)/test-io_png
	$(srcdir)/test-read_write_png
	$(srcdir)/test-pbmpng
	$(srcdir)/test-imgbrushfire
	$(srcdir)/test-imggauss
	$(srcdir)/test-imgmap
	$(srcdir)/test-imgmisc
	$(srcdir)/test-imglabels
	$(srcdir)/test-simple
