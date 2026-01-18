#include "models/valm00.h"
#include <cmath>
#include <algorithm>

Valm00::Valm00(VarStorage& in, VarStorage& out, VarStorage& inner, const std::string& name)
        : in_vars(in), out_vars(out), inner_vars(inner) {
    base_name = name;

    // Регистрация всех переменных (аналог init() в C#)
    ons1_idx = in_vars.add_or_get(name + "ons1");
    ofs1_idx = in_vars.add_or_get(name + "ofs1");

    brOn_idx = inner_vars.add_or_get(name + "brOn", 0.0f);
    brOf_idx = inner_vars.add_or_get(name + "brOf", 0.0f);

    uon_idx = inner_vars.add_or_get(name + "uon", 0.0f);
    uof_idx = inner_vars.add_or_get(name + "uof", 0.0f);

    ipv_idx = in_vars.add_or_get(name + "ipv");
    ip_idx = in_vars.add_or_get(name + "ip");

    QF1_idx = inner_vars.add_or_get(name + "QF1", 1.0f);
    KY_idx = inner_vars.add_or_get(name + "KY", 0.0f);
    KA_idx = inner_vars.add_or_get(name + "KA", 0.0f);
    bKY_idx = inner_vars.add_or_get(name + "bKY", 0.0f);
    brKO_idx = inner_vars.add_or_get(name + "brKO", 0.0f);
    brKC_idx = inner_vars.add_or_get(name + "brKC", 0.0f);

    MU_idx = inner_vars.add_or_get(name + "MU", 0.0f);
    brA_idx = inner_vars.add_or_get(name + "brA", 0.0f);
    brB_idx = inner_vars.add_or_get(name + "brB", 0.0f);
    brC_idx = inner_vars.add_or_get(name + "brC", 0.0f);
    jam_idx = inner_vars.add_or_get(name + "jam", 0.0f);

    eof_idx = inner_vars.add_or_get(name + "eof", 0.0f);
    eon_idx = inner_vars.add_or_get(name + "eon", 0.0f);
    beof_idx = inner_vars.add_or_get(name + "beof", 0.0f);
    beon_idx = inner_vars.add_or_get(name + "beon", 0.0f);

    uY1_idx = inner_vars.add_or_get(name + "uY1", 24.0f);

    UAp_idx = inner_vars.add_or_get(name + "UAp", UA);
    UBp_idx = inner_vars.add_or_get(name + "UBp", UB);
    UCp_idx = inner_vars.add_or_get(name + "UCp", UC);
    UAy_idx = inner_vars.add_or_get(name + "UAy", UA);
    UBy_idx = inner_vars.add_or_get(name + "UBy", UB);
    UCy_idx = inner_vars.add_or_get(name + "UCy", UC);

    klM_idx = inner_vars.add_or_get(name + "klM", 1.0f);

    UAm_idx = inner_vars.add_or_get(name + "UAm", UA);
    UBm_idx = inner_vars.add_or_get(name + "UBm", UB);
    UCm_idx = inner_vars.add_or_get(name + "UCm", UC);
    Nm_idx = inner_vars.add_or_get(name + "Nm", 0.0f);
    Zpbr_idx = inner_vars.add_or_get(name + "Zpbr", 0.0f);

    Rm1_idx = inner_vars.add_or_get(name + "Rm1", 4.0f);
    Rm2_idx = inner_vars.add_or_get(name + "Rm2", 4.0f);
    Rm3_idx = inner_vars.add_or_get(name + "Rm3", 4.0f);
    man_idx = inner_vars.add_or_get(name + "man", 0.0f);
    Im_idx = inner_vars.add_or_get(name + "Im", 0.0f);

    // Выходные переменные (если нужно обновлять из DLL — здесь просто регистрируем)
    out_vars.add_or_get(name + "ipv", -1.0f);
    out_vars.add_or_get(name + "ip", -1.0f);
    out_vars.add_or_get(name + "csig", -1.0f);
}

void Valm00::calc(float dt) {
    // base.Calc(dt, time);  // если в базовом классе есть логика — раскомментируйте

    float _man = inner_vars.get_by_index(man_idx);
    _man = (std::signbit(_man) ? -1.0f : 1.0f) * BaseModel::calc_btn(std::abs(_man), dt);
    inner_vars.set_by_index(man_idx, _man);

    float _ipv = in_vars.get_by_index(ipv_idx);
    float _ip = in_vars.get_by_index(ip_idx);

    float UfA = UA * inner_vars.get_by_index(QF1_idx);
    float UfB = UB * inner_vars.get_by_index(QF1_idx);
    float UfC = UC * inner_vars.get_by_index(QF1_idx);

    float _ons = in_vars.get_by_index(ons1_idx);
    float _ofs = in_vars.get_by_index(ofs1_idx);

    // Debug-строка из оригинала (опционально, для отладки)
    // if (base_name == "SIN1A") {
    //     std::cout << "[DEBUG] ons = " << in_vars.get_by_index(ons1_idx) << " indx = " << ons1_idx << "\n";
    // }

    if (inner_vars.get_by_index(brOn_idx) > 0.0f) _ons = 1.0f;
    if (inner_vars.get_by_index(brOn_idx) < 0.0f) _ons = 0.0f;
    if (inner_vars.get_by_index(brOf_idx) > 0.0f) _ofs = 1.0f;
    if (inner_vars.get_by_index(brOf_idx) < 0.0f) _ofs = 0.0f;

    int _KY = 0;
    int _KA = 0;
    int _bKY = static_cast<int>(inner_vars.get_by_index(bKY_idx));
    float _Im = 0.0f;
    int _eon = 0;
    int _eof = 0;

    if (_ipv >= inon) _eon = 1;
    if (_ipv <= inof) _eof = 1;
    if (inner_vars.get_by_index(beon_idx) > 0.0f) _eon = 1;
    if (inner_vars.get_by_index(beof_idx) > 0.0f) _eof = 1;
    if (inner_vars.get_by_index(beon_idx) < 0.0f) _eon = 0;
    if (inner_vars.get_by_index(beof_idx) < 0.0f) _eof = 0;

    if (inner_vars.get_by_index(brA_idx) > 0.0f) UfA = 0.0f;
    if (inner_vars.get_by_index(brB_idx) > 0.0f) UfB = 0.0f;
    if (inner_vars.get_by_index(brC_idx) > 0.0f) UfC = 0.0f;

    inner_vars.set_by_index(UAp_idx, UfA);
    inner_vars.set_by_index(UBp_idx, UfB);
    inner_vars.set_by_index(UCp_idx, UfC);

    bool fAll = (UfA > 0.0f) && (UfB > 0.0f) && (UfC > 0.0f);

    if (inner_vars.get_by_index(MU_idx) > 0.0f) {
        _KY = _bKY;
    } else {
        if (_ons > 0.0f) _KY = 1;
        if (_ofs > 0.0f) _KY = -1;
    }

    if (inner_vars.get_by_index(brKO_idx) > 0.0f) _KY = 1;
    if (inner_vars.get_by_index(brKC_idx) > 0.0f) _KY = -1;
    if (inner_vars.get_by_index(brKO_idx) < 0.0f && _KY > 0) _KY = 0;
    if (inner_vars.get_by_index(brKC_idx) < 0.0f && _KY < 0) _KY = 0;

    if (_KY > 0 && _eon == 1) _KY = 0;
    if (_KY < 0 && _eof == 1) _KY = 0;

    if (!fAll || inner_vars.get_by_index(uY1_idx) < ubpMin) {
        _KY = 0;
        _ons = 0.0f;
        _ofs = 0.0f;
    }

    if (_KY > 0) {
        inner_vars.set_by_index(uon_idx, inner_vars.get_by_index(uY1_idx));
    } else {
        inner_vars.set_by_index(uon_idx, 0.0f);
    }

    if (_KY < 0) {
        inner_vars.set_by_index(uof_idx, inner_vars.get_by_index(uY1_idx));
    } else {
        inner_vars.set_by_index(uof_idx, 0.0f);
    }

    bool _needMove = false;

    if (_KY != 0) {
        inner_vars.set_by_index(UAy_idx, UA);
        inner_vars.set_by_index(UBy_idx, UB);
        inner_vars.set_by_index(UCy_idx, UC);
    } else {
        inner_vars.set_by_index(UAy_idx, obrivU);  // obrivU — добавьте константу в класс (из оригинала, вероятно 0 или специальное значение)
        inner_vars.set_by_index(UBy_idx, obrivU);
        inner_vars.set_by_index(UCy_idx, obrivU);
    }

    if (inner_vars.get_by_index(jam_idx) == 0.0f) {
        inner_vars.set_by_index(UAm_idx, inner_vars.get_by_index(UAy_idx) * inner_vars.get_by_index(klM_idx));
        inner_vars.set_by_index(UBm_idx, inner_vars.get_by_index(UBy_idx) * inner_vars.get_by_index(klM_idx));
        inner_vars.set_by_index(UCm_idx, inner_vars.get_by_index(UCy_idx) * inner_vars.get_by_index(klM_idx));

        if (inner_vars.get_by_index(UAm_idx) > fazeMin && inner_vars.get_by_index(UBm_idx) > fazeMin && inner_vars.get_by_index(UCm_idx) > fazeMin) {
            if (BaseModel::in_range(inner_vars.get_by_index(Rm1_idx), RobmMin, RobmMax) &&
                BaseModel::in_range(inner_vars.get_by_index(Rm2_idx), RobmMin, RobmMax) &&
                BaseModel::in_range(inner_vars.get_by_index(Rm3_idx), RobmMin, RobmMax)) {
                _ipv += 100.0f * _KY * dt / inti;
                _Im = 1.5f;
                _needMove = true;
            }

            if (inner_vars.get_by_index(Rm1_idx) < RobmMin || inner_vars.get_by_index(Rm2_idx) < RobmMin || inner_vars.get_by_index(Rm3_idx) < RobmMin) {
                _needMove = true;
                _Im = 100.0f;
                _KA = 1;
                inner_vars.set_by_index(QF1_idx, 0.0f);
            }
        }

        if (_KY == 0 && _man != 0.0f) {
            _ipv += 100.0f * (std::signbit(_man) ? -1.0f : 1.0f) * dt / inti;
        }

        if (_ipv <= inof) {
            _ipv = inof;
            if (_needMove) _KA = 1;
        } else if (_ipv >= inon) {
            _ipv = inon;
            if (_needMove) _KA = 1;
        }
    } else {  // клин
        if (_KY != 0) _KA = 1;
    }

    inner_vars.set_by_index(eon_idx, static_cast<float>(_eon));
    inner_vars.set_by_index(eof_idx, static_cast<float>(_eof));
    inner_vars.set_by_index(Im_idx, _Im);
    inner_vars.set_by_index(KY_idx, static_cast<float>(_KY));
    inner_vars.set_by_index(KA_idx, static_cast<float>(_KA));

    _ip = _ipv;
    csig = 0;

    // Битовые флаги csig
    if (UfA > 0.0f) csig |= (1 << 0);      // 0 - питание
    csig |= (1 << 1);                      // 1 - состояние упр. кабеля (всегда, как в оригинале)
    if (_eof > 0) csig |= (1 << 2);
    if (_eon > 0) csig |= (1 << 3);
    if (_KA > 0) csig |= (1 << 4);

    // Выходные переменные (аналог updateOutVar)
    out_vars.set(base_name + "ipv", _ipv);
    out_vars.set(base_name + "ip", _ip);
    out_vars.set(base_name + "csig", static_cast<float>(csig));
}