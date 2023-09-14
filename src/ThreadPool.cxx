#include "ThreadPool.h"


void ThreadPool::Start(uint32_t num_threads) {
  const uint32_t max_num_threads = std::thread::hardware_concurrency() ; // Max # of threads the system supports
  _num_threads = num_threads < max_num_threads ? num_threads : max_num_threads;

  for (uint32_t ii = 0; ii < _num_threads; ++ii)
    _threads.emplace_back(std::thread(&ThreadPool::ThreadLoop,this));
}

void ThreadPool::ThreadLoop() {
  while (true) {
    std::function<void()> job;
    {
      std::unique_lock<std::mutex> lock(_queue_mutex);
      _mutex_condition.wait(lock, [this] {
        return !_jobs.empty() || _should_terminate;
      });
      if (_should_terminate)
        return;
      job = _jobs.front();
      _jobs.pop();
    }
    job();
    //--_running_jobs;
  }
}

void ThreadPool::QueueJob(const std::function<void()>& job) {
  {
    std::unique_lock<std::mutex> lock(_queue_mutex);
    _jobs.push(job);
    //++_running_jobs;
  }
  _mutex_condition.notify_one();
}

void ThreadPool::QueueJobs(const std::vector<std::function<void()>>::iterator & jobs_begin,
                           const std::vector<std::function<void()>>::iterator & jobs_end) {
  {
    std::unique_lock<std::mutex> lock(_queue_mutex);
    for (auto it = jobs_begin; it != jobs_end; ++it)
    {
      _jobs.push(*it);
      //++_running_jobs;
    }
  }
  _mutex_condition.notify_one();
}


bool ThreadPool::Wait() {
  //while (_running_jobs != 0) // (_jobs.size > 0)
  while(_jobs.size() > 0)
  {
    //if (_running_jobs < 0)
    //{
    //  std::cout << "THREADING ERROR!" << std::endl;
    //  return false;
    //}
  }
  return true;
}

void ThreadPool::Stop() {
  {
    std::unique_lock<std::mutex> lock(_queue_mutex);
    _should_terminate = true;
  }
  _mutex_condition.notify_all();
  for (std::thread& active_thread : _threads) 
      active_thread.join();
  _threads.clear();
}