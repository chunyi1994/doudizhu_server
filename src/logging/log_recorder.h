#ifndef LOG_RECORDER_H
#define LOG_RECORDER_H
#include <string>
#include <memory>
#include <atomic>
#include <functional>
#include "blocking_queue.h"
#include "logger.h"
namespace logging {
class LogRecorder {
public:
    LogRecorder();
    void submit(std::string msg);
    void stop();
    static Logger::OutputCallback callback();

private:
    BlockingQueue<std::string> queue_;
    std::atomic_bool quit_;

};

} // namespace
#endif // LOG_RECORDER_H
