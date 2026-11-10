// Created by Vadim on 18.01.2026.
#pragma once

#include "var_storage.h"
#include <cmath>
#include <algorithm>  // для std::clamp, если понадобится

class BaseModel {
public:
    virtual ~BaseModel() = default;
    virtual void calc(float dt) = 0;  // упрощено: time не используется ни в одной модели

    // Аналог calcBtn из baseModel.cs
    static float calc_btn(float val, float step) {
        if (val > 0.0f) val -= step;
        if (val < 0.0f) val = 0.0f;
        return val;
    }

    // Аналог inRange
    static bool in_range(float val, float min, float max) {
        return (val <= max && val >= min);
    }

protected:
    std::string base_name;
};