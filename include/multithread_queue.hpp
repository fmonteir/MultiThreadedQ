/*******************************************************************************
 * Multi-thread element queue project
 * 
 * Copyright 2024 FRANCISCO BRITO
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the “Software”), to deal in 
 * the Software without restriction, including without limitation the rights to 
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
 * of the Software, and to permit persons to whom the Software is furnished to do 
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#ifndef multithread_queue_hpp
#define multithread_queue_hpp

#include <iostream>
#include <limits>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <memory>

/*! Class representation of a queue */
template <typename T> class Queue {
    int front;                                        //!< Front of the queue
    int rear;                                         //!< Back of the queue
    int count;                                        //!< Current number of elements in the queue
    unsigned queueSize;                               //!< Maximum number of elements (queue size)
    T* array;                                         //!< Element array
    std::mutex qMutex;                                //!< Mutual exclusion
    std::condition_variable mutexCondition;           //!< Condition for Mutex
public:
    /*******************************************************************************
     * @brief Constructs a queue with a finite number of primitive types.
     * @param [in]  size  Size of the queue
     * @details The queue count is initialized to 0.
     ******************************************************************************/
    Queue(unsigned size){
        queueSize = size;                             //!< Initialize maximum queue size
        front = count = 0;                            //!< Initialize array index of front and current no. elements
        rear = size - 1;                              //!< Initialize array index of rear (for a queue of 2, it's 1)
        array = new T[queueSize];                     //!< Initialize array for queue dynamically
    }
    /*******************************************************************************
     * @brief Adds element to the queue.
     * @param [in]  element  Element to be added to the queue
     ******************************************************************************/
    void Push(T element){
        std::unique_lock<std::mutex> lock(qMutex);    //!< Unique lock Mutex
        std::cout << "Thread ID " << std::this_thread::get_id()
                                  << "\nPush(" << element << ")\n";
        if (count == queueSize)                       //!< If the queue is full, then pop
        {
            T element = array[front];                 //!< Front of the queue is dropped.
            std::cout << "Thread ID " << std::this_thread::get_id() << "\nElement " 
                                  << element << " dropped " << "\n";
            front = (front + 1) % queueSize;          //!< The array index for the front element changes correspondingly
            count = count - 1;                        //!< The pop decreases the number of elements in the queue
        }
        rear = (rear + 1) % queueSize;                //!< The array index for the back element changes when we push
        array[rear] = element;                        //!< An element is pushed to the back of the queue
        count = count + 1;                            //!< The push increases the number of elements in the queue
        lock.unlock();                                //!< Unlock Mutex
        mutexCondition.notify_one();                  //!< Lets the other thread know that we're done
    }
    /*******************************************************************************
     * @brief Removes element from the queue.
     * @param [out]  element  Element to be removed from the queue
     ******************************************************************************/
    T Pop()
    {
        std::unique_lock<std::mutex> lock(qMutex);    //!< Unique lock Mutex

        mutexCondition.wait(lock, 
                [this]() { return count != 0; });     //!< Block indefinitely until queue is not empty anymore

        T element = array[front];
        std::cout << "Thread ID " << std::this_thread::get_id()
                                  << "\nPop() -> " << element << "\n";
        front = (front + 1) % queueSize;              //!< The array index for the front element changes when we pop
        count = count - 1;                            //!< The pop decreases the number of elements in the queue
        
        return element;                               //!< Pop returns the element removed from the front of the queue
    }
    /*******************************************************************************
     * @brief Removes element from the queue, timing out after an amount time received as an input.
     * @param [in]   miliseconds  Threshold time in miliseconds for timeout to occur
     * @param [out]  element      Element to be removed from the queue
     ******************************************************************************/
    T PopWithTimeout(int milliseconds)
    {
        std::unique_lock<std::mutex> lock(qMutex);    //!< Unique lock Mutex

        auto thresholdTime = std::chrono::system_clock::now() 
        + std::chrono::milliseconds(milliseconds);     //!< Time threshold for exception to be thrown
        while(count == 0)
        {
            auto res = mutexCondition.wait_until(lock, thresholdTime);
            if (res == std::cv_status::timeout)       //!< If timeout occurs, Pop throws an exception
            {
                throw std::range_error( "Can't pop because the queue is already empty." );
            }
        }

        T element = array[front];
        std::cout << "Thread ID " << std::this_thread::get_id()
                                  << "\nPop() -> " << element << "\n";
        front = (front + 1) % queueSize;              //!< The array index for the front element changes when we pop
        count = count - 1;                            //!< The pop decreases the number of elements in the queue
        
        return element;                               //!< Returns the element removed from the front of the queue
    }
    /*******************************************************************************
     * @brief Outputs number of elements stored now.
     * @param [out]  count  Number of stored elements
     ******************************************************************************/
    int Count()
    {
        std::unique_lock<std::mutex> lock(qMutex);    //!< Unique lock Mutex
        return count;                                 
    }
    /*******************************************************************************
     * @brief Outputs maximum number of elements before push prompts a pop.
     * @param [out]  queueSize  Maximum number of elements
     ******************************************************************************/
    unsigned Size()
    {
        std::unique_lock<std::mutex> lock(qMutex);    //!< Unique lock Mutex
        return queueSize;
    }
};

#endif