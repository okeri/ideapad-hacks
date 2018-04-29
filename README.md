# ideapad-hacks
Yoga 920 fn-lock and conservation mode acpi platform driver

##Building
1. Be sure actual kernel headers installed
2. Clone repo, change directory and type **make**

##Running
1. **sudo insmod ideapad-hacks.ko**
2. **sudo find /sys -name "fn_lock"**
3. In my case find says /sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/VPC2004:00/fn_lock
4. Check current fn_lock value: **sudo cat /sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/VPC2004:00/fn_lock**
5. Write new setting **sudo echo 0 > /sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/VPC2004:00/fn_lock**
