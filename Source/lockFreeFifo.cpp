#include "lockFreeFifo.h"


lockFreeFifo::lockFreeFifo() : fifoSize(524288), abstractFifo(fifoSize), endPos(0)
{
	memset(buffer, 0.0f, sizeof(float) * fifoSize);
}

/* Adds data to fifo, and incraments the adding location by amountToIncrament. */
void lockFreeFifo::addToFifo(const float* data, int amountToAdd, int increment)
{
	int start1, size1, start2, size2;
	int i, j;
	abstractFifo.prepareToWrite(increment, start1, size1, start2, size2);

	/* Set the initial endPos.  Else increment endPos. */
	if (endPos == start1)
		endPos = amountToAdd;
	else if (endPos + increment > fifoSize)
		endPos = (endPos + increment) - fifoSize;
	else
		endPos += increment;


	/* If there is overflow and the buffer needs to write past the end.  Else, normal write. */
	int dataTracker = 0;
	if (start1 > endPos)
	{
		for (i = start1; i < fifoSize; ++i)
		{
			buffer[i] += data[dataTracker];
			++dataTracker;
		}
		for (i = 0; i < endPos; ++i)
		{
			buffer[i] += data[dataTracker];
			++dataTracker;
		}
	}
	else
	{
		for (i = start1; i < endPos; ++i)
		{
			buffer[i] += data[dataTracker];
			++dataTracker;
		}
	}

	abstractFifo.finishedWrite(increment);
}

/* Reads data from fifo.  Increases the get pointer.  Zero out what was removed. */
void lockFreeFifo::readFromFifo(float* data, int amount)
{
	int start1, size1, start2, size2;
	int i, j;
	abstractFifo.prepareToRead(amount, start1, size1, start2, size2);

	if (size1 > 0)
	{
		j = start1;
		for (i = 0; i < size1; ++i)
		{
			data[i] = buffer[j];
			buffer[j] = 0.0f;
			++j;
		}
	}
	if (size2 > 0)
	{
		j = start2;
		for (i = size1; i < size1 + size2; ++i)
		{
			data[i] = buffer[j];
			buffer[j] = 0.0f;
			++j;
		}
	}

	abstractFifo.finishedRead(size1 + size2);
}

void lockFreeFifo::reset()
{
	abstractFifo.reset();
}

lockFreeFifo::~lockFreeFifo()
{
}
