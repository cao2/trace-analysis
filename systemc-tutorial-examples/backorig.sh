#! /bin/sh

find . -name Makefile.orig -print | while read xmkfl ; do
    echo "Moving $xmkfl ..."
    mv $xmkfl `dirname $xmkfl`/`basename $xmkfl .orig`
done

