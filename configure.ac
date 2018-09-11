AC_INIT([NetworkRPiClock], [1.2.2])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_SUBST(target)

AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
LT_INIT

AC_CONFIG_SUBDIRS([restclient-cpp])


AC_CONFIG_MACRO_DIR([m4])
if test "x${prefix}" = "xNONE"; then
    prefix=/usr/local
fi
if test "x${exec_prefix}" = "xNONE"; then
    exec_prefix="${prefix}"
fi
dnl SOUNDLIB=${datadir}/${PACKAGE}/sounds dnl This does not work.
SOUNDLIB=${prefix}/share/${PACKAGE}/sounds
AC_DEFINE_UNQUOTED([SOUNDLIB],"$SOUNDLIB",[The location of the sounds])
AC_DEFINE([BASEURL],["http://sauron.deepsoft.com:8080/"],[The default baseURL])
                
BUILD_DIR=`pwd`
TOP_DIR=${BUILD_DIR}

AC_SUBST(TOP_DIR)
AC_SUBST(BUILD_DIR)

srcdir=`dirname $0`
cd $srcdir
SRC_DIR=`pwd`
cd $BUILD_DIR
AC_SUBST(SRC_DIR)
AC_PROG_CXX
AC_PATH_PROG([AR],ar)
dnl AC_PROG_RANLIB
AC_CHECK_LIB([curl], [curl_easy_setopt])
AC_CHECK_HEADERS([sys/types.h regex.h time.h unistd.h getopt.h])
PKG_CHECK_MODULES([CAIRO],[cairo])
PKG_CHECK_MODULES([PANGO_CAIRO],[pangocairo])
PKG_CHECK_MODULES([LIBXML2],[libxml-2.0])
PKG_CHECK_MODULES([ALSA],[alsa])
AX_PKG_DOXYGEN
AC_PATH_PROG(PDFLATEX,pdflatex,/bin/true)
AC_PATH_PROG(LATEX,latex,/bin/true)
AC_PATH_PROG(MAKEINDEX,makeindex,/bin/true)
AC_PATH_PROG(BIBTEX,bibtex,/bin/true)
AC_PATH_PROG(DVIPS,dvips,/bin/true)
AC_PATH_PROG(PS2PDF,ps2pdf,/bin/true)

AC_CONFIG_HEADER([config.h])

AC_CONFIG_FILES([src/Makefile src/Doxyfile sounds/Makefile Makefile Doxyfile])
AC_OUTPUT
