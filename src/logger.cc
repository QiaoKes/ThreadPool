#include "logger.h"
#include <regex>
#include <ctime>
#include <chrono>
#include <map>
#include <iomanip>
#include <cassert>

#ifdef WIN32
#define localtime_r(_TIME, _TM) localtime_s(_TM, _TIME)
#endif

namespace Thread {
namespace Logger {

LoggerStream::LoggerStream(LoggerBase& logger, Level level)
    : _logger(logger), _level(level) { }

LoggerStream::LoggerStream(const LoggerStream& ls)
    : _logger(ls._logger), _level(ls._level) { }

LoggerStream::~LoggerStream() {
    try {
        _logger.end_line(_level, this->str());
    }
    catch (...) {
        //...
    }
}

std::tm LoggerBase::get_local_time() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    localtime_r(&time_t, &_localtime);
    return _localtime;
}

LoggerStream LoggerBase::operator()(Level level) {
    return LoggerStream(*this, level); //创建一个默认安全级别为debug的缓冲区
}

static const std::map<Level, std::string> lmap = {
        { Level::Debug, "Debug" },
        { Level::Info, "Info" },
        { Level::Warning, "Warning" },
        { Level::Error, "Error" },
        { Level::Fatal, "Fatal" },
};

void LoggerBase::end_line(Level level, const std::string& info) {
    std::unique_lock<std::mutex> lock(_mtx);
    auto local_time = get_local_time();
    auto level_str = lmap.find(level)->second;
    output(local_time, level_str, info);
}

std::ostream& operator<<(std::ostream& stream, const std::tm& tm)
{
    using std::setw;
    using std::setfill;
    return stream << 1900 + tm.tm_year << '-'
        << setfill('0') << setw(2) << tm.tm_mon + 1 << '-'
        << setfill('0') << setw(2) << tm.tm_mday << ' '
        << setfill('0') << setw(2) << tm.tm_hour << ':'
        << setfill('0') << setw(2) << tm.tm_min << ':'
        << setfill('0') << setw(2) << tm.tm_sec;
}

void ConsoleLogger::output( const std::tm& tm,
                            const std::string& level,
                            const std::string& info) {
    std::cout << "[" << tm << "]"
        << "[" << level << "]"
        << " " << info << '\n';
    std::cout.flush();
}

FileLogger::FileLogger(const std::string& filename) 
    : LoggerBase() {
    std::regex r("/|:| |>|<|\"|\\*|\\?|\\|");
    auto real_name = std::regex_replace(filename, r, "_");
    _file.open(real_name, std::fstream::out | std::fstream::app | std::fstream::ate);
    assert(!_file.fail());
}

FileLogger::~FileLogger()
{
    try {
        _file.flush();
        _file.close();  
    }
    catch(...) {
        //...
    }
}

void FileLogger::output(const std::tm& tm,
                        const std::string& level,
                        const std::string& info) {
    _file << "[" << tm << "]"
        << "[" << level << "]"
        << " " << info << '\n';
    _file.flush();
}

}
}