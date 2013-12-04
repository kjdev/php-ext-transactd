dnl config.m4 for extension transactd

dnl Check PHP version:
AC_MSG_CHECKING(PHP version)
if test ! -z "$phpincludedir"; then
    PHP_VERSION=`grep 'PHP_VERSION ' $phpincludedir/main/php_version.h | sed -e 's/.*"\([[0-9\.]]*\)".*/\1/g' 2>/dev/null`
elif test ! -z "$PHP_CONFIG"; then
    PHP_VERSION=`$PHP_CONFIG --version 2>/dev/null`
fi

if test x"$PHP_VERSION" = "x"; then
    AC_MSG_WARN([none])
else
    PHP_MAJOR_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\1/g' 2>/dev/null`
    PHP_MINOR_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\2/g' 2>/dev/null`
    PHP_RELEASE_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\3/g' 2>/dev/null`
    AC_MSG_RESULT([$PHP_VERSION])
fi

if test $PHP_MAJOR_VERSION -lt 5; then
    AC_MSG_ERROR([need at least PHP 5 or newer])
fi

dnl Transactd Extension
PHP_ARG_ENABLE(transactd, whether to enable transactd support,
[  --enable-transactd      Enable transactd support])

if test "$PHP_TRANSACTD" != "no"; then

    dnl compiler C++:
    PHP_REQUIRE_CXX()

    dnl Check for Transactd header
    PHP_ARG_WITH(transactd-includedir, for transactd header,
    [ --with-transactd-includedir=DIR transactd header path], yes)

    if test "$PHP_TRANSACTD_INCLUDEDIR" != "no" && test "$PHP_TRANSACTD_INCLUDEDIR" != "yes"; then
        if test -r "$PHP_TRANSACTD_INCLUDEDIR/bzs/db/protocol/tdap/client/database.h"; then
            TRANSACTD_INCLUDES="$PHP_TRANSACTD_INCLUDEDIR"
        else
            AC_MSG_ERROR([Can't find transactd headers under "$PHP_TRANSACTD_INCLUDEDIR"])
        fi
    else
        SEARCH_PATH="/usr/local /usr"
        SEARCH_FOR="/include/transactd/bzs/db/protocol/tdap/client/database.h"
        if test -r $PHP_TRANSACTD/$SEARCH_FOR; then
            TRANSACTD_INCLUDES="$PHP_TRANSACTD/include/transactd"
        else
            AC_MSG_CHECKING([for transactd header files in default path])
            for i in $SEARCH_PATH ; do
                if test -r $i/$SEARCH_FOR; then
                    TRANSACTD_INCLUDES="$i/include/transactd"
                    AC_MSG_RESULT(found in $i)
                fi
            done
        fi
    fi

    if test -z "$TRANSACTD_INCLUDES"; then
        AC_MSG_RESULT([not found])
        AC_MSG_ERROR([Can't find transactd headers])
    fi

    PHP_ADD_INCLUDE($TRANSACTD_INCLUDES)
    PHP_ADD_INCLUDE($TRANSACTD_INCLUDES/linux)

    dnl Check for Transactd library
    PHP_ARG_WITH(transactd-libdir, for transactd library,
    [ --with-transactd-libdir=DIR transactd library path], yes)

    LIBNAME_C=tdclc_64
    LIBNAME_CXX=tdclcpp_64m
    AC_MSG_CHECKING([for transactd])
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    save_CPPFLAGS="$CPPFLAGS"
    transactd_CPPFLAGS="-I$TRANSACTD_INCLUDES -I$TRANSACTD_INCLUDES/linux"
    CPPFLAGS="$save_CPPFLAGS $transactd_CPPFLAGS"

    save_LDFLAGS="$LDFLAGS"
    transactd_LDFLAGS="-L$PHP_TRANSACTD_LIBDIR -l$LIBNAME_C -l$LIBNAME_CXX -ldl -lstdc++"
    LDFLAGS="$save_LDFLAGS $transactd_LDFLAGS"

    AC_TRY_LINK(
    [
        #include <stdio.h>
        #if defined(__linux__) & !defined(LINUX)
        #define LINUX 1
        #endif
        #include "$TRANSACTD_INCLUDES/bzs/db/protocol/tdap/client/database.h"
    ],[
        bzs::db::protocol::tdap::client::database::create();
    ],[
        AC_MSG_RESULT(yes)
        PHP_ADD_LIBRARY_WITH_PATH($LIBNAME_C, $PHP_TRANSACTD_LIBDIR, TRANSACTD_SHARED_LIBADD)
        PHP_ADD_LIBRARY_WITH_PATH($LIBNAME_CXX, $PHP_TRANSACTD_LIBDIR, TRANSACTD_SHARED_LIBADD)
        PHP_ADD_LIBRARY(stdc++, , TRANSACTD_SHARED_LIBADD)
        PHP_ADD_LIBRARY(dl, , TRANSACTD_SHARED_LIBADD)
        AC_DEFINE(HAVE_TRANSACTDLIB,1,[ ])
    ],[
        AC_MSG_RESULT([error])
        AC_MSG_ERROR([wrong transactd lib version or lib not found])
    ])
    CPPFLAGS="$save_CPPFLAGS"
    LDFLAGS="$save_LDFLAGS"
    AC_LANG_RESTORE

    PHP_SUBST(TRANSACTD_SHARED_LIBADD)

    dnl PHP Extension
    PHP_NEW_EXTENSION(transactd, transactd.cpp database.cpp dbdef.cpp tabledef.cpp fielddef.cpp keydef.cpp keysegment.cpp flags.cpp table.cpp exception.cpp, $ext_shared)
fi

dnl coverage
PHP_ARG_ENABLE(coverage, whether to enable coverage support,
[  --enable-coverage     Enable coverage support], no, no)

if test "$PHP_COVERAGE" != "no"; then
    EXTRA_CFLAGS="--coverage"
    PHP_SUBST(EXTRA_CFLAGS)
fi
