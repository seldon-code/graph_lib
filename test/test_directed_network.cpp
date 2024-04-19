#include "directed_network.hpp"
#include "util/network_generation.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <cstddef>
#include <random>
#include <set>
#include <vector>

TEST_CASE("Testing the directed network class") {
  using namespace Graph;
  using DirectedNetwork = DirectedNetwork<double>;
  using WeightT = double;

  // Generate some network
  const size_t n_agents = 4;

  auto network = DirectedNetwork(
      std::vector<std::vector<size_t>>{{1, 2}, {1}, {0}, {}},
      std::vector<std::vector<WeightT>>{{0.5, 0.5}, {0.5}, {0.2}, {}},
      DirectedNetwork::EdgeDirection::Incoming);

  // Does n_agents work?
  REQUIRE(network.n_agents() == n_agents);
  // Does n_edges work?
  REQUIRE(network.n_edges() == 4);

  // Check that the function for setting neighbours and a single weight work
  // Agent 3
  std::vector<size_t> neigh{{0, 10}};                     // new neighbours
  std::vector<DirectedNetwork::WeightT> weight{0.5, 0.5}; // new weights (const)
  network.set_neighbours_and_weights(3, neigh, 0.5);
  auto buffer_w_get = network.get_weights(3);

  REQUIRE_THAT(weight, Catch::Matchers::UnorderedRangeEquals(buffer_w_get));

  SECTION("Checking that set_weight, get_neighbour work") {

    weight = {0.25, 0.55};
    network.set_weights(3, weight);
    auto buffer_w_get = network.get_weights(3);
    REQUIRE_THAT(weight, Catch::Matchers::UnorderedRangeEquals(buffer_w_get));
    REQUIRE(network.n_edges(3) == 2);

    size_t n = network.get_neighbours(3)[0];
    REQUIRE(n == neigh[0]);
    n = 2;
    // Set the neighbour
    network.set_edge(3, 0, n);
    REQUIRE(network.get_neighbours(3)[0] == 2);

    DirectedNetwork::WeightT w = network.get_weights(3)[1];
    REQUIRE(w == 0.55);
    w = 0.9;
    network.set_edge_weight(3, 1, w);
    REQUIRE(network.get_weights(3)[1] == w);
  }

  SECTION("Checking that set_neighbours_and_weights works with a vector of "
          "weights, push_back and transpose") {
    // Change the connections for agent 3
    std::vector<size_t> buffer_n{0};                     // new neighbours
    std::vector<DirectedNetwork::WeightT> buffer_w{0.1}; // new weights
    network.set_neighbours_and_weights(3, buffer_n, buffer_w);

    // Make sure the changes worked
    auto buffer_n_get = network.get_neighbours(3);
    auto buffer_w_get = network.get_weights(3);

    REQUIRE_THAT(buffer_n_get, Catch::Matchers::UnorderedRangeEquals(buffer_n));
    REQUIRE_THAT(buffer_w_get, Catch::Matchers::UnorderedRangeEquals(buffer_w));

    // Check that the push_back function works for agent 3
    buffer_n.push_back(1);   // new neighbour
    buffer_w.push_back(1.0); // new weight for this new connection
    network.push_back_neighbour_and_weight(
        3, 1, 1.0); // new connection added with weight
    // Check that the change worked for the push_back function
    buffer_n_get = network.get_neighbours(3);
    buffer_w_get = network.get_weights(3);
    REQUIRE_THAT(buffer_n_get, Catch::Matchers::UnorderedRangeEquals(buffer_n));
    REQUIRE_THAT(buffer_w_get, Catch::Matchers::UnorderedRangeEquals(buffer_w));

    // Now we test the toggle_incoming_outgoing() function

    // First record all the old edges as tuples (i,j,w) where this edge goes
    // from j -> i with weight w
    std::set<std::tuple<size_t, size_t, DirectedNetwork::WeightT>> old_edges;
    for (size_t i_agent = 0; i_agent < network.n_agents(); i_agent++) {
      auto buffer_n = network.get_neighbours(i_agent);
      auto buffer_w = network.get_weights(i_agent);

      for (size_t i_neighbour = 0; i_neighbour < buffer_n.size();
           i_neighbour++) {
        auto neighbour = buffer_n[i_neighbour];
        auto weight = buffer_w[i_neighbour];
        std::tuple<size_t, size_t, DirectedNetwork::WeightT> edge{
            i_agent, neighbour, weight};
        old_edges.insert(edge);
      }
    }

    auto old_direction = network.direction();
    network.toggle_incoming_outgoing();
    auto new_direction = network.direction();

    // Direction should have changed as well
    REQUIRE(old_direction != new_direction);

    // Now we go over the toggled network and try to re-identify all edges
    for (size_t i_agent = 0; i_agent < network.n_agents(); i_agent++) {
      auto buffer_n = network.get_neighbours(i_agent);
      auto buffer_w = network.get_weights(i_agent);

      for (size_t i_neighbour = 0; i_neighbour < buffer_n.size();
           i_neighbour++) {
        auto neighbour = buffer_n[i_neighbour];
        auto weight = buffer_w[i_neighbour];
        std::tuple<size_t, size_t, DirectedNetwork::WeightT> edge{
            neighbour, i_agent, weight}; // Note that i_agent and neighbour are
                                         // flipped compared to before
        REQUIRE(old_edges.contains(edge)); // can we find the transposed edge?
        old_edges.extract(edge);           // extract the edge afterwards
      }
    }

    REQUIRE(old_edges.empty());
  }

  SECTION("Test remove double counting") {
    // clang-format off
        std::vector<std::vector<size_t>> neighbour_list =  { 
                { 2, 1, 1, 0 }, 
                { 2, 0, 1, 2}, 
                { 1, 1, 0, 2, 1 },
                {},
                {3,1}
            };
  
        std::vector<std::vector<double>> weight_list =  { 
                { -1, 1, 2, 0 }, 
                { -1, 1, 2, -1 }, 
                { -1, 1, 2, 3, 1 },
                {},
                {1, 1}
            };

        std::vector<std::vector<size_t>> neighbour_no_double_counting =  { 
            { 0, 1, 2 }, 
            { 0, 1, 2}, 
            { 0, 1, 2},
            {},
            {1,3}
        };

        std::vector<std::vector<double>> weights_no_double_counting =  { 
            { 0, 3, -1 }, 
            { 1, 2, -2}, 
            { 2, 1, 3},
            {},
            {1,1}
        };
    // clang-format on

    auto network = Graph::DirectedNetwork<double>(
        std::move(neighbour_list), std::move(weight_list),
        Graph::DirectedNetwork<double>::EdgeDirection::Incoming);

    network.remove_double_counting();

    for (size_t i_agent = 0; i_agent < network.n_agents(); i_agent++) {
      auto weights = network.get_weights(i_agent);
      auto neighbours = network.get_neighbours(i_agent);
      REQUIRE_THAT(neighbours, Catch::Matchers::RangeEquals(
                                   neighbour_no_double_counting[i_agent]));
      REQUIRE_THAT(weights, Catch::Matchers::RangeEquals(
                                weights_no_double_counting[i_agent]));
    }
  }
}