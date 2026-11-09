#pragma once
// Created by Vadim 18.01.2026.
#include "udp_handler.h"
#include "var_storage.h"

class ASU_monitor {
public:
    bool init();
    void send_all_vars_binary(const VarStorage& in, const VarStorage& out, const VarStorage& inner);

private:
    UDPHandler udp;
};