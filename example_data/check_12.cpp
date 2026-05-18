#include <bits/stdc++.h>

using namespace std;

struct Link {
  int destination;
  int cost;
};

class NetworkSystem {
private:
  unordered_map<int, vector<Link>> connections;

public:
  void connectNodes(int a, int b, int c) {
    connections[a].push_back({b, c});
    connections[b].push_back({a, c});
  }

  void computeRoutes(int start, int totalNodes) {

    vector<int> minimumCost(totalNodes, INT_MAX);

    using P = pair<int, int>;

    priority_queue<P, vector<P>, greater<P>> heap;

    minimumCost[start] = 0;

    heap.push({0, start});

    while (!heap.empty()) {

      auto current = heap.top();
      heap.pop();

      int currentCost = current.first;
      int currentNode = current.second;

      if (currentCost > minimumCost[currentNode])
        continue;

      for (const auto &path : connections[currentNode]) {

        int nextNode = path.destination;
        int nextCost = path.cost;

        int updated = currentCost + nextCost;

        if (updated < minimumCost[nextNode]) {
          minimumCost[nextNode] = updated;
          heap.push({updated, nextNode});
        }
      }
    }

    cout << "Minimum route costs:\n";

    for (int i = 0; i < totalNodes; ++i) {
      cout << start << " => " << i << " : " << minimumCost[i] << "\n";
    }
  }
};

int main() {

  NetworkSystem system;

  system.connectNodes(0, 1, 2);
  system.connectNodes(0, 2, 4);
  system.connectNodes(1, 2, 1);
  system.connectNodes(1, 3, 7);
  system.connectNodes(2, 4, 3);
  system.connectNodes(3, 4, 1);

  system.computeRoutes(0, 5);

  return 0;
}