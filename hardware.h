// ================================
// COMPANY: Feni Design
// AUTHOR: Dominic Frugoli
// CREATE DATE: 8/25/2024
// PROJECT: Fall 2024 Independent Project
// HEADER FILE FOR USE WITH: "sardinia.cpp"
// SPECIAL NOTES: Code is meant for Electrosmith Daisy microcontroller
// ================================


using namespace daisy;
using namespace daisy::seed;


// |Pin Declarations|------------------------------------------------------------------------------------------
// Setting up pin locations for all hardware

#define KnobMuxPin A0
#define keypadMuxPin_Row1 A1
#define keypadMuxPin_Row2 A2
#define keypadMuxPin_Row3 A3
#define keypadMuxPin_Row4 A4

#define muxConPin1 D1
#define muxConPin2 D2
#define muxConPin3 D3

#define encoderPinA D11
#define encoderPinB D10
#define encoderPinClick D12

#define playButtonPin D14 
#define recButtonPin D13


// |ADC Config Setup|------------------------------------------------------------------------------------------
// Setting up ADC system for Daisy

enum AdcChannel // Just add knobs before last NUM_ADC_CHANNELS as needed
{
	keypadMuxOne,
	knobMux,
	NUM_ADC_CHANNELS // Enum allows for auto tracking the amount
};


// |Hardware Object Instantiations|------------------------------------------------------------------------------------------
// Instantiating objects for hardware control classes

AdcChannelConfig adcConfig[NUM_ADC_CHANNELS]; // array size for number of ADC channels you need


Encoder encoder;
Switch playButton;
Switch recButton;


// |Variable Declarations|------------------------------------------------------------------------------------------
// Variables for all values that need to be stored

int encoderTurnVal;


// |Function Declarations|------------------------------------------------------------------------------------------
// Function declarations for any functions needed

void ProcessControlsKR(); // For controls that only need to be processed every audio block
void ProcessControlsAR(); // For controls that need to process every sample

void InitControls()
{
	adcConfig[keypadMuxOne].InitMux(keypadMuxPin_Row1, 8, muxConPin1, muxConPin2, muxConPin3);
	adcConfig[knobMux].InitMux(KnobMuxPin, 8, muxConPin1, muxConPin2, muxConPin3);

	playButton.Init(playButtonPin, 1000.0f);
	recButton.Init(recButtonPin, 1000.0f);
	encoder.Init(encoderPinA, encoderPinB, encoderPinClick);

}

bool EncoderTurned()
{
	float encoderInc = encoder.Increment();
	if(encoderInc != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool EncoderTurned(Encoder enc)
{
	float encoderInc = enc.Increment();
	if(encoderInc != 0)
	{
		encoderInc = 0;
		return true;
	}
	else
	{
		encoderInc = 0;
		return false;
	}
}