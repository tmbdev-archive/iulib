DIE() {
    echo "ERROR: $*" 1>&2
    exit 1
}

# -- Prelude --
AC_INIT(iulib, 0.3, )
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([colib/colib.h])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([float.h malloc.h stdint.h stdlib.h string.h unistd.h wchar.h])

# check if we are on a POSIX system
AC_CHECK_HEADER(unistd.h,,AC_SUBST(noposix, 1))

# --- libz (required) ---
AC_CHECK_LIB(z, compress,,
    AC_MSG_ERROR([no zlib; please install zlib or equivalent]))

# --- libpng (required) ---
AC_CHECK_HEADER(png.h,,
    AC_MSG_ERROR([no png.h; please install libpng12-dev or equivalent]))
AC_CHECK_LIB(png, png_sig_cmp,,
    AC_MSG_ERROR([no png_sig_cmp; please install libpng12-dev or equivalent]))

# --- libjpg (required) ---
AC_CHECK_HEADER(jpeglib.h,,
    AC_MSG_ERROR([no jpeglib.h; please install libjpeg62-dev or equivalent]))
AC_CHECK_LIB(jpeg, jpeg_std_error,,
    AC_MSG_ERROR([no jpeglib.h; please install libjpeg62-dev or equivalent]))

# --- libtiff (required) ---
AC_CHECK_LIB(tiff, TIFFOpen,,
    AC_MSG_ERROR([no TIFFOpen; please install libtiff4-dev or equivalent]))

# --- SDL (optional for graphical debugging) ---
AC_ARG_WITH(SDL,[  --without-SDL           disable SDL (graphical debugging)],
    [ac_cv_use_sdl=$withval], [ac_cv_use_sdl=yes])
if test x$ac_cv_use_sdl != xno; then
    AC_CHECK_LIB(SDL,SDL_Init,,AC_SUBST(nosdl,1))
    AC_CHECK_LIB(SDL_gfx,rotozoomSurface,,AC_SUBST(nosdl,1))
else
    AC_SUBST(nosdl, 1)
fi
AM_CONDITIONAL([have_sdl], [test "$nosdl" != 1])

# vidio needs C++ headers
AC_LANG(C++)

# --- vidio (optional video in-/output) ---
AC_SUBST(novidio, 0)
AC_CHECK_HEADER(ffmpeg/avcodec.h,,AC_SUBST(novidio, 1))
AC_CHECK_HEADER(ffmpeg/avformat.h,,AC_SUBST(novidio, 1))
AC_TRY_COMPILE([#include <ffmpeg/avio.h>
                #include <ffmpeg/avformat.h>],
               [AVFormatContext fc; url_fclose(fc.pb);],
               [], [AC_DEFINE(HAVE_OLD_AVFORMAT)])
AM_CONDITIONAL([have_vidio], [test "$novidio" != 1])

AC_SUBST(nov4l2, 0)
AC_CHECK_HEADER(linux/videodev2.h,,AC_SUBST(nov4l2, 1))
AM_CONDITIONAL([have_v4l2], [test "$nov4l2" != 1])

AC_TYPE_INT64_T
AC_TYPE_PID_T

AC_FUNC_FORK
AC_CHECK_FUNCS([dup2 memset sqrt strchr strdup strrchr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo
echo "OK! You can build and install iulib the usual way:"
echo 
echo "    make"
echo "    sudo make install"
echo
