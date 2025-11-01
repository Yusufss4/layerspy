#include "types/mac_address.hpp"
#include <cstring> // For memcmp
#include <iomanip> // For setw, setfill
#include <sstream> // For stringstream
#include <string_view>

MacAddress::MacAddress() : m_bytes{} { // Zero-initialize
  m_bytes.fill(0);
}

MacAddress::MacAddress(const unsigned char *mac_bytes) {
  std::copy(mac_bytes, mac_bytes + LENGTH, m_bytes.begin());
}

bool MacAddress::operator==(const MacAddress &other) const {
  return m_bytes == other.m_bytes;
}

bool MacAddress::operator==(std::string_view other_str) const {

  // Compare sizes first to avoid unnecessary work.
  std::string s = toString();
  if (s.size() != other_str.size())
    return false;

  // Safe to compare raw bytes because both are sequences of chars (hex digits
  // and ':')

  return std::memcmp(s.data(), other_str.data(), s.size()) == 0;
}

std::string MacAddress::toString() const {

  // If our cache is clean, just return the cached string
  if (!m_cache_dirty) {
    return m_string_cache;
  }

  // Build the string (this is your old mac_to_string logic)
  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  for (int i = 0; i < 6; ++i) {
    ss << std::setw(2) << static_cast<int>(m_bytes[i]);
    if (i < 5)
      ss << ":";
  }

  // Update the cache
  m_string_cache = ss.str();
  m_cache_dirty = false; // The cache is now clean
  return m_string_cache;
}