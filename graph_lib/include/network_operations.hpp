#include "network_base.hpp"
#include <cstddef>
#include <vector>

namespace Graph {

/*Algorithms adapted from
Algorithms, 4th Edition by Robert Sedgewick and Kevin Wayne.
Git repository: https://github.com/kevin-wayne/algs4
*/
template <typename WeightType = double> class NetworkOperations {
public:
  using WeightT = WeightType;

  NetworkOperations(const NetworkBase<WeightT> &network)
      : network(network), marked(std::vector<bool>(network.n_agents(), false)),
        count(0), edge_to_vertex(std::vector<size_t>(network.n_agents())) {}

  // Returns a path from a source s to a vertex v using DFS (if it exists)
  // If it does not exist, return nullopt
  // This will not give all the paths (or even the shortest path)
  std::optional<std::vector<size_t>> path_from_dfs(size_t s, size_t v) {
    std::vector<size_t> path{}; // Path from s to v
    reset_variables_counters(); // Reset marked and count
    // DFS starting from the source
    dfs(s);
    if (!path_exists_to_vertex(v)) {
      return std::nullopt; // the path does not exist
    }
    // If the path exists, return it
    // Start from v
    for (int x = v; x != s; x = edge_to_vertex[x]) {
      path.push_back(x);
    }
    path.push_back(s); // Finally, add the source
    return path;
  }

private:
  const NetworkBase<WeightT> &network; // UndirectedNetwork or DirectedNetwork
  std::vector<bool>
      marked{};   // Chronicles whether a vertex has been visited or not
  size_t count{}; // Number of vertices visited
  std::vector<size_t>
      edge_to_vertex{}; // Last vertex on known path, to a vertex
                        // (given by the index in this vector)

  // Depth first search from vertex v
  void dfs(size_t v) {
    count++; // Update the number of vertices visited
    marked[v] = true;
    for (size_t w : network.get_neighbours(v)) {
      if (!marked[w]) {
        edge_to_vertex[w] = v;
        dfs(w);
      }
    }
  }

  // Reset marked (that chronicles visited vertices) and count (number of
  // vertices visited)
  void reset_variables_counters() {
    count = 0;
    marked.resize(network.n_agents(), false);
  }

  // Returns false if a path to vertex v does not exist
  bool path_exists_to_vertex(size_t v) { return marked[v]; }
};
} // namespace Graph
