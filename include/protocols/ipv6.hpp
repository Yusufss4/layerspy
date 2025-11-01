#pragma once
#include "base_protocol.hpp"
#include "types/ipv6_address.hpp"
#include <cstdint>
#include <string>

/**
 * @brief Holds decoded data for an IPv6 packet header.
 *
 * Reference: RFC 8200 (high level).
 *
 * Note:
 * - IPv6 has no header checksum (unlike IPv4).
 * - No fragmentation fields in the base header (fragmentation is done via an
 * extension header).
 */
struct IPv6 : BaseProtocol {
  // ---- Version / Traffic / Flow ----
  uint8_t version;       // should be 6
  uint8_t traffic_class; // like DSCP/ECN combined; QoS-ish
  uint32_t flow_label;   // 20-bit label, used for flow identification / QoS

  // ---- Payload / Next Header / Hop Limit ----
  uint16_t payload_length; // length of payload after this header (bytes)
  uint8_t next_header; // similar to IPv4 'protocol'. e.g. 6 = TCP, 17 = UDP, 58
                       // = ICMPv6
  uint8_t hop_limit;   // like IPv4 TTL, decremented by routers

  // ---- Addresses ----
  Ipv6Address source_ip; // Source IPv6 address
  Ipv6Address dest_ip;   // Destination IPv6 address

  // Common next_header values for chaining upper layers
  inline static const uint8_t NH_TCP = 6;        // TCP
  inline static const uint8_t NH_UDP = 17;       // UDP
  inline static const uint8_t NH_ICMPv6 = 58;    // ICMPv6
  inline static const uint8_t NH_FRAG = 44;      // Fragment header
  inline static const uint8_t NH_ROUTING = 43;   // Routing header
  inline static const uint8_t NH_HOP_BY_HOP = 0; // Hop-by-Hop Options

  /**
   * @brief Convenience: is this carrying TCP directly?
   *
   * NOTE: In real IPv6, there may be extension headers (hop-by-hop, routing,
   * fragment, etc.) between the base header and TCP. This just checks the
   * immediate next_header value. A more advanced parser would walk extension
   * headers to find the "final" upper-layer header.
   */
  bool is_tcp_immediate() const { return next_header == NH_TCP; }

  std::string get_name() const override { return "IPv6"; }
};
