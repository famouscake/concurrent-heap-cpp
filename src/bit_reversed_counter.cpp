#include "bit_reversed_counter.h"

int BitReversedCounter::xorBit(int i, int bit)
{
    int mask = 1 << (bit - 1);
    return i ^ mask;
}

bool BitReversedCounter::andBit(int i, int bit)
{
    int mask = (1 << (bit - 1));
    return i & mask;
}

int BitReversedCounter::bitReversedIncrement()
{
    counter++;

    int bit = highBit - 1;

    while (bit > 0)
    {
        reversed = xorBit(reversed, bit);

        if (andBit(reversed, bit))
        {
            break;
        }
        bit--;
    }

    if (bit <= 0)
    {
        reversed = counter;
        highBit++;
    }

    return reversed;
}

int BitReversedCounter::bitReversedDecrement()
{
    counter--;

    int bit = highBit - 1;

    while (bit > 0)
    {
        reversed = xorBit(reversed, bit);

        if (!andBit(reversed, bit))
        {
            break;
        }
        bit--;
    }

    if (bit <= 0)
    {
        reversed = counter;
        highBit--;
    }

    return reversed;
}
