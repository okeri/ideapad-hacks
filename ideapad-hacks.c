/*
 *  ideapad-hacks.c - Lenovo Yoga 920 features(may be 910 710, 720 works as well)
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301, USA.
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/acpi.h>
#include <linux/platform_device.h>
#include <linux/device.h>


#define BM_CONSERVATION_BIT (5)
#define EC_FNLOCK_BIT  (10)

enum {
	BMCMD_CONSERVATION_ON = 3,
	BMCMD_CONSERVATION_OFF = 5,
        HCMD_FNLOCK_ON = 0xe,
        HCMD_FNLOCK_OFF = 0xf

};


struct ideapad {
	struct acpi_device *adev;
	struct platform_device *platform_device;
};


static int read_method_int(acpi_handle handle, const char *method, unsigned long *val)
{
	acpi_status status;
	unsigned long long result;

	status = acpi_evaluate_integer(handle, (char *)method, NULL, &result);
	if (ACPI_FAILURE(status)) {
		*val = -1;
		return -1;
	}
	*val = result;
	return 0;
}

static int write_method_int(acpi_handle handle, char *method, int cmd)
{
	acpi_status status;
	status = acpi_execute_simple_method(handle, method, cmd);
	return ACPI_FAILURE(status) ? -1 : 0;
}


static ssize_t conservation_mode_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	struct ideapad *priv = dev_get_drvdata(dev);
	unsigned long result;

	if (read_method_int(priv->adev->handle, "GBMD", &result))
		return sprintf(buf, "-1\n");
	return sprintf(buf, "%u\n", test_bit(BM_CONSERVATION_BIT, &result));
}

static ssize_t conservation_mode_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	struct ideapad *priv = dev_get_drvdata(dev);
	bool state;
	int ret;

	ret = kstrtobool(buf, &state);
	if (ret)
		return ret;

	ret = write_method_int(priv->adev->handle, "SBMC",state ?
					      BMCMD_CONSERVATION_ON :
					      BMCMD_CONSERVATION_OFF);
	if (ret < 0)
		return -EIO;
	return count;
}

static DEVICE_ATTR_RW(conservation_mode);

static ssize_t fn_lock_show(struct device *dev,
                            struct device_attribute *attr,
                            char *buf)
{
        unsigned long result = 0;
	struct ideapad *priv = dev_get_drvdata(dev);
	int fail = read_method_int(priv->adev->handle, "HALS", &result);
	if (fail)
            return sprintf(buf, "-1\n");
        return sprintf(buf, "%u\n", test_bit(EC_FNLOCK_BIT, &result));
}

static ssize_t fn_lock_store(struct device *dev,
                             struct device_attribute *attr,
                             const char *buf, size_t count)
{
    	struct ideapad *priv = dev_get_drvdata(dev);
        acpi_status status;
	bool state;
	int ret;

	ret = kstrtobool(buf, &state);
	if (ret)
		return ret;

	status = acpi_execute_simple_method(priv->adev->handle, "SALS",
                                            state ? HCMD_FNLOCK_ON : HCMD_FNLOCK_OFF);
	if (ACPI_FAILURE(status))
		return -EIO;
	return count;
}

static DEVICE_ATTR_RW(fn_lock);


static struct attribute *ideapad_attributes[] = {
	&dev_attr_conservation_mode.attr,
        &dev_attr_fn_lock.attr,
	NULL
};

static umode_t ideapad_is_visible(struct kobject *kobj,
				 struct attribute *attr,
				 int idx)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct ideapad *ideapad = dev_get_drvdata(dev);
        acpi_handle adev = ideapad->adev->handle;
        return (acpi_has_method(adev, "GBMD") &&
                acpi_has_method(adev, "SBMC") &&
                acpi_has_method(adev, "HALS") &&
                acpi_has_method(adev, "SALS")) ? attr->mode : 0;
}

static const struct attribute_group ideapad_attribute_group = {
	.is_visible = ideapad_is_visible,
	.attrs = ideapad_attributes
};

static int ideapad_add(struct platform_device *pdev)
{
        int ret;
	struct ideapad *ideapad;
	struct acpi_device *adev;

        adev = acpi_fetch_acpi_dev(ACPI_HANDLE(&pdev->dev));
        if (!adev)
            return -ENODEV;

	ideapad = devm_kzalloc(&pdev->dev, sizeof(*ideapad), GFP_KERNEL);
	if (!ideapad)
		return -ENOMEM;

	dev_set_drvdata(&pdev->dev, ideapad);
	ideapad->adev = adev;
	ideapad->platform_device = pdev;
        ret = sysfs_create_group(&pdev->dev.kobj,
                           &ideapad_attribute_group);
	return ret;
}

static int ideapad_remove(struct platform_device *pdev)
{
        sysfs_remove_group(&pdev->dev.kobj,
			   &ideapad_attribute_group);

	dev_set_drvdata(&pdev->dev, NULL);
	return 0;
}

static const struct acpi_device_id ideapad_device_ids[] = {
	{ "VPC2004", 0},
	{ "", 0},
};

MODULE_DEVICE_TABLE(acpi, ideapad_device_ids);

static struct platform_driver ideapad_hacks = {
	.probe = ideapad_add,
	.remove = ideapad_remove,
	.driver = {
		.name   = "ideapad-hacks",
		.acpi_match_table = ACPI_PTR(ideapad_device_ids),
	},
};

module_platform_driver(ideapad_hacks);

MODULE_AUTHOR("Oleg Keri <ezhi99@gmail.com>");
MODULE_DESCRIPTION("IdeaPad hacks");
MODULE_LICENSE("GPL");
