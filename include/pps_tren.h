#pragma once
// Created by Vadim 18.01.2026.
#include "var_storage.h"
#include "models/base_model.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

class PPS_tren {
public:
    static PPS_tren* instance;

    bool init(const std::string& proc_id = "pm3dvs",
              const std::string& root = "/home/kafsu/sim",
              const std::string& tmp = "/home/kafsu/tmp");

    void calc_step();
    void load_config(const std::string& path); // использует DbfReader

    VarStorage in_vars;
    VarStorage out_vars;
    VarStorage inner_vars;

    std::unordered_map<std::string, std::unique_ptr<BaseModel>> models;

private:
    void* dll_handle = nullptr;
    void* pps_instance = nullptr;

    // типы функций из DLL
    using CreatePPS_t = void* (*)(const char*, const char*, const char*);
    using FreePPS_t = void (*)(void*);
    using SetFunc_t = bool (*)(void*, void*, void*, void*, void*, void*, void*, void*);
    using Calc_t = void (*)(void*);
    using GetInVarIndex_t = int (*)(void*, const char*);
    using GetOutVarIndex_t = int (*)(void*, const char*);
    using GetInVarValue_t = float (*)(void*, int);
    using SetOutVarValue_t = void (*)(void*, int, float);

    CreatePPS_t create_pps = nullptr;
    FreePPS_t free_pps = nullptr;
    SetFunc_t set_func = nullptr;
    Calc_t calc = nullptr;
    GetInVarIndex_t get_in_idx = nullptr;
    GetOutVarIndex_t get_out_idx = nullptr;
    GetInVarValue_t get_in_val = nullptr;
    SetOutVarValue_t set_out_val = nullptr;

    // callbacks
    static void on_accept_callback(int value);
    static void on_calc_callback(double step, double mod_time);
    static void on_new_data_callback(double mod_time);
    static int on_load_callback(int reason, const char* str);
    static int on_save_callback(int reason, const char* str);
    static int on_target_callback(int reason, const char* str);
    static int on_run_freeze_callback(int run);
};