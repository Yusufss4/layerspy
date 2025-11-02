#include "decoder.hpp"

// Include all our protocol data structs
#include "protocols/ethernet.hpp"
#include "protocols/ipv4.hpp" // We will create this next
#include "protocols/ipv6.hpp" // And this after
#include "protocols/tcp.hpp"  // And this after

// System headers for byte manipulation
#include <arpa/inet.h> // For ntohs() (Network to Host Short)

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
  if (data.length() < Ethernet::HEADER_SIZE) {
    return nullptr;
  }

  auto eth = std::make_unique<Ethernet>();
  const unsigned char *bytes =
      reinterpret_cast<const unsigned char *>(data.data());

  // --- NEW, CLEANER PARSING ---
  // 1. Destination MAC (First 6 bytes)
  eth->dest_mac = MacAddress(bytes);

  // 2. Source MAC (Next 6 bytes)
  eth->source_mac = MacAddress(bytes + MacAddress::LENGTH);
  // --- END NEW PARSING ---

  eth->eth_type = ntohs(
      *reinterpret_cast<const uint16_t *>(bytes + Ethernet::ETH_TYPE_OFFSET));

  data.remove_prefix(Ethernet::HEADER_SIZE);

  // --- CHAIN OF RESPONSIBILITY ---
  // Look at the EtherType to decide which parser to call next.
  switch (eth->eth_type) {
  case Ethernet::ETH_TYPE_IPV4:      // 0x0800
    eth->payload = parse_ipv4(data); // Call the L3 parser
    break;

  case Ethernet::ETH_TYPE_IPV6:      // 0x86DD
    eth->payload = parse_ipv6(data); // (You can add this later)
    break;

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
  if(data.length() < IPv4::MIN_HEADER_SIZE) {
    return nullptr;
  }

  auto ipv4 = std::make_unique<IPv4>();
  const unsigned char *bytes =
      reinterpret_cast<const unsigned char *>(data.data());

  

  // For now, we'll just set the raw payload and return
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

std::unique_ptr<BaseProtocol> Decoder::parse_ipv6(std::string_view &data) {
  // YOUR TODO:
  // 1. Create `include/protocols/ipv6.hpp` (like we did for IPv4)
  //    It needs fields for: version, traffic_class, flow_label,
  //    payload_length, next_header, hop_limit, source_ip, dest_ip, etc.
  // 2. An IPv6 header is 40 bytes. Check data.length().
  // 3. Create `auto ipv6 = std::make_unique<IPv6>();`
  // 4. Parse all the fields. Use ntohs() for 16-bit fields.
  // 5. Consume 40 bytes from the data (data.remove_prefix(40)).
  // 6. Use a `switch(ipv6->next_header)` to call the next parser:
  //    case 6: // TCP
  //        ipv6->payload = parse_tcp(data);
  //        break;
  //    case 17: // UDP
  //        // ipv6->payload = parse_udp(data);
  //        break;

  // For now, we'll just set the raw payload and return
  auto ipv6 = std::make_unique<IPv6>(); // (This won't compile until you create
                                        // the file)
  ipv6->raw_payload = data;
  return ipv6;
}