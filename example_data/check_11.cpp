#include <iostream>
#include <queue>
#include <vector>

using namespace std;

class Graph {
  int vertices;
  vector<vector<pair<int, int>>> adj;

public:
  Graph(int v) {
    vertices = v;
    adj.resize(v);
  }

  void addEdge(int u, int v, int w) {
    adj[u].push_back({v, w});
    adj[v].push_back({u, w});
  }

  vector<int> shortestPath(int src) {
    priority_queue<pair<int, int>, vector<pair<int, int>>,
                   greater<pair<int, int>>>
        pq;

    vector<int> dist(vertices, 1e9);

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
      int node = pq.top().second;
      int d = pq.top().first;
      pq.pop();

      for (auto edge : adj[node]) {
        int next = edge.first;
        int weight = edge.second;

        if (d + weight < dist[next]) {
          dist[next] = d + weight;
          pq.push({dist[next], next});
        }
      }
    }

    return dist;
  }
};

int main() {
  int n = 5;

  Graph g(n);

  g.addEdge(0, 1, 2);
  g.addEdge(0, 2, 4);
  g.addEdge(1, 2, 1);
  g.addEdge(1, 3, 7);
  g.addEdge(2, 4, 3);
  g.addEdge(3, 4, 1);

  vector<int> ans = g.shortestPath(0);

  cout << "Shortest distances:\n";

  for (int i = 0; i < ans.size(); i++) {
    cout << "0 -> " << i << " = " << ans[i] << endl;
  }

  return 0;
}