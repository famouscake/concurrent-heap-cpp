#include <algorithm>
#include <cassert>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#include "bit_reversed_counter.h"
#include "heap.h"
#include "heap_node.h"
#include "heap_node_status.h"

using namespace std;

/**
 * Used from threads to insert a vector a values to the Heap
 */
void insertElement(Heap<int> &, vector<int>, int);

/**
 * Used from threads to remove a number of values from the Heap
 */
void removeItem(Heap<int> &, int);

/**
 * Generates a vector ot be added to the Heap
 */
vector<int> getRandomVector(int, int, int);

/**
 * Insert values with multiple threads then join and then delete half
 * using MULTIPLE threads and check if the resulting tree is valid
 */
void testInsertConcurentThenDeleteConcurent(int, int, int, int);

/**
 * Insert values with multiple threads then join and then delete using ONE thread
 */
void testInsertConcurentThenDeleteSequential(int, int, int, int, bool);

int main()
{
    const int n = (1 << 15) - 1;

    int threadCount = 4;

    testInsertConcurentThenDeleteConcurent(n, threadCount, 1, 1000);
    testInsertConcurentThenDeleteSequential(n, threadCount, 1, 1000, false);

    cout << "All tests ran successfully!!!\n";

    return 0;
}

void testInsertConcurentThenDeleteConcurent(int N, int threadCount, int min, int max)
{
    Heap<int> h(N);
    vector<thread> threads;
    int chunkSize = N / threadCount;

    for (auto i = 0; i < threadCount; i++)
    {
        vector<int> data = getRandomVector(chunkSize, min, max);

        int pid = i;
        threads.push_back(thread(insertElement, std::ref(h), data, pid));
    }

    for (auto i = threads.begin(); i != threads.end(); i++)
    {
        i->join();
    }

    assert(h.isHeapValid());

    // Delete only half the items so that there is come some data left to check if the
    // resulting tree is valid
    threads.clear();
    for (auto i = 0; i < threadCount - 1; i++)
    {
        threads.push_back(thread(removeItem, std::ref(h), chunkSize));
    }

    for (auto i = threads.begin(); i != threads.end(); i++)
    {
        i->join();
    }

    assert(h.isHeapValid());
}

void testInsertConcurentThenDeleteSequential(int N, int threadCount, int min, int max, bool debug)
{
    Heap<int> h(N);
    vector<thread> threads;
    int chunkSize = N / threadCount;
    vector<int> control;

    for (auto i = 0; i < threadCount; i++)
    {
        vector<int> data = getRandomVector(chunkSize, min, max);
        control.insert(control.end(), data.begin(), data.end());

        int pid = i;
        threads.push_back(thread(insertElement, std::ref(h), data, pid));
    }

    for (auto i = threads.begin(); i != threads.end(); i++)
    {
        i->join();
    }

    assert(h.isHeapValid());
    assert(!h.isEmpty());

    vector<int> sorted;

    int t;
    while (true == h.pop(t))
    {
        sorted.push_back(t);
    }

    assert(h.isEmpty());

    std::sort(control.begin(), control.end(), [](int a, int b) { return a > b; });

    if (debug)
    {
        cout << endl;
        for (auto i = 0; i < N; i++)
        {
            cout << setw(4) << sorted[i];
        }

        cout << endl << endl;

        for (auto i = 0; i < N; i++)
        {
            cout << setw(4) << control[i];
        }
        cout << endl;
    }

    assert(sorted == control);
}

vector<int> getRandomVector(int n, int min, int max)
{
    random_device rnd_device;
    mt19937 mersenne_engine(rnd_device());
    uniform_int_distribution<int> dist(min, max);
    auto gen = std::bind(dist, mersenne_engine);

    vector<int> data(n);
    std::generate(data.begin(), data.end(), gen);

    return data;
}

void insertElement(Heap<int> &h, vector<int> data, int pid)
{
    for (auto i : data)
    {
        h.insert(i, i, pid);
    }
}

void removeItem(Heap<int> &h, int times)
{
    for (auto i = 0; i < times; i++)
    {
        int t;
        h.pop(t);
    }
}
