// Take from here: https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

#include <iostream>
#include <vector>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>

class ThreadPool {
public:
  void Start(uint32_t num_threads);
  void QueueJob(const std::function<void()>& job);
  void QueueJobs(const std::vector<std::function<void()>>::iterator & jobs_begin,
                 const std::vector<std::function<void()>>::iterator& jobs_end);
  void Stop();
  bool Wait();
  inline const int GetNumThreads() const {return _num_threads;};

private:
  void ThreadLoop();

  uint32_t _num_threads = 1;
  //std::atomic<int> _running_jobs = 0;       // Actually, solution with this var is processed twice as slow
  bool _should_terminate = false;           // Tells threads to stop looking for jobs
  std::mutex _queue_mutex;                  // Prevents data races to the job queue
  std::condition_variable _mutex_condition; // Allows threads to wait on new jobs or termination 
  std::vector<std::thread> _threads;
  std::queue<std::function<void()>> _jobs;
};