#include "asu_monitor.h"
#include <vector>
#include <cstring>
#include <iostream>

bool ASU_monitor::init() {
    return udp.init(5000);
}

void ASU_monitor::send_all_vars_binary(const VarStorage& in, const VarStorage& out, const VarStorage& inner) {
    // Выносим Header наружу и инициализируем поля отдельно
    struct Header {
        uint32_t magic = 0xDEADBEEF;
        uint32_t in_count = 0;
        uint32_t out_count = 0;
        uint32_t inner_count = 0;
    };

    Header header;
    header.in_count = static_cast<uint32_t>(in.values.size());
    header.out_count = static_cast<uint32_t>(out.values.size());
    header.inner_count = static_cast<uint32_t>(inner.values.size());

    std::vector<char> buffer(sizeof(header) +
                             (in.values.size() + out.values.size() + inner.values.size()) * sizeof(float));

    std::memcpy(buffer.data(), &header, sizeof(header));

    float* ptr = reinterpret_cast<float*>(buffer.data() + sizeof(header));
    std::memcpy(ptr, in.values.data(), in.values.size() * sizeof(float));
    ptr += in.values.size();
    std::memcpy(ptr, out.values.data(), out.values.size() * sizeof(float));
    ptr += out.values.size();
    std::memcpy(ptr, inner.values.data(), inner.values.size() * sizeof(float));

    udp.send_binary(buffer.data(), buffer.size());
}