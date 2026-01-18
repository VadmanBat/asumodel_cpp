#include "models/valve_kruzap.h"
#include <algorithm>
#include <cmath>
#include <iostream>

ValveKRUZAP::ValveKRUZAP(VarStorage& in, VarStorage& out, VarStorage& inner, const std::string& name)
        : in_vars(in), out_vars(out), inner_vars(inner) {
    base_name = name;

    // Входные команды
    cmdOpen_idx = in_vars.add_or_get(name + "cmdOpen");
    cmdClose_idx = in_vars.add_or_get(name + "cmdClose");

    // Входные параметры среды
    pressureIn_idx = in_vars.add_or_get(name + "pressureIn");
    pressureOut_idx = in_vars.add_or_get(name + "pressureOut");
    temperature_idx = in_vars.add_or_get(name + "temperature");
    density_idx = in_vars.add_or_get(name + "density", 1.0f);

    // Параметры клапана
    kv_idx = inner_vars.add_or_get(name + "kv", 100.0f);
    diameter_idx = inner_vars.add_or_get(name + "diameter", 100.0f);

    // Состояние и положение
    position_idx = out_vars.add_or_get(name + "position", 0.0f);  // 0..100%
    state_idx = inner_vars.add_or_get(name + "state", 0.0f);

    // Конечные положения и скорости
    posOpen_idx = inner_vars.add_or_get(name + "posOpen", 100.0f);
    posClose_idx = inner_vars.add_or_get(name + "posClose", 0.0f);
    speedOpen_idx = inner_vars.add_or_get(name + "speedOpen", 10.0f);   // % в секунду
    speedClose_idx = inner_vars.add_or_get(name + "speedClose", 10.0f);

    // Таймеры
    timerOpen_idx = inner_vars.add_or_get(name + "timerOpen", 0.0f);
    timerClose_idx = inner_vars.add_or_get(name + "timerClose", 0.0f);
    alarmTime_idx = inner_vars.add_or_get(name + "alarmTime", alarmTimeout);

    // Аварии и блокировки
    fault_idx = out_vars.add_or_get(name + "fault", 0.0f);
    blockOpen_idx = inner_vars.add_or_get(name + "blockOpen", 0.0f);
    blockClose_idx = inner_vars.add_or_get(name + "blockClose", 0.0f);

    // Выходной расход
    flow_idx = out_vars.add_or_get(name + "flow", 0.0f);
}

void ValveKRUZAP::calc(float dt) {
    float cmdOpen = in_vars.get_by_index(cmdOpen_idx);
    float cmdClose = in_vars.get_by_index(cmdClose_idx);

    float pressureIn = in_vars.get_by_index(pressureIn_idx);
    float pressureOut = in_vars.get_by_index(pressureOut_idx);
    float temperature = in_vars.get_by_index(temperature_idx) + 273.15f;  // в Кельвинах
    float density = in_vars.get_by_index(density_idx);

    float position = out_vars.get_by_index(position_idx);
    int state = static_cast<int>(inner_vars.get_by_index(state_idx));

    float timerOpen = inner_vars.get_by_index(timerOpen_idx);
    float timerClose = inner_vars.get_by_index(timerClose_idx);
    float alarmTimer = inner_vars.get_by_index(alarmTime_idx);

    float blockOpen = inner_vars.get_by_index(blockOpen_idx);
    float blockClose = inner_vars.get_by_index(blockClose_idx);

    // Сброс аварии при отсутствии команд (как в оригинале)
    if (cmdOpen < 0.5f && cmdClose < 0.5f) {
        out_vars.set_by_index(fault_idx, 0.0f);
    }

    // Логика управления состоянием
    if (state == 0) {  // закрыто
        if (cmdOpen > 0.5f && blockOpen < 0.5f) {
            state = 1;  // начало открытия
            timerOpen = 0.0f;
            alarmTimer = alarmTimeout;
        }
    } else if (state == 1) {  // открытие
        if (cmdOpen < 0.5f || blockOpen > 0.5f) {
            state = 0;  // отмена открытия
        } else {
            position += speedOpen_idx * dt * 100.0f;  // скорость в %/сек
            position = std::min(position, inner_vars.get_by_index(posOpen_idx));

            timerOpen += dt;
            alarmTimer -= dt;

            if (position >= inner_vars.get_by_index(posOpen_idx) - 0.1f) {
                state = 2;  // открыто
                position = inner_vars.get_by_index(posOpen_idx);
            }

            if (alarmTimer <= 0.0f) {
                out_vars.set_by_index(fault_idx, 1.0f);  // авария по таймауту
                state = 0;
            }
        }
    } else if (state == 2) {  // открыто
        if (cmdClose > 0.5f && blockClose < 0.5f) {
            state = 3;  // начало закрытия
            timerClose = 0.0f;
            alarmTimer = alarmTimeout;
        }
    } else if (state == 3) {  // закрытие
        if (cmdClose < 0.5f || blockClose > 0.5f) {
            state = 2;  // отмена закрытия
        } else {
            position -= speedClose_idx * dt * 100.0f;
            position = std::max(position, inner_vars.get_by_index(posClose_idx));

            timerClose += dt;
            alarmTimer -= dt;

            if (position <= inner_vars.get_by_index(posClose_idx) + 0.1f) {
                state = 0;  // закрыто
                position = inner_vars.get_by_index(posClose_idx);
            }

            if (alarmTimer <= 0.0f) {
                out_vars.set_by_index(fault_idx, 1.0f);
                state = 2;
            }
        }
    }

    // Расчёт расхода (гидравлическая формула по Kv)
    float dp = pressureIn - pressureOut;
    if (dp > 0.0f && position > 0.0f) {
        float kv_effective = inner_vars.get_by_index(kv_idx) * (position / 100.0f);
        float flow = kv_effective * std::sqrt(dp / density) * 3600.0f;  // м³/ч
        flow = std::min(flow, maxFlow);
        out_vars.set_by_index(flow_idx, flow);
    } else {
        out_vars.set_by_index(flow_idx, 0.0f);
    }

    // Запись обратно
    out_vars.set_by_index(position_idx, position);
    inner_vars.set_by_index(state_idx, static_cast<float>(state));
    inner_vars.set_by_index(timerOpen_idx, timerOpen);
    inner_vars.set_by_index(timerClose_idx, timerClose);
    inner_vars.set_by_index(alarmTime_idx, alarmTimer);
}