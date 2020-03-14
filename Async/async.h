//
// Created by markfqwu on 2020-02-20.
//

#ifndef CPPTEMPLATE_ASYNC_H
#define CPPTEMPLATE_ASYNC_H

#include <iostream>
#include <cmath>
#include <thread>
#include <future>
#include <functional>

int f(int x, int y) { return std::pow(x,y); }

void task_lambda()
{
    std::packaged_task<int(int,int)> task([](int a, int b) {
        return std::pow(a, b);
    });
    std::future<int> result = task.get_future();

    task(2, 9);

    std::cout << "task_lambda:\t" << result.get() << '\n';
}

void task_bind()
{
    std::packaged_task<int()> task(std::bind(f, 2, 11));
    std::future<int> result = task.get_future();

    task();

    std::cout << "task_bind:\t" << result.get() << '\n';
}

void task_thread()
{
    std::packaged_task<int(int,int)> task(f);
    std::future<int> result = task.get_future();

    std::thread task_td(std::move(task), 2, 10);
    task_td.join();

    std::cout << "task_thread:\t" << result.get() << '\n';
}

void echo_test(int a){
    std::cout << a << std::endl;
}

class Base{
public:
    int a_;
    Base(int a){a_=a;}
    virtual void echo(){std::cout << a_ << std::endl;}
};

class Derived: public Base{
public:
    int b_;
    Derived(int a, int b):Base(a), b_(b){}
    void echo()override {std::cout << a_ << " and " << b_ << std::endl;}
};


#endif //CPPTEMPLATE_ASYNC_H
