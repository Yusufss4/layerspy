#pragma once
#include <array>
#include <cstdint>
#include <string>

/**
 * @brief Represents an IPv6 address (128-bit).
 * Stores the address as 16 bytes for straightforward parsing and formatting.
 */
class Ipv6Address {
public:
  // Size constant for IPv6 addresses
  inline static constexpr std::size_t LENGTH = 16;

  Ipv6Address();

  // Constructor from raw bytes (what our parser will use)
  explicit Ipv6Address(const unsigned char *ip_bytes);

  // Convert to human-readable IPv6 string (compressed format like "2001:db8::1")
  std::string toString() const;

  bool operator==(const Ipv6Address &other) const;

private:
  // Store the 128-bit IPv6 address as 16 bytes
  std::array<uint8_t, 16> m_bytes;

  mutable std::string m_string_cache;
  mutable bool m_cache_dirty = true;
};
