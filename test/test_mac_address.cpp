#include "types/mac_address.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string_view> // Include for std::string_view

TEST_CASE("MacAddress - Construction and toString formatting", "[MacAddress]") {

  SECTION("Default constructor") {
    MacAddress mac;
    REQUIRE(mac.toString() == "00:00:00:00:00:00");
  }

  SECTION("Constructor from raw bytes - typical") {
    unsigned char bytes[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    MacAddress mac(bytes);
    REQUIRE(mac.toString() == "aa:bb:cc:dd:ee:ff");
  }

  SECTION("Constructor from raw bytes - broadcast") {
    unsigned char bytes[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    MacAddress mac(bytes);
    REQUIRE(mac.toString() == "ff:ff:ff:ff:ff:ff");
  }

  SECTION("Constructor from raw bytes - checks zero padding") {
    // This test ensures that bytes < 0x10 are padded with a leading '0'
    unsigned char bytes[] = {0x01, 0x02, 0x03, 0x0A, 0x0B, 0x0C};
    MacAddress mac(bytes);
    REQUIRE(mac.toString() == "01:02:03:0a:0b:0c");
  }
}

TEST_CASE("MacAddress - Equality operator (MacAddress to MacAddress)",
          "[MacAddress]") {
  unsigned char bytes1[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  unsigned char bytes2[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  unsigned char bytes3[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFE};

  MacAddress mac1(bytes1);
  MacAddress mac2(bytes2);
  MacAddress mac3(bytes3);

  REQUIRE(mac1 == mac2);       // Test equality
  REQUIRE_FALSE(mac1 == mac3); // Test inequality
}

TEST_CASE("MacAddress - Equality operator (MacAddress to string_view)",
          "[MacAddress]") {
  unsigned char bytes[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE};
  MacAddress mac(bytes);

  SECTION("Matching string (lowercase)") {
    REQUIRE(mac == "de:ad:be:ef:ca:fe");
    REQUIRE(mac == std::string_view("de:ad:be:ef:ca:fe"));
  }

  SECTION("Non-matching string (uppercase)") {
    // Based on the implementation, toString() produces lowercase,
    // so a case-sensitive compare should fail.
    REQUIRE_FALSE(mac == "DE:AD:BE:EF:CA:FE");
  }

  SECTION("Non-matching string (different value)") {
    REQUIRE_FALSE(mac == "aa:bb:cc:dd:ee:fe");
    REQUIRE_FALSE(mac == "00:00:00:00:00:00");
  }

  SECTION("Invalid format strings") {
    REQUIRE_FALSE(mac == "");
    REQUIRE_FALSE(mac == "not-a-mac-address");
  }
}