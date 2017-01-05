#ifndef BIT_REVERSED_COUNTER
#define BIT_REVERSED_COUNTER

/**
 *
 * This class is used to implement a Bit Reversed Counter.
 * It is used to generate a sequence for the insertion and deletion of HeapNodes by the Heap class
 *
 */
class BitReversedCounter
{
  private:
    /**
     * Reverses the ith bit of a number using the XOR binary function.
     *
     * @param number  number whose bit to reverse
     * @param position of bit to reverse
     *
     * return the number with ths ith bit reversed
     *
     */
    int xorBit(int number, int i);

    /**
     * Tests whether ith bit of a number is set using the AND binary function.
     *
     * @param number  number whose bit to reverse
     * @param i position of bit to reverse
     *
     * @return true if the ith is set | false otherwise
     *
     */
    bool andBit(int number, int i);

  public:

    // Keeps the current count
    int counter = 0;

    // Keeps the current member of the Bit Reversed Sequence
    int reversed = 0;

    // Keeps the position of the highest bit of the counter
    int highBit = -1;


    /**
     * Calculates the the next number in the Bit Reversed Sequence and modifies
     * counter, reversed and highbit accordingly
     *
     * @return the new value for reversed
     */
    int bitReversedIncrement();

    /**
     * Calculates the the previous number in the Bit Reversed Sequence and modifies
     * counter, reversed and highbit accordingly
     *
     * @return the new value for reversed
     */
    int bitReversedDecrement();
};

#endif /* ifndef BIT_REVERSED_COUNTER */
