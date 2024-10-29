// ================================
// COMPANY: Feni Design
// AUTHOR: Dominic Frugoli
// CREATE DATE: 10/29/2024
// PROJECT: Fall 2024 Independent Project
// HEADER FILE FOR USE WITH: "sardinia.cpp"
// SPECIAL NOTES: Code is meant for Electrosmith Daisy microcontroller
// ================================


using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;


// |Audio Objects|------------------------------------------------------------------------------------------


Chorus chorus;
Overdrive overdrive;
OnePole odTone;
Svf lopass;


// |Audio Variables|------------------------------------------------------------------------------------------

int currentEffect = 0; // 0: overdrive, 1: chorus, 2: LoPass
bool effectStates[3] = {false, false, false}; // 0: overdrive, 1: chorus, 2: LoPass

float effectValues[3][2] = {{0.5f, 550.0f}, {0.5f, 1.0f}, {1000.0f, 0.25f}};
/*
    {[0][0]: overdrive gain, [0][1] overdrive tone freq}
    {[1][0]: chorus lfo depth, [1][1] chorus lfo freq}
    {[2][0]: LoPass cutoff, [2][1] LoPass resonance}
*/


// |Init Functions|------------------------------------------------------------------------------------------


void InitEffects(float samplerate)
{
    // Overdrive
    overdrive.Init();
    odTone.Init();
    overdrive.SetDrive(effectValues[0][0]);
    odTone.SetFilterMode(OnePole::FILTER_MODE_LOW_PASS);
    odTone.SetFrequency(effectValues[0][1]);
    // Chorus
    chorus.Init(samplerate);
    chorus.SetLfoDepth(effectValues[1][0]);
    chorus.SetLfoFreq(effectValues[1][1]);
    // LoPass
    lopass.Init(samplerate);
    lopass.SetFreq(effectValues[2][0]);
    lopass.SetRes(effectValues[2][1]);
}

float ProcessEffects(float sample)
{
    float output = sample;

    if(effectStates[0]) // Overdrive
    {
        output = overdrive.Process(output) * (1.1f - effectValues[0][0]);
        output = odTone.Process(output);
    }
    if (effectStates[1]) // Chorus
    {
        output = chorus.Process(output) * 2.0f;
    }
    if(effectStates[2]) // LoPass
    {
        lopass.Process(output);
        output = lopass.Low();
    }   

    return output;
}

void ProcessEffectParameters()
{
    if(currentEffect == 0)
    {
        overdrive.SetDrive(effectValues[0][0]);
        odTone.SetFrequency(effectValues[0][1]);
    }
    else if(currentEffect == 1)
    {
        chorus.SetLfoDepth(effectValues[1][0]);
        chorus.SetLfoFreq(effectValues[1][1]);
    }
    else if(currentEffect == 2)
    {
        lopass.SetFreq(effectValues[2][0]);
        lopass.SetRes(effectValues[2][1]);
    }
}

void IncrementEffect(int increment)
{
    currentEffect += increment;
    if(currentEffect < 0)
    {
        currentEffect = 2;
    }
    else if(currentEffect > 2)
    {
        currentEffect = 0;
    }
}
