AC_PREREQ[(2.69)]
AC_INIT([cityinfo], [0.5.19], [parazyd@dyne.org])
AC_CONFIG_SRCDIR([src])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIRS([m4])

AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_LIBTOOL

PKG_CHECK_MODULES(CITYINFO, glib-2.0 gconf-2.0)
AC_SUBST(CITYINFO_CFLAGS)
AC_SUBST(CITYINFO_LIBS)

AC_OUTPUT([Makefile src/Makefile])
