//
// Created by Luke on 06/10/2025.
//

#ifndef MINECRAFT_PERFORMANCETIMER_H
#define MINECRAFT_PERFORMANCETIMER_H
#include <chrono>

#include "Assert.h"

#define PERFORMANCE_TIMER 1

#ifdef PERFORMANCE_TIMER
struct PerformanceTimer {
    explicit PerformanceTimer(const std::string &label) {
        this->label = label;
        this->clock = std::chrono::high_resolution_clock::now();
    }

    ~PerformanceTimer() {
        const auto end = std::chrono::system_clock::now();
        const std::chrono::duration<double, std::milli> elapsed_time = end - clock;

        PRINT_DEBUG_IF(PERFORMANCE_TIMER, "timer " << label << " elapsed " << elapsed_time);
        std::cout << std::flush;
    }

    PerformanceTimer(const PerformanceTimer &) = delete;

    PerformanceTimer &operator=(const PerformanceTimer &) = delete;

    PerformanceTimer(PerformanceTimer &&) = delete;

    PerformanceTimer &operator=(PerformanceTimer &&) = delete;

    void *operator new(std::size_t) = delete;

    void *operator new[](std::size_t) = delete;

private:
    std::string label;
    std::chrono::system_clock::time_point clock;
};
#else
#define PerformanceTimer(...)
#endif


#endif //MINECRAFT_PERFORMANCETIMER_H
