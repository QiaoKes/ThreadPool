#include "logger.h"

namespace Thread {
namespace Logger {

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


void ConsoleLogger::output(const std::tm& tm,
                            const std::string& level,
                            const std::string& info) {
    std::cout << "[" << tm << "]"
        << "[" << level << "]"
        << " " << info << '\n';
    std::cout.flush();
}

FileLogger::FileLogger(const std::string& filename)
    : BaseLogger() {
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
    catch (...) {
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