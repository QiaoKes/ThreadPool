#include "thread_pool.h"
#include "logger.h"

namespace Thread {

	void Task::run() {
		_func();
	}

	Task::~Task() = default;

	thread_pool::thread_pool(std::size_t thread_size = 5)
		: _thread_size(thread_size), _is_shutdown(false),
		_mtx(), _mtx2(), _queue_mtx(), _cond(), _queue_cond() {
	}



	thread_pool::~thread_pool() {
		if (!_is_shutdown) {
			try {
				stop();
			}
			catch (std::exception& e) {
				//log;
				using namespace Thread::Logger;
				auto&& FL = FileLogger::get_instance();
				FL(Level::Fatal) << e.what();
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

	void thread_pool::stop_until_empty() {
		std::unique_lock<std::mutex> lock1(_mtx2);
		std::unique_lock<std::mutex> lock2(_queue_mtx);
		while (!_Tasks.empty()) {
			_queue_cond.wait(lock1);
		}
		stop();
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
			if (_Tasks.empty()) {
				std::unique_lock<std::mutex> lock2(_mtx2);
				_queue_cond.notify_all();
			}
		}

		return t;
	}

	void thread_pool::__add_one_task(thread_pool::TaskSPtr&& t) {
		std::unique_lock<std::mutex> lock(_mtx);
		_Tasks.emplace(t);
		_cond.notify_one();
	}

}
