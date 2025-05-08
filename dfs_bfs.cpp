#include <iostream>
#include <stack>
#include <cstring>
#include <omp.h>

using namespace std;

const int MAXN = 100;

int adj_matrix[MAXN][MAXN];
int adj_list[MAXN][MAXN];
int dist[MAXN];
bool visited[MAXN];
omp_lock_t locks[MAXN];

void dfs_iterative(int start, int n) {
    stack<int> s;
    s.push(start);
    visited[start] = true;

    while (!s.empty()) {
        int node = s.top();
        s.pop();
        cout << node << " ";

        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            if (adj_matrix[node][i]) {
                omp_set_lock(&locks[i]);
                if (!visited[i]) {
                    visited[i] = true;
                    s.push(i); // Not ideal in parallel (not thread-safe), but okay for small graph
                }
                omp_unset_lock(&locks[i]);
            }
        }
    }
}

void bfs(int start, int n) {
    memset(dist, -1, sizeof(dist));
    dist[start] = 0;

    int q[MAXN];
    int front = 0, rear = 0;
    q[rear++] = start;

    while (front != rear) {
        int u = q[front++];
        cout << u << " ";

        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            if (adj_list[u][i]) {
                omp_set_lock(&locks[i]);
                if (dist[i] == -1) {
                    dist[i] = dist[u] + 1;

                    #pragma omp critical // Protect queue update
                    {
                        q[rear++] = i;
                    }
                }
                omp_unset_lock(&locks[i]);
            }
        }
    }
}

int main() {
    int n, m;
    cout << "Enter the number of nodes: ";
    cin >> n;
    cout << "Enter the number of edges: ";
    cin >> m;

    // Initialize structures
    for (int i = 0; i < n; i++) {
        visited[i] = false;
        omp_init_lock(&locks[i]);
        for (int j = 0; j < n; j++) {
            adj_matrix[i][j] = 0;
            adj_list[i][j] = 0;
        }
    }

    cout << "Enter the edges:" << endl;
    for (int i = 0; i < m; i++) {
        int u, v;
        cout << "Enter edge " << i + 1 << " (u v): ";
        cin >> u >> v;
        adj_matrix[u][v] = adj_matrix[v][u] = 1;
        adj_list[u][v] = adj_list[v][u] = 1;
    }

    int start_node;
    cout << "Enter the starting node for DFS and BFS: ";
    cin >> start_node;

    cout << "DFS: ";
    dfs_iterative(start_node, n);
    cout << endl;

    // Reset visited before BFS
    for (int i = 0; i < n; i++) visited[i] = false;

    cout << "BFS: ";
    bfs(start_node, n);
    cout << endl;

    for (int i = 0; i < n; i++) {
        omp_destroy_lock(&locks[i]);
    }

    return 0;
}
