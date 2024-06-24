#include "fmt/core.h"
#include "fmt/ranges.h"
#include "network_base.hpp"
#include <climits>
#include <cstddef>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <optional>
#include <queue>
#include <vector>

namespace Graph {

// Breadth-first search from a source node, up to an optional user-defined
// max_depth. Requires a vector for the depth level or distance from
// the source (initialized to MAX_INT at first), and also a vector of vectors
// for the parent nodes of each node.
template <typename WeightType = double>
void bfs(const NetworkBase<WeightType> &network,
         std::vector<std::vector<int>> &parent, std::vector<int> &depth_level,
         size_t source, std::optional<int> max_depth) {
  std::queue<size_t> q; // To keep track of nodes to visit
  // Insert the source node in the queue
  q.push(source);
  // Update its parent to be -1 (characteristic of the source) and its
  // depth_level
  parent[source] = {-1};
  depth_level[source] = 0;

  // Keep traversing until the queue is empty
  while (!q.empty()) {
    // Remove next vertex from the queue
    size_t v = q.front();
    q.pop();
    // Check to see if the maximum depth has been reached, break if it has
    // been reached
    if (max_depth.has_value()) {
      if (depth_level[v] > max_depth.value() - 1) {
        break; // Stop BFS if the maximum search depth has been reached
      }
    }

    // Go through the neighbours of v
    for (size_t w : network.get_neighbours(v)) {
      // If a shorter distance has been found, remove all the previous parents
      // and update the new parent to be v
      if (depth_level[w] > depth_level[v] + 1) {
        depth_level[w] = depth_level[v] + 1;
        parent[w].clear();
        parent[w].push_back(v);
        q.push(w); // Add the current node w to the queue
      }
      // Otherwise, another candidate parent (i.e. node v) has been found for
      // the shortest path
      else if (depth_level[w] == depth_level[v] + 1) {
        parent[w].push_back(v);
      }
    }
  }
}

// Given all the parents for each node, *all* paths are reconstructed by
// iterating up the parents recursively
inline void reconstruct_paths(const std::vector<std::vector<int>> &parent,
                              std::vector<std::vector<int>> &result_paths,
                              std::vector<int> &path, const int v) {
  // When you have reached the source parent, add the path and return
  if (v == -1) {
    result_paths.push_back(path);
    return;
  }

  // Loop through all the parents for the given vertex
  for (auto par : parent[v]) {

    // Add the current vertex to the current path
    path.push_back(v);

    // Recursively iterate through parents
    reconstruct_paths(parent, result_paths, path, par);

    // Remove the current vertex v
    path.pop_back();
  }
}

// For this function to work, you must have already performed BFS using a source
// node. This will create the depth_level vector containing the
// distances/depth_level from the source This tells you whether a path from the
// source to v exists or not
bool inline path_exists_to_destination(const std::vector<int> &depth_level,
                                       const int v) {

  return !(depth_level[v] == INT_MAX);
}

} // namespace Graph
