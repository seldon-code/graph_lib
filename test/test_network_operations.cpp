#include "catch2/matchers/catch_matchers.hpp"
#include "network_operations.hpp"
#include "undirected_network.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <cstddef>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <memory>
#include <set>
#include <vector>

TEST_CASE("Testing network operations") {
  using namespace Graph;
  using UndirectedNetwork = UndirectedNetwork<double>;
  using WeightT = double;

  // Generate some network
  const auto n_agents = 6;
  auto network = UndirectedNetwork(n_agents);
  const WeightT weight_edge = 1.0;
  const auto total_n_edges = 5;
  // Add edges to the network
  network.push_back_neighbour_and_weight(0, 2, weight_edge);
  network.push_back_neighbour_and_weight(2, 1, weight_edge);
  network.push_back_neighbour_and_weight(2, 3, weight_edge);
  network.push_back_neighbour_and_weight(3, 4, weight_edge);
  network.push_back_neighbour_and_weight(3, 5, weight_edge);

  // Check that the network has been created with the prescribed number of edges
  REQUIRE(network.n_edges() == total_n_edges);
  // There is only one edge connected to 0
  REQUIRE(network.n_edges(0) == 1);

  // Create a graph traversal object
  auto graph_traversal = NetworkOperations<WeightT>(network);

  // Find a path from 0 to 5
  const auto path_ref = std::vector<size_t>{0, 2, 3, 5}; // Desired path
  auto path = graph_traversal.path_from_dfs(0, 5);
  INFO(fmt::format("path from DFS = {}\n", path.value()));

  REQUIRE_THAT(path.value(), Catch::Matchers::UnorderedRangeEquals(path_ref));
}