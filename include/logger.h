#ifndef _PROJECT_LOGGER_H_
#define _PROJECT_LOGGER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <string>

namespace Thread {

namespace Logger {

enum class Level { Debug, Info, Warning, Error, Fatal };
class LoggerStream;
class LoggerBase;
class FileLogger;
class ConsoleLogger;


class LoggerStream : public std::ostringstream {
public:
    LoggerStream(LoggerBase&, Level);
    LoggerStream() = delete;
    LoggerStream(const LoggerStream&);
    LoggerStream(LoggerStream&&) = default;
    LoggerStream& operator=(const LoggerStream&) = delete;

    ~LoggerStream();
private:
    LoggerBase& _logger;
    Level       _level;
};


class LoggerBase {
    friend class LoggerStream;
public:
    LoggerBase() = default;
    virtual ~LoggerBase() = default;
    virtual LoggerStream operator()(Level level = Level::Debug);
private:
    std::tm get_local_time();
    void end_line(Level, const std::string&);
    virtual void output(const std::tm&, const std::string&, const std::string&) = 0;
private:
    std::mutex  _mtx;
    std::tm     _localtime;
};


class ConsoleLogger : public LoggerBase {
public:
    ConsoleLogger() = default;
    ConsoleLogger(const ConsoleLogger&) = delete;
    ConsoleLogger(ConsoleLogger&&) = delete;
    ConsoleLogger& operator=(const ConsoleLogger&) = delete;
    virtual ~ConsoleLogger() = default;
private:
    void output(const std::tm&, const std::string&, const std::string&) override;
};

class FileLogger : public LoggerBase {
public:
    FileLogger(const std::string& fn = "create_at_" __DATE__ "_" __TIME__ ".log");
    FileLogger(const FileLogger&) = delete;
    FileLogger(FileLogger&&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;
    virtual ~FileLogger();
private:
    void output(const std::tm&, const std::string&, const std::string&) override;
private:
    std::ofstream _file;
};

}
}

#endif