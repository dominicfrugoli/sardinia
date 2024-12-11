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

#define numKeyPads 32

#define knobMuxPin A0
#define keypadMuxPin_Row1 A1
#define keypadMuxPin_Row2 A2
#define keypadMuxPin_Row3 A3
#define keypadMuxPin_Row4 A4

#define muxConPin1 D1
#define muxConPin2 D2
#define muxConPin3 D3

#define encoderPinB D4
#define encoderPinA D5
#define encoderPinClick D6 

#define controlButtonPin_One D7
#define controlButtonPin_Two D8
#define controlButtonPin_Three D9
#define controlButtonPin_Four D10
#define controlButtonPin_Five D11
#define controlButtonPin_Six D12
#define controlButtonPin_Seven D13
#define controlButtonPin_Eight D14

Pin controlButtonPins[8] = {controlButtonPin_One, controlButtonPin_Two, controlButtonPin_Three, controlButtonPin_Four, controlButtonPin_Five, controlButtonPin_Six, controlButtonPin_Seven, controlButtonPin_Eight};



// |ADC Config Setup|------------------------------------------------------------------------------------------
// Setting up ADC system for Daisy

enum AdcChannel // Just add knobs before last NUM_ADC_CHANNELS as needed
{
	knobMux,
	keypadMuxOne,
	keypadMuxTwo,
	keypadMuxThree,
	keypadMuxFour,
	NUM_ADC_CHANNELS // Enum allows for auto tracking the amount
};


// |Hardware Object Instantiations|------------------------------------------------------------------------------------------
// Instantiating objects for hardware control classes


AdcChannelConfig adcConfig[NUM_ADC_CHANNELS]; // array size for number of ADC channels you need

Switch controlButtons[8];

Encoder encoder;


// |Variable Declarations|------------------------------------------------------------------------------------------
// Variables for all values that need to be stored


int encoderTurnVal;

// 0: play, 1: record, 2: ???, 3: ???, 4: ModeOne, 5: ModeTwo, 6: ModeThree, 7: ModeFour
bool controlButtonStates[8] = {false, false, false, false, false, false, false, false};
bool prevControlButtonStates[8] = {false, false, false, false, false, false, false, false};

bool playButtonOverrule; // For when the playing sample off of play button pressed (overrules keypad)


// |Function Declarations|------------------------------------------------------------------------------------------
// Function declarations for any functions needed

void ProcessControlsKR(); // For controls that only need to be processed every audio block
void ProcessControlsAR(); // For controls that need to process every sample

void InitControls()
{
	adcConfig[knobMux].InitMux(knobMuxPin, 8, muxConPin1, muxConPin2, muxConPin3);
	adcConfig[keypadMuxOne].InitMux(keypadMuxPin_Row1, 8, muxConPin1, muxConPin2, muxConPin3);
	adcConfig[keypadMuxTwo].InitMux(keypadMuxPin_Row2, 8, muxConPin1, muxConPin2, muxConPin3);
	adcConfig[keypadMuxThree].InitMux(keypadMuxPin_Row3, 8, muxConPin1, muxConPin2, muxConPin3);
	adcConfig[keypadMuxFour].InitMux(keypadMuxPin_Row4, 8, muxConPin1, muxConPin2, muxConPin3);

	for(int i = 0; i < 8; i++)
	{
		controlButtons[i].Init(controlButtonPins[i], 1000);
	}

	encoder.Init(encoderPinA, encoderPinB, encoderPinClick);
}

bool RisingEdge(bool buttonState, bool previousButtonState)
{
	if(buttonState == true && previousButtonState == false)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool FallingEdge(bool buttonState, bool previousButtonState)
{
	if(buttonState == false && previousButtonState == true)
	{
		return true;
	}
	else
	{
		return false;
	}
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