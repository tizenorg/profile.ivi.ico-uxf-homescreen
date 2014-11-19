#!/bin/sh

########################
#
# Setting value
#
########################
# directory to put result
log_dir="./result"
# test log tag
test_tag="TestCase"

# log file name
date_str=`date '+%Y%m%d'`
time_str=`date '+%H%M'`
file_str="${date_str}_${time_str}.txt"

# sample app dir
app_dir="/opt/usr/apps/test.ico.res.app"

########################
#
# Initialize
#
########################
cd ../../../
curpath=`pwd`;
export LD_LIBRARY_PATH=$curpath/lib/apps-framework/.libs:$LD_LIBRARY_PATH
#/usr/bin/launchpad_preloading_preinitializing_daemon &
cd ./tests/apps-framework/

# set library path & reboot launchpad
pids=(`ps -ef | grep launchpad | grep -v grep | awk '{ print $2 }'`)
for pid in ${pids[*]}
do
	kill -9 ${pid}
done

# setting for appresctl test
cp ./data/* /opt/share/applications
if [ ! -e ${app_dir} ]; then
	mkdir ${app_dir}
	mkdir ${app_dir}/bin
fi
cp tst_appresctl ${app_dir}/bin/

# make directory to put result
if [ ! -e ${log_dir} ]; then
	mkdir ${log_dir}
fi

########################
#
# Function
#
########################
print_result()
{
	local l_test="$1"
	local l_log="$2"

	# count OK/NG
	l_cnt_ok=`grep ${test_tag} ${l_log} | grep " OK" | wc -l`
	l_cnt_ng=`grep ${test_tag} ${l_log} | grep " NG" | wc -l`
	echo "## ${l_test}  OK: ${l_cnt_ok}, NG: ${l_cnt_ng}"
}

kill_server()
{
    pids=(`ps -ef | grep tst_server | grep -v grep | awk '{ print $2 }'`)
    for pid in ${pids[*]}
    do
	    kill -9 ${pid}
    done
    sleep 1
}

########################
#
# Test
#
########################
# kill test server
kill_server

echo "===== System Controller API Test ====="
log_server="${log_dir}/server_${file_str}"
./tst_server >> ${log_server}&
sleep 2

log_common="${log_dir}/common_${file_str}"
./tst_common 2>&1 | tee ${log_common}
sleep 1

log_winctl="${log_dir}/winctl_${file_str}"
./tst_winctl 2>&1 | tee ${log_winctl}
sleep 1

log_inputctl="${log_dir}/inputctl_${file_str}"
./tst_inputctl 2>&1 | tee ${log_inputctl}
sleep 1

log_userctl="${log_dir}/userctl_${file_str}"
./tst_userctl 2>&1 | tee ${log_userctl}
sleep 1

log_appresctl="${log_dir}/appresctl_${file_str}"
./tst_appresctl_main -add
${app_dir}/bin/tst_appresctl 2>&1 | tee ${log_appresctl}
./tst_appresctl_main -del
sleep 2

echo ""
echo ""
echo "<<Results of System Controller API Test>>"
print_result "Common          API" ${log_common}
print_result "WindowControl   API" ${log_winctl}
print_result "InputControl    API" ${log_inputctl}
print_result "UserControl     API" ${log_userctl}
print_result "ResourceControl API" ${log_appresctl}
echo ""
echo ""

# kill test server
kill_server

# remove test application
rm -fr /opt/share/applications/test.ico.*
rm -fr /opt/apps/test.ico.res.app
