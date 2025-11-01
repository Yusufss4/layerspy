#pragma once
#include <array>
#include <cstdint> // For uint8_t
#include <string>
#include <string_view>

class MacAddress {
public:
  // Number of bytes in a MAC address
  inline static constexpr std::size_t LENGTH = 6;

  // Default constructor (initializes to 00:00:00:00:00:00)
  MacAddress();

  // Constructor from raw bytes (this is what our parser will use)
  explicit MacAddress(const unsigned char *mac_bytes);

  // Converts the 6-byte array into a human-readable string
  std::string toString() const;

  // (Optional but good) Overload the == operator for comparisons
  bool operator==(const MacAddress &other) const;

  bool operator==(std::string_view other_str) const;

private:
  std::array<uint8_t, 6> m_bytes;

  // We can cache the string representation so we only build it once
  mutable std::string m_string_cache;
  mutable bool m_cache_dirty = true;
};