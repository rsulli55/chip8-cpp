#pragma once

#include "common.h"

struct Rom {
    const std::vector<u8> data_;

    Rom(std::size_t size) : data_{std::vector<u8>(size)} {};
    Rom(std::vector<u8> &data) : data_ {data} {}
    Rom(std::vector<u8> &&data) : data_ {data} {}
    // maybe more things later?
};
