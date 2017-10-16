TEMPLATE = subdirs

SUBDIRS += \
	ARToolKit \
	Audio \
	Colour \
	Core \
	dlib \
	ffmpeg \
	FFTW \
	File \
	Firmata \
	GUI \
	Image \
	Kinect \
	LeapMotion \
	Lua \
	LuaQt \
	Math \
	MIDI \
	OculusRift \
	OpenCV \
	OpenGL \
	OSC \
	PortAudio \
	PortMidi \
	RaspberryPi \
	Network \
	Spout \
	Syphon \
	Text \
	Time \
	VideoCapture \
	VST \
	XInput \
    ISF

qtHaveModule( serialport ) {
	SUBDIRS += Serial
}

