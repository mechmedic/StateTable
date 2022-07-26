/**
 ***************************************************************************
 * \file  cmdQueue.h
 * \brief Header file for 'cmdQueue' (Command Queue) class.
 * This class is based on 'mtsQueue' of the CISST library which is a
 * queue that can be accessed in a thread-safe manner, assuming that there
 * is only one reader and one writer and that pointer updates are atomic.
 * Thus only one thread should call 'Get' and one thread should call 'Put'
 * and one cmdQue is needed for each connection between the components.
 * This queue is used to implement 'MailBox' of each software component
 * where the commands and events are queued
 * Created : 2022.07.06 Chunwoo Kim (CKim)
 * Last Updated : 2022.07.06 Chunwoo Kim (CKim)
 * Contact Info : cwkim@kist.re.kr
 ***************************************************************************
**/

#ifndef CMDQUEUE_H
#define CMDQUEUE_H

//#include <cisstMultiTask/mtsGenericObjectProxy.h>

#include <cstddef>      // ptrdiff_t
#include <stddef.h>     // size_t
#include <atomic>       // atomic pointer update
#include <iostream>


template<class _elementType>
class cmdQueue
{
public:
    typedef _elementType value_type;
    typedef value_type * pointer;
    typedef std::atomic<value_type*> atomicPointer;
    typedef const value_type * const_pointer;
    typedef value_type & reference;
    typedef const value_type & const_reference;
    typedef size_t size_type;
    typedef size_t index_type;

protected:
    pointer Data;
    atomicPointer Head;
    atomicPointer Tail;
    pointer Sentinel;  // end marker
    size_type Size;

    // private method, can only be used once by constructor.  Doesn't support resize!
    void Allocate(size_type size, const_reference value) {
        this->Size = size;
        if (this->Size > 0) {
            this->Data = new value_type[this->Size];
            index_type index;
            // CKim - new(ptr) MyClass() : Construct a new MyClass at ptr
            for (index = 0; index < this->Size; index++) {
                new(&this->Data[index]) value_type(value);
            }
        } else {
            this->Data = 0;
        }
        // head == tail implies empty queue
        this->Head = this->Data;
        this->Tail = this->Data;
        this->Sentinel = this->Data + this->Size;
    }

public:

    inline cmdQueue(void):
        Data(0),
        Head(0),
        Tail(0),
        Sentinel(0),
        Size(0)
    {}


    inline cmdQueue(size_type size, const_reference value) {
        Allocate(size, value);
    }


    inline ~cmdQueue() {
        delete [] Data;
    }


    /*! Sets the size of the queue (destructive, i.e. won't preserve
      previously queued elements). */
    inline void SetSize(size_type size, const_reference value) {
        delete [] Data;
        this->Allocate(size, value);
    }


    /*! Returns size of queue. */
    inline size_type GetSize(void) const {
        return Size;
    }


    /*! Returns number of elements available in queue, i.e. the number
      of slots used. */
    inline size_type GetAvailable(void) const
    {
        ptrdiff_t available = Head - Tail;
        if (available < 0) {
            available += Size;
        }
        return available;
    }

    
    /*! Returns true if queue is full. */
    inline bool IsFull(void) const {
        pointer oneAfterHead = this->Head + 1;
        if (oneAfterHead >= this->Sentinel) {
            oneAfterHead = this->Data;
        }
        return oneAfterHead == this->Tail;
    }


    /*! Returns true if queue is empty. */
    inline bool IsEmpty(void) const {
        return Head == Tail;
    }


    /*! Copy an object to the queue.
      \param in reference to the object to be copied
      \result Pointer to element in queue (use iterator instead?)
    */
    inline const_pointer Put(const_reference newObject)
    {
        pointer newHead = this->Head + 1;
        // test if end of buffer (same as IsFull method)
        if (newHead >= this->Sentinel) {
            newHead = this->Data;
        }
        // test if full
        if (newHead == this->Tail) {
            return 0;    // queue full
        }
        // Queue new object and move head
        // CKim - Put new object at the head. Called from the writer thread
        // Dereferencing atomic pointer is not atomic and also copy operation of
        // the newObject may not be atomic. So reading thread may interrupt but
        // the reading thread will be accessing 'Tail'
        *(this->Head) = newObject;
        // CKim - Storing address to atomic pointer is an atomic operation
        this->Head.store(newHead);
        return this->Head;
    }


    /*! Get a pointer to the next object to be read, but do not
        remove the item from the queue.
        \result Pointer to top element in queue (use iterator instead?)
     */
    inline pointer Peek(void) const {
        if (this->IsEmpty()) {
            return 0;
        }
        return this->Tail;
    }


    /*! Pop the next object to be read from the queue.
        \result Pointer to element just popped (use iterator instead?)
     */
    inline const_pointer Get(void) {
        if (this->IsEmpty()) {
            return 0;
        }
        // CKim - Reading address from atomic pointer is an atomic operation
        pointer result = this->Tail.load();
        pointer newTail = this->Tail +1;
        if (newTail >= this->Sentinel) {
            newTail = this->Data;
        }
        // CKim - Storing address to atomic pointer is an atomic operation
        this->Tail.store(newTail);
        return result;
    }

};


#endif // CMDQUEUE_H

