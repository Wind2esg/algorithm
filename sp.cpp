#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <stack>
#include <climits>
#include <vector>
#include <algorithm>

#include "sp.h"

using namespace std;

// We map the point number of source to that of graph in the code by -1
// That is point 1 in souce file, its number in the graph in the code is 0
bool PrepareGraph(string sourcefile, Graph &graph)
{
    ifstream input_file(sourcefile);

    if(input_file.is_open())
    {
        cout << sourcefile + " opened" << endl;
    }
    else
    {
        cout<< "fail to open file, halt";
        return false;
    }

    char data[15];
    int vertices;
    int edges;

    int tail;
    int head;
    int cost;
    multimap<int, EdgeNoTail> edgeset;

    input_file >> vertices >> edges;

    int i = 0;

    // indexd by tail
    while(!input_file.eof())
    {
        input_file >> tail >> head >> cost;
        EdgeNoTail edge(head - 1, cost);
        edgeset.insert(make_pair(tail - 1, edge));
        i++;
    }

    input_file.close();
    
    cout << "vertices " << vertices << endl;
    cout << "edges " << edges << endl;
    cout << sourcefile + " closed" << endl;
    
    Graph result(vertices, edges, edgeset);
    graph = result;

    return true;
}

// Dijkstra's Algorithm
// Dijkstra's algorithm solves shortestpath problem without any negative edge cost. 
// We start the search from the source point s. S is a collection that we track all searched points.
// D[v] is used to tracking so far we have searched the shortest path from s-v.
// This is a process that we search points one by one to form the final shortest path route.
// Everytime, we explorer one point u, D[v] = min{D[u] + cost, D[v]}, update the searched result depends on the searched shortest path so far pass u or not.
// D[v] is SEARCHED shortest path, not the FINAL answer. 
// However the graph do not have any negative cost edge, we can sure the smallest D[] is final a shortest path.
// For example, D[x] is the smallest. D[x] is the shortest path from s-x. Because we can't find any other point y to form a path s-y-x less than s-x.
// D[x] < D[y] => D[x] < D[y] + cost
// The key is here. How to choose the next point to search?
// From x. Because the path from s-x is a certain path in the final shortest path route.
// After we searched x, put x into S. There might be a collection T record unsearched points.
// Here I do a trick to track unsearched points using S and D.
vector<int> Dijkstra(Graph graph, int real_s)
{   
    int s = real_s - 1;

    // init 
    vector<int> D;
    map<int, int> S;
    for(int i = 0; i < graph.vertices; i++)
    {
        D.push_back(INT_MAX);
    }

    D[s] = 0;
    int u;
    
    // iteration to search the graph by releasing point
    for(int i = 0; i < graph.vertices; i++)
    {
        for(map<int, int>::iterator S_it = S.begin(); S_it != S.end(); S_it++)
        {
            D[S_it->first] = INT_MAX;
        }

        vector<int>::iterator D_min = min_element(D.begin(), D.end());
        u = distance(D.begin(), D_min);
        S.insert(pair<int, int>(u, D[u]));
        for(map<int, int>::iterator S_it = S.begin(); S_it != S.end(); S_it++)
        {
            D[S_it->first] = S_it->second;
        }

        pair<multimap<int, EdgeNoTail>::const_iterator, multimap<int, EdgeNoTail>::const_iterator> range;
        range = graph.edgeset.equal_range(u);
        for(multimap<int, EdgeNoTail>::const_iterator edgeset_it = range.first; edgeset_it != range.second; edgeset_it++)
        {
            if(D[edgeset_it->second.head] > D[u] + edgeset_it->second.cost)
            {
                D[edgeset_it->second.head] = D[u] + edgeset_it->second.cost;
            }
        }
    }
    
    return D;
}

// Bellman Ford's Algorithm
// Dijkstra is similar but simple to Bellman Ford. Because we can sure one shortest path after a searching, we can use a D[] to track the current search result. It can be treated as releasing point.
// But in graph with negative path or negative cycle, we cant ensure any path.
// As a reslut, we have to search all points everytime and reform the currently searched shortest path route, namely release one edge.
// We used A to track the last search result and record current search result
// Two points: 1 early halt 2 negative
// What is the mark of job done? For every point, we can't update any shortest path.
// Proof, we do one more iteration. A[i, v] = min{A[i - 1, v], min{A[i -1, w] + cost}}. A[i - 1, v] = min{A[i - 2, v], min{A[i - 2, w] + cost}}
// A[i - 2, v] = A[i - 1, v] Thus, A[i, v] = A[i - 1]. So we can halt early
// The possible longest shortest path has n -1 (n is the number of edges) edges if no negative cycle exists that is we searched all edges. Similar to early halt problem, if we get updated in the n iteration, there should be a negative cycle in the graph. 
// This update should occurs in one negative edge of the negative cycle 
bool BellmanFord(Graph graph, int real_s, vector<int> &result)
{
    int s = real_s - 1;
    
    // init
    vector<vector<int>> A;
    vector<int> a;
    for(int i = 0; i < graph.vertices; i++)
    {
        a.push_back(INT_MAX);
    }
    A.push_back(a);
    A.push_back(a);
    A[0][s] = 0;

    int u;
    bool early_halt;
    stack<int> new_path;

    // iteration to search the graph by releasing edge
    for(int i = 0; i < graph.edges; i++)
    {
        cout << "edge " << i << endl;

        early_halt = true;
        
        for(int v = 0; v < graph.vertices; v++)
        {
            if(A[i % 2][v] < A[(i + 1) % 2][v])
            {
                new_path.push(v);
            }
            else
            {
                A[i % 2][v] = A[(i + 1) % 2][v];
            }
        }
        pair<multimap<int, EdgeNoTail>::const_iterator, multimap<int, EdgeNoTail>::const_iterator> range;
        
        while(!new_path.empty())
        {
            u = new_path.top();
            new_path.pop();
            range = graph.edgeset.equal_range(u);
            for(multimap<int, EdgeNoTail>::const_iterator edgeset_it = range.first; edgeset_it != range.second; edgeset_it++)
            {
                A[(i + 1) % 2][edgeset_it->second.head] = min(A[(i + 1) % 2][edgeset_it->second.head], min(A[i % 2][edgeset_it->second.head], A[i % 2][u] + edgeset_it->second.cost));
                early_halt = false;
            }
        }
        
        if(early_halt){
            cout << "early halt" << endl;
            break;
        }
    }

    result = A[(graph.edges - 1 ) % 2];

    // negative cycle check
    int i = graph.edges;
    for(vector<int>::iterator A_it = A[i % 2].begin(); A_it != A[i % 2].end(); A_it++)
    {
        if(*A_it != INT_MAX)
        {
            u = distance(A[i %  2].begin(), A_it);
            pair<multimap<int, EdgeNoTail>::const_iterator, multimap<int, EdgeNoTail>::const_iterator> range;
            range = graph.edgeset.equal_range(u);
            for(multimap<int, EdgeNoTail>::const_iterator edgeset_it = range.first; edgeset_it != range.second; edgeset_it++)
            {
                if(A[(i + 1) % 2][edgeset_it->second.head] > min(A[i % 2][edgeset_it->second.head], A[i % 2][u] + edgeset_it->second.cost))
                {
                    return false;
                }
            }
        }
    }
    result = A[(graph.edges - 1 ) % 2];
    return true;
}

// Floyd-Warshall Algorithm
// Incomming...