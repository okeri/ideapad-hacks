# ideapad-hacks
Yoga 920 fn-lock and conservation mode acpi platform driver

[b]Building[/b]
1. Be sure actual kernel headers installed
2. Clone repo, change directory and type "make"

[b]Running[/b]
1. "insmod ideapad-hacks.ko"
2. find /sys -name "fn_lock"
3. In my case find says /sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/VPC2004:00/fn_lock
4. sudo echo 0 > /sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/VPC2004:00/fn_lock