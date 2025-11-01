#include "types/ipv6_address.hpp"
#include <arpa/inet.h> // For inet_ntop
#include <cstring>     // For memcpy

Ipv6Address::Ipv6Address() : m_bytes{} { m_bytes.fill(0); }

Ipv6Address::Ipv6Address(const unsigned char *ip_bytes) {
  std::memcpy(m_bytes.data(), ip_bytes, 16);
}

bool Ipv6Address::operator==(const Ipv6Address &other) const {
  return m_bytes == other.m_bytes;
}

std::string Ipv6Address::toString() const {
  if (!m_cache_dirty) {
    return m_string_cache;
  }

  // Use inet_ntop to convert 16 bytes to IPv6 string (with compression)
  char buf[INET6_ADDRSTRLEN]; // 46 bytes for IPv6
  if (inet_ntop(AF_INET6, m_bytes.data(), buf, sizeof(buf)) == nullptr) {
    // On error return empty string (shouldn't happen for valid IPs)
    return std::string();
  }

  m_string_cache = buf;
  m_cache_dirty = false;
  return m_string_cache;
}
