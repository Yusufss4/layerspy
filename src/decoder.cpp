#include "decoder.hpp"

// Include all our protocol data structs
#include "protocols/ethernet.hpp"
#include "protocols/ipv4.hpp" // We will create this next
#include "protocols/tcp.hpp"  // And this after

// System headers for byte manipulation
#include <arpa/inet.h> // For ntohs() (Network to Host Short)
#include <iomanip>     // For formatting hex
#include <sstream>     // For building MAC/IP strings

// --- Helper Functions for Manual Parsing ---

/**
 * @brief Converts 6 bytes of a MAC address to a string.
 */
std::string mac_to_string(const unsigned char *mac_bytes) {
  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  for (int i = 0; i < 6; ++i) {
    ss << std::setw(2) << static_cast<int>(mac_bytes[i]);
    if (i < 5)
      ss << ":";
  }
  return ss.str();
}

/**
 * @brief (Helper for you) Converts 4 bytes of an IP address to a string.
 */
std::string ip_to_string(const unsigned char *ip_bytes) {
  std::stringstream ss;
  ss << std::dec;
  for (int i = 0; i < 4; ++i) {
    ss << static_cast<int>(ip_bytes[i]);
    if (i < 3)
      ss << ".";
  }
  return ss.str();
}

// --- Public Decoder Methods ---

std::unique_ptr<BaseProtocol> Decoder::decodePacket(std::string_view data) {
  // The chain always starts at Layer 2.
  // We pass the string_view by reference so the parsers can modify it.
  return parse_ethernet(data);
}

// --- Private Parser Chain Methods ---

/**
 * @brief Parses the Layer 2 Ethernet header.
 * THIS IS WHERE YOU LEARN MANUAL PARSING.
 */
std::unique_ptr<BaseProtocol> Decoder::parse_ethernet(std::string_view &data) {
  // An Ethernet header is 14 bytes. Check if we have enough data.
  if (data.length() < 14) {
    return nullptr; // Not a valid packet
  }

  // Create the C++ object to store our data
  auto eth = std::make_unique<Ethernet>();

  // Get a C-style pointer to the raw byte data
  const unsigned char *bytes =
      reinterpret_cast<const unsigned char *>(data.data());

  // --- MANUAL PARSING ---
  // 1. Destination MAC (First 6 bytes)
  eth->dest_mac = mac_to_string(bytes);

  // 2. Source MAC (Next 6 bytes)
  eth->source_mac = mac_to_string(bytes + 6);

  // 3. EtherType (Last 2 bytes)
  // We must use ntohs() (Network To Host Short) because the packet
  // data is in "Network Byte Order" (Big Endian), and our
  // computers (x86) are "Host Byte Order" (Little Endian).
  eth->eth_type = ntohs(*reinterpret_cast<const uint16_t *>(bytes + 12));
  // --- END MANUAL PARSING ---

  // **CRITICAL STEP**: "Consume" the 14 bytes we just parsed
  // from the string_view. The view now points to the L3 payload.
  data.remove_prefix(14);

  // --- CHAIN OF RESPONSIBILITY ---
  // Look at the EtherType to decide which parser to call next.
  switch (eth->eth_type) {
  case Ethernet::ETH_TYPE_IPV4:      // 0x0800
    eth->payload = parse_ipv4(data); // Call the L3 parser
    break;

    // case Ethernet::ETH_TYPE_IPV6: // 0x86DD
    //     eth->payload = parse_ipv6(data); // (You can add this later)
    //     break;

  default:
    // We don't know this L3 protocol. Stop parsing
    // and store the rest as the raw payload.
    eth->raw_payload = data;
    break;
  }

  return eth;
}

/**
 * @brief STUB for Layer 3 IPv4 Parser.
 * This is your next task!
 */
std::unique_ptr<BaseProtocol> Decoder::parse_ipv4(std::string_view &data) {
  // YOUR TODO:
  // 1. Create `include/protocols/ipv4.hpp` (like we did for Ethernet)
  //    It needs fields for: version, IHL, total_length, protocol,
  //    source_ip, dest_ip, etc.
  // 2. An IPv4 header is *at least* 20 bytes. Check data.length().
  // 3. Create `auto ipv4 = std::make_unique<IPv4>();`
  // 4. Parse all the fields. Use ntohs() for 16-bit fields.
  // 5. **IMPORTANT**: Get the IHL (Internet Header Length) field.
  //    The header length is `ihl * 4` bytes!
  // 6. Consume `ihl * 4` bytes from the data (data.remove_prefix(...)).
  // 7. Use a `switch(ipv4->protocol)` to call the next parser:
  //    case 6: // TCP
  //        ipv4->payload = parse_tcp(data);
  //        break;
  //    case 17: // UDP
  //        // ipv4->payload = parse_udp(data);
  //        break;

  // For now, we'll just set the raw payload and return
  auto ipv4 = std::make_unique<IPv4>(); // (This won't compile until you create
                                        // the file)
  ipv4->raw_payload = data;
  return ipv4;
}

/**
 * @brief STUB for Layer 4 TCP Parser.
 * This is your task after IPv4.
 */
std::unique_ptr<BaseProtocol> Decoder::parse_tcp(std::string_view &data) {
  // YOUR TODO:
  // 1. Create `include/protocols/tcp.hpp`
  //    Fields: source_port, dest_port, seq_num, ack_num, flags (SYN/ACK/...),
  //    window_size
  // 2. A TCP header is *at least* 20 bytes.
  // 3. Parse all fields. Use ntohs() and ntohl() (Network to Host Long)
  // 4. **IMPORTANT**: Get the "Data Offset" field.
  //    The header length is `data_offset * 4` bytes!
  // 5. Consume the header bytes from the data.
  // 6. The remaining data is the Application Layer (e.g., HTTP).
  //    Set `tcp->raw_payload = data;`

  // For now, we'll just set the raw payload
  auto tcp =
      std::make_unique<TCP>(); // (This won't compile until you create the file)
  tcp->raw_payload = data;
  return tcp;
}