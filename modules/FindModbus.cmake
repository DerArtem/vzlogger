# -*- mode: cmake; -*-
# - Try to find libmbus include dirs and libraries
# Usage of this module as follows:
# This file defines:
# * MODBUS_FOUND if protoc was found
# * MODBUS_LIBRARY The lib to link to (currently only a static unix lib, not
# portable)
# * MODBUS_INCLUDE_DIR The include directories for libmbus.

message(STATUS "FindModbus check")
IF (NOT WIN32)
  include(FindPkgConfig)
  if ( PKG_CONFIG_FOUND )

	  pkg_check_modules (PC_MODBUS modbus)

	  set(MODBUS_DEFINITIONS ${PC_MODBUS_CFLAGS_OTHER})
  endif(PKG_CONFIG_FOUND)
endif (NOT WIN32)

message(STATUS "Looking for libmodbus in ${PC_MODBUS_INCLUDEDIR}")

# find the include files
FIND_PATH(MODBUS_INCLUDE_DIR modbus/modbus.h
   HINTS
   ${PC_MODBUS_INCLUDEDIR}
   ${PC_MODBUS_INCLUDE_DIRS}
    ${CMAKE_INCLUDE_PATH}
)

# locate the library
IF(WIN32)
	SET(MODBUS_LIBRARY_NAMES ${MODBUS_LIBRARY_NAMES} libmodbus.lib)
ELSE(WIN32)
	SET(MODBUS_LIBRARY_NAMES ${MODBUS_LIBRARY_NAMES} modbus)
ENDIF(WIN32)
FIND_LIBRARY(MODBUS_LIBRARY NAMES ${MODBUS_LIBRARY_NAMES}
  HINTS
  ${PC_MODBUS_LIBDIR}
  ${PC_MODBUS_LIBRARY_DIRS}
)

# if the include and the program are found then we have it
IF(MODBUS_INCLUDE_DIR AND MODBUS_LIBRARY)
	SET(MODBUS_FOUND "YES")
	message("libmodbus found: '${MODBUS_INCLUDE_DIR}'")
	#  SET(MODBUS_INCLUDE_DIR ${MODBUS-C_INCLUDE_DIR})
ENDIF(MODBUS_INCLUDE_DIR AND MODBUS_LIBRARY)

if( NOT WIN32)
	list(APPEND MODBUS_LIBRARY "-lmodbus")
endif( NOT WIN32)

MARK_AS_ADVANCED(
	MODBUS_FOUND
	MODBUS_LIBRARY
	MODBUS_INCLUDE_DIR
)
