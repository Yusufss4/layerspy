#include "catch2/catch_test_macros.hpp"

// --- Headers to Test ---
#include "decoder.hpp"
#include "protocols/ethernet.hpp"
#include "protocols/ipv4.hpp" // (Needed for the stubbed parse_ipv4)

// --- System Headers ---
#include <cstdint>
#include <memory>
#include <string_view>

// This is the raw byte data for a real packet.
// We'll use this as our test input.
//
// This packet is:
// L2: Ethernet
//   - Dest MAC:   00:11:22:33:44:55
//   - Source MAC: aa:bb:cc:dd:ee:ff
//   - EtherType:  0x0800 (IPv4)
// L3: IPv4
//   - (20 bytes of dummy IPv4 header starting with 0x45)
//
const unsigned char golden_packet_bytes[] = {
    // --- Ethernet Header (14 bytes) ---
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // Dest MAC
    0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, // Source MAC
    0x08, 0x00,                         // EtherType (IPv4)

    // --- Dummy IPv4 Header (20 bytes) ---
    // (This is what will be passed to your stubbed parse_ipv4)
    0x45, 0x00, 0x00, 0x28, 0x12, 0x34, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00,
    0xc0, 0xa8, 0x01, 0x01, 0x0a, 0x00, 0x00, 0x01};

TEST_CASE("Decoder can parse a L2 Ethernet header", "[decoder]") {

  // 1. ARRANGE
  // Create the Decoder and the raw data view.
  Decoder decoder;
  std::string_view packet_data(
      reinterpret_cast<const char *>(golden_packet_bytes),
      sizeof(golden_packet_bytes));

  // 2. ACT
  // Call the method we want to test.
  std::unique_ptr<BaseProtocol> parsed_tree = decoder.decodePacket(packet_data);

  // 3. ASSERT
  // Now we check if everything is correct.

  // Check 1: Did we get a packet at all?
  REQUIRE(parsed_tree != nullptr);

  // Check 2: Is the base of the tree an Ethernet packet?
  // We use dynamic_cast to safely check and cast.
  Ethernet *eth = dynamic_cast<Ethernet *>(parsed_tree.get());
  REQUIRE(eth != nullptr);

  // Check 3: Is the parsed data correct?
  CHECK(eth->get_name() == "Ethernet II");
  CHECK(eth->dest_mac == "00:11:22:33:44:55");
  CHECK(eth->source_mac == "aa:bb:cc:dd:ee:ff");
  CHECK(eth->eth_type == Ethernet::ETH_TYPE_IPV4); // 0x0800

  // Check 4: Was the payload chained correctly?
  // (This checks if the 'switch' statement in parse_ethernet worked)
  // Our stubbed parse_ipv4 returns a non-null IPv4 object, so
  // the payload pointer should not be null.
  REQUIRE(eth->payload != nullptr);

  // Check 5: Was the payload object the correct type?
  IPv4 *ipv4 = dynamic_cast<IPv4 *>(eth->payload.get());
  REQUIRE(ipv4 != nullptr);

  // Check 6: Was the *remaining* data passed to the IPv4 payload?
  // We check that the raw_payload of the *next* layer is correct.
  // (This proves our string_view.remove_prefix(14) worked!)
  CHECK(ipv4->raw_payload.length() == 20);
  CHECK(ipv4->raw_payload.data()[0] == '\x45'); // First byte of IPv4
}

TEST_CASE("Decoder handles undersized packets", "[decoder]") {
  // 1. ARRANGE
  Decoder decoder;
  // Create a packet that is too small to be an Ethernet frame
  const unsigned char tiny_packet[] = {0x00, 0x11, 0x22};
  std::string_view tiny_data(reinterpret_cast<const char *>(tiny_packet),
                             sizeof(tiny_packet));

  // 2. ACT
  std::unique_ptr<BaseProtocol> parsed_tree = decoder.decodePacket(tiny_data);

  // 3. ASSERT
  // The parser should have safely returned nothing.
  REQUIRE(parsed_tree == nullptr);
}