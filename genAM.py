#!/usr/bin/python

import glob, os, string as s

print """# Copyright 2008 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
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
"""

dirs = """
    imgio
    imglib
    imgbits
""".split()

print "libiulib_a_SOURCES = ",
for d in dirs:
    print '\\'
    for cc in glob.glob(d + "/*.cc"):
        if os.path.basename(cc).startswith("test-"): continue
        print "$(srcdir)/" + cc,
print
print

print "include_HEADERS = ",
for d in dirs:
    print '\\'
    for h in glob.glob(d + "/*.h"):
        print "$(srcdir)/" + h,
print
print "include_HEADERS += $(srcdir)/utils/dgraphics.h"
print

print "colib_HEADERS = \\"
for h in glob.glob("colib/*.h"):
    print "$(srcdir)/" + h,
print
print

# gather all test-* files
tests = glob.glob("colib/tests/test-*.cc")
for d in dirs:
    tests += glob.glob(d +"/test-*.cc")
    tests += glob.glob(d + "/tests/test-*.cc")
#name the resulting binaries (strip folder and suffix)
print "check_PROGRAMS = " + s.join(" " + os.path.basename(t)[:-3] for t in tests)
for t in tests:
    tName = os.path.basename(t)[:-3].replace('-','_')
    print tName + "_SOURCES = $(srcdir)/" + t
    print tName + "_LDADD = libiulib.a"
    print tName + "_CPPFLAGS = -I$(srcdir)/colib -I$(srcdir)/imgio -I$(srcdir)/imglib"




print """
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
"""

print
print "check:"
print '	@echo "# running tests"'
for t in tests:
    print "	$(srcdir)/" + os.path.basename(t)[:-3]
