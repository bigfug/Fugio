#ifndef FIRMATA_DEFINES_H
#define FIRMATA_DEFINES_H

enum FirmataMessage
{
	ANALOG_MESSAGE = 0xE0,
	DIGITAL_MESSAGE = 0x90,
	REPORT_ANALOG_PIN = 0xC0,
	REPORT_DIGITAL_PIN = 0xD0,
	START_SYSEX = 0xF0,
	SET_PIN_MODE = 0xF4,
	SET_DIGITAL_VALUE = 0xF5,
	END_SYSEX = 0xF7,
	PROTOCOL_VERSION = 0xF9
};

enum FirmataSysEx
{
	SERIAL_MESSAGE              = 0x60, // communicate with serial devices, including other boards
	ENCODER_DATA                = 0x61, // reply with encoders current positions
	ANALOG_MAPPING_QUERY        = 0x69, // ask for mapping of analog to pin numbers
	ANALOG_MAPPING_RESPONSE     = 0x6A, // reply with mapping info
	CAPABILITY_QUERY            = 0x6B, // ask for supported modes and resolution of all pins
	CAPABILITY_RESPONSE         = 0x6C, // reply with supported modes and resolution
	PIN_STATE_QUERY             = 0x6D, // ask for a pin's current mode and state (different than value)
	PIN_STATE_RESPONSE          = 0x6E, // reply with a pin's current mode and state (different than value)
	EXTENDED_ANALOG             = 0x6F, // analog write (PWM, Servo, etc) to any pin
	SERVO_CONFIG                = 0x70, // pin number and min and max pulse
	STRING_DATA                 = 0x71, // a string message with 14-bits per char
	STEPPER_DATA                = 0x72, // control a stepper motor
	ONEWIRE_DATA                = 0x73, // send an OneWire read/write/reset/select/skip/search request
	SHIFT_DATA                  = 0x75, // shiftOut config/data message (reserved - not yet implemented)
	I2C_REQUEST                 = 0x76, // I2C request messages from a host to an I/O board
	I2C_REPLY                   = 0x77, // I2C reply messages from an I/O board to a host
	I2C_CONFIG                  = 0X78, // Enable I2C and provide any configuration settings
	REPORT_FIRMWARE             = 0x79, // report name and version of the firmware
	SAMPLEING_INTERVAL          = 0x7A, // the interval at which analog input is sampled (default = 19ms)
	SCHEDULER_DATA              = 0x7B, // send a createtask/deletetask/addtotask/schedule/querytasks/querytask request to the scheduler
	SYSEX_NON_REALTIME          = 0x7E, // MIDI Reserved for non-realtime messages
	SYSEX_REALTIME              = 0X7F // MIDI Reserved for realtime messages
};

enum FirmataPinMode
{
	INPUT,
	OUTPUT,
	ANALOG,
	PWM,
	SERVO,
	SHIFT,
	I2C,
	ONEWIRE,
	STEPPER,
	ENCODER,
	SERIAL,
	PULLUP
};

#endif // FIRMATA_DEFINES_H
