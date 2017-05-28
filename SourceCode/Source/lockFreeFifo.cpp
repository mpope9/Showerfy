#include "lockFreeFifo.h"


lockFreeFifo::lockFreeFifo() : fifoSize(524288), abstractFifo(fifoSize)
{
	memset(buffer, 0.0f, sizeof(float) * fifoSize);
}

/* Adds data to fifo, and incraments the adding location by amountToIncrament. */
void lockFreeFifo::addToFifo(const float* data, const float* bufferData, int dataSize, int bufferDataSize,
	int numSegments, int sampleRate, float delay)
{
	int start1, size1, start2, size2;
	int i, j;

	/* Should be equal to numSegemnts amount of buffers plus one overlap size. */
	int segmentSize = dataSize / numSegments;
	int overlapSize = segmentSize - bufferDataSize;
	int newEndPos = (bufferDataSize * numSegments) + overlapSize;

	abstractFifo.prepareToWrite(bufferDataSize, start1, size1, start2, size2);

	/* Add delay to the segmentStart */
	int segmentStart = start1 + int(delay * sampleRate);
	int segmentEnd = 0;
	int dataTracker = 0;
	int remainder = 0;

	/* You need to get fancy with the copy to prevent fuck-ups. */
	for (i = 0; i < numSegments; ++i)
	{
		remainder = 0;
		/* Clever circular part. */
		if (segmentStart + segmentSize >= fifoSize)
		{
			remainder = fifoSize - segmentStart;
			for (j = segmentStart; j < fifoSize; ++j)
			{
				buffer[j] += data[dataTracker];
				++dataTracker;
			}
			segmentStart = 0;
			/* TESTING */
			segmentEnd = segmentSize - remainder;
		}
		else
			segmentEnd = segmentStart + segmentSize;

		for (j = segmentStart; j < segmentEnd && dataTracker < dataSize; ++j)
		{
			buffer[j] += data[dataTracker];
			++dataTracker;
		}
		segmentStart += bufferDataSize - remainder;		/* I think remainder has to be subracted for the circular part...? */
	}

	/* Add origional buffer to the bad boy. */
	if (start1 + bufferDataSize > fifoSize)
	{
		dataTracker = 0;
		for (i = start1; i < fifoSize; ++i)
		{
			buffer[i] += bufferData[dataTracker];
			++dataTracker;
		}
		for (i = 0; i < (start1 + bufferDataSize) - fifoSize; ++i)
		{
			buffer[i] += bufferData[dataTracker];
			++dataTracker;
		}
	}
	else
	{
		dataTracker = 0;
		for (i = start1; i < bufferDataSize; ++i)
		{
			buffer[i] += bufferData[dataTracker];
			++dataTracker;
		}
	}
	abstractFifo.finishedWrite(bufferDataSize);
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
