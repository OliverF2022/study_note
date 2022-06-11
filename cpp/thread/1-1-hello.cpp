#include <iostream>

//管理线程的函数和类在thread中声明
#include <thread>

void hello()
{
    std::cout<<"Hello Concurrent World\n";
}

int main()
{
	//创建一个线程
    std::thread t(hello);
	//等待线程执行结束
    t.join();
}
