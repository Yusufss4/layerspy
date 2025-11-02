## Quick orientation for AI coding agents

This repository is a small C++ network packet analyzer (LayerSpy). The goal
of this file is to give an AI agent the precise, discoverable facts needed to
be productive in this codebase.

- Project layout
  - `CMakeLists.txt` — canonical build. Sources are mirrored under `include/`
    (headers) and `src/` (implementations). Tests live in `test/` and are
    built into `layerspy_test` via Catch2.
  - `app/main.cpp` — CLI entrypoint (uses CLI11). Currently contains TODOs;
    adding runtime behaviour should update/consume `layerspy_lib`.
  - `include/` — public headers. Important files:
    - `include/decoder.hpp` — the Decoder class and parsing chain (chain of
      responsibility). Core place to add new protocol parsing order.
    - `include/protocols/*.hpp` — protocol model structs (Ethernet, IPv4,
      TCP, HTTP). Each struct implements `get_name()` and stores parsed fields.
    - `include/types/*` — small value types (MacAddress, Ipv4/Ipv6Address)
    - `include/display.hpp`, `include/sniffer.hpp`, `include/layerspy_engine.hpp`
      exist (some are currently stubs) and show intended boundaries.

- Key architecture and patterns
  - Decoder implements a sequential parser chain: parse_ethernet -> parse
    ip -> parse_tcp -> parse_http. Parsers mutate a `std::string_view &data`
    to consume headers and return a `std::unique_ptr<BaseProtocol>`.
  - `BaseProtocol` (include/protocols/base_protocol.hpp) is the polymorphic
    node: it contains `payload` (unique_ptr to next layer) and
    `raw_payload` (a string_view into the remaining bytes). Important: the
    `raw_payload` is a view — don't let it outlive the original packet buffer.
  - Protocol headers are simple POD-like structs with helpers (e.g.,
    `TCP::is_http_candidate()`) and `get_name()` for display.

- Build & test workflows (commands you should run)
  - Create build dir, configure, build (out-of-source):
    ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build -- -j
    ```
    - Debug build activates AddressSanitizer/UBSan because the CMake sets
      `CMAKE_CXX_FLAGS_DEBUG` to include `-fsanitize=address -fsanitize=undefined`.
  - Run tests (Catch2 + CTest discovery):
    ```bash
    cmake --build build --target layerspy_test
    ctest --test-dir build -V
    # or run test binary directly:
    ./build/layerspy_test
    ```
    - Notes about dependencies: libpcap is required. CMake tries `find_package(PCAP)`
      and falls back to pkg-config. If missing, install libpcap-dev (system
      package) before configuring.

- Project-specific conventions & gotchas
  - Sources are globbed in `CMakeLists.txt` (file(GLOB_RECURSE CORE_SOURCES
    "src/*.cpp" ...)). Adding a header+src pair under `include/` and `src/`
    will be picked up automatically.
  - Compiler flags are strict: `-Werror -Wextra -Wpedantic`. Expect build to
    fail on warnings. Keep changes warning-free.
  - Use `std::string_view` heavily for parsing. Beware lifetime: `raw_payload`
    references the original packet buffer passed to `Decoder::decodePacket`.
  - Add a protocol: create `include/protocols/foo.hpp`, implement parser in
    `src/protocols/foo.cpp`, and add a `parse_foo()` call in `Decoder` chain.
    CMake will include the new source file via the glob.

- Tests & examples to follow for style
  - Tests use Catch2 (third_party/Catch2) with `catch_discover_tests` in CMake.
    Example tests: `test/test_ip_address.cpp` shows how to construct
    Ipv4/Ipv6 types (uses `inet_pton` and `htonl` helpers).
  - Use the existing types' constructors in tests (raw byte arrays or network
    order uint32_t) and call `toString()` for comparisons.

- Integration points
  - `libpcap` (PCAP) — packet capture dependency; code will parse buffers from
    pcap if sniffer is implemented. CMake exposes `PCAP_LIBRARIES` to
    link against `layerspy_lib`.
  - `CLI11` — used by `app/main.cpp` for CLI parsing.

- Where to start for common tasks
  - Implement parser for a protocol: edit `include/protocols` +
    `src/protocols`, then wire a `parse_*` method in `include/decoder.hpp` and
    implement in `src/decoder.cpp` (decoder.cpp currently exists — follow the
    existing parse_* signatures).
  - Add a utility or test: put header in `include/types` (or `include/`),
    implementation in `src/types`, test under `test/` and rely on CMake globbing.

If anything here is ambiguous or you want more detail on a specific area
(parsing chain internals, display formatting, or the sniffer integration), say
which file(s) you'd like expanded and I'll extend this guide.
