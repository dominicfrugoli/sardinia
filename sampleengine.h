// ================================
// COMPANY: Feni Design
// AUTHOR: Dominic Frugoli
// CREATE DATE: 9/15/2024
// PROJECT: Fall 2024 Independent Project
// HEADER FILE FOR USE WITH: "sardinia.cpp"
// SPECIAL NOTES: Code is meant for Electrosmith Daisy microcontroller
// ================================


using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;


// |Audio Objects|------------------------------------------------------------------------------------------


#define numKeyPads 8
#define MAX_SIZE (48000 * 60) // 60 seconds of floats at 48 khz
float DSY_SDRAM_BSS buffer[MAX_SIZE];

float readIndex = 0; // index for reading buffer
uint32_t readIndexInt;
float readIndexFraction;
float readFactor = 1; // playback speed
float readPitch;
uint32_t writeIndex = 0; // index for writing into buffer
uint32_t recordingLength = MAX_SIZE; // tracks size of sample recording
uint32_t lengthTrack = 0; // for tracking recording size while recording

uint32_t keyStartIndex[numKeyPads];
uint32_t keyStopIndex[numKeyPads];
uint32_t startPoint;
uint32_t stopPoint;

bool isRecording = false;
bool isPlaying = false;


// |Init Functions|------------------------------------------------------------------------------------------


void InitBuffer()
{
    readIndex = 0.0f;
    for(uint32_t i = 0; i < MAX_SIZE; i++) // initializes buffer to all 0s
    {
        buffer[i] = 0.0f;
    }
}

void RecordSample(float sample) // Records sample from argument and moves forward write index
{
    buffer[writeIndex] = sample;
    writeIndex++;
}

float GetSample()
{
    float a, b, out;

    readIndex += readFactor; // increase index by play speed

    readIndexInt = static_cast<int32_t>(readIndex);
	readIndexFraction = readIndex - readIndexInt;

	a = buffer[readIndexInt]; // current sample
	b = buffer[readIndexInt + 1]; // next sample
	out = a + (b - a) * readIndexFraction; // Linear Interpolation

    return out;
}

void SetKeyIndexs()
{
    for(int i = 0; i < numKeyPads; i++)
    {
        keyStartIndex[i] = static_cast<uint32_t>((recordingLength * (i/numKeyPads)) + 1);
        keyStopIndex[i] = static_cast<uint32_t>(recordingLength * ((i+1)/numKeyPads));
    }
}

uint32_t GetStartPoint() 
{
    return keyStartIndex[0];
}

uint32_t GetStopPoint() // check if it works with just one stop point then change it
{
    return keyStopIndex[0];
}
