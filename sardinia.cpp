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

float testValFloat = 0.0f;
int testValInt = 0;

void UpdateKeys();
void UpdateControlButtons();
void RecordControlButtonStates();
void ModeSelect();


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

		if(isPlaying && AnyKeyIsPressed() == false) // If no keys are pressed, play the full sample
		{
			sampleSig = FullSampleGetSample();
		}
		else if(isPlaying)
		{
			sampleSig = GetSample();
			AdvanceReadIndex();
			if(readIndex > recordingLength) // recordingLength dynamically changes the size of sample based on recording
			{
				readIndex = 0;
			}
		}
		else if(SpliceBufferOneIsPlaying)
		{
			sampleSig = SpliceBufferOneGetSample();
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
		lcd.PrintInt(testValFloat);
		lcd.SetCursor(1, 8);
		lcd.PrintInt(testValInt);
		



		//hw.PrintLine("Print a float value: %f", recordingLength);

		System::Delay(50);
	}
}


// |Offboard Control Functions|------------------------------------------------------------------------------------------


void ProcessControlsKR() // For controls that only need to be processed every audio block
{
	UpdateKeys();	


	outputAmp = fmap(hw.adc.GetMuxFloat(0, 0), 0, 2, Mapping::LINEAR); // bottom far left
	testValFloat = outputAmp * 10;
	readFactor = fmap(hw.adc.GetMuxFloat(0, 1), 0.5, 2, Mapping::EXP); // top far left
}

void ProcessControlsAR() // For controls that need to process every sample
{
	// All control keys must be updated at same clock cycle 
	// So all control key functions must be done at audio rate
	UpdateControlButtons(); 
	ModeSelect();


	if(RisingEdge_ControlButtons(0)) // Starts the sample from the beginning when pressed
	{
		if(isPlaying == false)
		{
			fullSampleReadIndex = 0;
			isPlaying = true;
			playButtonOverrule = true;
		}
		else
		{
			isPlaying = false;
			playButtonOverrule = false;
		}
	}
	else if(fullSampleReadIndex== 0 && !controlButtonStates[0]) // Stops the sample when let go
	{
		isPlaying = false;
		playButtonOverrule = false;
	}

	if(playButtonOverrule == false)
	{
		if(AnyKeyIsPressed())
		{
			isPlaying = true;
		}
		else
		{
			isPlaying = false;
		}
	}

	if(RisingEdge_ControlButtons(2))
	{
		SpliceBufferOneIsPlaying = true;
	}
	else if(spliceBufferOneReadIndex == 0 && !controlButtonStates[2])
	{
		SpliceBufferOneIsPlaying = false;
	}


	if(RisingEdge_ControlButtons(1)) // Starts recording from start on press and resets the length track
	{
		isRecording = true;
		testValInt = 1;
		lengthTrack = 0;
	}
	else if(FallingEdge_ControlButtons(1)) // Stops recording when let go, resets write index, and assigns recording length
	{
		isRecording = false;
		writeIndex = 0;
		recordingLength = lengthTrack;
		testValInt = recordingLength;
		SetKeyIndexs();
	}

	if(RisingEdge_ControlButtons(4))
	{
		StoreSpliceBufferOne();
	}
	if(RisingEdge_ControlButtons(5))
	{
		StoreSpliceBufferTwo();
	}
	if(RisingEdge_ControlButtons(6))
	{
		StoreSpliceBufferThree();
	}
	if(RisingEdge_ControlButtons(7))
	{
		StoreSpliceBufferFour();
	}

	RecordControlButtonStates();
}

void UpdateKeys()
{
	for(int i = 0; i < 8; i++)
	{
		keyStates[i] = hw.adc.GetMuxFloat(1, i) < 0.5f;
	}
	for(int i = 8; i < 16; i++)
	{
		keyStates[i] = hw.adc.GetMuxFloat(2, i - 8) < 0.5f;
	}
	for(int i = 16; i < 24; i++)
	{
		keyStates[i] = hw.adc.GetMuxFloat(3, i - 16) < 0.5f;
	}
	for(int i = 24; i < 32; i++)
	{
		keyStates[i] = hw.adc.GetMuxFloat(4, i - 24) < 0.5f;
	}
}

void UpdateControlButtons()
{
	for(int i = 0; i < 8; i++)
	{
		controlButtonStates[i] = hw.adc.GetMuxFloat(5, i) < 0.5f;
	}
}

void RecordControlButtonStates()
{
	for(int i = 0; i < 8; i++)
	{
		prevControlButtonStates[i] = controlButtonStates[i];
	}
}

void ModeSelect()
{
	if(RisingEdge_ControlButtons(2)) // Adjusts the output amplitude based on the knob
	{
		UpdateMenu(0);
	}
	else if(RisingEdge_ControlButtons(3))
	{
		UpdateMenu(1);
	}
}


// |Audio Object Init Functions|------------------------------------------------------------------------------------------

	// Most of these are now in sampleengine.h

