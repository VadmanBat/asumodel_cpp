// dbf_reader.h
#pragma once
#include <vector>
#include <string>
#include <cstdint>

struct DbfField {
    std::string name;
    char type;
    uint8_t length;
};

struct DbfRecord {
    std::vector<std::string> fields;
};

class DbfTable {
public:
    bool load(const std::string& filename);
    const std::vector<DbfField>& fields() const { return fields_; }
    const std::vector<DbfRecord>& records() const { return records_; }

private:
    std::vector<DbfField> fields_;
    std::vector<DbfRecord> records_;
};