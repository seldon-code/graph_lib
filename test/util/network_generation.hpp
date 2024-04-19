#pragma once
#include "directed_network.hpp"
#include "undirected_network.hpp"
#include <cstddef>
#include <random>
#include <vector>

// These are usually inside Seldon. We have some functions here to help test...

namespace Graph::DirectedNetworkGeneration {

// @TODO generate_fully_connected does not need to be overloaded..perhaps a
// std::optional instead to reduce code duplication?
template <typename AgentType>
DirectedNetwork<AgentType> generate_fully_connected(
    size_t n_agents,
    typename DirectedNetwork<AgentType>::WeightT weight = 0.0) {
  using DirectedNetworkT = DirectedNetwork<AgentType>;
  using WeightT = typename DirectedNetworkT::WeightT;

  std::vector<std::vector<size_t>>
      neighbour_list; // Neighbour list for the connections
  std::vector<std::vector<WeightT>>
      weight_list; // List for the interaction weights of each connection
  auto incoming_neighbour_buffer =
      std::vector<size_t>(n_agents); // for the j_agents indices connected to
                                     // i_agent (adjacencies/neighbours)
  auto incoming_neighbour_weights = std::vector<WeightT>(
      n_agents, weight); // Vector of weights of the j neighbours of i

  // Create the incoming_neighbour_buffer once. This will contain all agents,
  // including itself
  for (size_t i_agent = 0; i_agent < n_agents; ++i_agent) {
    incoming_neighbour_buffer[i_agent] = i_agent;
  }

  // Loop through all the agents and update the neighbour_list and weight_list
  for (size_t i_agent = 0; i_agent < n_agents; ++i_agent) {
    // Add the neighbour vector for i_agent to the neighbour list
    neighbour_list.push_back(incoming_neighbour_buffer);
    // Add the weight interactions for the neighbours of i_agent
    weight_list.push_back(incoming_neighbour_weights);

  } // end of loop through n_agents

  return DirectedNetworkT(std::move(neighbour_list), std::move(weight_list),
                          DirectedNetworkT::EdgeDirection::Incoming);
}

template <typename AgentType>
DirectedNetwork<AgentType> generate_fully_connected(size_t n_agents,
                                                    std::mt19937 &gen) {
  using DirectedNetworkT = DirectedNetwork<AgentType>;
  using WeightT = typename DirectedNetworkT::WeightT;

  std::vector<std::vector<size_t>>
      neighbour_list; // Neighbour list for the connections
  std::vector<std::vector<WeightT>>
      weight_list; // List for the interaction weights of each connection
  auto incoming_neighbour_buffer =
      std::vector<size_t>(n_agents); // for the j_agents indices connected to
                                     // i_agent (adjacencies/neighbours)
  std::uniform_real_distribution<> dis(
      0.0, 1.0); // Values don't matter, will be normalized
  auto incoming_neighbour_weights = std::vector<WeightT>(
      n_agents); // Vector of weights of the j neighbours of i
  WeightT outgoing_norm_weight = 0;

  // Create the incoming_neighbour_buffer once. This will contain all agents,
  // including itself
  for (size_t i_agent = 0; i_agent < n_agents; ++i_agent) {
    incoming_neighbour_buffer[i_agent] = i_agent;
  }

  // Loop through all the agents and create the neighbour_list and weight_list
  for (size_t i_agent = 0; i_agent < n_agents; ++i_agent) {

    outgoing_norm_weight = 0.0;

    // Initialize the weights
    for (size_t j = 0; j < n_agents; ++j) {
      incoming_neighbour_weights[j] = dis(gen); // Draw the weight
      outgoing_norm_weight += incoming_neighbour_weights[j];
    }

    // ---------
    // Normalize the weights so that the row sums to 1
    // Might be specific to the DeGroot model?
    for (size_t j = 0; j < incoming_neighbour_buffer.size(); ++j) {
      incoming_neighbour_weights[j] /= outgoing_norm_weight;
    }

    // Add the neighbour vector for i_agent to the neighbour list
    neighbour_list.push_back(incoming_neighbour_buffer);
    // Add the weight interactions for the neighbours of i_agent
    weight_list.push_back(incoming_neighbour_weights);

  } // end of loop through n_agents

  return DirectedNetworkT(std::move(neighbour_list), std::move(weight_list),
                          DirectedNetworkT::EdgeDirection::Incoming);
}

/* Constructs a new network on a square lattice of edge length n_edge (with
 * PBCs)*/
template <typename AgentType>
DirectedNetwork<AgentType> generate_square_lattice(
    size_t n_edge, typename DirectedNetwork<AgentType>::WeightT weight = 0.0) {
  using DirectedNetworkT = DirectedNetwork<AgentType>;
  using WeightT = typename DirectedNetworkT::WeightT;
  auto n_agents = n_edge * n_edge;

  // Create an empty DirectedNetwork
  auto network = DirectedNetworkT(n_agents);

  auto wrap_edge_index = [&](int k) {
    if (k >= int(n_edge)) {
      return k - int(n_edge);
    } else if (k < 0) {
      return int(n_edge) + k;
    } else {
      return k;
    }
  };

  auto linear_index = [&](int i, int j) {
    auto idx = wrap_edge_index(i) + n_edge * wrap_edge_index(j);
    return idx;
  };

  for (int i = 0; i < int(n_edge); i++) {
    // other edge
    for (int j = 0; j < int(n_edge); j++) {
      // Central agent
      auto central_index = linear_index(i, j);

      // clang-format off
            std::vector<size_t> neighbours = {
                linear_index( i - 1, j ), 
                linear_index( i + 1, j ), 
                linear_index( i, j - 1 ),
                linear_index( i, j + 1 )
            };

      // clang-format on
      network.set_neighbours_and_weights(central_index, neighbours, weight);
    }
  }

  return network;
}

} // namespace Graph::DirectedNetworkGeneration

namespace Graph::UndirectedNetworkGeneration {
/* Constructs a new network on a square lattice of edge length n_edge (with
 * PBCs)*/
template <typename AgentType>
UndirectedNetwork<AgentType> generate_square_lattice(
    size_t n_edge,
    typename UndirectedNetwork<AgentType>::WeightT weight = 0.0) {
  using UndirectedNetworkT = UndirectedNetwork<AgentType>;
  using WeightT = typename UndirectedNetworkT::WeightT;
  auto n_agents = n_edge * n_edge;

  // Create an empty DirectedNetwork
  auto network = UndirectedNetworkT(n_agents);

  auto wrap_edge_index = [&](int k) {
    if (k >= int(n_edge)) {
      return k - int(n_edge);
    } else if (k < 0) {
      return int(n_edge) + k;
    } else {
      return k;
    }
  };

  auto linear_index = [&](int i, int j) {
    auto idx = wrap_edge_index(i) + n_edge * wrap_edge_index(j);
    return idx;
  };

  for (int i = 0; i < int(n_edge); i++) {
    // other edge
    for (int j = 0; j < int(n_edge); j++) {
      // Central agent
      auto central_index = linear_index(i, j);

      // clang-format off
            std::vector<size_t> neighbours = {
                linear_index( i - 1, j ), 
                linear_index( i, j - 1 ),
            };

      // clang-format on
      for (size_t j_idx = 0; j_idx < neighbours.size(); j_idx++) {
        network.push_back_neighbour_and_weight(central_index, neighbours[j_idx],
                                               weight);
      }
    }
  }

  return network;
}
} // namespace Graph::UndirectedNetworkGeneration