#pragma once
#ifndef _PROJECT_THREAD_POOL_H_
#define _PROJECT_THREAD_POOL_H_
#include<functional>
#include<vector>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<memory>
#include<iostream>
#include<atomic>
#include<type_traits>

namespace Thread {


class Task {
public:
	template<typename Func, typename... Args>
	Task(Func&&, Args&&...);

	void run();
	~Task();
private:
	std::function<void()> _func;
};


template<typename Func, typename... Args>
Task::Task(Func&& f, Args&&... args) {
	_func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
}

void Task::run() {
	_func();
}

Task::~Task() = default;

class thread_pool {
public:
	typedef std::unique_ptr<std::thread> ThreadUPtr;
	typedef std::shared_ptr<Task>		 TaskSPtr;
	typedef std::vector<ThreadUPtr>		 ThreadUPtrVector;
	typedef std::queue<TaskSPtr>		 TaskSPtrQueue;

	thread_pool(std::size_t);
	~thread_pool();

	void start();
	void stop();

	template<typename Func, typename... Args>
	void add_one_task(Func&&, Args&&...);
	
private:
	void thread_loop();
	TaskSPtr get_one_task(); 
	void __add_one_task(TaskSPtr&&);

	std::size_t   _thread_size;
	volatile bool _is_shutdown;
	
	ThreadUPtrVector _Threads;
	TaskSPtrQueue	 _Tasks;

	std::mutex				_mtx;
	std::condition_variable _cond;
};


thread_pool::thread_pool(std::size_t thread_size = 5) 
	: _thread_size(thread_size), _is_shutdown(false), _mtx(), _cond() {
}



thread_pool::~thread_pool() {
	if (!_is_shutdown) {
		try {
			stop();
		}
		catch (...) {
			//log;
			std::abort();
		}
	}
}

void thread_pool::start() {
	std::unique_lock<std::mutex> lock(_mtx);
	_is_shutdown = false;
	for (std::size_t i = 0; i < _thread_size; ++i) {
		_Threads.emplace_back(
			std::make_unique<std::thread>(&thread_pool::thread_loop, this)
		);
	}
}

void thread_pool::stop() {
	{
		std::unique_lock<std::mutex> lock(_mtx);
		_is_shutdown = true;
		_cond.notify_all();
	}

	for (std::size_t i = 0; i < _thread_size; ++i) {
		_Threads[i]->join();
	}
	
	_Threads.clear();
}

void thread_pool::thread_loop() {
	while (!_is_shutdown) {
		TaskSPtr t = get_one_task();
		if (t != nullptr) {
			t->run();
		}
	}
}

thread_pool::TaskSPtr thread_pool::get_one_task() {
	std::unique_lock<std::mutex> lock(_mtx);
	while (_Tasks.empty() && !_is_shutdown) {
		_cond.wait(lock);
	}

	TaskSPtr t = nullptr;
	if (!_Tasks.empty() && !_is_shutdown) {
		t = _Tasks.front();
		_Tasks.pop();
	}

	return t;
}

template<typename Func, typename... Args>
void thread_pool::add_one_task(Func&& f, Args&&... args) {
	__add_one_task(
		std::make_shared<Task>(
			std::forward<Func>(f),
			std::forward<Args>(args)...
		)
	);
}

void thread_pool::__add_one_task(thread_pool::TaskSPtr&& t) {
	std::unique_lock<std::mutex> lock(_mtx);
	_Tasks.emplace(t);
	_cond.notify_one();
}



}
#endif // !_PROJECT_THREAD_POOL_H_
