#ifndef DBUS_UPOWER_H
#define DBUS_UPOWER_H

#include <dbus/dbus.h>

#define UPOWER_DISPLAY_DEVICE  "/org/freedesktop/UPower/devices/DisplayDevice"
#define LENGTH(X) (sizeof X / sizeof X[0])


typedef struct {
	DBusConnection *conn;
	DBusError err;
} DBusUpower;

typedef struct {
	char *icon_name;
	char *model;
	char *native_path;
	char *serial;
	char *vendor;
	dbus_uint32_t battery_level;
	double capacity;
	double energy; 
	double energy_empty; 
	double energy_full; 
	double energy_full_design; 
	double energy_rate; 
	dbus_bool_t has_history;
	dbus_bool_t has_statistics;
	dbus_bool_t is_present;
	dbus_bool_t is_rechargable;
	double luminosity;
	dbus_bool_t online;
	double percentage;
	dbus_bool_t power_supply;
	dbus_uint32_t state;
	dbus_uint32_t technology;
	double temperature;
	dbus_int64_t time_to_empty;
	dbus_int64_t time_to_full;
	dbus_uint32_t type;
	dbus_uint64_t update_time;
	double voltage;
	dbus_uint32_t warning_level;
} Device;

/* Init DBus Upower wrapper */
void dw_init(DBusUpower *dw);

/** Clean resources asociated with the wrapper */
void dw_unref(DBusUpower *dw);

/** Call the EnumerateDevices method
 * @return List of object paths, `free()` the list (not each string) when
 * you're done
 */
char** dw_enumerate_devices(DBusUpower *dw);


/** Call the GetCriticalAction method
 * @return Action to be taken by the system when power supply is critical.
 * Shouldn't be `free`d
 */
char* dw_get_critical_action(DBusUpower *dw);

/** Get the Device info by listing it's properties
 * @return If `path` exists, return a pointer to the device instance (should be
 * `free`d), otherwise NULL Shouldn't be `free`d
 */
Device* dw_get_device(DBusUpower *dw, char *path);

/** Call the Refresh method
 * @return 1 if the call was successful, 0 otherwise
 */
int dw_refresh(DBusUpower *dw);

#endif // DBUS_UPOWER_H
