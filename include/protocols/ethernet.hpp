#pragma once
#include "base_protocol.hpp"
#include "types/mac_address.hpp"
#include <cstdint>
#include <string>

/**
 * @brief Holds data for an Ethernet II frame.
 */
struct Ethernet : BaseProtocol {
  MacAddress dest_mac;
  MacAddress source_mac;
  uint16_t eth_type;

  inline static const uint16_t ETH_TYPE_IPV4 = 0x0800;
  inline static const uint16_t ETH_TYPE_IPV6 = 0x86DD;

  inline static constexpr std::size_t HEADER_SIZE = 14; // 6+6+2
  inline static constexpr std::size_t ETH_TYPE_OFFSET = 12;

  std::string get_name() const override { return "Ethernet II"; }
};