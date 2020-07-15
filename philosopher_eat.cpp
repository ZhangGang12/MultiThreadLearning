#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class semaphore
{
private:
    mutex mtx;
    condition_variable cv;
    int count;
public:
    semaphore(int value = 1):count(value) {}

    void wait()
    {
        unique_lock<mutex> lck(mtx);
        if(--count < 0)cv.wait(lck);
    }

    void signal()
    {
        unique_lock<mutex> lck(mtx);
        if(++count <= 0)
            cv.notify_one();
    }
};


semaphore sem[5];

mutex mt;

void pid(int id)
{
    int left = (id+6)%5;
    int right = (id+4)%5;
    while(true)
    {
    unique_lock<mutex> lck(mt);
    sem[left].wait();
    sem[right].wait();
    lck.unlock();
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "Philosopher " << id << " eat \n";
    sem[left].signal();
    sem[right].signal();
    }
}

int main()
{
    thread ths[5];
    for(int i = 0; i < 5; i++)
    {
        ths[i] = thread(pid,i+1);
    }

    for(int i = 0; i < 5; i++)
    {
        ths[i].join();
    }
    return 0;
}
