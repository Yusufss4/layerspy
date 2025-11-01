#pragma once
#include "base_protocol.hpp"
#include "types/ipv4_address.hpp"
#include <cstdint>
#include <string>

/**
 * @brief Holds decoded data for an IPv4 packet header.
 */
struct IPv4 : BaseProtocol {
  // Basic header info
  uint8_t version; // should be 4
  uint8_t ihl;     // Internet Header Length in 32-bit words
  uint8_t dscp;    // Differentiated Services Code Point (6 bits)
  uint8_t ecn;     // Explicit Congestion Notification (2 bits)

  uint16_t total_length; // Total length of IP packet (header + payload), bytes
  uint16_t identification;

  // Flags + Fragment offset
  bool dont_fragment;
  bool more_fragments;
  uint16_t fragment_offset; // in 8-byte units

  uint8_t ttl;      // Time To Live
  uint8_t protocol; // 6 = TCP, 17 = UDP, 1 = ICMP, etc.
  uint16_t header_checksum;

  Ipv4Address source_ip; // Source IPv4 address
  Ipv4Address dest_ip;   // Destination IPv4 address

  // For chaining
  inline static const uint8_t PROTO_TCP = 6;
  inline static const uint8_t PROTO_UDP = 17;

  std::string get_name() const override { return "IPv4"; }
};
