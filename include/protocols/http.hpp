#pragma once
#include "base_protocol.hpp"
#include <cstdint>
#include <map>
#include <string>

/**
 * @brief Represents parsed HTTP data (request OR response).
 *
 * We don't try to model every RFC detail.
 * Goal: something easy to print / log.
 */
struct HTTP : BaseProtocol {
  // High-level classification
  bool is_request; // true = HTTP request, false = HTTP response

  // ---- Request-line fields (only valid if is_request == true) ----
  std::string method;       // "GET", "POST", ...
  std::string uri;          // "/index.html", "/api/v1/user?id=5"
  std::string http_version; // "HTTP/1.1"

  // ---- Status-line fields (only valid if is_request == false) ----
  std::string status_code;   // "200", "404", ...
  std::string reason_phrase; // "OK", "Not Found", ...
  // http_version is reused above

  // ---- Headers ----
  // Header names canonicalized to lowercase for lookup convenience
  std::map<std::string, std::string> headers;

  // ---- Body ----
  std::string body; // raw body payload (may be binary in real life, but string
                    // is fine for inspection)

  // Helper: check common headers
  std::string get_header(const std::string &name_lowercase) const {
    auto it = headers.find(name_lowercase);
    if (it != headers.end()) {
      return it->second;
    }
    return "";
  }

  // Helper: guess content type
  std::string content_type() const { return get_header("content-type"); }

  // Helper: guess host (useful for requests)
  std::string host() const { return get_header("host"); }

  std::string get_name() const override { return "HTTP"; }
};
