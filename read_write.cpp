#include <iostream>
#include <thread>
#include <condition_variable>
#include <vector>
#include <mutex>

using namespace std;

//先实现一个信号量的类
class semaphore
{
public:
    semaphore(int value = 1) :counts(value){}

    void wait()
    {
        unique_lock<mutex> lck(mtxs);
        if(--counts < 0)
            cv.wait(lck);
    }

    void signal()
    {
        unique_lock<mutex> lck(mtxs);
        if(++counts <= 0)
            cv.notify_one();
    }
private:
    int counts;
    mutex mtxs;
    condition_variable cv;
};
//读者与读者不互斥，写者与写者与读者互斥
//读者与写者互斥
int count; //当前读者的数量


mutex mtx; //用于保证读者与写者的互斥
semaphore sm; //保证读者写者的互斥


void write(int i)
{
    while(true)
    {
    sm.wait();
    cout << "writer " << i << " start writing " << endl;
    cout << "writer " << i << " end writing " << endl;
    sm.signal();
    }
}

void reader(int i)
{
    while(true)
    {
    mtx.lock();
    if(count == 0) sm.wait();
    ++count;
    mtx.unlock();
    cout << "reader " << i << " start read " << endl;
    cout << "reader " << i << " end read " << endl;
    mtx.lock();
    --count;
    if(count == 0) sm.signal();
    mtx.unlock();
    }
}

int main()
{
    thread ths[8];
    for(int i = 0; i < 2; i++)
    {
       ths[i] = thread(write,i);
    }
    for(int i = 2; i < 8; i++)
    {
        ths[i] = thread(reader,i);
    }

    for(int i = 0; i < 8; i++)
        ths[i].join();
    return 0;
}

