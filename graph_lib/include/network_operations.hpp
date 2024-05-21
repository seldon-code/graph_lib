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

  NetworkOperations(NetworkBase<WeightT> network)
      : network(network), marked(std::vector<bool>(network.n_agents(), false)),
        count(0) {}

private:
  NetworkBase<WeightT> network; // UndirectedNetwork or DirectedNetwork
  std::vector<bool>
      marked;   // Chronicles whether a vertex has been visited or not
  size_t count; // Number of vertices visited

  // Depth first search from vertex v
  void dfs(int v) {
    count++;
    marked[v] = true;
    for (int w : network.get_neighbours(v)) {
      if (!marked[w]) {
        dfs(w);
      }
    }
  }

  // Reset marked (that chronicles visited vertices) and count (number of
  // vertices visited)
  void reset_counters() {
    count = 0;
    marked.resize(network.n_agents(), false);
  }
};
} // namespace Graph
