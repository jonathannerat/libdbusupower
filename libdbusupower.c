#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libdbusupower.h"

#define UPOWER_BUS_NAME        "org.freedesktop.UPower"
#define UPOWER_BUS_PATH        "/org/freedesktop/UPower"
#define UPOWER_IFACE           UPOWER_BUS_NAME
#define UPOWER_DEVICE_IFACE    (UPOWER_BUS_NAME ".Device")
#define DBUS_PROP_IFACE        "org.freedesktop.DBus.Properties"

typedef struct {
	char type;
	void *value;
} Arg;

typedef struct {
	int length;
	Arg* args;
} ArgList;

void dw_init(DBusUpower* dw) {
	dbus_error_init(&dw->err);
	dw->conn = dbus_bus_get(DBUS_BUS_SYSTEM, &dw->err);
}

int dw_call(DBusUpower *dw, char* path, char *iface, char *method, int nargs, const Arg* args, DBusMessageIter *out) {
	DBusMessage *msg, *reply;
	DBusMessageIter msgargs;
	const Arg *p;

	msg = dbus_message_new_method_call(
		UPOWER_BUS_NAME,
		path,
		iface,
		method
	);

	if (!msg) return 0;

	if (args) {
		p = args;
		dbus_message_iter_init_append(msg, &msgargs);

		for (int i = 0; i < nargs; i++, p++) {
			switch (p->type) {
				case 's':
					dbus_message_iter_append_basic(&msgargs, DBUS_TYPE_STRING, &p->value);
					break;
			}
		}
	}

	reply = dbus_connection_send_with_reply_and_block(dw->conn, msg, -1, &dw->err);
	if (!reply) return 0;

	dbus_message_iter_init(reply, out);
	dbus_message_unref(msg);
	dbus_message_unref(reply);

	return 1;
}

char** dw_enumerate_devices(DBusUpower *dw) {
	DBusMessageIter out, iout;
	int length, res;
	char** objects, **iobjects;

	res = dw_call(dw, UPOWER_BUS_PATH, UPOWER_IFACE, "EnumerateDevices", 0, NULL, &out);
	if (!res) return NULL;
	
	if (dbus_message_iter_get_element_type(&out) == DBUS_TYPE_INVALID)
		return NULL;

	dbus_message_iter_recurse(&out, &iout);
	objects = malloc(length * sizeof(char*) + 1);
	iobjects = objects;

	while (1) {
		dbus_message_iter_get_basic(&iout, iobjects++);
		if (!dbus_message_iter_has_next(&iout))
			break;
		dbus_message_iter_next(&iout);
	}

	*iobjects = NULL;

	return objects;
}

char* dw_get_critical_action(DBusUpower *dw) {
	DBusMessageIter out;
	char *action;
	int ok;

	ok = dw_call(dw, UPOWER_BUS_PATH, UPOWER_IFACE, "GetCriticalAction", 0, NULL, &out);
	if (!ok) return NULL;

	if (dbus_message_iter_get_arg_type(&out) == DBUS_TYPE_INVALID) return NULL;
	dbus_message_iter_get_basic(&out, &action);

	return action;
}

Device* dw_get_device(DBusUpower *dw, char *path) {
	Device *device;
	DBusMessageIter out, prop_entry, prop_keyval, prop_variant;
	Arg args[] = {
		{ 's', UPOWER_DEVICE_IFACE },
	};
	char *key;
	int res;

	res = dw_call(dw, path, DBUS_PROP_IFACE, "GetAll", LENGTH(args), args, &out);
	if (!res) return NULL;

	if (dbus_message_iter_get_element_type(&out) == DBUS_TYPE_INVALID) return NULL;

	dbus_message_iter_recurse(&out, &prop_entry);
	device = malloc(sizeof(Device));

	while (1) {
		dbus_message_iter_recurse(&prop_entry, &prop_keyval);
		dbus_message_iter_get_basic(&prop_keyval, &key);
		dbus_message_iter_next(&prop_keyval);
		dbus_message_iter_recurse(&prop_keyval, &prop_variant);

		if (key[0] == 'B')
			dbus_message_iter_get_basic(&prop_variant, &device->battery_level);
		else if (key[0] == 'C')
			dbus_message_iter_get_basic(&prop_variant, &device->capacity);
		else if (key[0] == 'L')
			dbus_message_iter_get_basic(&prop_variant, &device->luminosity);
		else if (key[0] == 'M')
			dbus_message_iter_get_basic(&prop_variant, &device->model);
		else if (key[0] == 'N')
			dbus_message_iter_get_basic(&prop_variant, &device->native_path);
		else if (key[0] == 'O')
			dbus_message_iter_get_basic(&prop_variant, &device->online);
		else if (key[0] == 'U')
			dbus_message_iter_get_basic(&prop_variant, &device->update_time);
		else if (key[0] == 'W')
			dbus_message_iter_get_basic(&prop_variant, &device->warning_level);
		else if (!strncmp(key, "Ic", 2))
			dbus_message_iter_get_basic(&prop_variant, &device->icon_name);
		else if (!strncmp(key, "Pe", 2))
			dbus_message_iter_get_basic(&prop_variant, &device->percentage);
		else if (!strncmp(key, "Po", 2))
			dbus_message_iter_get_basic(&prop_variant, &device->power_supply);
		else if (!strncmp(key, "Se", 2))
			dbus_message_iter_get_basic(&prop_variant, &device->serial);
		else if (!strncmp(key, "St", 2))
			dbus_message_iter_get_basic(&prop_variant, &device->state);
		else if (!strncmp(key, "Ty", 2))
			dbus_message_iter_get_basic(&prop_variant, &device->type);
		else if (!strncmp(key, "Ve", 2))
			dbus_message_iter_get_basic(&prop_variant, &device->vendor);
		else if (!strncmp(key, "Vo", 2))
			dbus_message_iter_get_basic(&prop_variant, &device->voltage);
		else if (!strncmp(key, "IsP", 3))
			dbus_message_iter_get_basic(&prop_variant, &device->is_present);
		else if (!strncmp(key, "IsR", 3))
			dbus_message_iter_get_basic(&prop_variant, &device->is_rechargable);
		else if (!strncmp(key, "Tec", 3))
			dbus_message_iter_get_basic(&prop_variant, &device->technology);
		else if (!strncmp(key, "Tem", 3))
			dbus_message_iter_get_basic(&prop_variant, &device->temperature);
		else if (!strncmp(key, "HasH", 4))
			dbus_message_iter_get_basic(&prop_variant, &device->has_history);
		else if (!strncmp(key, "HasS", 4))
			dbus_message_iter_get_basic(&prop_variant, &device->has_statistics);
		else if (!strncmp(key, "EnergyE", 7))
			dbus_message_iter_get_basic(&prop_variant, &device->energy_empty);
		else if (!strncmp(key, "EnergyR", 7))
			dbus_message_iter_get_basic(&prop_variant, &device->energy_rate);
		else if (!strncmp(key, "TimeToE", 7))
			dbus_message_iter_get_basic(&prop_variant, &device->time_to_empty);
		else if (!strncmp(key, "TimeToF", 7))
			dbus_message_iter_get_basic(&prop_variant, &device->time_to_full);
		else if (!strncmp(key, "EnergyFullD", 11))
			dbus_message_iter_get_basic(&prop_variant, &device->energy_full_design);
		else if (!strcmp(key, "Energy"))
			dbus_message_iter_get_basic(&prop_variant, &device->energy);
		else if (!strcmp(key, "EnergyFull"))
			dbus_message_iter_get_basic(&prop_variant, &device->energy_full);

		if (!dbus_message_iter_has_next(&prop_entry))
			break;

		dbus_message_iter_next(&prop_entry);
	}

	return device;
}

int dw_refresh(DBusUpower *dw) {
	DBusMessageIter out;
	int ok = dw_call(dw, UPOWER_BUS_PATH, UPOWER_DEVICE_IFACE, "Refresh", 0, NULL, &out);

	if (!ok) return !ok;
}

void dw_unref(DBusUpower *dw) {
	dbus_connection_flush(dw->conn);
	dbus_error_free(&dw->err);
}
