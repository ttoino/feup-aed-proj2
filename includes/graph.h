#ifndef AED_PROJ2_2021_GRAPH_H
#define AED_PROJ2_2021_GRAPH_H

class Graph;
struct Edge;
struct Node;

#include "Stop.h"
#include "filters.h"
#include "minHeap.h"

#include <deque>
#include <iostream>
#include <limits>
#include <list>
#include <queue>
#include <vector>

#define INF (std::numeric_limits<double>::max() / 2)

/**
 * @brief Represents an edge connecting two nodes.
 */
struct Edge {
    /**
     * @brief The destination node.
     */
    std::string dest;
    /**
     * @brief The distance between the two nodes.
     */
    double distance;
    /**
     * @brief The line code associated with this edge.
     */
    std::string code;
};

/**
 * @brief Represents a node in a Graph.
 */
struct Node {
    /**
     * @brief The outgoing edges connecting this node to others.
     */
    std::deque<Edge> adj{};
    /**
     * @brief Whether this node has been visited.
     *
     * @note Set after running an algorithm.
     */
    bool visited{false};
    /**
     * @brief The distance between this node and its predecessor.
     *
     * @note Set after running an algorithm.
     */
    double dist{};
    /**
     * @brief This node's predecessor.
     *
     * @note Set after running an algorithm.
     */
    std::string pred{};
    /**
     * @brief This node's line.
     *
     * @note Set after running an algorithm.
     *
     */
    std::string line{};
    /**
     * @brief The stop associated with this node.
     */
    Stop stop{};

    /**
     * @brief Creates a new node for the specified stop.
     *
     * @param stop The stop
     */
    Node(Stop stop) : stop(stop) {}
    /**
     * @brief Default constructor.
     */
    Node() {}
};

/**
 * @brief A directed multigraph representing a public transit network.
 */
class Graph {
    /**
     * @brief This graph's nodes.
     *
     * The keys are each node's stop code and the values are the nodes
     * themselves.
     */
    std::unordered_map<std::string, Node> nodes;

    /**
     * @brief Reads the stops (nodes) from file.
     */
    void readStops();
    /**
     * @brief Reads the lines (edges) from file.
     */
    void readLines();
    /**
     * @brief Adds all by foot edges between every node.
     */
    void addFootEdges();

public:
    /**
     * @brief Creates an empty graph.
     */
    Graph(){};

    /**
     * @brief Adds a new edge between two nodes with the specified distance and
     *        code.
     *
     * @param src The code of the source node.
     * @param dest The code of the destination node.
     * @param code The line code.
     * @param distance The distance between the nodes.
     */
    void addEdge(const std::string &src, const std::string &dest,
                 const std::string &code, const double &distance = 1.0);
    /**
     * @brief Adds a new edge between two nodes with the specified distance and
     *        code.
     *
     * @param src The source node.
     * @param dest The destination node.
     * @param code The line code.
     * @param distance The distance between the nodes.
     */
    void addEdge(Node &src, Node &dest, const std::string &code,
                 const double &distance = 1.0);

    /**
     * @return This graph's nodes.
     */
    std::unordered_map<std::string, Node> &getNodes() { return nodes; };
    /**
     * @brief Get the node with the specified code.
     *
     * @param id The stop code.
     * @return The node.
     */
    Node &getNode(const std::string &id) { return nodes.at(id); };

    /**
     * @brief Creates a node with the code START_NODE, the name
     *        START_NODE_NAME and the specified position and calculates all by
     *        foot edges from this node.
     *
     * @param lat The latitude of the start node.
     * @param lon The longitude of the start node.
     */
    void setStart(const double &lat, const double &lon);
    /**
     * @brief Creates a node with the code END_NODE, the name
     *        END_NODE_NAME and the specified position and calculates all by
     *        foot edges to this node.
     *
     * @param lat The latitude of the end node.
     * @param lon The longitude of the end node.
     */
    void setEnd(const double &lat, const double &lon);

    /**
     * @brief Populates this graph with info from files and by foot edges.
     */
    void populate();

    /**
     * @brief Applies the regular dijkstra algorithm. [O(|E| log(|V|))]
     *
     * @param src The code of the source node.
     * @param dest The code of the destination node.
     * @param f The filter to use in the creation of the path.
     */
    void dijkstra(const std::string &src, const std::string &dest,
                  const filter &f);

    /**
     * @brief Calls dijkstra() to calculate and return the shortest path.
     *
     * @param src The code of the source node.
     * @param dest The code of the destination node.
     * @param f The filter to use in the creation of the path.
     * @return A list of nodes that represent the optimal path given the imposed
     * restrictions.
     */
    std::list<Node> dijkstraPath(const std::string &src,
                                 const std::string &dest, const filter &f);

    /**
     * @brief Applies a variant of the dijkstra algorithm that inflates the
     * distance to nodes of different zones. [O(|E| log(|V|))]
     *
     * @param src The code of the source node.
     * @param dest The code of the destination node.
     * @param f The filter to use in the creation of the path.
     */
    void dijkstraCost(const std::string &src, const std::string &dest,
                      const filter &f);

    /**
     * @brief Calls dijkstraCost() to calculate and return the cheapest path.
     *
     * @param src The code of the source node.
     * @param dest The code of the destination node.
     * @param f The filter to use in the creation of the path.
     * @return A list of nodes that represent the optimal path given the imposed
     * restrictions.
     */
    std::list<Node> dijkstraCostPath(const std::string &src,
                                     const std::string &dest, const filter &f);

    /**
     * @brief Applies a variant of the dijkstra algorithm that inflates the
     * distance to nodes of different lines. [O(|E| log(|V|))]
     *
     * @param src The code of the source node.
     * @param dest The code of the destination node.
     * @param f The filter to use in the creation of the path.
     */
    void dijkstraLines(const std::string &src, const std::string &dest,
                       const filter &f);

    /**
     * @brief Calls dijkstraLines() to calculate and return the path with the
     * least amount of line changes.
     *
     * @param src The code of the source node.
     * @param dest The code of the destination node.
     * @param f The filter to use in the creation of the path.
     * @return A list of nodes that represent the optimal path given the imposed
     * restrictions.
     */
    std::list<Node> dijkstraLinesPath(const std::string &src,
                                      const std::string &dest, const filter &f);

    /**
     * @brief Applies the bfs (breadth-first search) algorithm. [O(|V| + |E|)]
     *
     * @param src The code of the source node.
     * @param dest The code of the destination node.
     * @param f The filter to use in the creation of the path.
     */
    void bfs(const std::string &src, const std::string &dest, const filter &f);

    /**
     * @brief Calls bfs() to calculate and return the path with the least amount
     * of stops.
     *
     * @param src The code of the source node.
     * @param dest The code of the destination node.
     * @param f The filter to use in the creation of the path.
     * @return A list of nodes that represent the optimal path given the imposed
     * restrictions.
     */
    std::list<Node> bfsPath(const std::string &src, const std::string &dest,
                            const filter &f);
};

#endif
