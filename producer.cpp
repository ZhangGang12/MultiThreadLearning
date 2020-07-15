#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;
const int MAX = 20;
mutex mtx;
condition_variable pro,com; //生产者消费者条件变量

queue<int> q;

void consumer() //消费者线程的入口
{
    while(true)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));

        unique_lock<mutex> lck(mtx);  
        com.wait(lck, []{return q.size() != 0; }); 

        cout << "comsumer " << this_thread::get_id() << ":";
        q.pop();
        cout << q.size() << "\n";

        pro.notify_all();
    }
}

void producer(int id) //生产者入口
{
    while(true)
    {
        this_thread::sleep_for(chrono::milliseconds(900)); //生产者比消费者快
        unique_lock<mutex> lck(mtx);
        pro.wait(lck,[]{return q.size() < MAX;});  //如果小于MAX就进入，否则不能进入
        
        cout << "->prodcer " << this_thread::get_id() << ":";
        q.push(id);
        cout << q.size() << endl;

        com.notify_all();
    }
}

int main()
{
    thread consumers[2],producers[2];

    for(int i = 0; i < 2; i++)
    {
        //创建两个消费者和生产者
        producers[i] = thread(producer, i + 1);
        consumers[i] = thread(consumer);
    }

    for(int i = 0; i < 2; ++i)
    {
        producers[i].join();
        consumers[i].join();
    }
    system("pasue");
    return 0;
}
