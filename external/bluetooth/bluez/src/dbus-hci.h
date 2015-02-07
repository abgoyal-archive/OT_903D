
int hcid_dbus_request_pin(int dev, bdaddr_t *sba, struct hci_conn_info *ci);
void hcid_dbus_inquiry_result(bdaddr_t *local, bdaddr_t *peer, uint32_t class, int8_t rssi, uint8_t *data);
void hcid_dbus_set_legacy_pairing(bdaddr_t *local, bdaddr_t *peer, gboolean legacy);
void hcid_dbus_remote_class(bdaddr_t *local, bdaddr_t *peer, uint32_t class);
void hcid_dbus_remote_name(bdaddr_t *local, bdaddr_t *peer, uint8_t status, char *name);
void hcid_dbus_conn_complete(bdaddr_t *local, uint8_t status, uint16_t handle, bdaddr_t *peer);
void hcid_dbus_disconn_complete(bdaddr_t *local, uint8_t status, uint16_t handle, uint8_t reason);
void hcid_dbus_bonding_process_complete(bdaddr_t *local, bdaddr_t *peer, uint8_t status);
void hcid_dbus_simple_pairing_complete(bdaddr_t *local, bdaddr_t *peer, uint8_t status);
void hcid_dbus_setscan_enable_complete(bdaddr_t *local);
void hcid_dbus_write_simple_pairing_mode_complete(bdaddr_t *local);
void hcid_dbus_returned_link_key(bdaddr_t *local, bdaddr_t *peer);
int hcid_dbus_get_io_cap(bdaddr_t *local, bdaddr_t *remote);
int hcid_dbus_set_io_cap(bdaddr_t *local, bdaddr_t *remote,
						uint8_t cap, uint8_t auth);
int hcid_dbus_user_confirm(bdaddr_t *sba, bdaddr_t *dba, uint32_t passkey);
int hcid_dbus_user_passkey(bdaddr_t *sba, bdaddr_t *dba);
int hcid_dbus_user_notify(bdaddr_t *sba, bdaddr_t *dba, uint32_t passkey);
int hcid_dbus_link_key_notify(bdaddr_t *local, bdaddr_t *peer,
				uint8_t *key, uint8_t key_type,
				int pin_length, uint8_t old_key_type);
int hcid_dbus_get_oob_data(bdaddr_t *sba, bdaddr_t *dba);

gboolean get_adapter_and_device(bdaddr_t *src, bdaddr_t *dst,
					struct btd_adapter **adapter,
					struct btd_device **device,
					gboolean create);

const char *class_to_icon(uint32_t class);

void set_dbus_connection(DBusConnection *conn);

DBusConnection *get_dbus_connection(void);
