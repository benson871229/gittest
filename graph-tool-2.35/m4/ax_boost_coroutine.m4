# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_boost_coroutine.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_COROUTINE
#
# DESCRIPTION
#
#   Test for Coroutine library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation is
#   available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_COROUTINE_LIB)
#
#   And sets:
#
#     HAVE_BOOST_COROUTINE
#
# LICENSE
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Michael Tindal
#   Copyright (c) 2013 Daniel Casimiro <dan.casimiro@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_BOOST_COROUTINE],
[
	AC_ARG_WITH([boost-coroutine],
		AS_HELP_STRING([--with-boost-coroutine@<:@=special-lib@:>@],
		[use the Coroutine library from boost - it is possible to specify a certain library for the linker
			e.g. --with-boost-coroutine=boost_coroutine-gcc-mt ]), [
		if test "$withval" = "no"; then
			want_boost="no"
		elif test "$withval" = "yes"; then
			want_boost="yes"
			ax_boost_user_coroutine_lib=""
		else
			want_boost="yes"
			ax_boost_user_coroutine_lib="$withval"
		fi
		], [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
		AC_REQUIRE([AC_PROG_CC])
		AC_REQUIRE([AC_CANONICAL_BUILD])

		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

		AC_CACHE_CHECK(whether the Boost::Coroutine library is available,
			ax_cv_boost_coroutine,
			[AC_LANG_PUSH([C++])
			CXXFLAGS_SAVE=$CXXFLAGS

			AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
				[[@%:@include <boost/range.hpp>]
                                 [@%:@include <boost/coroutine/all.hpp>]],
				[[boost::coroutines::coroutine< void() > f;]])],
				ax_cv_boost_coroutine=yes, ax_cv_boost_coroutine=no)
				CXXFLAGS=$CXXFLAGS_SAVE
			AC_LANG_POP([C++])
		])

		if test "x$ax_cv_boost_coroutine" = "xyes"; then
			AC_SUBST(BOOST_CPPFLAGS)

			AC_DEFINE(HAVE_BOOST_COROUTINE,,[define if the Boost::Coroutine library is available])
			BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

			if test "x$ax_boost_user_coroutine_lib" = "x"; then
				for libextension in `ls $BOOSTLIBDIR/libboost_coroutine*.so* $BOOSTLIBDIR/libboost_coroutine*.dylib* $BOOSTLIBDIR/libboost_coroutine*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_coroutine.*\)\.so.*$;\1;' -e 's;^lib\(boost_coroutine.*\)\.dylib.*$;\1;' -e 's;^lib\(boost_coroutine.*\)\.a.*$;\1;'` ; do
					ax_lib=${libextension}
					AC_CHECK_LIB($ax_lib, exit,
						[BOOST_COROUTINE_LIB="-l$ax_lib"; AC_SUBST(BOOST_COROUTINE_LIB) link_coroutine="yes"; break],
					[link_coroutine="no"])
				done

				if test "x$link_coroutine" != "xyes"; then
					for libextension in `ls $BOOSTLIBDIR/boost_coroutine*.dll* $BOOSTLIBDIR/boost_coroutine*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_coroutine.*\)\.dll.*$;\1;' -e 's;^\(boost_coroutine.*\)\.a.*$;\1;'` ; do
						ax_lib=${libextension}
						AC_CHECK_LIB($ax_lib, exit,
							[BOOST_COROUTINE_LIB="-l$ax_lib"; AC_SUBST(BOOST_COROUTINE_LIB) link_coroutine="yes"; break],
							[link_coroutine="no"])
					done
				fi

			else
				for ax_lib in $ax_boost_user_coroutine_lib boost_coroutine-$ax_boost_user_coroutine_lib; do
					AC_CHECK_LIB($ax_lib, exit,
						[BOOST_COROUTINE_LIB="-l$ax_lib"; AC_SUBST(BOOST_COROUTINE_LIB) link_coroutine="yes"; break],
						[link_coroutine="no"])
				done
			fi

			if test "x$ax_lib" = "x"; then
				AC_MSG_ERROR(Could not find a version of the Boost::Coroutine library!)
			fi

			if test "x$link_coroutine" = "xno"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
			fi
		fi

		CPPFLAGS="$CPPFLAGS_SAVED"
		LDFLAGS="$LDFLAGS_SAVED"
	fi
])
