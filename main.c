#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "libdbusupower.h"

void print_device_info(const Device* device);

int main(int argc, char** arv) {
	DBusUpower dw;
	Device *device;
	char **objects, **o;

	dw_init(&dw);

	objects = dw_enumerate_devices(&dw);
	if (objects) {
		printf("Objecs:\n");
		for (o = objects; o && *o; o++) {
			printf("- Device: %s\n", *o);
			device = dw_get_device(&dw, *o);

			if (!device) break;

			print_device_info(device);
			free(device);
		}
		free(objects);
	}

	char *action = dw_get_critical_action(&dw);
	printf("Critical action: %s\n", action);


	dw_unref(&dw);

	return 0;
}

void print_device_info(const Device* device) {
	printf("\t- Icon: %s\n", device->icon_name);
	printf("\t- Model: %s\n", device->model);
	printf("\t- Serial: %s\n", device->serial);
	printf("\t- Vendor: %s\n", device->vendor);
	printf("\t- WarningLevel: %d\n", device->warning_level);
	printf("\t- Percentage: %f\n", device->percentage);
	printf("\t- State: %d\n", device->state);
	printf("\t- Type: %d\n", device->type);
	printf("\t- Is rechargable: %d\n", device->is_rechargable);
}
