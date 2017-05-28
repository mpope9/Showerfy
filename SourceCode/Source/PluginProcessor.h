/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "lockFreeFifo.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "FFTW/fftw3.h"


//==============================================================================
/**
*/
class ShowerfyAudioProcessor  : public AudioProcessor
{
public:
	//==============================================================================
	ShowerfyAudioProcessor();
	~ShowerfyAudioProcessor();

	//==============================================================================
	void prepareToPlay (double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

	void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

	//==============================================================================
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram (int index) override;
	const String getProgramName (int index) override;
	void changeProgramName (int index, const String& newName) override;

	//==============================================================================
	void getStateInformation (MemoryBlock& destData) override;
	void setStateInformation (const void* data, int sizeInBytes) override;

private:
	//==============================================================================

	/* Parameters */
	AudioParameterFloat* showerSoundGain;
	AudioParameterFloat* impulseResponseDelay;
	AudioParameterFloat* impulseResponseWetDry;
	AudioParameterBool*  impulseResponseChanLock;

	/* IR stuff. */
	File impulseResponseFile;
	AudioFormatReader* impulseResponseReader;
	AudioSampleBuffer impulseResponseBuffer;

	/* Shower Sound stuff. */
	File showerSoundFile;									/* The shower sound file*/
	AudioFormatReader* showerSoundReader;					/* Reader for the file, fills the buffer. */
	AudioSampleBuffer showerSoundBuffer;					/* A buffer of samples for the audio sample buffer. */
	int showerSoundPosition;								/* Position of the showerSound buffer */
	bool filesChosen;										/* If the files have been chosen yet. */

	/* FFT related stuff. */
	Array<float*> prepCopysIn;
	Array<fftwf_complex*> prepCopysOut;
	Array<fftwf_complex*> prepImpulseResponseTransformed;
	Array<float*> prepConvolvedInverse;
	Array<float*> prepFromFifo;
	Array<lockFreeFifo*> prepFifoChannels;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShowerfyAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
