#!/usr/bin/env bash

CSA_SOURCE=$1
CSA_DEST=src/csa/
EXPECTED_FILE_COUNT=6

# make a directory for the source
rm -rf $CSA_DEST
mkdir $CSA_DEST
echo "Created directory '$CSA_DEST'"

# copy over all relevant files (including copies via symlinks)
BASEFILES="refine.c update_epsilon.c stack.c timer.c"
HEADERS="csa_types.h csa_defs.h"
for FILE in $BASEFILES $HEADERS; do
	cp $CSA_SOURCE/prec_costs/$FILE $CSA_DEST
done
echo "Copied files from $CSA_SOURCE to $CSA_DEST"

TOTAL_FILE_COUNT=`ls $CSA_DEST | wc -l | xargs echo -n`
if [ $TOTAL_FILE_COUNT != $EXPECTED_FILE_COUNT ]; then
	echo "There should be $EXPECTED_FILE_COUNT files, but there are $TOTAL_FILE_COUNT."
fi

# replace common strings
perl -i -p -e's/(queue|stack)/csa_$1/g' $CSA_DEST/*
echo "Replaced common symbols"

# adding QUICK_MIN to the top of all files
for FILE in `find $CSA_DEST -type f`; do
	echo '#define QUICK_MIN' | cat - $FILE > temp && mv temp $FILE
done
echo "Defined QUICK_MIN"