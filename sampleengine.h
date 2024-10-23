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
bool keyStates[numKeyPads];
uint32_t startPoint;
uint32_t stopPoint;
int currentKey = 0;

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

void ResetReadIndex()
{
    readIndex = startPoint;
}

float GetSample()
{
    float a, b, out;

    readIndexInt = static_cast<int32_t>(readIndex);
	readIndexFraction = readIndex - readIndexInt;

	a = buffer[readIndexInt]; // current sample
	b = buffer[readIndexInt + 1]; // next sample
	out = a + (b - a) * readIndexFraction; // Linear Interpolation

    return out;
}

bool AnyKeyIsPressed()
{
    for(int i = 0; i < numKeyPads; i++)
    {
        if(keyStates[i])
        {
            return true;
        }
    }
    return false;
}

int FindNextPressedKey(int key)
{
    int nextKey = key;
    for(int i = 0; i < numKeyPads; i++)
    {
        nextKey++;
        if(nextKey >= numKeyPads)
        {
            nextKey = 0;
        }
        if(keyStates[nextKey])
        {
            break;
        }
    }
    return nextKey;
}

void SetKeyIndexs()
{
    float fraction;
    auto recordingLengthFloat = static_cast<float>(recordingLength);
    for(int i = 0; i < numKeyPads; i++)
    {
        fraction = static_cast<float>(i) / static_cast<float>(numKeyPads);
        keyStartIndex[i] = static_cast<uint32_t>((recordingLengthFloat * fraction) + 1.0f);
        fraction = static_cast<float>(i + 1) / static_cast<float>(numKeyPads);
        keyStopIndex[i] = static_cast<uint32_t>(recordingLengthFloat * fraction);
    }
    currentKey = FindNextPressedKey(-1);
    startPoint = keyStartIndex[currentKey];
    stopPoint = keyStopIndex[currentKey];
    ResetReadIndex();
}

void AdvanceReadIndex()
{
    readIndex += readFactor;

    if(readIndex >= stopPoint)
    {
        currentKey = FindNextPressedKey(currentKey);
        startPoint = keyStartIndex[currentKey];
        stopPoint = keyStopIndex[currentKey];
        ResetReadIndex();
    }
}

uint32_t GetStartPoint()
{
    return keyStartIndex[1];
}

uint32_t GetStopPoint() // check if it works with just one stop point then change it
{
    return keyStopIndex[1];
}
