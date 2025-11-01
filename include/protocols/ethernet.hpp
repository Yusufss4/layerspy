#pragma once
#include "base_protocol.hpp"
#include <cstdint> // For uint16_t, etc.
#include <string>

/**
 * @brief Holds data for an Ethernet II frame.
 */
struct Ethernet : BaseProtocol {
  std::string dest_mac;
  std::string source_mac;
  uint16_t eth_type; // e.g., 0x0800 for IPv4

  // EtherType constants for our parser chain
  inline static const uint16_t ETH_TYPE_IPV4 = 0x0800;
  inline static const uint16_t ETH_TYPE_IPV6 = 0x86DD;

  std::string get_name() const override { return "Ethernet II"; }
};