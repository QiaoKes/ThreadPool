#ifndef _PROJECT_LOGGER_H_
#define _PROJECT_LOGGER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <string>
#include <map>
#include <regex>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <cassert>

#ifdef WIN32
#define localtime_r(_TIME, _TM) localtime_s(_TM, _TIME)
#endif


namespace Thread {
namespace Logger {

enum class Level { Debug, Info, Warning, Error, Fatal };

template<typename T> class BaseLogger;

template<typename T>
class LoggerStream : public std::ostringstream {
public:
    LoggerStream(BaseLogger<T>&, Level);
    LoggerStream() = delete;
    LoggerStream(const LoggerStream&);
    LoggerStream(LoggerStream&&) = default;
    LoggerStream& operator=(const LoggerStream&) = delete;

    ~LoggerStream();
private:
    BaseLogger<T>& _logger;
    Level          _level;
};

template<typename T>
class BaseLogger {
    friend class LoggerStream<T>;
public:
    virtual LoggerStream<T> operator()(Level level = Level::Debug);

    template<typename... Args>
    static T& get_instance(Args&&... args);

protected:  
    BaseLogger() = default;
    BaseLogger(BaseLogger&) = delete;
    BaseLogger& operator=(const BaseLogger&) = delete;
    virtual ~BaseLogger() = default;
private:
    std::tm get_local_time();
    void end_line(Level, const std::string&);
    virtual void output(const std::tm&, const std::string&, const std::string&) = 0;
private:
    std::mutex   _mtx;
    std::tm      _localtime;
};


class ConsoleLogger : public BaseLogger<ConsoleLogger> {
    friend class BaseLogger<ConsoleLogger>;
private:
    ConsoleLogger() = default;
    void output(const std::tm&, const std::string&, const std::string&) override;
    ~ConsoleLogger() = default;
};

class FileLogger : public BaseLogger<FileLogger> {
    friend class BaseLogger<FileLogger>;
private:
    FileLogger(const std::string& fn = "create_at_" __DATE__ "_" __TIME__ ".log");
    void output(const std::tm&, const std::string&, const std::string&) override;
    ~FileLogger();
private:
    std::ofstream _file;
};


template<typename T>
LoggerStream<T>::LoggerStream(BaseLogger<T>& logger, Level level)
    : _logger(logger), _level(level) { }

template<typename T>
LoggerStream<T>::LoggerStream(const LoggerStream<T>& ls)
    : _logger(ls._logger), _level(ls._level) { }

template<typename T>
LoggerStream<T>::~LoggerStream() {
    try {
        _logger.end_line(_level, this->str());
    }
    catch (...) {
        //...
    }
}


template<typename T>
template<typename... Args>
T& BaseLogger<T>::get_instance(Args&&... args) {
    static T instance(std::forward<Args>(args)...);
    return instance;
}

template<typename T>
std::tm BaseLogger<T>::get_local_time() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    localtime_r(&time_t, &_localtime);
    return _localtime;
}

template<typename T>
LoggerStream<T> BaseLogger<T>::operator()(Level level) {
    return LoggerStream<T>(*this, level); //创建一个默认安全级别为debug的缓冲区
}

static const std::map<Level, std::string> lmap = {
        { Level::Debug, "Debug" },
        { Level::Info, "Info" },
        { Level::Warning, "Warning" },
        { Level::Error, "Error" },
        { Level::Fatal, "Fatal" },
};

template<typename T>
void BaseLogger<T>::end_line(Level level, const std::string& info) {
    std::unique_lock<std::mutex> lock(_mtx);
    auto local_time = get_local_time();
    auto level_str = lmap.find(level)->second;
    output(local_time, level_str, info);
}

}
}

#endif