// Created by Vadim on 18.01.2026.
#pragma once

#include "base_model.h"

class ValveKRUZAP : public BaseModel {
public:
    explicit ValveKRUZAP(VarStorage& in, VarStorage& out, VarStorage& inner, const std::string& name = "ValveKRUZAP");

    void calc(float dt) override;

private:
    VarStorage& in_vars;
    VarStorage& out_vars;
    VarStorage& inner_vars;

    // Все индексы переменных из оригинального ValveKRUZAP.cs
    size_t cmdOpen_idx = 0, cmdClose_idx = 0;
    size_t position_idx = 0, flow_idx = 0;
    size_t pressureIn_idx = 0, pressureOut_idx = 0;
    size_t temperature_idx = 0;

    size_t state_idx = 0;                   // 0 - закрыто, 1 - открытие, 2 - открыто, 3 - закрытие
    size_t timeOpen_idx = 0, timeClose_idx = 0;
    size_t maxPosition_idx = 0, minPosition_idx = 0;

    size_t fault_idx = 0;                   // авария
    size_t blockOpen_idx = 0, blockClose_idx = 0;

    size_t kv_idx = 0;                      // коэффициент пропускной способности
    size_t diameter_idx = 0;
    size_t density_idx = 0;

    size_t timerOpen_idx = 0, timerClose_idx = 0;
    size_t alarmTime_idx = 0;               // время на срабатывание аварии по таймеру

    size_t posOpen_idx = 0, posClose_idx = 0;  // конечные положения
    size_t speedOpen_idx = 0, speedClose_idx = 0;

    // Константы из оригинала (можно вынести в конфиг позже)
    float openTimeNominal = 10.0f;          // номинальное время открытия (сек)
    float closeTimeNominal = 10.0f;
    float alarmTimeout = 15.0f;             // таймаут на аварию
    float maxFlow = 1000.0f;                // пример максимального расхода
};