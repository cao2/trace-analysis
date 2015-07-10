#! /bin/sh

# keep this space-separated
TARGETHOSTS="macosx sparcOS5 gccsparcOS5 linux cygwin hpux11 gcchpux11"

usage () {
    builtin echo "============================================================="
    builtin echo "Usage: $0 target-host systemc-path"
    builtin echo "target-host is one of:"
    builtin echo "{$TARGETHOSTS}"
    builtin echo "systemc-path is expected to contain: include/*, lib-.../libsystemc.a"
    builtin echo "Example: $0 macosx /opt/systemc2";
    builtin echo "============================================================="
    exit 1;
}

# $1 -- user-specified target host
# $2 -- user-specified systemc path
checkSafeArgs () {
    # verify target host in list
    if ! (echo " $TARGETHOSTS " | grep " $1 " >/dev/null) ; then
        builtin echo "Expecting target-arch to be in:"
        builtin echo "{$TARGETHOSTS}"
        exit 1
    fi
    # verify sysc path for "include" and library
    if ( ! test -d "$2/include" ) ; then
        echo "\"$2\" does not contain needed systemc \"include/\" directory."
        exit 1
    fi
    if ! (test -f "$2/lib-$1/libsystemc.a") ; then
        echo "\"$2\" does not contain needed systemc lib-whatever/ lib directory."
        exit 1
    fi;
    # args ok!
}

# $1 -- user-specified target host
# $2 -- user-specified systemc path
# $3 -- relative path to the file to edit
adaptMk () {
    NWFL=`mktemp sysc.XXX`
    # replacing "__TARGETHOST__" and "__SYSCPATH__"
    sed "s@__TARGETHOST__@$1@g" < $3 | sed "s@__SYSCPATH__@$2@g" > $NWFL
    if ( test $? -eq 0 ) ; then
        cp $3 ${3}.orig
        mv $NWFL $3
    else rm $NWFL ; fi
}

# $1 -- user-specified target host
# $2 -- user-specified systemc path
updateMakefiles () {
    find . -name 'Makefile' -type f | while read mkfile ; do
        echo "Updating $mkfile..."
        adaptMk $1 $2 $mkfile
    done;
}

# MAIN HERE

[ x$1 = "x" ] || test x$2 = "x" && usage   # empty OS or sysc-path
[ x$3 = "x" ] || usage     # too many args given

checkSafeArgs $1 $2      # verify target host and systemc path

updateMakefiles "$1" "$2"
