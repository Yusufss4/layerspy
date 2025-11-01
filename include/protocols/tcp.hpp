#pragma once
#include "base_protocol.hpp"
#include <cstdint>
#include <string>

/**
 * @brief Holds decoded data for a TCP segment header.
 */
struct TCP : BaseProtocol {
  // Ports
  uint16_t src_port;
  uint16_t dst_port;

  // Sequence / ack numbers
  uint32_t seq_number;
  uint32_t ack_number;

  // Header length & flags
  uint8_t data_offset; // in 32-bit words (i.e. *4 = header bytes)
  bool flag_ns;
  bool flag_cwr;
  bool flag_ece;
  bool flag_urg;
  bool flag_ack;
  bool flag_psh;
  bool flag_rst;
  bool flag_syn;
  bool flag_fin;

  // Windowing / reliability
  uint16_t window_size;
  uint16_t checksum;
  uint16_t urgent_pointer; // valid if URG flag set

  // Convenience helpers for higher layer logic
  bool is_syn_only() const {
    return flag_syn && !flag_ack && !flag_fin && !flag_rst;
  }

  bool is_http_candidate() const {
    // Common heuristic: HTTP usually rides on 80/8080 (HTTP) or 443 (HTTPS/TLS)
    // We treat HTTPS as "not HTTP" at app layer because it's encrypted.
    return (dst_port == 80 || src_port == 80 || dst_port == 8080 ||
            src_port == 8080);
  }

  std::string get_name() const override { return "TCP"; }
};
