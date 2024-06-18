#include "network_base.hpp"
#include <climits>
#include <cstddef>
#include <optional>
#include <queue>
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
        edge_to_vertex(std::vector<size_t>(network.n_agents())),
        depth(std::vector<int>(network.n_agents(), INT_MAX)) {}

  // Returns a path from a source s to a vertex v using DFS (if it exists)
  // If it does not exist, returns nullopt
  // This will not give all the paths (or even the shortest path)
  std::optional<std::vector<size_t>> path_from_dfs(size_t s, size_t v) {
    std::vector<size_t> path{}; // Path from s to v
    reset_variables_counters(); // Reset marked and depth
    // DFS starting from the source
    dfs(s);
    // Get one path if it exists
    return path_to_vertex(s, v);
  }

  // Returns one (of possibly many) shortest paths from a source s to a vertex v
  // using BFS (if it exists) If a shortest path, within an optional depth
  // cutoff (inclusive) does not exist, returns nullopt This will not give all
  // the shortest paths
  std::optional<std::vector<size_t>>
  shortest_path_from_bfs(size_t s, size_t v,
                         std::optional<int> max_depth = std::nullopt) {
    std::vector<size_t> path{}; // One shortest path from s to v
    reset_variables_counters(); // Reset marked and depth
    // BFS starting from the source
    bfs(s, max_depth);
    // Get one shortest path if it exists
    return path_to_vertex(s, v);
  }

  // Get the depth of vertices or distances from the source
  // Using this function only makes sense if you have used BFS
  std::vector<int> get_depth_from_source() { return depth; }

private:
  const NetworkBase<WeightT> &network; // UndirectedNetwork or DirectedNetwork
  // Chronicles whether a vertex has been visited or not
  std::vector<bool> marked{};
  std::vector<int>
      depth{}; // Distance of the node from the source (given by BFS)
  std::vector<size_t>
      edge_to_vertex{}; // Last vertex on known path, to a vertex
                        // (given by the index in this vector)

  // Depth-first search from vertex v
  void dfs(size_t v) {
    marked[v] = true;
    for (size_t w : network.get_neighbours(v)) {
      if (!marked[w]) {
        edge_to_vertex[w] = v;
        dfs(w);
      }
    }
  }

  // Breadth-first search from a source node/vertex s
  void bfs(size_t s, std::optional<int> max_depth = std::nullopt) {
    std::queue<size_t> q; // To keep track of vertices to visit
    depth[s] = 0;         // Distance from the source
    // Mark the source and put it on the queue
    marked[s] = true;
    q.push(s);

    while (!q.empty()) {
      // Remove next vertex from the queue
      size_t v = q.front();
      q.pop();
      // Check to see if the maximum depth has been reached, break if it has
      // been reached
      if (max_depth.has_value()) {
        if (depth[v] > max_depth) {
          break; // Stop BFS if the maximum search depth has been reached
        }
      }

      // Go through the neighbours of v
      for (size_t w : network.get_neighbours(v)) {
        // Process every unmarked adjacent vertex
        // All unmarked vertices that are adjacent to v are added to the queue
        if (!marked[w]) {
          edge_to_vertex[w] = v;   // Save last edge on *a* shortest path
          marked[w] = true;        // Mark it because the path is known
          depth[w] = depth[v] + 1; // Update the distance from the source for w
          q.push(w);               // Add it to the queue
        }
      }
    }
  }

  // Reset marked (that chronicles visited vertices) and depth (distance of the
  // vertex from the source)
  void reset_variables_counters() {
    depth.clear();
    depth.resize(network.n_agents(), INT_MAX);
    marked.clear();
    marked.resize(network.n_agents(), false);
  }

  // Returns false if a path to vertex v does not exist
  bool path_exists_to_vertex(size_t v) { return marked[v]; }

  // Returns one path from s to v (shortest if BFS was used)
  std::optional<std::vector<size_t>> path_to_vertex(size_t s, size_t v) {
    if (!path_exists_to_vertex(v)) {
      return std::nullopt; // the path does not exist
    }
    // If the path exists, return it
    // Start from v
    std::vector<size_t> path{}; // Path from s to v
    for (size_t x = v; x != s; x = edge_to_vertex[x]) {
      path.push_back(x);
    }
    path.push_back(s); // Finally, add the source
    return path;
  }
};
} // namespace Graph
