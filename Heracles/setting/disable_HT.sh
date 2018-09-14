for i in {14..27}
do
	su -c "/bin/echo 0 > /sys/devices/system/cpu/cpu"$i"/online"
done
