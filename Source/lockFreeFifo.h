#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/*
 * Fifo that uses the overlap-add method.
*/
class lockFreeFifo
{
public:
	lockFreeFifo();
	~lockFreeFifo();

	void addToFifo(const float* data, const float* bufferData, int dataSize, int bufferDataSize, int numSegements);
	void readFromFifo(float* data, int amount);
	void reset();

private:
	int fifoSize;
	AbstractFifo abstractFifo;
	float buffer[524288]; // 2 ^ 19
	int endPos;
};

