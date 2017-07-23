CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )

PROJECT( Aircraft )

ADD_SUBDIRECTORY( Dependencies )

IF( CMAKE_SYSTEM_NAME MATCHES Linux )
	# The source code of the different I2C device classes include "I2Cdev.h" (without path),
	# so we add the folder containing that header to the include directories
	INCLUDE_DIRECTORIES( ${I2CDevLib_SOURCE_DIR}/I2Cdev )
	INCLUDE_DIRECTORIES( ${I2CDevLib_SOURCE_DIR} )
	MESSAGE( "I2C: " ${I2CDevLib_SOURCE_DIR} )

	#INCLUDE_DIRECTORIES( ${RPIOPWM_SOURCE_DIR} )
	#MESSAGE( "PWM: " ${RPIOPWM_SOURCE_DIR} )	
ENDIF()

ADD_DEFINITIONS(-std=c++11)   # for <cstdint> on Raspberry Pi
SET	(	SOURCES
		Platform/Platform.h
		Platform/Platform.cpp
		Platform/LocoTime.h
		Platform/LocoTime.cpp
		Platform/LocoThread.h
		Platform/LocoThread.cpp
		Platform/UDPSocket.h
		Platform/UDPSocket.cpp
		
		SensorsSample.h
		Sensors.h
		Sensors.cpp

		FlightControls.h
		FlightControlsReceiver.h
		FlightControlsReceiver.cpp

		PWMGenerator.h
		PWMGenerator.cpp
		FlightParameters.h
		FlightParameters.cpp
		FlightController.h
		FlightController.cpp

		TelemetrySender.h
		TelemetrySender.cpp
		
		Aircraft.h
		Aircraft.cpp
		
		Main.cpp
	)

ADD_EXECUTABLE( ${PROJECT_NAME} ${SOURCES} )

IF( CMAKE_SYSTEM_NAME MATCHES Linux )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} I2CDevLib )
	#TARGET_LINK_LIBRARIES( ${PROJECT_NAME} RPIOPWM ) 
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} pigpio ) 
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} rt ) 
ENDIF()