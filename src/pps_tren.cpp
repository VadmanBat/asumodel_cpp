#include "pps_tren.h"
#include "dbf_reader.h"
#include "models/valm00.h"
#include "models/valve_kruzap.h"   // уже включено
#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

PPS_tren* PPS_tren::instance = nullptr;

bool PPS_tren::init(const std::string& proc_id, const std::string& root, const std::string& tmp) {
    instance = this;

#ifdef _WIN32
    dll_handle = LoadLibraryA("ppsdll.dll");
#else
    dll_handle = dlopen("pps23.so.0.0.1", RTLD_LAZY);
#endif

    if (!dll_handle) {
        std::cerr << "Не удалось загрузить native библиотеку\n";
        return false;
    }

#ifdef _WIN32
    create_pps = reinterpret_cast<CreatePPS_t>(GetProcAddress((HMODULE)dll_handle, "createPPS"));
    free_pps = reinterpret_cast<FreePPS_t>(GetProcAddress((HMODULE)dll_handle, "freePPS"));
    set_func = reinterpret_cast<SetFunc_t>(GetProcAddress((HMODULE)dll_handle, "SetFunc"));
    calc = reinterpret_cast<Calc_t>(GetProcAddress((HMODULE)dll_handle, "calc"));
    get_in_idx = reinterpret_cast<GetInVarIndex_t>(GetProcAddress((HMODULE)dll_handle, "getInVarIndex"));
    get_out_idx = reinterpret_cast<GetOutVarIndex_t>(GetProcAddress((HMODULE)dll_handle, "getOutVarIndex"));
    get_in_val = reinterpret_cast<GetInVarValue_t>(GetProcAddress((HMODULE)dll_handle, "getInVarValue"));
    set_out_val = reinterpret_cast<SetOutVarValue_t>(GetProcAddress((HMODULE)dll_handle, "setOutVarValue"));
#else
    create_pps = reinterpret_cast<CreatePPS_t>(dlsym(dll_handle, "createPPS"));
    free_pps = reinterpret_cast<FreePPS_t>(dlsym(dll_handle, "freePPS"));
    set_func = reinterpret_cast<SetFunc_t>(dlsym(dll_handle, "SetFunc"));
    calc = reinterpret_cast<Calc_t>(dlsym(dll_handle, "calc"));
    get_in_idx = reinterpret_cast<GetInVarIndex_t>(dlsym(dll_handle, "getInVarIndex"));
    get_out_idx = reinterpret_cast<GetOutVarIndex_t>(dlsym(dll_handle, "getOutVarIndex"));
    get_in_val = reinterpret_cast<GetInVarValue_t>(dlsym(dll_handle, "getInVarValue"));
    set_out_val = reinterpret_cast<SetOutVarValue_t>(dlsym(dll_handle, "setOutVarValue"));
#endif

    if (!create_pps || !set_func || !calc) {
        std::cerr << "Не найдены обязательные функции в DLL\n";
        return false;
    }

    pps_instance = create_pps(proc_id.c_str(), root.c_str(), tmp.c_str());

    set_func(pps_instance,
             (void*)on_accept_callback,
             (void*)on_calc_callback,
             (void*)on_new_data_callback,
             (void*)on_load_callback,
             (void*)on_save_callback,
             (void*)on_run_freeze_callback,
             (void*)on_target_callback);

    return true;
}

void PPS_tren::calc_step() {
    if (pps_instance && calc) {
        calc(pps_instance);

        for (size_t i = 0; i < in_vars.values.size(); ++i) {
            in_vars.values[i] = get_in_val(pps_instance, static_cast<int>(i));
        }

        for (auto& m : models) {
            m.second->calc(0.1f);
        }

        for (size_t i = 0; i < out_vars.values.size(); ++i) {
            set_out_val(pps_instance, static_cast<int>(i), out_vars.values[i]);
        }
    }
}

void PPS_tren::load_config(const std::string& path) {
    DbfTable table;
    if (table.load(path + "/vars.dbf")) {
        for (const auto& rec : table.records()) {
            std::string name = rec.fields[0];
            in_vars.add_or_get(name);
            out_vars.add_or_get(name);
            inner_vars.add_or_get(name);
        }
    }

    models["valm00"] = std::make_unique<Valm00>(in_vars, out_vars, inner_vars);
    models["kruzap"] = std::make_unique<ValveKRUZAP>(in_vars, out_vars, inner_vars);  // исправлено имя класса
}

// callbacks без изменений
void PPS_tren::on_accept_callback(int value) { std::cout << "[PPS] Accept: " << value << "\n"; }
void PPS_tren::on_calc_callback(double step, double mod_time) { std::cout << "[PPS] Calc step: " << step << " time: " << mod_time << "\n"; }
void PPS_tren::on_new_data_callback(double mod_time) { std::cout << "[PPS] New data at: " << mod_time << "\n"; }
int PPS_tren::on_load_callback(int reason, const char* str) {
    std::cout << "[PPS] Load reason " << reason << ": " << (str ? str : "") << "\n";
    if (instance && str) instance->load_config(str);
    return 0;
}
int PPS_tren::on_save_callback(int reason, const char* str) { std::cout << "[PPS] Save reason " << reason << ": " << (str ? str : "") << "\n"; return 0; }
int PPS_tren::on_target_callback(int reason, const char* str) { std::cout << "[PPS] Target reason " << reason << ": " << (str ? str : "") << "\n"; return 0; }
int PPS_tren::on_run_freeze_callback(int run) { std::cout << "[PPS] Run/Freeze: " << run << "\n"; return 0; }