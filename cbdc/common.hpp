#pragma once

#include <cstdint>
#include <unordered_map>

using wallet_id_t = uint64_t;
using coin_id_t = uint64_t;
using coin_value_t = uint32_t;
using wallet_t = struct wallet_t {
    std::unordered_map<coin_id_t,coin_value_t> coins;
    uint8_t reserved[32]; // TODO check signature
};

