/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
UniversalCombFilterAudioProcessor::UniversalCombFilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    delayBuffer(2,1)
{
    addParameter(sweepWidth = new AudioParameterFloat("sweepwidth", "Sweep Width", NormalisableRange<float>(0.0f, 0.05f), 0.002f));
    addParameter(lfoFreq = new AudioParameterFloat("lfofreq", "LFO Frequency", 0.0f, 250.0f, 0.5f));
    addParameter(bleed = new AudioParameterFloat("bleed", "Bleed", 0.0f, 1.0f, 0.7f));
    addParameter(feedforward = new AudioParameterFloat("feedforward", "Feedforward", 0.0f, 1.0f, 0.7f));
    addParameter(feedback = new AudioParameterFloat("feedback", "Feedback", 0.0f, 1.0f, 0.7f));
    addParameter(delay = new AudioParameterFloat("delay", "Minimum Delay", 0.0f, 0.5f, 0.0f));
    addParameter(tremolo = new AudioParameterBool("tremolo", "Tremolo", false));
    
    delayRead = 0; delayWrite = 0;
}

UniversalCombFilterAudioProcessor::~UniversalCombFilterAudioProcessor()
{
}

//==============================================================================
const juce::String UniversalCombFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UniversalCombFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool UniversalCombFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UniversalCombFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UniversalCombFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UniversalCombFilterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UniversalCombFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void UniversalCombFilterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String UniversalCombFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void UniversalCombFilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void UniversalCombFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Initialize delay buffer
    delayBufferLength = (int)(0.55*sampleRate) + 3; // max 0.55s delay
    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();
    lfoPhase = 0.0f;
    
    samplePeriod = 1.0f/sampleRate;
}

void UniversalCombFilterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UniversalCombFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void UniversalCombFilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    float dpr, ph;
    double currentDelay;
    int dpw;
    
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);
    
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto* delayData = delayBuffer.getWritePointer(jmin(channel, delayBuffer.getNumChannels()-1));

        dpw = delayWrite;
        ph = lfoPhase;
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float in = channelData[sample];
            float interpolated = 0.0f;
            float out = 0.0f;
            float lfo = 0.5f + 0.5f*sinf(2.0f*M_PI*ph);
            
            // computing M[n]
            currentDelay = (float)*delay + *sweepWidth*lfo;
            
            dpr = fmodf((float)dpw - (float)(currentDelay*getSampleRate()) + (float)delayBufferLength - 3.0f, (float)delayBufferLength);
            
            // linear interpolation
            float frac = dpr - floorf(dpr);
            int prev = (int)floorf(dpr);
            int next = (prev + 1) % delayBufferLength;
            interpolated = (1.0f-frac)*delayData[prev] + frac*delayData[next];
            
            /*
            // cubic interpolation
            int prev1 = floorf(dpr);                                            // n
            int prev2 = (prev1 - 1 + delayBufferLength) % delayBufferLength;    // n - 1
            int next1 = (prev1 + 1) % delayBufferLength;                        // n + 1
            int next2 = (next1 + 1) % delayBufferLength;                        // n + 2
            
            float frac = dpr - prev1;
            float frac2 = frac*frac;
            float frac3 = frac2*frac;
            
            float c0 = delayData[prev1];
            float c1 = delayData[next1] - delayData[prev1];
            float c2 = delayData[prev2] - delayData[prev1];
            float c3 = delayData[prev1] - delayData[prev2] - delayData[next1] + delayData[next2];
            
            interpolated = c3*frac3 + c2*frac2 + c1*frac + c0;
            */
            
            float xh = in + *feedback*interpolated;         // xh[n] = x[n] + fb*xh[n-M]
            out = *bleed*xh + *feedforward*interpolated;    // y[n] = bl*xh[n] + ff*xh[n-M]
            delayData[dpw] = xh;
        
            // apply tremolo if toggled
            *tremolo ? channelData[sample] = lfo*out : channelData[sample] = out;
            
            // increment write pointer, loop if necessary
            dpw = (dpw + 1) % delayBufferLength;
            
            // increment LFO phase by Ts = 1/fs
            if (*lfoFreq == 0 && ph > 0.01) {
                // resets phase after frequency is set to 0. maybe this should be a toggle on/off instead?
                ph = fmodf(ph + 0.05*samplePeriod, 1.0f);
            } else if (*lfoFreq != 0) {
                ph = fmodf(ph + *lfoFreq*samplePeriod, 1.0f);
            }
        }
    }
    
    // update permanent variables
    delayWrite = dpw;
    lfoPhase = ph;
}

//==============================================================================
bool UniversalCombFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* UniversalCombFilterAudioProcessor::createEditor()
{
    return new UniversalCombFilterAudioProcessorEditor (*this);
}

//==============================================================================
void UniversalCombFilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void UniversalCombFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UniversalCombFilterAudioProcessor();
}
