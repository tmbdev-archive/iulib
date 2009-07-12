DIE() {
    echo "ERROR: $*" 1>&2
    exit 1
}

# -- Prelude --
AC_INIT(iulib, 0.4, )
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
CPPFLAGS+=" -I/usr/include/ffmpeg"
AC_ARG_WITH(vidio,[  --without-vidio         disable video i/o support (using ffmpeg)],
    [ac_cv_use_vidio=$withval], [ac_cv_use_vidio=yes])
if test x$ac_cv_use_vidio != xno; then
    AC_CHECK_HEADER(libavutil/avutil.h, [], AC_SUBST(novidio, 1))
    AC_CHECK_HEADER(libavcodec/avcodec.h, [], AC_SUBST(novidio, 1), [#include <libavutil/avutil.h>])
    AC_CHECK_HEADER(libavformat/avformat.h,,AC_SUBST(novidio, 1))
    AC_TRY_COMPILE([#include <libavformat/avio.h>
                    #include <libavformat/avformat.h>],
                   [AVFormatContext fc; url_fclose(fc.pb);],
                   [], [AC_DEFINE(HAVE_OLD_AVFORMAT)])
else
    AC_SUBST(novidio, 1)
fi
AM_CONDITIONAL([have_vidio], [test "$novidio" != 1])

AC_SUBST(nov4l2, 0)
AC_ARG_WITH(v4l2,[  --without-v4l2          disable v4l2 support],
    [ac_cv_use_v4l2=$withval], [ac_cv_use_v4l2=yes])
if test x$ac_cv_use_v4l2 != xno; then
    AC_CHECK_HEADER(linux/videodev2.h,,AC_SUBST(nov4l2, 1))
else
    AC_SUBST(nov4l2, 1)
fi
AM_CONDITIONAL([have_v4l2], [test "$nov4l2" != 1])

AC_TYPE_INT64_T
AC_TYPE_PID_T

AC_FUNC_FORK
AC_CHECK_FUNCS([dup2 memset sqrt strchr strdup strrchr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

if [[ "$novidio" != "1" ]]; then
    use_vidio="yes"
else
    use_vidio="no"
fi

if [[ "$nov4l2" != "1" ]]; then
    use_v4l2="yes"
else
    use_v4l2="no"
fi

echo
echo "IULIB configuration:"
echo "  SDL (debugging):   $ac_cv_use_sdl"
echo "  Video IO:          $use_vidio"
echo "  Video for Linux 2: $use_v4l2" 
echo
echo "OK! You can build and install iulib the usual way:"
echo 
echo "    make"
echo "    sudo make install"
echo
