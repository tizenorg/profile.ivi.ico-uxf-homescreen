#!/bin/sh

# Define
SCREEN_IS_FHD=1
GUI_TEST_NEXT_DAT=gui-test-FHD-next.dat
GUI_TEST_DAT=gui-test-FHD.dat
# for GUI test launch
LAUNCH_APP=org.tizen.music-player
LAUNCH_APP_SEAT=0
LAUNCH_APP_INDEX=0
LAUNCH_APP_SEATCNT=0
LAUNCH_APP_X=0
LAUNCH_APP_Y=0
SHIFT_X=896
SHIFT_Y=448

#determin screen size 768x1024 or 1080x1920
/bin/grep "ico_uxf_window_screen_size_get width=768 height=1024" $HOMESCREEN_LOG
if [ "$?" = 0 ]; then
	SCREEN_IS_FHD=0
	SHIFT_X=0
	SHIFT_Y=0
	GUI_TEST_NEXT_DAT=gui-test-XGA-next.dat
	GUI_TEST_DAT=gui-test-XGA.dat
fi

#determin launch app index/position on application list
for str in `/bin/grep "Application list: appid=$LAUNCH_APP" $ONSCREEN_LOG`
do
	para=`echo $str|cut -d "=" -f 1`
	val=`echo $str|cut -d "=" -f 2`
	if [ "$para" = "seat" ]; then
		LAUNCH_APP_SEAT=$val
	fi
	if [ "$para" = "idx" ]; then
		LAUNCH_APP_INDEX=$val
	fi
	if [ "$para" = "seatcnt" ]; then
		LAUNCH_APP_SEATCNT=$val
	fi
done

LAUNCH_APP_X=`expr $LAUNCH_APP_INDEX % 5 \* 130 + 124 + $SHIFT_X`
LAUNCH_APP_Y=`expr $LAUNCH_APP_INDEX / 5 \* 130 + 374 + $SHIFT_Y`

/bin/cp testdata/$GUI_TEST_DAT testdata/gui-test.dat

if [ $LAUNCH_APP_SEATCNT = 2 ]; then
	if [ $LAUNCH_APP_SEAT = 0 ]; then
		cat testdata/$GUI_TEST_NEXT_DAT >> testdata/gui-test.dat
	fi
elif [ $LAUNCH_APP_SEATCNT -gt 2 ]; then
	num=0
	if [ $LAUNCH_APP_SEAT = 0 ]; then
		num=`expr $LAUNCH_APP_SEATCNT - 1`
	elif [ $LAUNCH_APP_SEAT -gt 1 ]; then
		num=`expr $LAUNCH_APP_SEATCNT - $LAUNCH_APP_SEAT`
	fi
	for i in `seq 1 1 $num`; do
		cat testdata/$GUI_TEST_NEXT_DAT >> testdata/gui-test.dat
	done
fi

cat testdata/gui-test-launch-template.dat >> testdata/gui-test.dat
sed -i "s/COORD_X/$LAUNCH_APP_X/g" testdata/gui-test.dat
sed -i "s/COORD_Y/$LAUNCH_APP_Y/g" testdata/gui-test.dat


