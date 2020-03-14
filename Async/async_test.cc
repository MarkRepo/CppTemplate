//
// Created by markfqwu on 2020-02-20.
//

#include "gtest/gtest.h"
#include <future>
#include <iostream>
#include <thread>

#include <vector>
#include <thread>
#include <future>
#include <numeric>
#include <iostream>
#include <chrono>

#include "async.h"

TEST(AsyncTest, future){
    // 来自 packaged_task 的 future
    std::packaged_task<int()> task([](){ return 7; }); // 包装函数
    std::future<int> f1 = task.get_future();  // 获取 future
    std::thread(std::move(task)).detach(); // 在线程上运行

    // 来自 async() 的 future
    std::future<int> f2 = std::async(std::launch::async, [](){ return 8; });

    // 来自 promise 的 future
    std::promise<int> p;
    std::future<int> f3 = p.get_future();
    std::thread( [&p]{ p.set_value_at_thread_exit(9); }).detach();

    std::cout << "Waiting..." << std::flush;
    f1.wait();
    f2.wait();
    f3.wait();
    std::cout << "Done!\nResults are: " << f1.get() << ' ' << f2.get() << ' ' << f3.get() << '\n';
}

void accumulate(std::vector<int>::iterator first,
                std::vector<int>::iterator last,
                std::promise<int> accumulate_promise)
{
    int sum = std::accumulate(first, last, 0);
    accumulate_promise.set_value(sum);  // 提醒 future
}

void do_work(std::promise<void> barrier)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    barrier.set_value();
}

TEST(PromiseTest, promise)
{
    // 演示用 promise<int> 在线程间传递结果。
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
    std::promise<int> accumulate_promise;
    std::future<int> accumulate_future = accumulate_promise.get_future();
    std::thread work_thread(accumulate, numbers.begin(), numbers.end(),
                            std::move(accumulate_promise));

    // future::get() 将等待直至该 future 拥有合法结果并取得它
    // 无需在 get() 前调用 wait()
    //accumulate_future.wait();  // 等待结果
    std::cout << "result=" << accumulate_future.get() << '\n';
    work_thread.join();  // wait for thread completion

    // 演示用 promise<void> 在线程间对状态发信号
    std::promise<void> barrier;
    std::future<void> barrier_future = barrier.get_future();
    std::thread new_work_thread(do_work, std::move(barrier));
    barrier_future.wait();
    new_work_thread.join();
}

TEST(PackageTest, packageTask){

    task_lambda();
    task_bind();
    task_thread();

}

TEST(EchoTest, echo){

    using EchoFunc = void(*)(int);
    EchoFunc func = echo_test;
    func(5);
}

TEST(BDTest, bd){
    Base b = Base(5);
    Base &pb = b;
    pb.echo();
    Derived d = Derived(5,6);
    Base& pd = d;
    Derived& pd2 = dynamic_cast<Derived&>(d);
    std::cout << pd2.b_ << std::endl;
    pd2.echo();

}

TEST(BDTest, bd2){
    Derived* d = new Derived(5, 6);
    d->Base::echo();
}

