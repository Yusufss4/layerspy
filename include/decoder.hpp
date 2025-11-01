#pragma once
#include "protocols/base_protocol.hpp" // Our "interface"
#include <memory>
#include <string_view>

// Forward-declare the structs we will parse
struct Ethernet;
struct IPv4;
struct TCP;

/**
 * @brief The "Brain" of LayerSpy.
 * * This class takes raw bytes and implements the "Chain of Responsibility"
 * pattern to parse the protocol stack.
 */
class Decoder {
public:
  /**
   * @brief Main entry point. Decodes a raw packet.
   * @param data A string_view of the raw packet bytes.
   * @return A unique_ptr to the base of the protocol tree (Ethernet).
   */
  std::unique_ptr<BaseProtocol> decodePacket(std::string_view data);

private:
  // --- The Parser Chain ---
  // Each function parses its layer, modifies the 'data' view
  // to remove the header, and calls the next parser in the chain.

  std::unique_ptr<BaseProtocol> parse_ethernet(std::string_view &data);
  std::unique_ptr<BaseProtocol> parse_ipv4(std::string_view &data);
  std::unique_ptr<BaseProtocol> parse_ipv6(std::string_view &data);
  std::unique_ptr<BaseProtocol> parse_tcp(std::string_view &data);
  std::unique_ptr<BaseProtocol> parse_udp(std::string_view &data);
  std::unique_ptr<BaseProtocol> parse_icmp(std::string_view &data);
  std::unique_ptr<BaseProtocol> parse_http(std::string_view &data);

};