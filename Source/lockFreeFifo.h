#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class lockFreeFifo
{
public:
	lockFreeFifo();
	~lockFreeFifo();

	void addToFifo(const float* data, int amountToAdd, int incrementAddPos);
	void readFromFifo(float* data, int amount);
	void reset();

private:
	int fifoSize;
	AbstractFifo abstractFifo;
	float buffer[524288]; // 2 ^ 19
	int endPos;
};

