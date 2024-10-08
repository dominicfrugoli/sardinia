// ================================
// COMPANY: Feni Design
// AUTHOR: Dominic Frugoli
// CREATE DATE: 8/18/2024
// PROJECT: Fall 2024 Independent Project
// HEADER FILES NEEDED: "hardware.h"
// SPECIAL NOTES: Code is meant for Electrosmith Daisy microcontroller
// ================================


// |Includes and Namespaces|------------------------------------------------------------------------------------------


#include "daisy_seed.h"
#include "daisysp.h"
#include "hardware.h"
#include "sampleengine.h"
#include "display.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;


// |Hardware Instantiations|------------------------------------------------------------------------------------------


DaisySeed hw;


// |Audio Objects + Variables|------------------------------------------------------------------------------------------

float outputMix;
float sampleSig;
float inputSig;
float outputAmp = 0.5f;

int testVal;

uint32_t GetStartIndex();
uint32_t GetStopIndex();


// |Main Setup and Audio Callback|------------------------------------------------------------------------------------------


void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	ProcessControlsKR();

	for (size_t i = 0; i < size; i++)
	{
		sampleSig = 0; // Reset signal values to 0 in case no sample is being played
		outputMix = 0;

		ProcessControlsAR();

		inputSig = (in[0][i] + in[1][i]) * 0.5f;
		
		if(isRecording)
		{
			RecordSample((inputSig)); // Record the input and increment the sample length tracker
			lengthTrack++;
		}

		if(isPlaying)
		{
			sampleSig = GetSample();
			if(readIndex > recordingLength) // recordingLength dynamically changes the size of sample based on recording
			{
				readIndex = 0;
			}
		}

		
		outputMix = inputSig + sampleSig; // Mix the sample playback with the input
		outputMix *= outputAmp;

		// Output the mixed signals
        out[0][i] = outputMix;
		out[1][i] = outputMix;
	}
}

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	//float sampleRate = hw.AudioSampleRate();

	InitLCD();
	InitControls();
	hw.adc.Init(adcConfig, NUM_ADC_CHANNELS); //initializes adc system using the config and number of channels
	hw.adc.Start();
	
	InitBuffer(); // Initialize buffer to all 0s

	//hw.StartLog(true); // Use for serial testing. COMMENT OUT WHEN NOT USING!!

	hw.StartAudio(AudioCallback);

	while(1)
	{
		lcd.SetCursor(0, 0);
		PrintMenu();
		lcd.SetCursor(1, 0);
		//lcd.PrintInt(testVal); // UNCOMMENT IF WANTING TO TEST A VALUE (USE "testVal" VARIABLE)

		System::Delay(50);
	}
}


// |Offboard Control Functions|------------------------------------------------------------------------------------------


void ProcessControlsKR() // For controls that only need to be processed every audio block
{
	recButton.Debounce();
	playButton.Debounce();
	encoder.Debounce();

	if(recButton.RisingEdge()) // Starts recording from start on press and resets the length track
	{
		isRecording = true;
		lengthTrack = 0;
	}
	else if(recButton.FallingEdge()) // Stops recording when let go, resets write index, and assigns recording length
	{
		isRecording = false;
		writeIndex = 0;
		recordingLength = lengthTrack;
		SetKeyIndexs();
		startPoint = GetStartPoint();
		stopPoint = GetStopPoint();
	}

	if(playButton.RisingEdge()) // Starts the sample from the beginning when pressed
	{
		readIndex = 0;
	}

	encoderTurnVal = encoder.Increment();
	if(encoderTurnVal != 0)
	{
		IncrementMenu(encoderTurnVal);
		testVal = encoderTurnVal;
	}

	outputAmp = fmap(hw.adc.GetMuxFloat(1, 0), 0, 2, Mapping::LINEAR); // bottom far left
	readFactor = fmap(hw.adc.GetMuxFloat(1, 1), 0.5, 2, Mapping::EXP); // top far left
}

void ProcessControlsAR() // For controls that need to process every sample
{
	playButton.Debounce();

	if(playButton.Pressed()) // Play button is a toggle for the sample
	{
		isPlaying = true;
	}
	else
	{
		if(readIndex == 0)
		{
			isPlaying = false;
		}
	}
}

uint32_t GetStartIndex()
{
	bool foundVal = false;
	uint32_t startIndex = 0;

	for(int i = 0; i < numKeyPads; i++)
	{
		if(hw.adc.GetMuxFloat(0,i) == 0.0f)
		{
			startIndex = keyStartIndex[i];
			foundVal = true;
		}

		if(foundVal)
		{
			break;
		}
	}

	return startIndex;
}

uint32_t GetStopIndex()
{
	bool foundVal = false;
	uint32_t stopIndex = recordingLength;

	for(int i = 0; i < numKeyPads; i++)
	{
		if(hw.adc.GetMuxFloat(0,i) == 0.0f)
		{
			stopIndex = keyStopIndex[i];
			foundVal = true;
		}

		if(foundVal)
		{
			break;
		}
	}

	return stopIndex;
}



// |Audio Object Init Functions|------------------------------------------------------------------------------------------

	// Most of these are now in sampleengine.h

