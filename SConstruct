# -*- Python -*-

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
# Project: iulib - the open source image understanding library
# File: SConstruct
# Purpose: building iulib
# Responsible: kofler
# Reviewer: 
# Primary Repository: http://iulib.googlecode.com/svn/trunk/
# Web Sites: www.iupr.org, www.dfki.de

# hints for make users
#
#  make all = scons
#   install = scons install
#     clean = scons -c
# uninstall = scons -c install

EnsureSConsVersion(0,97)

import glob,os,sys,string,re

### Options exposed via SCons
opts = Variables('custom.py')
opts.Add('opt', 'Compiler flags for optimization/debugging', "-g -O3 -fPIC")
opts.Add('warn', 'Compiler flags for warnings', "-Wall -D__warn_unused_result__=__far__")
opts.Add('prefix', 'The installation root for iulib', "/usr/local")

opts.Add(BoolVariable('sdl', "provide SDL-based graphics routines", "no"))
opts.Add(BoolVariable('vidio', "provide video I/O functionality", "no"))
opts.Add(BoolVariable('v4l2', "provide v4l2 functionality", "no"))

opts.Add(BoolVariable('test', "Run some tests after the build", "no"))
# opts.Add(BoolVariable('style', 'Check style', "no"))

env = Environment(options=opts, CXXFLAGS=["${opt}","${warn}"])
Help(opts.GenerateHelpText(env))

conf = Configure(env)
if "-DUNSAFE" in env["opt"]:
    print "WARNING: compile with -DUNSAFE or high optimization only for production use"

if re.search(r'-O[234]',env["opt"]):
    print "compiling with high optimization"
else:
    print "compiling for development (slower but safer)"

assert conf.CheckLibWithHeader('png', 'png.h', 'C', 'png_byte;', 1),"please install: libpng12-dev"
assert conf.CheckLibWithHeader('jpeg', 'jconfig.h', 'C', 'jpeg_std_error();', 1),"please install: libjpeg62-dev"    
assert conf.CheckLibWithHeader('tiff', 'tiff.h', 'C', 'inflate();', 1), "please install: libtiff4-dev"

### check for optional parts

if env["sdl"]:
    have_sdl = conf.CheckCXXHeader("SDL/SDL_gfxPrimitives.h") and \
               conf.CheckCXXHeader("SDL/SDL.h")
    assert have_sdl,"SDL requested, but can't find it"
else:
    have_sdl = 0

if env["vidio"]:
    have_vidio = conf.CheckCXXHeader("libavcodec/avcodec.h") and \
                 conf.CheckCXXHeader("libavformat/avformat.h")
    assert have_vidio,"vidio requested, but can't find it"
else:
    have_vidio = 0

if env["v4l2"]:
    have_v4l2 = conf.CheckHeader("linux/videodev2.h")
    assert have_v4l2,"v4l2 requested, but can't find it"
else:
    have_v4l2 = 0

conf.Finish()

### install folders
prefix = "${prefix}"
incdir_iulib = prefix+"/include/iulib"
incdir_colib = prefix+"/include/colib"
libdir = prefix+"/lib"
datadir = prefix+"/share/iulib"
bindir = prefix+"/bin"

### collect sources etc.
env.Prepend(CPPPATH=[".","colib","imglib","imgio","imgbits","utils","vidio"])

sources = glob.glob("imglib/img*.cc") 
sources += glob.glob("imgbits/img*.cc")
sources += """
    imgio/autoinvert.cc imgio/imgio.cc imgio/io_jpeg.cc
    imgio/io_pbm.cc imgio/io_png.cc imgio/io_tiff.cc
""".split()

if have_vidio:
    sources += glob.glob("vidio/vidio.cc")
if have_v4l2:
    sources += glob.glob("vidio/v4l2cap.cc")
    env.Append(LIBS=["v4l2"])
if have_sdl:
    sources += ["utils/dgraphics.cc","utils/SDL_lines.cc"]
    env.Append(LIBS=["SDL","SDL_gfx","SDL_image"])
else:
    sources += ["utils/dgraphics_nosdl.cc"]

libiulib = env.SharedLibrary('libiulib',sources)

env.Append(CXXFLAGS=['-g','-fPIC'])
env.Append(LIBPATH=['.'])
progs = env.Clone()
progs.Append(LIBS=libiulib)
for file in glob.glob("commands/*.cc"):
    progs.Program(file[:-3],file)
if have_vidio:
    progs.Program("vidio/test-vidio","vidio/test-vidio.cc")
    progs.Append(LIBS=["avformat","avcodec"])
if 0 and have_v4l2:
    progs.Program("vidio/test-vidcap","vidio/test-vidcap.cc")

env.Install(libdir,libiulib)
for header in glob.glob("*/*.h"): 
    if "colib/" in header:
        env.Install(incdir_colib,header)
    else:
        env.Install(incdir_iulib,header)
env.Alias('install',[libdir,incdir_iulib,incdir_colib])

test_builder = Builder(action='$SOURCE&&touch $TARGET',
                  suffix = '.passed',
                  src_suffix = '')
progs.Append(BUILDERS={'Test':test_builder})

if env["test"]:
    for file in glob.glob("*/test-*.cc") + glob.glob("*/*/test-*.cc"):
        if not file.startswith('vidio'):
            progs.Program(file[:-3],file)
            progs.Test(file[:-3])
            progs.Alias("test",file[:-3]+".passed")
