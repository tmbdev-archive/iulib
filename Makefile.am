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
$(srcdir)/imgio/autoinvert.cc $(srcdir)/imgio/imgio.cc $(srcdir)/imgio/io_jpeg.cc $(srcdir)/imgio/io_pbm.cc $(srcdir)/imgio/io_png.cc \
$(srcdir)/imglib/imgbrushfire.cc $(srcdir)/imglib/imgedges.cc $(srcdir)/imglib/imgfilters.cc $(srcdir)/imglib/imggauss.cc $(srcdir)/imglib/imglabels.cc $(srcdir)/imglib/imgmap.cc $(srcdir)/imglib/imgmisc.cc $(srcdir)/imglib/imgmorph.cc $(srcdir)/imglib/imgops.cc $(srcdir)/imglib/imgrescale.cc $(srcdir)/imglib/imgthin.cc $(srcdir)/imglib/imgtrace.cc $(srcdir)/imglib/imggraymorph.cc \
$(srcdir)/imgbits/imgbits.cc $(srcdir)/imgbits/imgbsmp.cc $(srcdir)/imgbits/imgbthin.cc $(srcdir)/imgbits/imgrle.cc $(srcdir)/imgbits/imgblit_c.cc $(srcdir)/imgbits/imgblit.cc

include_HEADERS =  \
$(srcdir)/imgio/autoinvert.h $(srcdir)/imgio/imgio.h $(srcdir)/imgio/io_pbm.h $(srcdir)/imgio/io_png.h $(srcdir)/imgio/io_jpeg.h \
$(srcdir)/imglib/imgtrace.h $(srcdir)/imglib/imgthin.h $(srcdir)/imglib/imgmorph.h $(srcdir)/imglib/imgmisc.h $(srcdir)/imglib/imgops.h $(srcdir)/imglib/imglabels.h $(srcdir)/imglib/imglib.h $(srcdir)/imglib/imgfilters.h $(srcdir)/imglib/imgmap.h $(srcdir)/imglib/iulib.h $(srcdir)/imglib/imgrescale.h $(srcdir)/imglib/imggauss.h $(srcdir)/imglib/imgedges.h $(srcdir)/imglib/imggraymorph.h $(srcdir)/imglib/imgbrushfire.h \
$(srcdir)/imgbits/imgbits.h $(srcdir)/imgbits/bithacks.h $(srcdir)/imgbits/imgbitptr.h $(srcdir)/imgbits/imgrle.h
include_HEADERS += $(srcdir)/utils/dgraphics.h

colib_HEADERS = \
$(srcdir)/colib/rowarrays.h $(srcdir)/colib/coords.h $(srcdir)/colib/misc.h $(srcdir)/colib/narray-ops.h $(srcdir)/colib/narray.h $(srcdir)/colib/vec2.h $(srcdir)/colib/checks.h $(srcdir)/colib/unionfind.h $(srcdir)/colib/strbuf.h $(srcdir)/colib/objlist.h $(srcdir)/colib/narray-util.h $(srcdir)/colib/nustring.h $(srcdir)/colib/clib.h $(srcdir)/colib/nbest.h $(srcdir)/colib/smartptr.h $(srcdir)/colib/iarith.h $(srcdir)/colib/hash.h $(srcdir)/colib/compat.h $(srcdir)/colib/classifier.h $(srcdir)/colib/quicksort.h $(srcdir)/colib/ocrinterfaces.h $(srcdir)/colib/env.h $(srcdir)/colib/colib.h $(srcdir)/colib/heap.h $(srcdir)/colib/deprecation.h

check_PROGRAMS =  test-hash  test-quicksort  test-rowarrays  test-narray  test-pbmpng  test-imglabels  test-imgbrushfire  test-imgmisc  test-imgmap  test-simple
test_hash_SOURCES = $(srcdir)/colib/tests/test-hash.cc
test_hash_LDADD = libiulib.a
test_hash_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_quicksort_SOURCES = $(srcdir)/colib/tests/test-quicksort.cc
test_quicksort_LDADD = libiulib.a
test_quicksort_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_rowarrays_SOURCES = $(srcdir)/colib/tests/test-rowarrays.cc
test_rowarrays_LDADD = libiulib.a
test_rowarrays_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_narray_SOURCES = $(srcdir)/colib/tests/test-narray.cc
test_narray_LDADD = libiulib.a
test_narray_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_pbmpng_SOURCES = $(srcdir)/imgio/tests/test-pbmpng.cc
test_pbmpng_LDADD = libiulib.a
test_pbmpng_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_imglabels_SOURCES = $(srcdir)/imglib/tests/test-imglabels.cc
test_imglabels_LDADD = libiulib.a
test_imglabels_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_imgbrushfire_SOURCES = $(srcdir)/imglib/tests/test-imgbrushfire.cc
test_imgbrushfire_LDADD = libiulib.a
test_imgbrushfire_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_imgmisc_SOURCES = $(srcdir)/imglib/tests/test-imgmisc.cc
test_imgmisc_LDADD = libiulib.a
test_imgmisc_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
test_imgmap_SOURCES = $(srcdir)/imglib/tests/test-imgmap.cc
test_imgmap_LDADD = libiulib.a
test_imgmap_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib
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
	$(srcdir)/test-hash
	$(srcdir)/test-quicksort
	$(srcdir)/test-rowarrays
	$(srcdir)/test-narray
	$(srcdir)/test-pbmpng
	$(srcdir)/test-imglabels
	$(srcdir)/test-imgbrushfire
	$(srcdir)/test-imgmisc
	$(srcdir)/test-imgmap
	$(srcdir)/test-simple
