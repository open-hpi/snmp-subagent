#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
AM_INIT_AUTOMAKE(openhpi-subagent, 0.1)
AC_CONFIG_SRCDIR([include/saHpiTable.h])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

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

# Checks for libraries
# FIXME: Replace `main' with a function in `-lopenhpi':
AC_CHECK_LIB([openhpi], [saHpiInitialize], 
    [
        have_hpi=yes
        HPI_FLAGS=`pkg-config --cflags openhpi`
        HPI_LIBS=`pkg-config --libs openhpi`
    ],
    [OH_CHECK_FAIL(libopenhpi,openhpi-devel)])

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
	-Wcast-qual -Wcast-align "
#	-Werror"

CFLAGS="$CFLAGS $GLIB_CFLAGS $SNMPFLAGS $HPI_FLAGS $CC_WARNINGS"
if test "x$THREAD_SAFE" = "xyes" ; then
        CFLAGS="$CFLAGS -D_GNU_SOURCE -D_REENTRANT -fexceptions"
fi

LIBS="$LIBS $SNMPALIBS $HPI_LIBS -lm"
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
