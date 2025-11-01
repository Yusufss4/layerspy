#include <CLI/CLI.hpp>
#include <iostream>

int main(int argc, char **argv) {
  CLI::App app{"LayerSpy - Network Packet Analyzer"};

  // TODO: Add CLI options here
  std::string interface = "eth0";
  app.add_option("-i,--interface", interface,
                 "Network interface to capture from");

  CLI11_PARSE(app, argc, argv);

  std::cout << "LayerSpy starting on interface: " << interface << std::endl;
  std::cout << "TODO: Implement packet capture logic" << std::endl;

  return 0;
}
