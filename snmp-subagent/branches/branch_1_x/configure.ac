#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.53)
AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
AM_INIT_AUTOMAKE(openhpi-subagent, 1.0.0)
AC_CONFIG_SRCDIR([include/saHpiTable.h])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_LIBTOOL_DLOPEN
AM_PROG_LIBTOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_HEADER_STDC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([memset strncasecmp])

# check for helper progs
AC_PATH_PROGS(RPM, rpmbuild)

# set up some vars
HPI_PKG=openhpi-subagent
AC_SUBST(HPI_PKG)

# Checks for header files.
AC_CHECK_HEADERS([netinet/in.h])

have_openhpi=no

AC_CHECK_PROG([found_pkg_config],[pkg-config],[yes])

if test "x$found_pkg_config" != "xyes"; then
    OH_CHECK_FAIL(pkg-config,pkg-config)
fi


GLIB_CFLAGS=`pkg-config --cflags glib`
GLIB_LIBS=`pkg-config --libs glib`


AC_ARG_ENABLE([client-library],
    [[  --enable-client-library       build with thin client library [default=no]]],
    [if test "x$enableval" = "xyes"; then
        OPENHPILIB="openhpiclient"
     else
        OPENHPILIB="openhpi"
     fi
    ],
    [OPENHPILIB="openhpi"])


HPI_LIBS=`pkg-config --libs $OPENHPILIB`
if test "x$HPI_LIBS" = "x"; then
    AC_MSG_ERROR([
*** OpenHPI pkgconfig file wasn't found
*** we can't determine proper build flags without it
*** please check your installation
])
fi

# uncomment the section below when libopenhpiutils
# has its own pkgconfig file 'openhpiutils.pc'
# HPIUTILS_LIBS=`pkg-config --libs openhpiutils`
# if test "x$HPIUTILS_LIBS" = "x"; then
#     AC_MSG_ERROR([
# *** OpenHpiUtils pkgconfig file wasn't found
# *** we can't determine proper build flags without it
# *** please check your installation
# ])
# fi


# Checks for libraries
# FIXME: Replace `main' with a function in `-lopenhpi':
AC_CHECK_LIB([$OPENHPILIB], [saHpiInitialize], 
    [
        have_hpi=yes
        HPI_FLAGS=`pkg-config --cflags $OPENHPILIB`
        HPI_LIBS=`pkg-config --libs $OPENHPILIB`
    ],
    [OH_CHECK_FAIL(lib$OPENHPILIB,openhpi-devel)])
    
AC_CHECK_LIB([openhpiutils], [build_reading_strings], 
    [
        have_hpiutils=yes
# uncomment the 2 lines below when openhpiutils
# has its own pkgconfig file 'openhpiutils.pc'
#        HPIUTILS_FLAGS=`pkg-config --cflags openhpiutils`
#        HPIUTILS_LIBS=`pkg-config --libs openhpiutils`
    ],
    [OH_CHECK_FAIL(libopenhpiutils,openhpiutils-devel)])


OH_CHECK_NETSNMP

AC_SUBST(SNMPCONFDIR)

dnl 
dnl This code was borrowed from linux-ha configure.in file
dnl and is used for creating the proper substitutions
dnl in the spec file. Without it, the spec file will
dnl have ${exec_prefix}, etc. instead of the actual
dnl directory.
dnl
dnl

prefix=`eval echo "$prefix"`

case $prefix in
  NONE) prefix=/usr/local;;
esac
var() {
  case $1 in
    *'${'*)     res=`eval echo "$1"`;;
    *)          res="$1";;
  esac
  case "$res" in
    ""|NONE)    echo "$2";;
    *)          echo "$res";;
  esac
}

exec_prefix=`var "$exec_prefix" "$prefix"`
bindir=`var "$bindir" "$exec_prefix/bin"`
sbindir=`var "$sbindir" "$exec_prefix/sbin"`
libdir=`var "$libdir" "$exec_prefix/lib"`
includedir=`var "$includedir" "$exec_prefix/include"`
base_includedir="${includedir}"
AC_SUBST(base_includedir)

AC_C_STRINGIZE

CC_WARNINGS="-Wall -Wmissing-prototypes \
	-Wmissing-declarations \
	-Wstrict-prototypes \
	-Wpointer-arith \
#	-fprofile-arcs -ftest-coverage \
	-Wcast-qual -Wcast-align "
#	-Werror"

CFLAGS="$CFLAGS $GLIB_CFLAGS $SNMPFLAGS $HPI_FLAGS $HPIUTILS_FLAGS $CC_WARNINGS"
if test "x$THREAD_SAFE" = "xyes" ; then
        CFLAGS="$CFLAGS -D_GNU_SOURCE -D_REENTRANT -fexceptions"
fi

LIBS="$LIBS $GLIB_LIBS $SNMPALIBS $HPI_LIBS $HPIUTILS_LIBS -lm"
if test "x$THREAD_SAFE" = "xyes" ; then
        LIBS="$LIBS -lpthread"
fi

# pass this down to the spec file
AC_SUBST(ac_configure_args)

AC_CONFIG_FILES([Makefile 
    src/Makefile
    src/t/Makefile
    docs/Makefile
    openhpi-subagent.spec])
AC_OUTPUT
