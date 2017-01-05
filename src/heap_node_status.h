#ifndef HEAP_NODE_STATUS

/**
 *
 * An enumeration that is used to keep track of
 * what a node's status in during the operation of
 * the operation insert/delete of the Heap
 */
enum HeapNodeStatus
{
    empty,
    available,
    transit
};

#define HEAP_NODE_STATUS value
#endif /* ifndef HEAP_NODE_STATUS */
