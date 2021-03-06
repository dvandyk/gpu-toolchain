dnl vim: set sw=4 sts=4 ts=4 noet ft=config foldmethod=marker foldmarker={{{,}}} :

AC_INIT([gpu-toolchain], [0.0])
AC_PREREQ(2.5)
AC_CONFIG_SRCDIR([])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(1.9)
AC_LANG([C++])

dnl {{{ basic toolchain checks
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
dnl }}} 

dnl {{{ check for a sensible C++ compiler
dnl {{{ check for partial specialisation of class templates
AC_MSG_CHECKING([for compiler support for partial specialisation of template classes])
AC_COMPILE_IFELSE([
template <typename T_>
struct S
{
	static const int s = 0;
};

template <typename T_>
struct S<const T_>
{
};

template <typename T_>
struct S<const T_ &>
{
	static const int t = 0;
};

int main(int, char **)
{
	return S<int &>::s + S<const int &>::t;
}
],
		[AC_MSG_RESULT([yes])],
		[
		 AC_MSG_RESULT([no])
		 AC_MSG_ERROR([Your C++ compiler doesn't compile C++ code])
		])

dnl {{{ check for tr1 shared pointers
AC_MSG_CHECKING([for std::tr1::shared_ptr<>])
AC_COMPILE_IFELSE([
#include <tr1/memory>

int main(int, char **)
{
	std::tr1::shared_ptr<int> x(new int(10));
	return *x;
}
],
		[AC_MSG_RESULT([yes])],
		[
		 AC_MSG_RESULT([no])
		 AC_MSG_ERROR([Your C++ Standard Library doesn't support std::tr1::shared_ptr<>.])
		])
dnl }}}

dnl {{{ check for numeric limits
AC_MSG_CHECKING([for std::numeric_limits<>])
AC_COMPILE_IFELSE([
#include <limits>
int main(int, char **)
{
	int result(std::numeric_limits<float>::digits >> 3);
	result += (std::numeric_limits<double>::digits >> 3);

	return result;
}
],
	[AC_MSG_RESULT([yes])],
	[
	 AC_MSG_RESULT([no])
	 AC_MSG_ERROR([Your C++ Standard Library doesn't support std::numeric_limits<>.])
	])
dnl }}}
dnl }}}
dnl }}}

dnl {{{ set up definitions
dnl {{{ version string
if test -d "${GIT_DIR:-${ac_top_srcdir:-./}/.git}" ; then
	GPU_VERSION=`git describe 2>/dev/null`
	if test -z ${GPU_VERSION} ; then
			GPU_VERSION=`git rev-parse HEAD`
	fi
	if test -n "`git diff-index -m --name-only HEAD`" ; then
			GPU_VERSION=${GPU_VERSION}-dirty
	fi
else
	GPU_VERSION="unknown"
fi
AC_SUBST([GPU_VERSION])
dnl }}}
dnl }}}

dnl output
AM_CONFIG_HEADER(config.h)
AC_OUTPUT(
	Makefile
	common/Makefile
	elf/Makefile
	r6xx/Makefile
	tests/Makefile
	utils/Makefile
)
