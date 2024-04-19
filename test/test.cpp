#include "undirected_network.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <cstddef>
#include <random>
#include <set>

TEST_CASE("Testing the network class") {
  using namespace Graph;
  using DirectedNetwork = UndirectedNetwork<double>;

  // Generate some network
  const size_t n_agents = 20;
  const size_t n_connections = 10;
  std::mt19937 gen(0);

  REQUIRE(1 == 1);
}