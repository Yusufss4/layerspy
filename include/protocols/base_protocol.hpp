#pragma once
#include <memory>
#include <string>
#include <string_view> // The C++17 workhorse

/**
 * @brief Base struct for all parsed protocol headers.
 * * This is the "interface" that allows us to chain protocols together.
 * Each protocol (Ethernet, IPv4, TCP) will have its own struct that
 * inherits from this.
 */
struct BaseProtocol {
  // Virtual destructor for correct polymorphic cleanup
  virtual ~BaseProtocol() = default;

  // The next layer in the chain (e.g., Ethernet's payload is IPv4)
  std::unique_ptr<BaseProtocol> payload;

  // The raw bytes of the *next* layer, which might be
  // an unparsed protocol or the final application data.
  std::string_view raw_payload;

  // A helper for the Display component
  virtual std::string get_name() const = 0;
};