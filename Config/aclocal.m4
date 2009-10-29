#
#  Local macro definitions for AUTOCONF
#
#  © Vestris Inc., Geneva, Switzerland
#  http://www.vestris.com, 1994-1999 All Rights Reserved
#  _____________________________________________________
#
#  written by Daniel Doubrovkine - dblock@vestris.com
#

# 
# defines _OS_* for current platform and multithread settings
#

define(AC_CHECK_PLATFORM_BASELIB,[dnl
  AC_DEFINE(_REENTRANT)
  AC_DEFINE(_POSIX_PTHREAD_SEMANTICS)
  AC_DEFINE(_THREAD_SAFE)
  baselib_platform=[`uname | cut -f1 -d'/' | sed -e 's/[^a-zA-Z0-9_]//g'`]  
  BASEOS=$baselib_platform
  AC_SUBST(BASEOS)
  case $PLATFORM in    
    *-*-irix* ) 
#     AC_DEFINE(_SGIAPI) 
     AC_DEFINE(_SGI_MP_SOURCE)
     AC_DEFINE(_BSD_TYPES)
     AC_DEFINE(_POSIX_C_SOURCE,199506L)
     ;;
    *-*-bsd* )
     AC_DEFINE(_POSIX_C_SOURCE,199506L)
     ;;    
  esac
  echo "operating system: $BASEOS"
  BASEPROC=[`echo $PLATFORM | cut -f1 -d'-' | sed -e 's/i[0-9]86/x86/g'`]
  echo "processor: $BASEPROC"
  AC_SUBST(BASEPROC)
])dnl

#
# check current platform (real platform with revision, not operating system
#

define(AC_CHECK_PLATFORM,[dnl
  # autoconf sets $noopt
  msg="checking platform... "
  case ".$nonopt" in
	.NONE ) PLATFORM=`${CONFIG_SHELL-/bin/sh} ./config.guess` ;;
	* ) PLATFORM="$nonopt" ;;
  esac  
  PLATFORM=`${CONFIG_SHELL-/bin/sh} ./config.sub $PLATFORM`
  AC_SUBST(PLATFORM)
  msg="$msg$PLATFORM"
  AC_MSG_RESULT([$msg])
  AC_CHECK_PLATFORM_BASELIB
])dnl

define(AC_SET_PATH_SEPARATOR,[dnl
# Find the correct PATH separator.  Usually this is `:', but
# DJGPP uses `;' like DOS.
if test "X${PATH_SEPARATOR+set}" != "Xset"; then
  UNAME=${UNAME-`uname 2>/dev/null`}
  case X$UNAME in
	*-DOS) PATH_SEPARATOR=';' ;;
	*) PATH_SEPARATOR=':' ;;
  esac
fi
])dnl

# 
# check for egcs
#

define(AC_EGCS_CHECK, [dnl
  AC_MSG_CHECKING([whether $CC is egcs]) 
  AC_CACHE_VAL(ac_cv_compiler_egcs,  [dnl
  if test "X`$CC --version | cut -f1 -d'-'`" = "Xegcs"; then
	 ac_cv_compiler_egcs=yes
  else 
	 ac_cv_compiler_egcs=no
  fi  
  ])
  AC_MSG_RESULT([$ac_cv_compiler_egcs])
]) dnl

#
# check for g++ and reduce to gcc if not available
#

define(AC_GCC_CHECK, [dnl 
  AC_SET_PATH_SEPARATOR
  # If CC is not set, then try to find g++ or a usable CC.
  if test -z "$CC"; then
	echo $ac_n "checking for g++... $ac_c" 1>&6
	IFS="${IFS= }"; save_ifs="$IFS"; IFS="${IFS}${PATH_SEPARATOR}"
	for dir in $PATH; do  
	  test -z "$dir" && dir=.
	  if test -f $dir/g++; then
	CC="$dir/g++"
	break
	  elif test -f $dir/gcc; then
	CC="$dir/gcc"
	  fi  
	done
	IFS="$save_ifs"
	if test -n "$CC"; then
	  echo "$ac_t$CC" 1>&6
	else
	  echo "$ac_t"no 1>&6
	fi
  fi
  GCC=$CC
  AC_SUBST(GCC)
])dnl

#
# check for GNU make and reduce to gmake if available
#

define(AC_MAKE_CHECK, [dnl 
  AC_SET_PATH_SEPARATOR
  # If MAKE is not set, then try to find make or a usable gmake
  if test -z "$MAKE"; then
	echo $ac_n "checking for make... $ac_c" 1>&6
	IFS="${IFS= }"; save_ifs="$IFS"; IFS="${IFS}${PATH_SEPARATOR}"
	for dir in $PATH; do
	test -z "$dir" && dir=.
	if test -f $dir/make && (`$dir/make --version | grep GNU > /dev/null`); then 
		MAKE="$dir/make"
		break
	elif test -f $dir/gmake && (`$dir/gmake --version | grep GNU > /dev/null`); then 
		MAKE="$dir/gmake"
	fi
	done
	IFS="$save_ifs"
	if test -n "$MAKE"; then
	  echo "$ac_t$MAKE" 1>&6
	else
	  echo "$ac_t"no 1>&6
	fi
  fi  
  AC_SUBST(MAKE)
])dnl

#
# check whether g++ supports a particular set of options
#

define(AC_GCC_SUPPORTS_OPTIONS,[dnl
  for option in $1; do 
	AC_GCC_SUPPORTS_OPTION($option)   
  done  
])dnl

#
# check whether g++ supports a particular option (first from the list)
#

define(AC_GCC_SUPPORTS_OPTIONS_EXCLUSIVE,[dnl  
  for option in $1; do   
	AC_GCC_SUPPORTS_OPTION($option)
	if (echo "$CPPFLAGS" | grep $option > /dev/null) then		
		break;
	fi	
  done  
])dnl

#
# check whether g++ supports a particular option
#

define(AC_GCC_SUPPORTS_OPTION, [dnl  
  ac_cached_name=[`echo $1 | sed 's/-/_/g'`]
  eval compiler_option='$'ac_cv_compile_$ac_cached_name
  AC_MSG_CHECKING([whether $CC supports -$1])
  AC_CACHE_VAL(ac_cv_compile_$ac_cached_name,[ 
  save_CFLAGS=$CFLAGS
  ac_compile='g++ -c $CFLAGS $CPPFLAGS conftest.c 1>&5' 
  echo "int some_variable = 0;" > conftest.c
  CFLAGS="$CFLAGS -$1 -c conftest.c"
  echo "$progname:914: checking if g++ supports -$1" >&5
  if { (eval echo $progname:915: \"$ac_compile\") 1>&5; (eval $ac_compile) 2>conftest.err; } && test -s conftest.o; then
	# The compiler can only warn and ignore the option if not recognized
	# So say no if there are warnings
	  if test -s conftest.err; then
	compiler_option=no
	  else
	compiler_option=yes
	  fi
  else
	# Append any errors to the config.log.
	cat conftest.err 1>&5
	compiler_option=no
  fi
  rm conftest*
  eval ac_cv_compile_$ac_cached_name=$compiler_option
  CFLAGS=$save_CFLAGS
 ])
 if test "$compiler_option" = "yes"; then
	CPPFLAGS="$CPPFLAGS -$1"  
 fi
 AC_MSG_RESULT([$compiler_option])
])dnl

#
# check whether a certain function links (warnings are tolerated)
#

define(AC_GCC_LINKS,[dnl
 AC_MSG_CHECKING([$4])
 AC_CACHE_VAL(ac_cv_link_$3,[
 cross_compile=no
 AC_TRY_LINK([
  $1
  ],[
  $2
  ],
  ac_cv_link_$3=yes
  ,
  ac_cv_link_$3=no
  )]dnl
 )
 if test "$ac_cv_link_$3" = "yes"; then 
	AC_DEFINE($3)
	$3=yes
 fi
 AC_MSG_RESULT([$ac_cv_link_$3])
])dnl


#
# check whether a certain function compiles (warnings are tolerated)
#

define(AC_GCC_COMPILES,[dnl
AC_MSG_CHECKING([$4])
AC_CACHE_VAL(ac_cv_compile_$3,[ 
 cross_compile=no
 AC_TRY_COMPILE([
  $1 
  ],[
  $2
  ], 
  ac_cv_compile_$3=yes
  ,
  ac_cv_compile_$3=no
  )]dnl
 )
 if test "$ac_cv_compile_$3" = "yes"; then 
	AC_DEFINE($3)	
	$3=yes
 fi
 AC_MSG_RESULT([$ac_cv_compile_$3])
])dnl
 
#
# check for a static lib(s)
#

define(AC_CHECK_STATIC_LIBS,[dnl
  for library in $1; do 
	AC_CHECK_STATIC_LIB($library)   
  done  
])dnl

define(AC_CHECK_STATIC_LIB_FORCE, [dnl 
  AC_MSG_CHECKING([for $1])
  ac_cached_name=[`echo $1 | sed 's/[-.]/_/g'`]
  ac_link_location="(no -static) no"
  if test -n "`echo "$CPPFLAGS" | grep static`" || test "$2" = "force"; then
        ac_link_location="no"
        ac_dirs="/usr/lib:/usr/local/lib:/usr/libexec:$HOME/lib:$LD_LIBRARY_PATH:$PATH"
        ac_dirs=[`echo "$ac_dirs" | sed 's/:/ /g'`]
        for ac_dir in $ac_dirs; do 
                test -z "$ac_dir" && ac_dir=.
                if test -f $ac_dir/$1; then
                        ac_link_location="$ac_dir/$1"
                        STATIC_LIBS="$STATIC_LIBS $ac_link_location"                        
                        break
                fi
        done
  fi
  AC_MSG_RESULT($ac_link_location)
  AC_SUBST(STATIC_LIBS)
])dnl

define(AC_CHECK_STATIC_LIB, [dnl
  AC_CHECK_STATIC_LIB_FORCE($1, "")
])dnl
