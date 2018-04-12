#include <map>
#include <string>
#include <vector>
// Prepare graph from data source txt file

struct EdgeNoTail{
    int head;
    int cost;

    EdgeNoTail(int h, int c)
    {
        head = h;
        cost = c;
    }
};

struct Graph{
    int vertices;
    int edges;
    std::multimap<int, EdgeNoTail> edgeset;

    Graph(int v, int e, std::multimap<int, EdgeNoTail> set)
    {
        vertices = v;
        edges = e;
        edgeset = set;
    }
    Graph()
    {}
};

bool PrepareGraph(std::string sourcefile, Graph &graph);

std::vector<int> Dijkstra(Graph graph, int real_s);

bool BellmanFord(Graph graph, int real_s, std::vector<int> &result);