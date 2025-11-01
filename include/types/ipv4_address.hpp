#pragma once
#include <cstdint> // For uint32_t
#include <string>

/**
 * @brief Represents an IPv4 address (32-bit).
 * Stores the address in host byte order internally for efficient operations.
 */
class Ipv4Address {
public:
  // Size constant for IPv4 addresses
  inline static constexpr std::size_t LENGTH = 4;

  Ipv4Address();

  // Constructor from a 32-bit integer in *network byte order*
  explicit Ipv4Address(uint32_t ip_bytes_network_order);

  // Constructor from raw bytes (what our parser will use)
  explicit Ipv4Address(const unsigned char *ip_bytes);

  std::string toString() const;

  bool operator==(const Ipv4Address &other) const;

private:
  // Store the IP in *host byte order* for easier math
  uint32_t m_ip_host_order;

  mutable std::string m_string_cache;
  mutable bool m_cache_dirty = true;
};