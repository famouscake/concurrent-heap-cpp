#ifndef HEAP_NODE

#include "heap_node_status.h"
#include <memory>

/**
 * A single node in the Heap
 */
template <class T> class HeapNode
{
    public:

        /**
         * The PID of the thread that has inserted/deleted the node.
         */
        int pid;

        /**
         * The status of the ;ode. The value tag
         * in the paper is split into pid and status because I don't want
         * to empose any restriction on what the user chooses the pid of the thread to be.
         */
        HeapNodeStatus status;

        int priority;

        /**
         * The actual data that is stored in the node.
         *
         * It's stored via a shared pointer because it makes it more efficient to swap items.
         *
         * Furthermore since the Heap needs to preallocate empty items and T may not be guarateed to have a
         * default constructor I can set the shared pointer of data to null.
         */
        std::shared_ptr<T> data;

        HeapNode(T data, int priority, int pid, HeapNodeStatus status)
        {
            this->set(data, priority, pid, status);
        };

        HeapNode()
        {
            this->clear();
        }

        /**
         * Sets the data in the node so that the Heap can preform an insertion with it.
         */
        void set(T data, int priority, int pid, HeapNodeStatus status)
        {
            this->data = std::make_shared<T>(data);
            this->priority = priority;
            this->pid = pid;
            this->status = status;
        }

        /**
         * Clears the data in the node so that the Heap can tell if it's empty.
         */
        void clear()
        {
            this->priority = -1;
            this->pid = 0;
            this->status = HeapNodeStatus::empty;
        }
};

#define HEAP_NODE value
#endif /* ifndef HEAP_NODE */
