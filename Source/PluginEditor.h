/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class ShowerfyAudioProcessorEditor  : public AudioProcessorEditor,
									  public SliderListener,
									  public ButtonListener
{
public:
	ShowerfyAudioProcessorEditor (ShowerfyAudioProcessor&);
	~ShowerfyAudioProcessorEditor();

	//==============================================================================
	void paint (Graphics&) override;
	void resized() override;
	AudioParameterFloat* getParameterForSlider(Slider* slider);
	AudioParameterBool* getParameterForButton(ToggleButton* button);
	void sliderValueChanged(Slider* sliderThatWasMoved) override;
	void buttonClicked(Button* buttonThatWasClicked) override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	ShowerfyAudioProcessor& processor;
	ScopedPointer<Slider> slider;
	ScopedPointer<Slider> slider2;
	ScopedPointer<Slider> slider3;
	ScopedPointer<GroupComponent> groupComponent;
	ScopedPointer<ToggleButton> toggleButton;
	Array<Slider*> paramSliders;
	Array<ToggleButton*> paramButtons;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShowerfyAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
