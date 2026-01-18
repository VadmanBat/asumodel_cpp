// dbf_reader.cpp
#include "dbf_reader.h"
#include <fstream>
#include <iostream>

bool DbfTable::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    uint8_t version;
    file.read(reinterpret_cast<char*>(&version), 1);
    // пропускаем даты и кол-во записей
    file.seekg(4, std::ios::beg);

    uint16_t header_size, record_size;
    file.read(reinterpret_cast<char*>(&header_size), 2);
    file.read(reinterpret_cast<char*>(&record_size), 2);

    file.seekg(32, std::ios::beg); // начало описаний полей

    while (true) {
        char field_name[11] = {};
        file.read(field_name, 11);
        if (field_name[0] == 0x0D) break; // терминатор заголовка

        char type;
        file.read(&type, 1);
        file.seekg(4, std::ios::cur); // displacement
        uint8_t length;
        file.read(reinterpret_cast<char*>(&length), 1);
        file.seekg(14, std::ios::cur); // остальное

        fields_.push_back({std::string(field_name), type, length});
    }

    file.seekg(header_size, std::ios::beg);

    while (file) {
        char deleted;
        file.read(&deleted, 1);
        if (file.eof() || deleted == 0x1A) break;

        DbfRecord rec;
        for (const auto& f : fields_) {
            std::string val(f.length, '\0');
            file.read(val.data(), f.length);
            // trim
            val.erase(val.find_last_not_of(' ') + 1);
            rec.fields.push_back(val);
        }
        records_.push_back(std::move(rec));
    }
    return true;
}