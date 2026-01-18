#include "pps_tren.h"
#include "asu_monitor.h"
#include <chrono>
#include <thread>
#include <iostream>

int main() {
    PPS_tren pps;
    if (!pps.init()) {
        std::cerr << "Ошибка инициализации PPS\n";
        return 1;
    }

    ASU_monitor asu;
    if (!asu.init()) {
        std::cerr << "Ошибка инициализации UDP\n";
        return 1;
    }

    std::cout << "Тренажёр PPS запущен (консольная версия C++). Шаг 100 мс.\n";

    auto last = std::chrono::steady_clock::now();
    while (true) {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - last).count();
        if (dt >= 0.1f) {
            pps.calc_step();
            asu.send_all_vars_binary(pps.in_vars, pps.out_vars, pps.inner_vars);

            // пример вывода в консоль
            std::cout << "Time: " << dt << "  P1: " << pps.in_vars.get("P1")
                      << "  Volume: " << pps.out_vars.get("Volume") << "\n";

            last = now;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}