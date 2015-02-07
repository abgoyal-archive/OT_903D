


#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <sound/core.h>
#include "pmac.h"

static struct pmac_keywest *keywest_ctx;


static int keywest_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	i2c_set_clientdata(client, keywest_ctx);
	return 0;
}

static int keywest_attach_adapter(struct i2c_adapter *adapter)
{
	struct i2c_board_info info;

	if (! keywest_ctx)
		return -EINVAL;

	if (strncmp(adapter->name, "mac-io", 6))
		return 0; /* ignored */

	memset(&info, 0, sizeof(struct i2c_board_info));
	strlcpy(info.type, "keywest", I2C_NAME_SIZE);
	info.addr = keywest_ctx->addr;
	keywest_ctx->client = i2c_new_device(adapter, &info);
	if (!keywest_ctx->client)
		return -ENODEV;
	/*
	 * We know the driver is already loaded, so the device should be
	 * already bound. If not it means binding failed, and then there
	 * is no point in keeping the device instantiated.
	 */
	if (!keywest_ctx->client->driver) {
		i2c_unregister_device(keywest_ctx->client);
		keywest_ctx->client = NULL;
		return -ENODEV;
	}
	
	/*
	 * Let i2c-core delete that device on driver removal.
	 * This is safe because i2c-core holds the core_lock mutex for us.
	 */
	list_add_tail(&keywest_ctx->client->detected,
		      &keywest_ctx->client->driver->clients);
	return 0;
}

static int keywest_remove(struct i2c_client *client)
{
	i2c_set_clientdata(client, NULL);
	if (! keywest_ctx)
		return 0;
	if (client == keywest_ctx->client)
		keywest_ctx->client = NULL;

	return 0;
}


static const struct i2c_device_id keywest_i2c_id[] = {
	{ "keywest", 0 },
	{ }
};

static struct i2c_driver keywest_driver = {
	.driver = {
		.name = "PMac Keywest Audio",
	},
	.attach_adapter = keywest_attach_adapter,
	.probe = keywest_probe,
	.remove = keywest_remove,
	.id_table = keywest_i2c_id,
};

/* exported */
void snd_pmac_keywest_cleanup(struct pmac_keywest *i2c)
{
	if (keywest_ctx && keywest_ctx == i2c) {
		i2c_del_driver(&keywest_driver);
		keywest_ctx = NULL;
	}
}

int __devinit snd_pmac_tumbler_post_init(void)
{
	int err;
	
	if (!keywest_ctx || !keywest_ctx->client)
		return -ENXIO;

	if ((err = keywest_ctx->init_client(keywest_ctx)) < 0) {
		snd_printk(KERN_ERR "tumbler: %i :cannot initialize the MCS\n", err);
		return err;
	}
	return 0;
}

/* exported */
int __devinit snd_pmac_keywest_init(struct pmac_keywest *i2c)
{
	int err;

	if (keywest_ctx)
		return -EBUSY;

	keywest_ctx = i2c;

	if ((err = i2c_add_driver(&keywest_driver))) {
		snd_printk(KERN_ERR "cannot register keywest i2c driver\n");
		return err;
	}
	return 0;
}
