/**
 * Generate pseudo random data series with a random walk
 *
 * @package vzlogger
 * @copyright Copyright (c) 2011, The volkszaehler.org project
 * @license http://www.gnu.org/licenses/gpl.txt GNU Public License
 * @author Steffen Vogel <info@steffenvogel.de>
 *
 * "connection" sets the maximum value
 */
/*
 * This file is part of volkzaehler.org
 *
 * volkzaehler.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * volkzaehler.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with volkszaehler.org. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "protocols/MeterModbus.hpp"
#include "Options.hpp"
#include <VZException.hpp>
#include <modbus/modbus.h>

MeterModbus::MeterModbus(std::list<Option> options)
		: Protocol("modbus")
		, _device("")
{
	OptionList optlist;
	struct json_object *jso = optlist.lookup_json_array(options, "registers");

	// Read the registers we want to read from the device
	try {
		if (jso && (ModbusRegisterCount=json_object_array_length(jso))>=1){
			for (int i = 0; i < ModbusRegisterCount; i++) {
				struct json_object *jb = json_object_array_get_idx(jso, i);
				ModbusRegister[i] = json_object_get_int(jb);
			}
		}
	} catch (vz::VZException &e){
		print(log_error, "Could not read register configuration", name().c_str());
		throw;
	}

	try {
		_device = optlist.lookup_string(options, "device");
	} catch (vz::VZException &e){
		print(log_error, "Missing device", name().c_str());
		throw;
	}

        //baudrate = 9600; /* default to avoid compiler warning */
        try {
                _baudrate = optlist.lookup_int(options, "baudrate");
        } catch (vz::VZException &e) {
                print(log_error, "Failed to parse the baudrate", name().c_str());
                throw;
        }
}

MeterModbus::~MeterModbus() {
}

int MeterModbus::open() {

        ctx = modbus_new_rtu(device(), _baudrate, 'E', 8, 1);
        
	if (modbus_connect(ctx) == -1) {
		print(log_error, "Connection failed: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
        }
        
	modbus_set_slave(ctx, 1);
	modbus_flush(ctx);

	return SUCCESS;
}

int MeterModbus::close() {
        modbus_close(ctx);
        modbus_free(ctx);
	return SUCCESS;
}

ssize_t MeterModbus::read(std::vector<Reading> &rds, size_t n) {

	int rc;
	uint32_t readValue;
	char strIdentifier[16];

	if (rds.size() < 1) return -1;

	for (int i = 0; i < ModbusRegisterCount; i++) {
		rc = modbus_read_registers(ctx, ModbusRegister[i], 2, reg_val);
		if (rc == -1) {
			print(log_error, "Reading failed: %s\n", modbus_strerror(errno));
			return -1;
		}

		readValue = (uint32_t)reg_val[0] << 16 | (uint32_t)reg_val[1];

		rds[i].value(readValue);
		rds[i].time();
		sprintf(strIdentifier, "%d", ModbusRegister[i]);
		rds[i].identifier(new StringIdentifier (strIdentifier));
	}

	return ModbusRegisterCount;
}
