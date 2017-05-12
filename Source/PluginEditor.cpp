/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ShowerfyAudioProcessorEditor::ShowerfyAudioProcessorEditor (ShowerfyAudioProcessor& p)
	: AudioProcessorEditor (&p), processor (p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	Component::addAndMakeVisible(slider = new Slider("new slider"));
	slider->setRange(0, 1, 0);
	slider->setSliderStyle(Slider::Rotary);
	slider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
	slider->setColour(Slider::rotarySliderOutlineColourId, Colour(0xffffa100));
	slider->addListener(this);

	Component::addAndMakeVisible(slider2 = new Slider("new slider"));
	slider2->setRange(0, 1, 0);
	slider2->setSliderStyle(Slider::Rotary);
	slider2->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
	slider2->setColour(Slider::rotarySliderOutlineColourId, Colour(0xffffa100));
	slider2->addListener(this);

	Component::addAndMakeVisible(slider3 = new Slider("new slider"));
	slider3->setRange(0, 1, 0);
	slider3->setSliderStyle(Slider::Rotary);
	slider3->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
	slider3->setColour(Slider::rotarySliderOutlineColourId, Colour(0xffffa100));
	slider3->setColour(Slider::textBoxTextColourId, Colours::white);
	slider3->addListener(this);

	paramSliders.add(slider3);
	paramSliders.add(slider2);
	paramSliders.add(slider);

	Component::addAndMakeVisible(groupComponent = new GroupComponent("new group",
		TRANS("Adjustments")));
	groupComponent->setColour(GroupComponent::outlineColourId, Colour(0xffac4ad7));
	groupComponent->setColour(GroupComponent::textColourId, Colours::white);

	Component::addAndMakeVisible(toggleButton = new ToggleButton("new toggle button"));
	toggleButton->setButtonText(TRANS("Delay Channel Lock"));
	toggleButton->addListener(this);
	toggleButton->setToggleState(true, dontSendNotification);

	paramButtons.add(toggleButton);

	AudioProcessorEditor::setSize (837, 253);
}

ShowerfyAudioProcessorEditor::~ShowerfyAudioProcessorEditor()
{
	slider = nullptr;
	groupComponent = nullptr;
	slider2 = nullptr;
	slider3 = nullptr;
	toggleButton = nullptr;
}

//==============================================================================
void ShowerfyAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll(Colours::white);

	g.setColour(Colour(0xff530f93));
	g.fillRoundedRectangle(0.0f, 0.0f, 837.0f, 253.0f, 10.000f);

	g.setColour(Colour(0xffca7df4));
	g.drawRoundedRectangle(0.0f, 0.0f, 837.0f, 253.0f, 10.000f, 5.000f);

	g.setColour(Colour(0xffa05efc));
	g.fillRoundedRectangle(504.0f, 100.0f, 120.0f, 116.0f, 10.000f);

	g.setColour(Colour(0xffa05efc));
	g.fillRoundedRectangle(672.0f, 100.0f, 120.0f, 116.0f, 10.000f);

	g.setColour(Colour(0xffa05efc));
	g.fillRoundedRectangle(324.0f, 100.0f, 124.0f, 116.0f, 10.000f);

	g.setColour(Colours::white);
	g.setFont(Font("Verdana", 15.00f, Font::plain));
	g.drawText(TRANS("Background Shower"),
		292, 212, 200, 30,
		Justification::centred, true);

	g.setColour(Colours::white);
	g.setFont(Font("Verdana", 15.00f, Font::plain));
	g.drawText(TRANS("Pre-Delay"),
		460, 212, 200, 30,
		Justification::centred, true);

	g.setColour(Colours::white);
	g.setFont(Font("Verdana", 15.00f, Font::plain));
	g.drawText(TRANS("Wet/Dry"),
		628, 212, 200, 30,
		Justification::centred, true);

	g.setColour(Colours::white);
	g.setFont(Font("Copperplate Gothic", 95.80f, Font::plain));
	g.drawText(TRANS("Showerfy"),
		-20, -4, 588, 92,
		Justification::centred, true);

	g.setColour(Colour(0xffa05efc));
	g.fillRoundedRectangle(36.0f, 116.0f, 196.0f, 88.0f, 10.000f);

	g.setColour(Colours::white);
	g.setFont(Font("Bauhaus 93", 30.00f, Font::plain));
	g.drawText(TRANS("Prince Themed"),
		516, 44, 200, 30,
		Justification::centred, true);
}

void ShowerfyAudioProcessorEditor::resized()
{
	slider->setBounds(608, 104, 248, 120);
	groupComponent->setBounds(312, 80, 496, 160);
	slider2->setBounds(440, 104, 248, 120);
	slider3->setBounds(264, 104, 248, 120);
	toggleButton->setBounds(80, 136, 208, 48);
}

AudioParameterFloat* ShowerfyAudioProcessorEditor::getParameterForSlider(Slider* slider)
{
	const OwnedArray<AudioProcessorParameter>& params = processor.getParameters();
	return dynamic_cast<AudioParameterFloat*> (params[paramSliders.indexOf(slider)]);
}

AudioParameterBool* ShowerfyAudioProcessorEditor::getParameterForButton(ToggleButton* button)
{
	const OwnedArray<AudioProcessorParameter>& params = processor.getParameters();
	return dynamic_cast<AudioParameterBool*> (params[paramButtons.indexOf(button)]);
}

void ShowerfyAudioProcessorEditor::sliderValueChanged(Slider* sliderThatWasMoved)
{
	//[UsersliderValueChanged_Pre]
	//[/UsersliderValueChanged_Pre]

	if (sliderThatWasMoved == slider)
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider))
			*param = (float)slider->getValue();
	}
	else if (sliderThatWasMoved == slider2)
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider2))
			*param = (float)slider2->getValue();
	}
	else if (sliderThatWasMoved == slider3)
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider3))
			*param = (float)slider3->getValue();
	}

	//[UsersliderValueChanged_Post]
	//[/UsersliderValueChanged_Post]
}

void ShowerfyAudioProcessorEditor::buttonClicked(Button* buttonThatWasClicked)
{
	//[UserbuttonClicked_Pre]
	//[/UserbuttonClicked_Pre]

	if (buttonThatWasClicked == toggleButton)
	{
		//[UserButtonCode_toggleButton] -- add your button handler code here..
		if (AudioParameterBool* param = getParameterForButton(toggleButton))
		{
			if (param->get() == false)
				*param = true;
			else
				*param = false;
		}
		//[/UserButtonCode_toggleButton]
	}

	//[UserbuttonClicked_Post]
	//[/UserbuttonClicked_Post]
}
//==============================================================================
#if 0
/*  -- Projucer information section --

This is where the Projucer stores the metadata that describe this GUI layout, so
make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ShowerfyAudioProcessorEditor"
componentName="" parentClasses="public Component" constructorParams=""
variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
<BACKGROUND backgroundColour="ffffffff">
<ROUNDRECT pos="0 0 837 253" cornerSize="10" fill="solid: ff530f93" hasStroke="1"
stroke="5, mitered, butt" strokeColour="solid: ffca7df4"/>
<ROUNDRECT pos="504 100 120 116" cornerSize="10" fill="solid: ffa05efc"
hasStroke="0"/>
<ROUNDRECT pos="672 100 120 116" cornerSize="10" fill="solid: ffa05efc"
hasStroke="0"/>
<ROUNDRECT pos="324 100 124 116" cornerSize="10" fill="solid: ffa05efc"
hasStroke="0"/>
<TEXT pos="292 212 200 30" fill="solid: ffffffff" hasStroke="0" text="Background Shower"
fontname="Verdana" fontsize="15" bold="0" italic="0" justification="36"/>
<TEXT pos="460 212 200 30" fill="solid: ffffffff" hasStroke="0" text="Pre-Delay"
fontname="Verdana" fontsize="15" bold="0" italic="0" justification="36"/>
<TEXT pos="628 212 200 30" fill="solid: ffffffff" hasStroke="0" text="Wet/Dry"
fontname="Verdana" fontsize="15" bold="0" italic="0" justification="36"/>
<TEXT pos="-20 -4 588 92" fill="solid: ffffffff" hasStroke="0" text="Showerfy"
fontname="Copperplate Gothic" fontsize="95.799999999999997158"
bold="0" italic="0" justification="36"/>
<ROUNDRECT pos="36 116 196 88" cornerSize="10" fill="solid: ffa05efc" hasStroke="0"/>
<TEXT pos="516 44 200 30" fill="solid: ffffffff" hasStroke="0" text="Prince Themed"
fontname="Bauhaus 93" fontsize="30" bold="0" italic="0" justification="36"/>
</BACKGROUND>
<SLIDER name="new slider" id="b64f2dd86da492ae" memberName="slider" virtualName=""
explicitFocusOrder="0" pos="608 104 248 120" rotaryslideroutline="ffffa100"
min="0" max="10" int="0" style="Rotary" textBoxPos="NoTextBox"
textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
needsCallback="1"/>
<GROUPCOMPONENT name="new group" id="548d0c47fd67d15c" memberName="groupComponent"
virtualName="" explicitFocusOrder="0" pos="312 80 496 160" outlinecol="ffac4ad7"
textcol="ffffffff" title="Adjustments"/>
<SLIDER name="new slider" id="e2a7e98fadb04138" memberName="slider2"
virtualName="" explicitFocusOrder="0" pos="440 104 248 120" rotaryslideroutline="ffffa100"
min="0" max="10" int="0" style="Rotary" textBoxPos="NoTextBox"
textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
needsCallback="1"/>
<SLIDER name="new slider" id="f317ab81d2b8b556" memberName="slider3"
virtualName="" explicitFocusOrder="0" pos="264 104 248 120" rotaryslideroutline="ffffa100"
textboxtext="ffffffff" min="0" max="10" int="0" style="Rotary"
textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
textBoxHeight="20" skewFactor="1" needsCallback="1"/>
<TOGGLEBUTTON name="new toggle button" id="3f02800a3b4b21" memberName="toggleButton"
virtualName="" explicitFocusOrder="0" pos="104 136 208 48" buttonText="Delay Channel Lock"
connectedEdges="0" needsCallback="1" radioGroupId="0" state="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]