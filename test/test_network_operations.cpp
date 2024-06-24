#include "catch2/catch_message.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "network_operations.hpp"
#include "undirected_network.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <climits>
#include <cstddef>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <memory>
#include <optional>
#include <set>
#include <vector>

TEST_CASE("Testing bog-standard BFS with unique shortest paths") {
  using namespace Graph;
  using UndirectedNetwork = UndirectedNetwork<double>;
  using WeightT = double;

  // Generate some network
  /* Nodes: 0, 2, 1, 3, 4, 5
  ACII art made in https://asciiflow.com
      +-+
      |0|
      +-+
      +-+
      |2|
  +---------+
  |1|     |3|
  +-+     +-+
       +--+ +---+
       |4|    |5|
       +-+    +-+
  */
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

  // When the source is set as 0, test that the depth at 5 is 3 when max_depth
  // is not set If max_depth is not set, then the whole graph will be traversed.
  int depth_required = 3;
  auto depth_level(std::vector<int>(network.n_agents(), INT_MAX));
  auto parent = std::vector<std::vector<int>>{n_agents, std::vector<int>{}};
  // Perform the BFS from the source 0, and no max_depth
  bfs(network, parent, depth_level, 0, std::nullopt);
  REQUIRE(depth_level[5] == depth_required);

  // Clear parent and depth_level
  for (auto &p : parent)
    p.clear();
  depth_level.clear();
  depth_level.resize(network.n_agents(), INT_MAX);

  // Check that the BFS gives a depth level value of INT_MAX if the max_depth is
  // set to 2
  bfs(network, parent, depth_level, 0, 2);
  REQUIRE(depth_level[5] == INT_MAX);
}