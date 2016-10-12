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
{
//	OptionList optlist;

//	_min = 0;
//	_max = 40;
//	_last = (_max + _min) / 2; /* start in the middle */
//
//	try {
//		_min = optlist.lookup_double(options, "min");
//	} catch (vz::OptionNotFoundException &e) {
//		_min = 0;
//	} catch (vz::VZException &e) {
//		print(log_error, "Min value has to be a floating point number (e.g. '40.0')", name().c_str());
//		throw;
//	}
//
//	try {
//		_max = optlist.lookup_double(options, "max");
//	} catch (vz::OptionNotFoundException &e) {
//		_max = 0;
//	} catch (vz::VZException &e) {
//		print(log_error, "Max value has to be a floating point number (e.g. '40.0')", name().c_str());
//		throw;
//	}
}

MeterModbus::~MeterModbus() {
}

int MeterModbus::open() {

        ctx = modbus_new_rtu("/dev/ttyUSB0", 1200, 'E', 8, 1);
        
	modbus_set_debug(ctx, TRUE);

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
	uint32_t import_energy;
	uint32_t total_power;

	if (rds.size() < 1) return -1;

        rc = modbus_read_registers(ctx, 0x160, 2, reg_val);

        if (rc == -1) {
		print(log_error, "Reading failed: %s\n", modbus_strerror(errno));
                return -1;
        }

	import_energy = (uint32_t)reg_val[0] << 16 | (uint32_t)reg_val[1];

	rc = modbus_read_registers(ctx, 0x96, 2, reg_val);
	if (rc == -1) {
		print(log_error, "Reading failed: %s\n", modbus_strerror(errno));
		return -1;
	}

	total_power = (uint32_t)reg_val[0] << 16 | (uint32_t)reg_val[1];

	rds[0].value(import_energy);
	rds[0].time();
	rds[0].identifier(new StringIdentifier("Energy"));

	rds[1].value(total_power);
	rds[1].time();
	rds[1].identifier(new StringIdentifier("Power"));

	return 2;
}
