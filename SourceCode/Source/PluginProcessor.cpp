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
*/
ShowerfyAudioProcessor::ShowerfyAudioProcessor()
	: showerSoundPosition(0),										/* Initial shower sound pos is 0 */
	filesChosen(false),
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
	/* Audio Parameters. */
	addParameter(showerSoundGain = new AudioParameterFloat (
		"showerSoundGain",		/* param ID */
		"Shower Sound Gain",	/* param name */
		0.0f,					/* min value */
		1.0f,					/* max value */
		0.0f));					/* default value */
	addParameter(impulseResponseDelay = new AudioParameterFloat(
		"impulseResponseDelay",
		"Impulse Response Delay",
		0.0f,
		3.0f,
		0.0f));
	addParameter(impulseResponseWetDry = new AudioParameterFloat(
		"impulseResponseWetDry",
		"Impulse Response Wet/Dry",
		0.0f,
		1.0f,
		0.0f));
	addParameter(impulseResponseChanLock = new AudioParameterBool(
		"impulseResponseChanLock",
		"Impulse Response Channel Lock",
		true));
}

ShowerfyAudioProcessor::~ShowerfyAudioProcessor()
{
	fftwf_cleanup();
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

	/* FIFO TESTING */
	/*
	float inBuff[40];
	float testBuffer[8];
	float outBuff[8];
	int x = 0;
	int y = 0;
	int increment = 0;

	lockFreeFifo *fifo = new lockFreeFifo();
	for (y = 0; y < 10; ++y)
	{
		for (x = 0; x < 40; ++x)
		{
			inBuff[x] = increment;
			++increment;
		}
		fifo->addToFifo(inBuff, testBuffer, 40, 8, 4);
		fifo->readFromFifo(outBuff, 8);
	}
	*/

	/* Open the files, but only once. */
	if (!filesChosen) {

		AudioFormatManager manager;
		manager.registerBasicFormats();

		FileChooser showerchooser("Select the Shower Sound File (showerSound.wav)", File::nonexistent, "*.wav");
		if (showerchooser.browseForFileToOpen())
		{
			showerSoundFile = File(showerchooser.getResult());
			showerSoundReader = manager.createReaderFor(showerSoundFile);
			showerSoundBuffer.setSize(showerSoundReader->numChannels, showerSoundReader->lengthInSamples);
			showerSoundReader->read(&showerSoundBuffer, 0, showerSoundReader->lengthInSamples, 0, true, true);
			delete showerSoundReader;
		}

		/* Load IR */
		FileChooser irchooser("Select the Impulse Response File (impulse.wav)", File::nonexistent, "*.wav");
		if (irchooser.browseForFileToOpen())
		{
			impulseResponseFile = File(irchooser.getResult());
			impulseResponseReader = manager.createReaderFor(impulseResponseFile);
			impulseResponseBuffer.setSize(impulseResponseReader->numChannels, impulseResponseReader->lengthInSamples);
			impulseResponseReader->read(&impulseResponseBuffer, 0, impulseResponseBuffer.getNumSamples(), 0, true, true);
			delete impulseResponseReader;
		}


		filesChosen = true;
	}

	int IRBS = impulseResponseBuffer.getNumSamples();
	int fftSize = IRBS + samplesPerBlock - 1;
	int channels = getNumInputChannels();

	/* Init the plan and arrays for pre-transformation of the IR. */
	/* Mono channel.  Radical. */
	float* impulseResponseFloats = fftwf_alloc_real(fftSize);
	fftwf_complex* impulseResponseComplex = fftwf_alloc_complex(fftSize/2 + 1);
	fftwf_plan impulseResponseTransformer = fftwf_plan_dft_r2c_1d(fftSize,
		impulseResponseFloats, impulseResponseComplex, FFTW_ESTIMATE);
	const float* readPtrs = impulseResponseBuffer.getReadPointer(0);

	int j;
	for (j = 0; j < IRBS; ++j)
		impulseResponseFloats[j] = readPtrs[j];
	for (j = IRBS; j < fftSize; ++j)
		impulseResponseFloats[j] = 0;

	/* Perform the transformation on the IR, convert to store in impulseResponseComplex as fftw_complex types. */
	fftwf_execute(impulseResponseTransformer);

	/* Initialization of everything needed for the prepareToPlay */
	float* cpyIn = fftwf_alloc_real(fftSize);
	fftwf_complex* cpyOut = fftwf_alloc_complex(fftSize/2 + 1);
	float* convolvedInverse = fftwf_alloc_real(fftSize);
	float* fromFifo = new float[samplesPerBlock];

	prepCopysIn.add(cpyIn);
	prepCopysOut.add(cpyOut);
	prepImpulseResponseTransformed.add(impulseResponseComplex);
	prepConvolvedInverse.add(convolvedInverse);
	prepFromFifo.add(fromFifo);

	/* Fill all of the fifos needed! */
	int i;
	for (i = 0; i < channels; ++i)
	{
		prepFifoChannels.add(new lockFreeFifo());
		prepFifoChannels[i]->reset();
	}

	/* Clear out this temp stuff. */
	fftwf_destroy_plan(impulseResponseTransformer);
	fftwf_free(impulseResponseFloats);
}

void ShowerfyAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
	int channels = getNumInputChannels();
	int i;
	for (i = prepCopysIn.size() - 1; i >= 0; --i)
	{
		fftwf_free(prepCopysIn.removeAndReturn(i));
		delete prepFromFifo.removeAndReturn(i);
		fftwf_free(prepCopysOut.removeAndReturn(i));
		fftwf_free(prepImpulseResponseTransformed.removeAndReturn(i));
		fftwf_free(prepConvolvedInverse.removeAndReturn(i));
	}

	for (i = prepFifoChannels.size() - 1; i >= 0; --i)
		delete prepFifoChannels.removeAndReturn(i);
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
	int bufferNumChannels = buffer.getNumChannels();
	int bufferNumSamples = buffer.getNumSamples();
	int impulseResponseBufferNumSamples = impulseResponseBuffer.getNumSamples();
	int fftSize = impulseResponseBufferNumSamples + bufferNumSamples - 1;

	/* Buffers */
	float* convolvedInverse = prepConvolvedInverse.getLast();
	float* cpyIn = prepCopysIn.getLast();
	fftwf_complex* cpyOut = prepCopysOut.getLast();
	float* fromFifo = prepFromFifo.getLast();

	/* Impulse Response. */
	fftwf_complex* impulseResponse = prepImpulseResponseTransformed.getLast();

	/* Global Initializations */
	fftwf_plan plan = fftwf_plan_dft_r2c_1d(fftSize, cpyIn, cpyOut, FFTW_ESTIMATE);
	fftwf_plan invPlan = fftwf_plan_dft_c2r_1d(fftSize, cpyOut, convolvedInverse, FFTW_ESTIMATE);

	/* Various pointers. */
	const float* bufferReaders;
	float* bufferWriters;
	lockFreeFifo* curFifo;

	for (int channel = 0; channel < bufferNumChannels; ++channel)
	{
		/* Retrieve the read/write pointers for the channel we are on and make a copy of the read ones. */
		bufferWriters = buffer.getWritePointer(channel);
		bufferReaders = buffer.getReadPointer(channel);
		curFifo = prepFifoChannels[channel];

		/* Set the bufferCopy with the samples from the DAW, zero extend. */
		int j;
		for (j = 0; j < bufferNumSamples; ++j)
			cpyIn[j] = bufferReaders[j];
		for (j = bufferNumSamples; j < fftSize; ++j)
			cpyIn[j] = 0;

		/* Perform the FFT on the samples. */
		fftwf_execute(plan);

		/* Multiply the FFTs to get the convolution of the two. */
		float tmpReal;
		float tmpImagenary;
		int sample;
		for (sample = 0; sample < (fftSize / 2) + 1; ++sample)
		{
			tmpReal = impulseResponse[sample][0] * cpyOut[sample][0] -
				impulseResponse[sample][1] * cpyOut[sample][1];

			tmpImagenary = impulseResponse[sample][0] * cpyOut[sample][1] +
				impulseResponse[sample][1] * cpyOut[sample][0];

			cpyOut[sample][0] = tmpReal;
			cpyOut[sample][1] = tmpImagenary;
		}

		/* Inverse the buffer. */
		fftwf_execute(invPlan);

		/* Normalize the output.  Thanks FFTW. <3 FFTW FTW */
		int i;
		for (i = 0; i < fftSize; ++i)
			convolvedInverse[i] /= fftSize;

		/* Delay, checks for channel lock. */
		float curDelay;
		if (*impulseResponseChanLock)
		{
			if (channel == 0)
				curDelay = *impulseResponseDelay;
		}
		else
			curDelay = *impulseResponseDelay;

		if (*impulseResponseWetDry != 0)
		{
			curFifo->addToFifo(convolvedInverse, bufferReaders, fftSize, bufferNumSamples, 1, getSampleRate(), curDelay);
			curFifo->readFromFifo(fromFifo, bufferNumSamples);
			for (i = 0; i < bufferNumSamples; ++i)
			{
				bufferWriters[i] = (bufferReaders[i] * (1.0f - *impulseResponseWetDry)) +
					(fromFifo[i] * (*impulseResponseWetDry));
			}
		}

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

	fftwf_destroy_plan(plan);
	fftwf_destroy_plan(invPlan);
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
