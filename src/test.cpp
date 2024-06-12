#define BOOST_TEST_MODULE Example required in 1
#include "boost/test/included/unit_test.hpp"
#include "multithread_queue.hpp"

/*******************************************************************************
 * @brief Function for read thread.
 * @param [in]  queue  Class representing a queue
 ******************************************************************************/
template <typename T> 
void read(Queue<T>& queue)
{
    for (unsigned i = 0; i < 4; i++)                  //!< Pop 4 times
    {
        queue.Pop();
    }
}

/*******************************************************************************
 * @brief Function for write thread.
 * @param [in]  queue  Class representing a queue
 * @param [in]  msSleep  Small time (in milliseconds) to ensure that condition variable is seen by Pop
 * @param [in]  elements  Elements to be added to the queue
 ******************************************************************************/
template <typename T> 
void write(Queue<T>& queue, int msSleep, std::vector<T>& elements)
{
    queue.Push(elements.at(0));                       //!< Push first element
    std::this_thread::sleep_for(
        std::chrono::milliseconds{msSleep});          //!< A small amount of sleep allows read thread to lock
    queue.Push(elements.at(1));                       //!< Push second element
    queue.Push(elements.at(2));                       //!< Push third element
    queue.Push(elements.at(3));                       //!< Push fourth element
    std::this_thread::sleep_for(
        std::chrono::milliseconds{msSleep});          //!< A small amount of sleep allows read thread to lock
    queue.Push(elements.at(4));                       //!< Push fifth element
}

/*******************************************************************************
 * @brief Function for read thread with a longer queue with max 4 elements.
 * @param [in]  queue  Class representing a queue
 * @param [in]  msThrowException  Time in ms to throw exception
 ******************************************************************************/
template <typename T> 
void readLong(Queue<T>& queue, int msThrowException)
{
    for (unsigned i = 0; i < 6; i++)                  //!< Pop 6 times
    {
        queue.Pop();
    }
}

/*******************************************************************************
 * @brief Function for write thread with a longer queue with max 4 elements.
 * @param [in]  queue  Class representing a queue
 * @param [in]  msSleep  Small time (in milliseconds) to ensure that condition variable is seen by Pop
 * @param [in]  elements  Elements to be added to the queue
 ******************************************************************************/
template <typename T> 
void writeLong(Queue<T>& queue, int msSleep, std::vector<T>& elements)
{
    queue.Push(elements.at(0));                       //!< Push first element
    std::this_thread::sleep_for(
        std::chrono::milliseconds{msSleep});          //!< A small amount of sleep allows read thread to lock
    queue.Push(elements.at(1));                       //!< Push second element
    queue.Push(elements.at(2));                       //!< Push third element
    queue.Push(elements.at(3));                       //!< Push fourth element
    queue.Push(elements.at(4));                       //!< Push fifth element
    queue.Push(elements.at(5));                       //!< Push sixth element
                                                      //!< Many consecutive pushes may give Pop enough time to unblock
                                                      //!< So, on read thread, I use PopWithTimeout
    std::this_thread::sleep_for(
        std::chrono::milliseconds{msSleep});          //!< A small amount of sleep allows read thread to lock
    queue.Push(elements.at(6));                       //!< Push seventh element
}

/*******************************************************************************
 * @brief Function to check whether the right error message is printed.
 * @param [in]  ex  Range error thrown by PopWithTimeout
 ******************************************************************************/
bool errorMessage(const std::range_error& excep)
{
    BOOST_CHECK_EQUAL(excep.what(), std::string(      //!< Expected error message
        "Can't pop because the queue is already empty."));
    return true;
}

/*******************************************************************************
 * @brief Test case for example required in 1: Ensures that class methods work.
 * @par Parameters
 *    None.
 ******************************************************************************/
BOOST_AUTO_TEST_CASE(Example_1)
{
    std::cout << "\nFirst test case - Example required in 1.\n\n";
    Queue<int> queue(2);                               //!< Initialize queue: dynamic memory allocation inside class

    int msSleep = 5;                                   //!< Small time in ms to guarantee that Pop sees condition variable
    int msThrowException = 500;                        //!< Time in ms for PopWithTimeout to throw exception
    std::vector<int> q1{1, 2, 3, 4, 5};                //!< Elements of the queue

    std::thread writer(                                //!< Writing thread
        write<int>,                                    //!< Receives write function
        std::ref(queue),                               //!< Receives queue
        msSleep,                                       //!< Receives small ms delay
        std::ref(q1));                                 //!< Writing thread
    std::thread reader(                                //!< Reading thread
        read<int>,                                     //!< Receives read function
        std::ref(queue));                              //!< Receives queue
    writer.join(); reader.join();                      //!< Join threads at the end

    BOOST_TEST_MESSAGE( "Testing whether the maximum number of elements allowed in the queue is 2." );
    BOOST_TEST(queue.Size() == 2, "The size of the queue is not 2.");
    BOOST_TEST_MESSAGE( "Testing whether the queue is empty." );
    BOOST_TEST(queue.Count() == 0, "The queue is not empty.");
    BOOST_TEST_MESSAGE( "Testing whether PopWithTimeOut throws an exception when it tries to pop an empty queue at the end." );
    BOOST_CHECK_EXCEPTION(
        queue.PopWithTimeout(msThrowException), 
        std::range_error, errorMessage);                //!< Popping a 5th time should throw a range error exception.
}

/*******************************************************************************
 * @brief Test case to ensure that templates work: use char insted of int.
 * @par Parameters
 *    None.
 ******************************************************************************/
BOOST_AUTO_TEST_CASE(Test_Template_Char)
{
    std::cout << "\nSecond test case - Same as 1, but queue elements are now char insted of int.\n\n";
    Queue<char> qChar(2);                              //!< Initialize queue: dynamic memory allocation inside class

    int msSleep = 5;                                   //!< Small time in ms to guarantee that Pop sees condition variable
    int msThrowException = 500;                        //!< Time in ms for PopWithTimeout to throw exception
    std::vector<char> q2{'A', 'B', 'C', 'D', 'E'};     //!< Elements of the queue

    std::thread writer(                                //!< Writing thread
        write<char>,                                   //!< Receives write function
        std::ref(qChar),                               //!< Receives queue
        msSleep,                                       //!< Receives small ms delay
        std::ref(q2));                                 //!< Writing thread
    std::thread reader(                                //!< Reading thread
        read<char>,                                    //!< Receives read function
        std::ref(qChar));                              //!< Receives queue
    writer.join(); reader.join();                      //!< Join threads at the end

    BOOST_TEST_MESSAGE( "Testing whether the maximum number of elements allowed in the queue is 2." );
    BOOST_TEST(qChar.Size() == 2, "The size of the queue is not 2.");
    BOOST_TEST_MESSAGE( "Testing whether the queue is empty." );
    BOOST_TEST(qChar.Count() == 0, "The queue is not empty.");
    BOOST_TEST_MESSAGE( "Testing whether PopWithTimeOut throws an exception when it tries to pop an empty queue at the end." );
    BOOST_CHECK_EXCEPTION(
        qChar.PopWithTimeout(msThrowException), 
        std::range_error, errorMessage);                //!< Popping a 5th time should throw a range error exception.
}

/*******************************************************************************
 * @brief Test case for a queue with 4 elements: sequence explained in README.md
 * @par Parameters
 *    None.
 ******************************************************************************/
BOOST_AUTO_TEST_CASE(QueueOfFour)
{
    std::cout << 
    "\nThird test case - Example explained in README.md (queue of 4 with more consecutive pushes).\n\n";
    Queue<int> qLong(4);                               //!< Initialize queue: dynamic memory allocation inside class

    int msSleep = 5;                                   //!< Small time in ms to guarantee that Pop sees condition variable
    int msThrowException = 500;                        //!< Time in ms for PopWithTimeout to throw exception
    std::vector<int> q3{1, 2, 3, 4, 5, 6, 7};          //!< Elements of the queue

    std::thread writerLong(                            //!< Writing thread
        writeLong<int>,                                //!< Receives write function
        std::ref(qLong),                               //!< Receives queue
        msSleep,                                       //!< Receives small ms delay
        std::ref(q3));                                 //!< Writing thread
    std::thread readerLong(                            //!< Reading thread
        readLong<int>,                                 //!< Receives read function
        std::ref(qLong),                               //!< Receives queue
        msThrowException);                             //!< Receives time to throw exception                        
    writerLong.join(); readerLong.join();              //!< Join threads at the end

    BOOST_TEST_MESSAGE( "Testing whether the maximum number of elements allowed in the queue is 4." );
    BOOST_TEST(qLong.Size() == 4, "The size of the queue is not 4.");
    BOOST_TEST_MESSAGE( "Testing whether the queue is empty." );
    BOOST_TEST(qLong.Count() == 0, "The queue is not empty.");
    BOOST_TEST_MESSAGE( "Testing whether PopWithTimeOut throws an exception when it tries to pop an empty queue at the end." );
    BOOST_CHECK_EXCEPTION(
        qLong.PopWithTimeout(msThrowException), 
        std::range_error, errorMessage);                //!< Popping a 7th time should throw a range error exception.
}