/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================

/*	Constructor.
	I added the default ctor for the fft here.  The preprocessor part was generated
	by JUCE.
*/
ShowerfyAudioProcessor::ShowerfyAudioProcessor()
	: showerSoundPosition(0),										/* Initial shower sound pos is 0 */
#ifndef JucePlugin_PreferredChannelConfigurations
	AudioProcessor (BusesProperties()
					 #if ! JucePlugin_IsMidiEffect
					  #if ! JucePlugin_IsSynth
					   .withInput  ("Input",  AudioChannelSet::stereo(), true)
					  #endif
					   .withOutput ("Output", AudioChannelSet::stereo(), true)
					 #endif
					   )
#endif
{
	/* TODO: ADD OS DETECTION AND DYNAMIC PATH LOOK UP TO THE WAV FILES. */

	/* Manager for files. */
	AudioFormatManager manager;
	manager.registerBasicFormats();

	/* Shower sound buffer initialization. */
	showerSoundFile = File(showerSoundPath);
	showerSoundReader = manager.createReaderFor(showerSoundFile);
	showerSoundBuffer.setSize(showerSoundReader->numChannels, showerSoundReader->lengthInSamples);
	showerSoundReader->read(&showerSoundBuffer, 0, showerSoundReader->lengthInSamples, 0, true, true);

	/* Load IR */
	impulseResponseFile = File(impulseResponsePath);
	impulseResponseReader = manager.createReaderFor(impulseResponseFile);
	impulseResponseBuffer.setSize(impulseResponseReader->numChannels, impulseResponseReader->lengthInSamples);
	impulseResponseReader->read(&impulseResponseBuffer, 0, impulseResponseBuffer.getNumSamples(), 0, true, true);


	/* Audio Parameters. */
	addParameter(showerSoundGain = new AudioParameterFloat ("showerSoundGain",		/* param ID */
															"Shower Sound Gain",	/* param name */
															0.0f,					/* min value */
															1.0f,					/* max value */
															0.0f));					/* default value */
	addParameter(impulseResponseDelay = new AudioParameterFloat("impulseResponseDelay",
																"Impulse Response Delay",
																0.0f,
																1.0f,
																0.0f));
	addParameter(impulseResponseWetDry = new AudioParameterFloat(	"impulseResponseWetDry",
																	"Impulse Response Wet/Dry",
																	0.0f,
																	1.0f,
																	0.0f));
}

ShowerfyAudioProcessor::~ShowerfyAudioProcessor()
{
	//delete impulseResponseTransformed;
	delete impulseResponseReader;
	delete showerSoundReader;
}

//==============================================================================
const String ShowerfyAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool ShowerfyAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
	return true;
   #else
	return false;
   #endif
}

bool ShowerfyAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
	return true;
   #else
	return false;
   #endif
}

double ShowerfyAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int ShowerfyAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int ShowerfyAudioProcessor::getCurrentProgram()
{
	return 0;
}

void ShowerfyAudioProcessor::setCurrentProgram (int index)
{
}

const String ShowerfyAudioProcessor::getProgramName (int index)
{
	return String();
}

void ShowerfyAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ShowerfyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	/* DAMN YOU PREPARETOPLAY BEING CALLED MORE THAN ONCE! */
	/* The amount of hassle cause by this is unnecessary. */

	int IRBS = impulseResponseBuffer.getNumSamples();
	int fftSize = IRBS + samplesPerBlock;

	/* Init the float array for pre-transformation of the IR. */
	/* Mono channel.  Radical. */
	const float* readPtrs = impulseResponseBuffer.getReadPointer(0);
	float* impulseResponseFloats = new float[fftSize * 2];

	int j;
	for (j = 0; j < IRBS; ++j)
		impulseResponseFloats[j] = readPtrs[j];
	for (j = IRBS; j < fftSize * 2; ++j)
		impulseResponseFloats[j] = 0;

	/* Perform the transformation on the IR, convert to FFT::Complex */
	FFT impulseResponseTransformer(log2(fftSize), false);
	impulseResponseTransformer.performRealOnlyForwardTransform(impulseResponseFloats);
	FFT::Complex* impulseResponseComplex = new FFT::Complex[fftSize];
	int tracker = 0;
	int i;
	for (i = 0; i < fftSize * 2; i += 2)
	{
		impulseResponseComplex[tracker].r = impulseResponseFloats[i];
		impulseResponseComplex[tracker].i = impulseResponseFloats[i + 1];
		++tracker;
	}

	/* Initialization of everything needed for the prepareToPlay */
	prepCopys.add(new float[fftSize * 2]);
	prepImpulseResponseTransformed.add(impulseResponseComplex);
	prepFFTs.add(new FFT(log2(fftSize), false));
	prepInverseFFTs.add(new FFT(log2(fftSize), true));

	/* Clear out this temp stuff. */
	delete impulseResponseFloats;
}

void ShowerfyAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
	for (int i = prepCopys.size() - 1; i >= 0; --i)
	{
		delete prepCopys.removeAndReturn(i);
		delete prepImpulseResponseTransformed.removeAndReturn(i);
		delete prepFFTs.removeAndReturn(i);
		delete prepInverseFFTs.removeAndReturn(i);
	}
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ShowerfyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
	ignoreUnused (layouts);
	return true;
  #else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
	 && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void ShowerfyAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	/* Constants */
	int bufferNumSamples = buffer.getNumSamples();
	int bufferNumChannels = buffer.getNumChannels();
	int impulseResponseBufferNumSamples = impulseResponseBuffer.getNumSamples();
	int fftSize = impulseResponseBufferNumSamples + buffer.getNumSamples();

	/* Retrieve the FFT for the buffer. */
	FFT* bufferTransformer = prepFFTs.getLast();
	FFT* inverter = prepInverseFFTs.getLast();

	/* Impulse Response. */
	FFT::Complex* impulseResponseTransformed = prepImpulseResponseTransformed.getLast();

	/* Buffers for FFT */
	float* bufferCopy = prepCopys.getLast();

	const float* bufferInput;
	float* bufferWriters;

	for (int channel = 0; channel < bufferNumChannels; ++channel)
	{
		/* Retrieve the read/write pointers for the channel we are on and make a copy of the read ones. */
		bufferWriters = buffer.getWritePointer(channel);
		bufferInput = buffer.getReadPointer(channel);

		/* Set the bufferCopy with the samples from the DAW, zero extend. */
		int j;
		for (j = 0; j < bufferNumSamples; ++j)
			bufferCopy[j] = bufferInput[j];
		for (j = bufferNumSamples; j < fftSize * 2; ++j)
			bufferCopy[j] = 0;

		/* Perform the FFT on the samples. */
		bufferTransformer->performRealOnlyForwardTransform(bufferCopy);

		/* Multiply the FFTs to get the convolution of the two. */
		float tmpReal;
		float tmpImagenary;
		int sample;
		int half;
		for (sample = 0; sample < fftSize * 2; sample += 2)
		{
			half = sample / 2;
			tmpReal = impulseResponseTransformed[half].r * bufferCopy[sample] -
				impulseResponseTransformed[half].i * bufferCopy[sample + 1];

			tmpImagenary = impulseResponseTransformed[half].r * bufferCopy[sample] +
				impulseResponseTransformed[half].i * bufferCopy[sample + 1];

			bufferCopy[sample] = tmpReal;
			bufferCopy[sample + 1] = tmpImagenary;
		}

		/* Still holds the float values.  Revert back to non-complex floats. */
		inverter->performRealOnlyInverseTransform(bufferCopy);

		/* Overwrite the origional buffer, but does not get all of what was convolved.  FIFO?. */
		for (sample = 0; sample < bufferNumSamples; ++sample)
			bufferWriters[sample] = bufferCopy[sample];

		/* Mix the desired level of 'shower' noise into incoming signal. */
		/* Fist check buffer position, prevents clipping? */
		if (showerSoundPosition >= showerSoundBuffer.getNumSamples() ||
			showerSoundPosition + bufferNumSamples >= showerSoundBuffer.getNumSamples())
		{
			showerSoundPosition = 0;
		}

		buffer.addFrom(channel, 0, showerSoundBuffer, channel, showerSoundPosition, buffer.getNumSamples(), *showerSoundGain);
		showerSoundPosition += buffer.getNumSamples();
	}
}

//==============================================================================
bool ShowerfyAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ShowerfyAudioProcessor::createEditor()
{
	return new ShowerfyAudioProcessorEditor (*this);
}

//==============================================================================
void ShowerfyAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	/* Retrieve that state info. */
	MemoryOutputStream(destData, true).writeFloat(*showerSoundGain);
	MemoryOutputStream(destData, true).writeFloat(*impulseResponseDelay);
	MemoryOutputStream(destData, true).writeFloat(*impulseResponseWetDry);
}

void ShowerfyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	/* Store that state info. */
	*showerSoundGain = MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readFloat();
	*impulseResponseDelay = MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readFloat();
	*impulseResponseWetDry = MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readFloat();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new ShowerfyAudioProcessor();
}
