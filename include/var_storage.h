#pragma once
// Created by Vadim 18.01.2026.
#include <vector>
#include <string>
#include <unordered_map>
#include <cstddef>

struct VarStorage {
    std::vector<std::string> names;
    std::vector<float> values;
    std::unordered_map<std::string, size_t> name_to_index;

    size_t add_or_get(const std::string& name, float initial = 0.0f) {
        if (auto it = name_to_index.find(name); it != name_to_index.end()) {
            return it->second;
        }
        size_t idx = values.size();
        names.emplace_back(name);
        values.emplace_back(initial);
        name_to_index[name] = idx;
        return idx;
    }

    float get(const std::string& name) const {
        if (auto it = name_to_index.find(name); it != name_to_index.end()) {
            return values[it->second];
        }
        return 0.0f;
    }

    void set(const std::string& name, float val) {
        if (auto it = name_to_index.find(name); it != name_to_index.end()) {
            values[it->second] = val;
        }
    }

    float get_by_index(size_t idx) const {
        return (idx < values.size()) ? values[idx] : 0.0f;
    }

    void set_by_index(size_t idx, float val) {
        if (idx < values.size()) {
            values[idx] = val;
        }
    }
};