#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
AM_INIT_AUTOMAKE(openhpi-subagent, 0.1)
AC_CONFIG_SRCDIR([include/saHpiTable.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lopenhpi':
AC_CHECK_LIB([openhpi], [main])

# Checks for header files.
AC_CHECK_HEADERS([netinet/in.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_HEADER_STDC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([memset strncasecmp])

AC_CONFIG_FILES([Makefile 
    src/Makefile])
AC_OUTPUT
