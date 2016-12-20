#!/bin/sh


while [ 1 ]; do
	python2 ./speech2text.py &
	sleep 2
	LASTPID=$!

	ABS_DIFF=0
	until [  $ABS_DIFF -gt 15 ]; do
			NOW_SECONDS=$(date +"%-S")
			SECONDS_LAST_CALLBACK=$(head -n 1 ./last_callback.txt)
			DIFF=$((NOW_SECONDS-SECONDS_LAST_CALLBACK))
			ABS_DIFF=${DIFF#-}
			#echo $NOW_SECONDS
			#echo $SECONDS_LAST_CALLBACK
			#echo $DIFF
			#echo $ABS_DIFF
			test $? -gt 128 && break;
			sleep 5
	done
	echo "Speech 2 Text blocked, program needs to be restarted..."

	sleep 3;
	kill $LASTPID

	test $? -gt 128 && break;
done

# cleanup
kill $LASTPID
