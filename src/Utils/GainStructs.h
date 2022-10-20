#pragma once

// Classes used to represent camera gain values in the CVG system. 
// In practice they're only used for the Rpi (for now) but we'll attempt
// to keep their data as general as possible - as if it could extended
// for other polling methods beyond RPi MMAL.

struct WhiteBalanceGain
{
	float redGain;
	float blueGain;
};

struct CameraGain
{
	float digitalGain;
	float analogGain;
};