#!/bin/bash
#
# example of BASH script that will be used to (partially) test MP1 --V.3
#
### Correction of small problems in original code are marked with '###' -- V.2
# V.3 corrects a NOT SO SMALL problem: in the previous version, xmod run over the $DIR changed by chmod!
#
LOGDIR="/tmp"
PROGCH="chmod"

case $# in
	0 | 1 | 2) echo; echo "Usage: $0 <xmod_executable | xxx> <dir2test> <file2test_in_dir2test>" ; echo ; exit 1 ;;
	*) PROGX="$1" ; DIR="$2" ; FILE="$3" ;;
esac

# backup copy
mkdir $LOGDIR/backupMP1
rm -rf $LOGDIR/backupMP1/`basename $DIR`
cp -rp --remove-destination $DIR $LOGDIR/backupMP1/`basename $DIR`

# enforce some useful permissions
$PROGCH 444 $FILE
$PROGCH 755 $DIR

# save initial permissions of $FILE $DIR (can be useful...)
TESTN=0
stat --printf="%A\t%n\n" $FILE > $LOGDIR/perm.`basename $FILE`.$TESTN
find $DIR -exec stat --printf="%a\t%n\n" '{}' \; 2> /dev/null | sort -k 2  > $LOGDIR/perm.`basename $DIR`.$TESTN
# find $DIR -maxdepth 1 -exec stat --printf="%a\t%n\n" '{}' \; 2> /dev/null | sort -k 2  > $LOGDIR/perm.`basename $DIR`.$TESTN

# example of tests
ARGS1="0757 $FILE"
ARGS2="a-w $DIR"
ARGS3="-c g=x $FILE"
ARGS4="-c u+r $DIR"
ARGS5="-v u+r $FILE"
### ARGS6="-R 777 $DIR"	# Really, chmod also accepts octals w/o leading '0'
ARGS6="-R 0777 $DIR"

# sequence of tests for CHMOD
for TESTN in 1 2 3 4 5 6
do
	eval ARGS=\${ARGS$TESTN}
	$PROGCH $ARGS | sort -b > $LOGDIR/log.$PROGCH.$TESTN.sorted
done

# reset original dir/files
rm -rf $DIR
cp -rp --remove-destination $LOGDIR/backupMP1/`basename $DIR` $DIR

# sequence of tests for XMOD
if [ "$PROGX" != "xxx" ]
then
	for TESTN in 1 2 3 4 5 6
	do
		eval ARGS=\${ARGS$TESTN}
		$PROGX $ARGS | sort -b > $LOGDIR/log.$PROGX.$TESTN.sorted

		diff -b $LOGDIR/log.$PROGX.$TESTN.sorted $LOGDIR/log.$PROGCH.$TESTN.sorted
	done
fi

# reset original dir/files
rm -rf $DIR
cp -rp --remove-destination $LOGDIR/backupMP1/`basename $DIR` $DIR

exit
