#pragma once
#include "connectivity.hpp"
#include <algorithm>
#include <cstddef>
#include <fmt/format.h>
#include <numeric>
#include <optional>
#include <span>
#include <stdexcept>
#include <vector>

namespace Graph {

/*
    An abstract base class for undirected and directed networks.
*/
template <typename WeightType = double> class NetworkBase {
public:
  using WeightT = WeightType;

protected:
  std::vector<std::vector<size_t>>
      neighbour_list{}; // Neighbour list for the connections
  std::vector<std::vector<WeightT>>
      weight_list{}; // List for the interaction weights of each connections

public:
  NetworkBase() = default;

  NetworkBase(size_t n_agents)
      : neighbour_list(
            std::vector<std::vector<size_t>>(n_agents, std::vector<size_t>{})),
        weight_list(std::vector<std::vector<WeightT>>(n_agents,
                                                      std::vector<WeightT>{})) {
  }

  NetworkBase(std::vector<std::vector<size_t>> &&neighbour_list,
              std::vector<std::vector<WeightT>> &&weight_list)
      : neighbour_list(neighbour_list), weight_list(weight_list) {}

  virtual ~NetworkBase() = default;

  /*
  Gives the total number of nodes in the network
  */
  [[nodiscard]] std::size_t n_agents() const { return neighbour_list.size(); }

  /*
  Gives the number of edges connected to agent_idx
  If agent_idx is nullopt, gives the total number of edges
  The total number of edges is different for the DirectedNetwork and
  UndirectedNetworks
  */
  virtual std::size_t
  n_edges(std::optional<std::size_t> agent_idx = std::nullopt) const = 0;

  /*
  Sets the weight for agent_idx, for an existing neighbour index
  */
  virtual void set_edge_weight(std::size_t agent_idx,
                               std::size_t index_neighbour, WeightT weight) = 0;

  /*
  Adds an edge between agent_idx_i and agent_idx_j with weight w
  */
  virtual void push_back_neighbour_and_weight(size_t agent_idx_i,
                                              size_t agent_idx_j,
                                              WeightT w) = 0;

  /*
  Sorts the neighbours by index and removes doubly counted edges by summing the
  weights
  */
  virtual void remove_double_counting() = 0;

  /*
  Gives the strongly connected components in the graph
  @TODO: implement as visitor on the graph?
  */
  [[nodiscard]] std::vector<std::vector<size_t>>
  strongly_connected_components() const {
    // Now that we have the neighbour list (or adjacency list)
    // Run Tarjan's algorithm for strongly connected components
    auto tarjan_scc = TarjanConnectivityAlgo(neighbour_list);
    return tarjan_scc.scc_list;
  }

  /*
  Gives a view into the neighbour indices connected to agent_idx
  */
  [[nodiscard]] std::span<const size_t>
  get_neighbours(std::size_t agent_idx) const {
    return std::span(neighbour_list[agent_idx].data(),
                     neighbour_list[agent_idx].size());
  }

  /*
  Gives a view into the edge weights corresponding to edges connected to
  agent_idx
  */
  [[nodiscard]] std::span<const WeightT>
  get_weights(std::size_t agent_idx) const {
    return std::span<const WeightT>(weight_list[agent_idx].data(),
                                    weight_list[agent_idx].size());
  }

  /*
  Gets the weight for agent_idx, for a neighbour index
  */
  const WeightT get_edge_weight(std::size_t agent_idx,
                                std::size_t index_neighbour) const {
    return weight_list[agent_idx][index_neighbour];
  }

  /*
  Checks if a connection exists between two agents i_idx and j_idx
  */
  bool connection_exists(size_t i_idx, size_t j_idx) const {
    auto i_neighbours = get_neighbours(i_idx);
    if (std::find(i_neighbours.begin(), i_neighbours.end(), j_idx) ==
        std::end(i_neighbours)) {
      return false;
    } else {
      return true;
    }
  }

  /*
  Clears the network
  */
  void clear() {
    for (auto &w : weight_list)
      w.clear();

    for (auto &n : neighbour_list)
      n.clear();
  }
};

} // namespace Graph