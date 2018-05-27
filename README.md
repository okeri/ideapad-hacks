# ideapad-hacks
Yoga 920 fn-lock and conservation mode acpi platform driver. Actually it should be work on next models:
Yoga 700, Yoga 710, Yoga 720, Yoga 900, Yoga 910, Yoga 920, but this information needs confirmation.
Please NOTE: this code provided as is. You have to use it at your own risk.

## Building
1. Be sure actual kernel headers installed
2. Clone repo, change directory and type **make**

## Running
1. **sudo insmod ideapad-hacks.ko**
2. **sudo find /sys -name "fn_lock"**
3. In my case find says /sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/VPC2004:00/fn_lock
4. Check current fn_lock value: **sudo cat /sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/VPC2004:00/fn_lock**
5. Write new setting **sudo echo 0 > /sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/VPC2004:00/fn_lock**

## Known issues:
ideapad-hacks is not compatible with ideapad_laptop kernel module from linux kernel mainline. To use ideapad you **MUST** unload or blacklist ideapad_laptop. Ideapad-hacks is temporary solution and it will be deprecated as soon as fn_lock related patches will be reviewed and applyed by linux kernel mantainers.
