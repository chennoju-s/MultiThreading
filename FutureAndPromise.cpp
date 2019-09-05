/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <future>

using namespace std;

class A {
    mutex m_mutex;
    condition_variable m_condVar;
    bool signalSent = false;
    
public:
    void sendSignal() {
        
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::cout << "sending signal.... @ " << std::ctime(&now) << std::endl;
        
        std::lock_guard<mutex> lg(m_mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        this->signalSent = true;
        
        m_condVar.notify_one();
    }
    
    bool isSignalSent() const {
        return signalSent;
    }
    
    void receiveSignal(std::promise<int> *promiseObj) {
        std::unique_lock<mutex> mLock(m_mutex);
        //m_condVar.wait(mLock, std::bind(&A::isSignalSent, this) );
        m_condVar.wait(mLock, [this] { return signalSent; });
        
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::cout << "signal received @ " << std::ctime(&now) << std::endl;
        
        promiseObj->set_value(9);
    }
};

int main()
{
    printf("Hello World \n");
    A a;
    
    std::promise<int> promiseObj;
    std::future<int> futureObj = promiseObj.get_future();
    
    thread t1(&A::sendSignal, &a);
    thread t2(&A::receiveSignal, &a, &promiseObj);
    
    cout << futureObj.get() << endl;
    
    t1.join();
    t2.join();

    return 0;
}
