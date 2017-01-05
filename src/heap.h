#ifndef HEAP
#define HEAP value

#include <iostream>
#include <mutex>
#include <vector>
#include <iomanip>

#include "heap_node.h"
#include "heap_node_status.h"
#include "bit_reversed_counter.h"

template <class T> class Heap
{
    private:

        /**
         * The maximum size of the heap. It's always a 2^n - 1 because that's the maximum number of items a heap with height n can keep.
         * The 0th index is used as a sentinel and the 1st index is the first actual HeapNode.
         */
        int maxSize;

        /**
         * Keeps track of the Bit Reversed sequence and is used to determines
         * the index of where a HeapNode is inserted/deleted.
         */
        BitReversedCounter counter;


        /**
         * Keeps all the HeapNodes as a list of shared pointer.
         */
        std::vector<std::shared_ptr<HeapNode<int>>> nodes;

        /**
         * Keeps the whole Heap locked while operations on it's size are performed.
         */
        std::mutex heapGate;

        /**
         * Mutexes are kept in the Heap so that swapping HeapNodeItems does not
         * mess up the mutexes and cause a deadlock in the delete operation.
         */
        std::vector<std::mutex> nodeGate;

    public:

        Heap(int maxSize)
        {
            // Round to the next power of 2
            this->maxSize = 1;
            while(maxSize)
            {
                this->maxSize *= 2;
                maxSize /= 2;
            }

            for (int i = 0; i < this->maxSize + 1; ++i)
            {
                nodes.push_back(std::make_shared<HeapNode<int>>());
            }

            nodeGate = std::vector<std::mutex>(this->maxSize + 1);
        }

        /**
         * Tests whether the Heap contains no elements
         */
        bool isEmpty()
        {
            std::lock_guard<std::mutex> guardParent(heapGate);

            if (counter.counter == 0)
            {
                return true;
            }

            return false;
        }

        /**
         * Insert an elements in the Heap
         */
        void insert(const T &data, int priority, int pid)
        {
            int i = -1;
            {
                std::unique_lock<std::mutex> gate(heapGate); // Lock heapGate
                i = counter.bitReversedIncrement();
                gate = std::unique_lock<std::mutex>(nodeGate[i]); // Switch lock to i
                nodes[i]->set(data, priority, pid, HeapNodeStatus::transit);
            }

            while (i > 1)
            {
                int parent = i / 2;

                std::lock(nodeGate[parent], nodeGate[i]);
                std::lock_guard<std::mutex> guardParent(nodeGate[parent], std::adopt_lock);
                std::lock_guard<std::mutex> guardNode(nodeGate[i], std::adopt_lock);

                if (nodes[parent]->status == HeapNodeStatus::available &&
                        nodes[i]->status == HeapNodeStatus::transit && nodes[i]->pid == pid)
                {
                    if (nodes[i]->priority > nodes[parent]->priority)
                    {
                        swap(nodes[i], nodes[parent]);
                        i = parent;
                    }
                    else
                    {
                        nodes[i]->status = HeapNodeStatus::available;
                        i = 0;
                    }
                }
                else if (nodes[parent]->status == HeapNodeStatus::empty)
                {
                    i = 0;
                }
                else if (nodes[i]->status == HeapNodeStatus::transit && nodes[i]->pid != pid)
                {
                    i = parent;
                }
            }

            if (i == 1)
            {
                std::lock_guard<std::mutex> guardNode(nodeGate[i]);
                if (nodes[i]->status == HeapNodeStatus::transit && nodes[i]->pid == pid)
                {
                    nodes[i]->status = HeapNodeStatus::available;
                }
            }
        }

        /**
         * Pops the item with the largest priority
         */
        bool pop(T &returnValue)
        {
            int bottom = -1;
            std::unique_lock<std::mutex> iLock;
            {
                std::lock_guard<std::mutex> guardParent(heapGate);

                if (counter.counter == 0)
                {
                    return false;
                }

                bottom = counter.reversed;
                counter.bitReversedDecrement();
            }

            // Lock the item to be moved
            iLock = std::unique_lock<std::mutex>(nodeGate[1]);

            // Get return value
            returnValue = *(nodes[1]->data);

            // Swap
            swap(nodes[1], nodes[bottom]);

            // Set to available
            nodes[1]->status = HeapNodeStatus::available;

            // Clear Last
            nodes[bottom]->clear();

            int i = 1;
            while (i < maxSize / 2)
            {
                int child = -1;
                int leftChild = 2 * i;
                int rightChild = 2 * i + 1;

                // Used to keep the bigger child locked (don't call Social Services)
                std::unique_lock<std::mutex> childLock;
                {
                    std::lock(nodeGate[leftChild], nodeGate[rightChild]);
                    std::unique_lock<std::mutex> leftLock(nodeGate[leftChild], std::adopt_lock);
                    std::unique_lock<std::mutex> rightLock(nodeGate[rightChild], std::adopt_lock);

                    if (nodes[leftChild]->status == HeapNodeStatus::empty)
                    {
                        break;
                    }
                    else if (nodes[rightChild]->status == HeapNodeStatus::empty ||
                            nodes[leftChild]->priority > nodes[rightChild]->priority)
                    {
                        childLock = std::move(leftLock);
                        child = leftChild;
                    }
                    else
                    {
                        childLock = std::move(rightLock);
                        child = rightChild;
                    }
                }

                if (nodes[child]->priority > nodes[i]->priority)
                {
                    swap(nodes[i], nodes[child]);
                    iLock = std::move(childLock);
                    i = child;
                }
                else
                {
                    break;
                }
            }

            return true;
        }

        /**
         * Used to print the data in the Heap for debbuging purposes
         *
         */
        void printHeap()
        {
            std::cout << std::endl;

            for (int i = 0; i <= maxSize; i++)
            {
                std::cout << std::setw(4) << i;
            }

            std::cout << std::endl;

            for (int i = 0; i <= maxSize; i++)
            {
                std::cout << std::setw(4) << nodes[i]->pid;
            }

            std::cout << std::endl;

            for (int i = 0; i <= maxSize; i++)
            {
                std::cout << std::setw(4) << nodes[i]->priority;
            }
            std::cout << std::endl;

            for (int i = 0; i <= maxSize; i++)
            {
                if (nodes[i]->data)
                {
                    std::cout << std::setw(4) << *(nodes[i]->data);
                }
                else
                {
                    std::cout << std::setw(4) << "N\\A";
                }
            }
            std::cout << std::endl;
        }

        /**
         * Verifies whether the Heap is valid
         */
        bool isHeapValid()
        {
            for (int i = 2; i <= maxSize; i++)
            {
                if (nodes[i]->status == HeapNodeStatus::empty)
                {
                    return true;
                }
                if (nodes[i]->priority > nodes[i / 2]->priority)
                {
                    return false;
                }
            }
            return true;
        }
};

#endif /* ifndef HEAP */
