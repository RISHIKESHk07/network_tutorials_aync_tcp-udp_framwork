#include <mutex>
#include <queue>
namespace ThreadSafe_Queue {
template <typename T> class TSQueue {
public:
  TSQueue() = default;
  TSQueue(const TSQueue<T> &) = delete; // not letting the queue to be copied
  virtual ~TSQueue() { clear(); }
  const T &front() {
    std::scoped_lock lock(mutexQ);
    return Dqueue.front();
  }

  const T &insert(T &item) {
    std::scoped_lock lock(mutexQ);
    return Dqueue.emplace_back(item);
  }

  void clear() {
    std::scoped_lock lock(mutexQ);
    Dqueue.clear();
  }

  T pop_front() {
    std::scoped_lock lock(mutexQ);
    auto t = move(Dqueue.front());
    Dqueue.pop_front();
    return t;
  }

  T pop_back() {
    std::scoped_lock lock(mutexQ);
    return Dqueue.pop_back();
  }
  bool empty() {
    std::scoped_lock lock(mutexQ);
    return Dqueue.empty();
  }

protected:
  std::mutex mutexQ;
  std::deque<T> Dqueue;
};
} // namespace ThreadSafe_Queue
