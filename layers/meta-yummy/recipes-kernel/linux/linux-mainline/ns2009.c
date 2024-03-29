/*
 * Nsiway NS2009 touchscreen controller driver
 *
 * Copyright (C) 2023 Jan Patrovsky <nejcraftplays@gmail.com>
 * Copyright (C) 2017 Icenowy Zheng <icenowy@aosc.xyz>
 *
 * Some codes are from silead.c, which is
 *   Copyright (C) 2014-2015 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/input/touchscreen.h>
#include <linux/i2c.h>

/* polling interval in ms */
#define POLL_INTERVAL	100

/* this driver uses 12-bit readout */
#define MAX_12BIT	0xfff

#define NS2009_READ_X_LOW_POWER_12BIT	0xc0
#define NS2009_READ_Y_LOW_POWER_12BIT	0xd0
#define NS2009_READ_Z1_LOW_POWER_12BIT	0xe0
#define NS2009_READ_Z2_LOW_POWER_12BIT	0xf0

#define NS2009_DEF_X_FUZZ	32
#define NS2009_DEF_Y_FUZZ	25

/*
 * The chip have some error in z1 value when pen is up, so the data read out
 * is sometimes not accurately 0.
 * This value is based on experiements.
 */
#define NS2009_PEN_UP_Z1_ERR	80

struct ns2009_data {
	struct i2c_client		*client;
	struct input_dev		*input;

	struct touchscreen_properties	prop;

	bool				pen_down;
	u16			x_plate_ohms;
	u16			max_rt;
};

static int ns2009_ts_read_data(struct ns2009_data *data, u8 cmd, u16 *val)
{
	u8 raw_data[2];
	int error;

	error = i2c_smbus_read_i2c_block_data(data->client, cmd, 2, raw_data);
	if (error < 0)
		return error;

	if (unlikely(raw_data[1] & 0xf))
		return -EINVAL;

	*val = (raw_data[0] << 4) | (raw_data[1] >> 4);

	return 0;
}

static int ns2009_ts_report(struct ns2009_data *data)
{
	u16 x, y, z1, z2;
    u32 rt;
	int ret;

	/*
	 * NS2009 chip supports pressure measurement, but currently it needs
	 * more investigation, so we only use z1 axis to detect pen down
	 * here.
	 */
	ret = ns2009_ts_read_data(data, NS2009_READ_Z1_LOW_POWER_12BIT, &z1);
	if (ret)
		return ret;
	ret = ns2009_ts_read_data(data, NS2009_READ_Z2_LOW_POWER_12BIT, &z2);
	if (ret)
		return ret;

	ret = ns2009_ts_read_data(data, NS2009_READ_X_LOW_POWER_12BIT, &x);
	if (ret)
		return ret;
	ret = ns2009_ts_read_data(data, NS2009_READ_Y_LOW_POWER_12BIT, &y);
	if (ret)
		return ret;

   if(x == MAX_12BIT)
 		x = 0;

	rt  = z2 - z1;
	rt *= x;
	rt *= data->x_plate_ohms;
	rt /= z1;
	rt = rt >> 12;

	if (rt <= data->max_rt) {

		if (!data->pen_down) {
			input_report_key(data->input, BTN_TOUCH, 1);
			data->pen_down = true;
		}
		rt = MAX_12BIT - rt;

		//input_report_abs(data->input, ABS_X, x);
		//input_report_abs(data->input, ABS_Y, y);
		touchscreen_report_pos(data->input, &data->prop, x, y, false);
		input_report_abs(data->input, ABS_PRESSURE, rt);
		input_sync(data->input);
	} else if (data->pen_down) {
		input_report_key(data->input, BTN_TOUCH, 0);
		input_report_abs(data->input, ABS_PRESSURE, 0);
		input_sync(data->input);
		data->pen_down = false;
	}
	return 0;
}

static void ns2009_ts_poll(struct input_dev *input)
{
	struct ns2009_data *data =  input_get_drvdata(input);
	int ret;

	ret = ns2009_ts_report(data);
	if (ret)
		dev_err(&input->dev, "Poll touch data failed: %d\n", ret);
}

static int ns2009_ts_request_polled_input_dev(struct ns2009_data *data)
{
	struct device *dev = &data->client->dev;
	struct input_dev *input;
	int error;
/*
	polled_dev = devm_input_allocate_polled_device(dev);
	if (!polled_dev) {
		dev_err(dev,
			"Failed to allocate polled input device\n");
		return -ENOMEM;
	}
	data->input = polled_dev->input;

	ns2009_ts_config_input_dev(data);
	polled_dev->private = data;
	polled_dev->poll = ns2009_ts_poll;
	polled_dev->poll_interval = POLL_INTERVAL;

	error = input_register_polled_device(polled_dev);
	if (error) {
		dev_err(dev, "Failed to register polled input device: %d\n",
			error);
		return error;
	}
	*/


	input = devm_input_allocate_device(dev);
	if (!input) {
		dev_err(dev, "Failed to allocate input device\n");
		return -ENOMEM;
	}

	data->input = input;
	input_set_drvdata(input, data);

	input->name = "ns2009";
	input->phys = "input/ts";
	input->id.bustype = BUS_I2C;
	input_set_capability(input, EV_KEY, BTN_TOUCH);

	input_set_abs_params(input, ABS_X, 0, MAX_12BIT, NS2009_DEF_X_FUZZ, 0);
	input_set_abs_params(input, ABS_Y, 0, MAX_12BIT, NS2009_DEF_Y_FUZZ, 0);
	touchscreen_parse_properties(input, false, &data->prop);
	
	if (error) {
		dev_err(dev, "could not init mt slots, %d\n", error);
		return error;
	}

	error = input_setup_polling(input, ns2009_ts_poll);
	if (error) {
		dev_err(dev, "could not set up polling mode, %d\n", error);
		return error;
	}

	input_set_poll_interval(input, POLL_INTERVAL);

	error = input_register_device(input);
	if (error) {
		dev_err(dev, "could not register input device, %d\n", error);
		return error;
	}

	return 0;
}

static int ns2009_probe_properties(struct device *dev, struct ns2009_data *ts)
{
	u32 val32;

	if (!device_property_read_u32(dev, "max-rt", &val32))
		ts->max_rt = val32;
	else
		ts->max_rt = MAX_12BIT;
	if (!device_property_read_u32(dev, "x-plate-ohms", &val32)) {
		ts->x_plate_ohms = val32;
	} else {
		dev_err(dev, "Missing ti,x-plate-ohms device property\n");
		return -EINVAL;
	}

	return 0;
}

static int ns2009_ts_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	struct ns2009_data *data;
	struct device *dev = &client->dev;
	int error;

	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_I2C |
				     I2C_FUNC_SMBUS_READ_I2C_BLOCK |
				     I2C_FUNC_SMBUS_WRITE_I2C_BLOCK)) {
		dev_err(dev, "I2C functionality check failed\n");
		return -ENXIO;
	}

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	error = ns2009_probe_properties(dev, data);
	if (error)
		return error;

	i2c_set_clientdata(client, data);
	data->client = client;

	error = ns2009_ts_request_polled_input_dev(data);
	if (error)
		return error;


	return 0;
};


static const struct of_device_id ns2009_of_match[] = {
	{ .compatible = "nsiway,ns2009" },
	{},
};
MODULE_DEVICE_TABLE(of, ns2009_of_match);

static const struct i2c_device_id ns2009_id[] = {
	{ "ns2009", 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, ns2009_id);

static struct i2c_driver ns2009_driver = {
	.driver = {
		.name = "ns2009",
		.of_match_table = ns2009_of_match,
	},
	.id_table = ns2009_id,
	.probe = ns2009_ts_probe,
};
module_i2c_driver(ns2009_driver);

MODULE_DESCRIPTION("NS2009 Touchscreen driver");
MODULE_AUTHOR("JAN PATROVSKY <nejcraftplays@gmail.com>");
MODULE_LICENSE("GPL");