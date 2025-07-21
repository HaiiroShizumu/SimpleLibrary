#include "logger.h"
#include <iostream>
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>

std::queue<std::pair<std::string, LogLevel>> message_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;

std::atomic<bool> stop_thread(false);

void log_worker() {
    while (true) {
        std::pair<std::string, LogLevel> message;

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [] {
                return !message_queue.empty() || stop_thread.load();
                });

            if (stop_thread && message_queue.empty()) return;

            message = message_queue.front();
            message_queue.pop();
        }

        try {
            Logger::log(message.first, message.second);
        }
        catch (const std::exception& e) {
            std::cerr << "Logging error: " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <log_file> <log_level>" << std::endl;
        std::cerr << "Log levels: 0-DEBUG, 1-INFO, 2-WARNING, 3-ERROR" << std::endl;
        return 1;
    }

    std::string log_file = argv[1];
    LogLevel log_level = static_cast<LogLevel>(std::stoi(argv[2]));

    try {
        Logger::init(log_file, log_level);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to initialize logger: " << e.what() << std::endl;
        return 1;
    }

    std::thread worker(log_worker);

    std::cout << "Logger application started. Enter messages (example <1:message>) \nOr 'quit' for exit, 'setlevel' for change default level" << std::endl;

    while (true) {
        std::string input;
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "quit") {
            stop_thread = true;
            queue_cv.notify_one();
            break;
        }
        else if (input == "setlevel") {
            std::cout << "Enter new log level (0-3): ";
            std::getline(std::cin, input);
            try {
                int new_level = std::stoi(input);
                if (new_level >= 0 && new_level <= 3) {
                    Logger::set_log_level(static_cast<LogLevel>(new_level));
                    std::cout << "Log level changed to " << new_level << std::endl;
                } else {
                    std::cout << "Invalid level. Use 0-3." << std::endl;
                }
            } catch (...) {
                std::cout << "Invalid input." << std::endl;
            }
            continue;
        }
        size_t colon_pos = input.find(':');
        std::string message = input;
        LogLevel level = LogLevel::INFO;

        if (colon_pos != std::string::npos) {
            try {
                int level_num = std::stoi(input.substr(0, colon_pos));
                if (level_num >= 0 && level_num <= 3) {
                    level = static_cast<LogLevel>(level_num);
                    message = input.substr(colon_pos + 1);
                }
            }
            catch (...) {
            }
        }

        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            message_queue.emplace(message, level);
        }
        queue_cv.notify_one();
    }

    worker.join();
    std::cout << "Application stopped." << std::endl;
    return 0;
}