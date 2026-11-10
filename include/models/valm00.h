// Created by Vadim on 18.01.2026.
#pragma once

#include "base_model.h"

class Valm00 : public BaseModel {
public:
    explicit Valm00(VarStorage& in, VarStorage& out, VarStorage& inner, const std::string& name = "Valm00");

    void calc(float dt) override;

private:
    VarStorage& in_vars;
    VarStorage& out_vars;
    VarStorage& inner_vars;

    // Индексы (как раньше)
    size_t ons1_idx = 0, ofs1_idx = 0;
    size_t ipv_idx = 0, ip_idx = 0;

    size_t brOn_idx = 0, brOf_idx = 0;
    size_t uon_idx = 0, uof_idx = 0;

    size_t QF1_idx = 0, KY_idx = 0, bKY_idx = 0, brKO_idx = 0, brKC_idx = 0;
    size_t MU_idx = 0, KA_idx = 0;
    size_t brA_idx = 0, brB_idx = 0, brC_idx = 0;
    size_t jam_idx = 0;

    size_t eof_idx = 0, eon_idx = 0, beof_idx = 0, beon_idx = 0;
    size_t uY1_idx = 0;

    size_t UAp_idx = 0, UBp_idx = 0, UCp_idx = 0;
    size_t UAy_idx = 0, UBy_idx = 0, UCy_idx = 0;

    size_t klM_idx = 0;
    size_t UAm_idx = 0, UBm_idx = 0, UCm_idx = 0;
    size_t Nm_idx = 0, Zpbr_idx = 0;
    size_t Rm1_idx = 0, Rm2_idx = 0, Rm3_idx = 0;
    size_t man_idx = 0;
    size_t Im_idx = 0;

    // Константы
    float fazeMin = 180.0f;
    float ubpMin = 16.0f;
    float RobmMin = 2.0f;
    float RobmMax = 100.0f;
    float Imax = 5.0f;

    float UA = 220.0f;
    float UB = 220.0f;
    float UC = 220.0f;

    float inti = 25.0f;
    int csig = 0;

    float inof = 0.0f;
    float inon = 100.0f;

    float obrivU = 0.0f;  // добавлено: значение при обрыве (из оригинала — обычно 0)
};