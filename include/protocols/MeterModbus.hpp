/**
 * Generate pseudo random data series by a random walk
 *
 * @package vzlogger
 * @copyright Copyright (c) 2011, The volkszaehler.org project
 * @license http://www.gnu.org/licenses/gpl.txt GNU Public License
 * @author Steffen Vogel <info@steffenvogel.de>
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
 
#ifndef _METER_MODBUS_H_
#define _METER_MODBUS_H_

#include <modbus/modbus.h>
#include <protocols/Protocol.hpp>

class MeterModbus : public vz::protocol::Protocol {

public:
	MeterModbus(std::list<Option> options);
	virtual ~MeterModbus();

	int open();
	int close();
	ssize_t read(std::vector<Reading> &rds, size_t n);

	const char *device() const { return _device.c_str(); }

protected:
	std::string _device;
	int _baudrate;
	parity_type_t _parity;
	uint16_t reg_val[64];
	int ModbusRegisterCount;
	int ModbusRegister[16];

	modbus_t *ctx;
};

#endif /* _METER_MODBUS_H_ */
