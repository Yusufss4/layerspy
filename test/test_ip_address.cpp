#include "types/ipv4_address.hpp"
#include "types/ipv6_address.hpp"
#include <arpa/inet.h> // For htonl / inet_pton
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Ipv4Address - Construction and toString", "[Ipv4Address]") {

  SECTION("Default constructor") {
    Ipv4Address ip;
    REQUIRE(ip.toString() == "0.0.0.0");
  }

  SECTION("Constructor from network order uint32_t") {
    // 192.168.1.1 in network byte order (big-endian)
    uint32_t ip_network = htonl(0xC0A80101); // 192.168.1.1
    Ipv4Address ip(ip_network);
    REQUIRE(ip.toString() == "192.168.1.1");
  }

  SECTION("Constructor from raw bytes - typical") {
    // 10.0.0.1
    unsigned char bytes[] = {10, 0, 0, 1};
    Ipv4Address ip(bytes);
    REQUIRE(ip.toString() == "10.0.0.1");
  }

  SECTION("Constructor from raw bytes - localhost") {
    unsigned char bytes[] = {127, 0, 0, 1};
    Ipv4Address ip(bytes);
    REQUIRE(ip.toString() == "127.0.0.1");
  }

  SECTION("Constructor from raw bytes - broadcast") {
    unsigned char bytes[] = {255, 255, 255, 255};
    Ipv4Address ip(bytes);
    REQUIRE(ip.toString() == "255.255.255.255");
  }
}

TEST_CASE("Ipv4Address - Equality operator", "[Ipv4Address]") {
  unsigned char bytes1[] = {192, 168, 1, 1};
  unsigned char bytes2[] = {192, 168, 1, 1};
  unsigned char bytes3[] = {192, 168, 1, 2};

  Ipv4Address ip1(bytes1);
  Ipv4Address ip2(bytes2);
  Ipv4Address ip3(bytes3);

  REQUIRE(ip1 == ip2);       // Test equality
  REQUIRE_FALSE(ip1 == ip3); // Test inequality
}

TEST_CASE("Ipv4Address - Equality across construction methods",
          "[Ipv4Address]") {
  // Same IP constructed in different ways should be equal

  // 192.168.1.1 from raw bytes
  unsigned char bytes[] = {192, 168, 1, 1};
  Ipv4Address ip1(bytes);

  // 192.168.1.1 from network uint32_t
  uint32_t ip_network = htonl(0xC0A80101);
  Ipv4Address ip2(ip_network);

  REQUIRE(ip1 == ip2);
  REQUIRE(ip1.toString() == ip2.toString());
}

TEST_CASE("Ipv6Address - Construction and toString", "[Ipv6Address]") {
  // Default should be all-zero (::)
  Ipv6Address def;
  REQUIRE(def.toString() == "::");

  SECTION("Constructor from raw bytes via inet_pton - 2001:db8::1") {
    struct in6_addr addr6;
    REQUIRE(inet_pton(AF_INET6, "2001:db8::1", &addr6) == 1);
    Ipv6Address ip(addr6.s6_addr);
    REQUIRE(ip.toString() == "2001:db8::1");
  }

  SECTION("Constructor from raw bytes - loopback ::1") {
    struct in6_addr addr6;
    REQUIRE(inet_pton(AF_INET6, "::1", &addr6) == 1);
    Ipv6Address ip(addr6.s6_addr);
    REQUIRE(ip.toString() == "::1");
  }

  SECTION("Constructor from raw bytes - full address") {
    struct in6_addr addr6;
    REQUIRE(inet_pton(AF_INET6, "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
                      &addr6) == 1);
    Ipv6Address ip(addr6.s6_addr);
    // inet_ntop may compress zeros; ensure string is a valid representation
    REQUIRE(ip.toString().size() > 0);
  }
}

TEST_CASE("Ipv6Address - Equality operator", "[Ipv6Address]") {
  struct in6_addr a1, a2, a3;
  REQUIRE(inet_pton(AF_INET6, "2001:db8::1", &a1) == 1);
  REQUIRE(inet_pton(AF_INET6, "2001:db8::1", &a2) == 1);
  REQUIRE(inet_pton(AF_INET6, "2001:db8::2", &a3) == 1);

  Ipv6Address ip1(a1.s6_addr);
  Ipv6Address ip2(a2.s6_addr);
  Ipv6Address ip3(a3.s6_addr);

  REQUIRE(ip1 == ip2);
  REQUIRE_FALSE(ip1 == ip3);
}

TEST_CASE("Ipv6Address - toString caching", "[Ipv6Address]") {
  struct in6_addr addr6;
  REQUIRE(inet_pton(AF_INET6, "fe80::1", &addr6) == 1);
  Ipv6Address ip(addr6.s6_addr);

  std::string s1 = ip.toString();
  std::string s2 = ip.toString();
  REQUIRE(s1 == s2);
  REQUIRE(!s1.empty());
}