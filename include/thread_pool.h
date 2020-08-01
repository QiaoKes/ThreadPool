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
#include<thread>

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
		void stop_until_empty();

		template<typename Func, typename... Args>
		void add_one_task(Func&&, Args&&...);

	private:
		void thread_loop();
		TaskSPtr get_one_task();
		void __add_one_task(TaskSPtr&&);

		std::size_t			_thread_size;
		std::atomic<bool>	_is_shutdown;

		ThreadUPtrVector	_Threads;
		TaskSPtrQueue		_Tasks;

		std::mutex				_mtx;
		std::mutex              _mtx2;
		std::mutex              _queue_mtx;
		std::condition_variable	_cond;
		std::condition_variable	_queue_cond;
	};

	template<typename Func, typename... Args>
	void thread_pool::add_one_task(Func&& f, Args&&... args) {
		std::unique_lock<std::mutex> lock(_queue_mtx);
		__add_one_task(
			std::make_shared<Task>(
				std::forward<Func>(f),
				std::forward<Args>(args)...
			)
		);
	}

}
#endif // !_PROJECT_THREAD_POOL_H_
