#!/bin/sh

# XXX this should be a Makefile to facilitate parallel runs!

# note:  this script requires a modern POSIX shell with $(command substitution)
# and $((arithmetic + expressions))
#
# Finding a portable way to print output with or without a trailing newline is
# still more complex even with shells with modern features!
#
# always use ``$echo'' if any of the other variables are used...
#	$nl - print a newline (always required at end of line if desired)
#	$n - option to turn off final newline
#	$c - escape sequence to turn off final newline
# usage for a prompt is:
#	$echo $n "prompt: $c"
# and for a normal line
#	$echo "message$nl"
#
# Don't pretend to have print or printf if they are not builtin
#
HAVEPRINT=false ; export HAVEPRINT
if expr "`type print 2>/dev/null`" : '^print is a shell ' > /dev/null 2>&1 ; then
	HAVEPRINT=true
fi
HAVEPRINTF=false ; export HAVEPRINTF
if expr "`type printf 2>/dev/null`" : '^printf is a shell ' > /dev/null 2>&1 ; then
	HAVEPRINTF=true
fi
if ${HAVEPRINT} ; then
	#
	# XXX Ksh "print" is a horrible mess of unusability, but, if it is
	# builtin....
	#
	# XXX in theory "print -R", if available, is a better emulation of BSD
	# echo with '-n', but for $echo it doesn't really matter....
	#
	echo=print
	nl='\n'
	n='-n'
	# XXX in theory '\c' is equivalent of '-n' in all Ksh-compatible shells
	c=''
elif ${HAVEPRINTF} ; then
	echo=printf
	nl='\n'
	n=''
	c=''
	# for fun:
	portable_echo ()
	{
		[ "$1" = -n ] && { shift; FMT="%s"; } || FMT="%s\n"
		printf "$FMT" ${1+"${@}"}
	}
else
	# NOTE:  Assume if "echo" is builtin that it is OK and do not prefer an
	# external "echo" for $echo, even if that is more capable (though that
	# is not likely ever true).
	#
	echo=echo
	c=`echo "xyzzy\c" | sed 's/[^c]//g'`
	if test -n "${c}" ; then
		# BSD echo
		nl=''
		n='-n'
		c=''
	else
		# (SysVr2 or newer) Bourne Shell echo
		# (some may have -n, but ignore that as we know we have '\c')
		nl='\n'
		n=''
		c='\c'
	fi
	# n.b.:  ancient echo (V7) is not supportable for this use!
fi

DIFF_FLAGS="-u"
case "$(uname)" in
  *W32*)
    DIFF_FLAGS="-wu"
    ;;
esac

if [ -z "$testBin" ]; then
    testBin="$1"
fi

if [ -z "$testBin" ]; then
    if [ ! -x $testBin ] ; then
        testBin="./yajl_test"
        if [ ! -x $testBin ] ; then
            ${echo} "cannot execute test binary: '$testBin'${nl}"
            exit 1;
        fi
    fi
fi

${echo} "using test binary: $testBin${nl}"

testBinShort=$(basename $testBin)

testsSucceeded=0
testsTotal=0

for file in cases/*.json ; do
  allowComments=""
  allowGarbage=""
  allowMultiple=""
  allowPartials=""

  # if the filename starts with dc_, we disallow comments for this test
  case $(basename $file) in
    ac_*)
      allowComments="-c "
    ;;
    ag_*)
      allowGarbage="-g "
     ;;
    am_*)
     allowMultiple="-m ";
     ;;
    ap_*)
     allowPartials="-p ";
    ;;
  esac
  fileShort=$(basename $file)
  testName=$(echo $fileShort | sed -e 's/\.json$//')

  ${echo} $n " test ($testName): $c"
  iter=1
  success="SUCCESS"

  # ${echo} $n "${testBinShort} ${allowPartials}${allowComments}${allowGarbage}${allowMultiple}-b ${iter} < ${fileShort} > ${fileShort}.test : $c"

  # parse with a read buffer size ranging from 1-31 to stress stream parsing
  while [ $iter -lt 32  ] && [ $success = "SUCCESS" ] ; do
    $testBin $allowPartials $allowComments $allowGarbage $allowMultiple -b $iter < $file > ${file}.test  2>&1
    diff ${DIFF_FLAGS} ${file}.gold ${file}.test > ${file}.out
    if [ $? -eq 0 ] ; then
      if [ $iter -eq 31 ] ; then testsSucceeded=$(( $testsSucceeded + 1 )) ; fi
    else
      success="FAILURE"
      iter=32
      ${echo} "${nl}"
      cat ${file}.out
    fi
    iter=$(( iter + 1 ))
    rm ${file}.test ${file}.out
  done

  ${echo} "${success}${nl}"
  testsTotal=$(( testsTotal + 1 ))
done

${echo} "$testsSucceeded/$testsTotal tests successful${nl}"

if [ $testsSucceeded != $testsTotal ] ; then
  exit 1
fi

exit 0
