CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )

PROJECT( Aircraft )

ADD_SUBDIRECTORY( Dependencies )

IF( CMAKE_SYSTEM_NAME MATCHES Linux )
	# The source code of the different I2C device classes include "I2Cdev.h" (without path),
	# so we add the folder containing that header to the include directories
	INCLUDE_DIRECTORIES( ${I2CDevLib_SOURCE_DIR}/I2Cdev )
	INCLUDE_DIRECTORIES( ${I2CDevLib_SOURCE_DIR} )
	MESSAGE( "I2C: " ${I2CDevLib_SOURCE_DIR} )

	INCLUDE_DIRECTORIES( ${RPIOPWM_SOURCE_DIR} )
	MESSAGE( "PWM: " ${RPIOPWM_SOURCE_DIR} )	
ENDIF()

ADD_DEFINITIONS(-std=c++11)   # for <cstdint> on Raspberry Pi
SET	(	SOURCES
		Platform.h
		Platform.cpp
		LocoTime.h
		LocoTime.cpp
		LocoThread.h
		LocoThread.cpp
		UDPSocket.h
		UDPSocket.cpp
		
		TelemetryData.h
		TelemetryProvider.h
		TelemetryProvider.cpp
		TelemetrySender.h
		TelemetrySender.cpp

		FlightControls.h
		FlightControlsReceiver.h
		FlightControlsReceiver.cpp

		Main.cpp
	)

ADD_EXECUTABLE( ${PROJECT_NAME} ${SOURCES} )

IF( CMAKE_SYSTEM_NAME MATCHES Linux )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} I2CDevLib )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} RPIOPWM ) 
ENDIF()