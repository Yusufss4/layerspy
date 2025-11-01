#include "types/ipv4_address.hpp"
#include <arpa/inet.h> // For ntohl() and inet_ntoa()
#include <cstring>     // For memcpy

Ipv4Address::Ipv4Address() : m_ip_host_order(0) {}

Ipv4Address::Ipv4Address(uint32_t ip_bytes_network_order) {
  // Convert from network order (big-endian) to host order
  m_ip_host_order = ntohl(ip_bytes_network_order);
}

Ipv4Address::Ipv4Address(const unsigned char *ip_bytes) {
  // Copy the 4 bytes into a uint32_t
  uint32_t ip_network_order;
  std::memcpy(&ip_network_order, ip_bytes, 4);
  m_ip_host_order = ntohl(ip_network_order);
}

bool Ipv4Address::operator==(const Ipv4Address &other) const {
  return m_ip_host_order == other.m_ip_host_order;
}

std::string Ipv4Address::toString() const {
  if (!m_cache_dirty) {
    return m_string_cache;
  }

  // `in_addr` is the struct `inet_ntoa` expects
  struct in_addr addr;

  // Convert our host-order IP back to network-order for the C function
  addr.s_addr = htonl(m_ip_host_order);

  // Convert to presentation format using inet_ntop (thread-safe)
  char buf[INET_ADDRSTRLEN]; // 16 bytes for IPv4
  if (inet_ntop(AF_INET, &addr, buf, sizeof(buf)) == nullptr) {
    // On error return an empty string (shouldn't happen for valid IPs)
    return std::string();
  }
  m_string_cache = buf;
  m_cache_dirty = false;
  return m_string_cache;
}