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
#include "effects.h"

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
float inputAmp = 1.0f;

float testValFloat = 0.0f;
int testValInt = 0;

void UpdateKeys();
void UpdateControlButtons();
void RecordControlButtonStates();
void ModeSelect();
void PrintMenu();


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
		inputSig *= inputAmp;
		
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
		outputMix = ProcessEffects(outputMix); // Process the mixed signal through the effects
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
	float sampleRate = hw.AudioSampleRate();

	InitLCD();
	InitControls();
	hw.adc.Init(adcConfig, NUM_ADC_CHANNELS); //initializes adc system using the config and number of channels
	hw.adc.Start();
	
	InitBuffer(); // Initialize buffer to all 0s
	InitEffects(sampleRate);

	//hw.StartLog(true); // Use for serial testing. COMMENT OUT WHEN NOT USING!!

	hw.StartAudio(AudioCallback);

	while(1)
	{
		PrintMenu(currentEffect, effectStates);
		//hw.PrintLine("Print a float value: %f", recordingLength);
		System::Delay(50);
	}
}


// |Offboard Control Functions|------------------------------------------------------------------------------------------


void ProcessControlsKR() // For controls that only need to be processed every audio block
{
	UpdateKeys();	

	encoder.Debounce();

	if(EncoderTurned())
	{
		IncrementEffect(encoder.Increment());
	}
	if(encoder.RisingEdge())
	{
		effectStates[currentEffect] = !effectStates[currentEffect];
	}

	if(currentEffect == 0)
	{
		effectValues[0][0] = fmap(hw.adc.GetMuxFloat(0, 2), 0, 1, Mapping::LINEAR); // Overdrive Gain
		effectValues[0][1] = fmap(hw.adc.GetMuxFloat(0, 6), .01, .3, Mapping::EXP); // Overdrive Tone Freq
	}
	else if(currentEffect == 1)
	{
		effectValues[1][0] = fmap(hw.adc.GetMuxFloat(0, 2), 0, 1, Mapping::LINEAR); // Chorus LFO Depth
		effectValues[1][1] = fmap(hw.adc.GetMuxFloat(0, 6), 0, 10, Mapping::EXP); // Chorus LFO Freq
	}
	else if(currentEffect == 2)
	{
		effectValues[2][0] = fmap(hw.adc.GetMuxFloat(0, 2), 0, 5000, Mapping::EXP); // LoPass Freq
		effectValues[2][1] = fmap(hw.adc.GetMuxFloat(0, 6), 0, 1, Mapping::LINEAR); // LoPass Res
	}
	

	outputAmp = fmap(hw.adc.GetMuxFloat(0, 3), 0, 2, Mapping::LINEAR); // top far right
	inputAmp = fmap(hw.adc.GetMuxFloat(0, 7), 0, 2, Mapping::LINEAR); // bottom far right

	readFactor = fmap(hw.adc.GetMuxFloat(0, 0), 0.5, 2, Mapping::EXP); // top far left
	ProcessEffectParameters();
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
		testValFloat = 1;
		lengthTrack = 0;
	}
	else if(FallingEdge_ControlButtons(1)) // Stops recording when let go, resets write index, and assigns recording length
	{
		isRecording = false;
		writeIndex = 0;
		recordingLength = lengthTrack;
		testValFloat = 0;
		testValInt = recordingLength;
		SetKeyIndexs();
	}

	if(RisingEdge_ControlButtons(4))
	{
		if(AnyKeyIsPressed())
		{
			StoreSpliceBufferOne();
		}
		
	}
	if(RisingEdge_ControlButtons(5))
	{
		if(AnyKeyIsPressed())
		{
			StoreSpliceBufferTwo();
		}
	}
	if(RisingEdge_ControlButtons(6))
	{
		if(AnyKeyIsPressed())
		{
			StoreSpliceBufferThree();
		}
	}
	if(RisingEdge_ControlButtons(7))
	{
		if(AnyKeyIsPressed())
		{
			StoreSpliceBufferFour();
		}
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
		UpdateMode(0);
	}
	else if(RisingEdge_ControlButtons(3))
	{
		UpdateMode(1);
	}
}




// |Audio Object Init Functions|------------------------------------------------------------------------------------------

	// Most of these are now in sampleengine.h

