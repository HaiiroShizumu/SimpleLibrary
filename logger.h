#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <memory>
#include <mutex>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static void init(const std::string& log_file, LogLevel default_level);

    static void set_log_level(LogLevel new_level);

    static void log(const std::string& message, LogLevel level = LogLevel::INFO);

    static LogLevel get_current_level();

private:
    Logger() = default;

    static std::string level_to_string(LogLevel level);

    static std::string get_current_time();

    static std::unique_ptr<Logger> instance;

    static std::mutex log_mutex;

    static LogLevel current_level;

    static std::string log_file_name;
};

#endif