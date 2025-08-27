//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_MACROS_H
#define MINECRAFT_MACROS_H

#include <iostream>
#include <vector>
#include <sstream>

#define MINECRAFT_DEBUG 1
#define WORLD_DEBUG_FLAG 0

#define MINECRAFT_DEBUG_CONSOLE_HEIGHT 10
extern std::vector<std::string> debug_output;

inline void minecraft_debug_print(const std::string &message) {
    if (!debug_output.empty() && debug_output.size() > MINECRAFT_DEBUG_CONSOLE_HEIGHT)
        debug_output.erase(debug_output.begin());

    debug_output.push_back(message);
    std::cout << message << "\n";
}

#ifdef _MSC_VER
// Microsoft Visual Studio
#define DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
// GCC or Clang
#if defined(__i386__) || defined(__x86_64__)
#define DEBUG_BREAK() __asm__ volatile("int3")
#elif defined(__arm__) || defined(__aarch64__)
#define DEBUG_BREAK() __asm__ volatile("brk #0")
#else
#include <csignal>
#define DEBUG_BREAK() std::raise(SIGABRT)
#endif
#else
// Fallback for other compilers
#define DEBUG_BREAK() ((void)0)
#endif


#define ASSERT(condition, message) \
do { \
if (!(condition)) { \
std::cerr << "Assertion failed: " << #condition << std::endl; \
std::cerr << "Message: " << message << std::endl; \
std::cerr << "File: " << __FILE__ << ", Line: " << __LINE__ << std::endl; \
DEBUG_BREAK(); \
std::abort(); \
} \
} while(0)


#if MINECRAFT_DEBUG
#define WHEN_DEBUG(code) code
#define ASSERT_DEBUG(condition, message) ASSERT(condition, message)
#define PRINT_DEBUG_IF(condition, msg) if (condition) { PRINT_DEBUG(msg); }
#define PRINT_DEBUG(msg) do { \
std::ostringstream oss__; \
oss__ << msg; \
minecraft_debug_print(oss__.str()); \
} while(0)
#else
#define ASSERT_DEBUG(condition, message) ((void)0)
#define PRINT_DEBUG(message) ((void)0)
#define PRINT_DEBUG_IF(condition, msg) ((void)0)
#define WHEN_DEBUG(code) ((void)0)
#endif


#endif //MINECRAFT_MACROS_H