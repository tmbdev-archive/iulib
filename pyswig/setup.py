#!/usr/bin/env python

import os
from distutils.core import setup, Extension
from numpy.distutils.misc_util import get_numpy_include_dirs

baselibs = ['tiff','png','jpeg','SDL','SDL_gfx','SDL_image','m']

include_dirs = ['/usr/local/include'] + get_numpy_include_dirs()
swig_opts = ["-c++"] + ["-I" + d for d in include_dirs]
swiglib = os.popen("swig -swiglib").read()[:-1]

iulib = Extension('_iulib',
        libraries = ['iulib']+baselibs,
        swig_opts = swig_opts,
        include_dirs = include_dirs,
        sources=['iulib.i'])

setup (name = 'iulib',
       version = '0.4',
       author      = "Thomas Breuel",
       description = """iulib library bindings""",
       ext_modules = [iulib],
       py_modules = ["iulib","iuutils"],
       data_files = [(swiglib,['iulib.i'])],
       )
