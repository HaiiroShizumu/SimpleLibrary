#include "logger.h"
#include <fstream>
#include <ctime>
#include <iomanip>

std::unique_ptr<Logger> Logger::instance = nullptr;
std::mutex Logger::log_mutex;
LogLevel Logger::current_level = LogLevel::INFO;
std::string Logger::log_file_name = "";

void Logger::init(const std::string& log_file, LogLevel default_level) {
    std::lock_guard<std::mutex> lock(log_mutex);
    if (!instance) {
        instance = std::unique_ptr<Logger>(new Logger());
        current_level = default_level;
        log_file_name = log_file;
    }
}

void Logger::set_log_level(LogLevel new_level) {
    std::lock_guard<std::mutex> lock(log_mutex);
    current_level = new_level;
}

LogLevel Logger::get_current_level() {
    return current_level;
}

void Logger::log(const std::string& message, LogLevel level) {
    if (level < current_level) return;
    
    std::lock_guard<std::mutex> lock(log_mutex);
    
    std::ofstream log_file(log_file_name, std::ios::app);
    if (!log_file.is_open()) {
        throw std::runtime_error("Cannot open log file: " + log_file_name);
    }
    
    log_file << "[" << get_current_time() << "] "
             << "[" << level_to_string(level) << "] "
             << message << "\n";
}

std::string Logger::level_to_string(LogLevel level) {
    switch(level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:               return "UNKNOWN";
    }
}

std::string Logger::get_current_time() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}