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

#define MAX_RECORD_SIZE (48000 * 120) // 60 seconds of floats at 48 khz
#define MAX_SPLICE_SIZE (48000 * 20) // 5 seconds of floats at 48 khz
float DSY_SDRAM_BSS recordingBuffer[MAX_RECORD_SIZE];
float DSY_SDRAM_BSS spliceBufferOne[MAX_SPLICE_SIZE];
float DSY_SDRAM_BSS spliceBufferTwo[MAX_SPLICE_SIZE];
float DSY_SDRAM_BSS spliceBufferThree[MAX_SPLICE_SIZE];
float DSY_SDRAM_BSS spliceBufferFour[MAX_SPLICE_SIZE];

float readIndex = 0; // index for reading buffer
uint32_t readIndexInt;
float readIndexFraction;
float readFactor = 1; // playback speed
float readPitch;
uint32_t writeIndex = 0; // index for writing into buffer
uint32_t recordingLength = MAX_RECORD_SIZE; // tracks size of sample recording
uint32_t lengthTrack = 0; // for tracking recording size while recording

float fullSampleReadIndex = 0.0f;
uint32_t fullSampleReadIndexInt;
float fullSampleReadIndexFraction;

uint32_t keyStartIndex[numKeyPads];
uint32_t keyStopIndex[numKeyPads];
bool keyStates[numKeyPads];
uint32_t startPoint = 0;
uint32_t stopPoint = MAX_RECORD_SIZE;
int currentKey = 0;

bool isRecording = false;
bool isPlaying = false;

bool SpliceBufferOneIsPlaying = false;

float spliceBufferOneReadIndex = 0.0f;
uint32_t spliceBufferOneReadIndexInt;
float spliceBufferOneReadIndexFraction;
uint32_t spliceBufferOneLength = MAX_SPLICE_SIZE;


// |Init Functions|------------------------------------------------------------------------------------------

void InitBuffer();
int FindNextPressedKey(int key);
void RecordSample(float sample);
void ResetReadIndex();
void AdvanceReadIndex();
float GetSample();
float FullSampleGetSample();
void StoreSpliceBufferOne();
void StoreSpliceBufferTwo();
void StoreSpliceBufferThree();
void StoreSpliceBufferFour();
bool AnyKeyIsPressed();
void SetKeyIndexs();
uint32_t GetStartPoint();
uint32_t GetStopPoint();

void InitBuffer()
{
    readIndex = 0.0f;
    for(uint32_t i = 0; i < MAX_RECORD_SIZE; i++) // initializes buffer to all 0s
    {
        recordingBuffer[i] = 0.0f;
    }
    for (uint32_t i = 0; i < MAX_SPLICE_SIZE; i++)
    {
        spliceBufferOne[i] = 0.0f;
        spliceBufferTwo[i] = 0.0f;
        spliceBufferThree[i] = 0.0f;
        spliceBufferFour[i] = 0.0f;
    }
}

void RecordSample(float sample) // Records sample from argument and moves forward write index
{
    recordingBuffer[writeIndex] = sample;
    writeIndex++;
}

void ResetReadIndex()
{
    readIndex = startPoint;
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

float GetSample()
{
    float a, b, out;

    readIndexInt = static_cast<int32_t>(readIndex);
	readIndexFraction = readIndex - readIndexInt;

	a = recordingBuffer[readIndexInt]; // current sample
	b = recordingBuffer[readIndexInt + 1]; // next sample
	out = a + (b - a) * readIndexFraction; // Linear Interpolation

    return out;
}

float FullSampleGetSample()
{
    fullSampleReadIndex += readFactor;
    if(fullSampleReadIndex > recordingLength) 
    {
        fullSampleReadIndex = 0;
    }

    float a, b, out;
    fullSampleReadIndexInt = static_cast<int32_t>(fullSampleReadIndex);
    fullSampleReadIndexFraction = fullSampleReadIndex - fullSampleReadIndexInt;
    a = recordingBuffer[fullSampleReadIndexInt]; // current sample
    b = recordingBuffer[fullSampleReadIndexInt + 1]; // next sample
    out = a + (b - a) * fullSampleReadIndexFraction; // Linear Interpolation

    return out;
}

void StoreSpliceBufferOne()
{
    readIndex = startPoint;
    float startSample = startPoint;
    for (uint32_t i = 0; i < MAX_SPLICE_SIZE; i++)
    {
        spliceBufferOne[i] = GetSample();
        AdvanceReadIndex();
        if(readIndex == startSample)
        {
            spliceBufferOneLength = i;
            break;
        }
    }
    readIndex = startPoint;
}

float SpliceBufferOneGetSample()
{
    spliceBufferOneReadIndex += readFactor;
    if(spliceBufferOneReadIndex > spliceBufferOneLength) 
    {
        spliceBufferOneReadIndex = 0;
    }

    float a, b, out;
    spliceBufferOneReadIndexInt = static_cast<int32_t>(spliceBufferOneReadIndex);
    spliceBufferOneReadIndexFraction = spliceBufferOneReadIndex - spliceBufferOneReadIndexInt;
    a = spliceBufferOne[spliceBufferOneReadIndexInt]; // current sample
    b = spliceBufferOne[spliceBufferOneReadIndexInt + 1]; // next sample
    out = a + (b - a) * spliceBufferOneReadIndexFraction; // Linear Interpolation

    return out;
}

void StoreSpliceBufferTwo()
{
    readIndex = startPoint;
    float startSample = startPoint;
    for (uint32_t i = 0; i < MAX_SPLICE_SIZE; i++)
    {
        spliceBufferTwo[i] = GetSample();
        AdvanceReadIndex();
        if(readIndex == startSample)
        {
            break;
        }
    }
    readIndex = startPoint;
}

void StoreSpliceBufferThree()
{
    readIndex = startPoint;
    float startSample = startPoint;
    for (uint32_t i = 0; i < MAX_SPLICE_SIZE; i++)
    {
        spliceBufferThree[i] = GetSample();
        AdvanceReadIndex();
        if(readIndex == startSample)
        {
            break;
        }
    }
    readIndex = startPoint;
}

void StoreSpliceBufferFour()
{
    readIndex = startPoint;
    float startSample = startPoint;
    for (uint32_t i = 0; i < MAX_SPLICE_SIZE; i++)
    {
        spliceBufferFour[i] = GetSample();
        AdvanceReadIndex();
        if(readIndex == startSample)
        {
            break;
        }
    }
    readIndex = startPoint;
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

uint32_t GetStartPoint()
{
    return keyStartIndex[1];
}

uint32_t GetStopPoint() // check if it works with just one stop point then change it
{
    return keyStopIndex[1];
}
