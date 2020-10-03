#!/bin/sh

RESULTS=dwt_junit.xml

cat $RESULTS | sed -e "s/\x1b\[.\{1,5\}m//g" > $RESULTS~
mv $RESULTS~ $RESULTS
