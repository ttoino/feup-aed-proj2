#include <algorithm>
#include <filesystem>
#include <fstream>

#include "../includes/Utils.h"
#include "../includes/constants.h"
#include "../includes/graph.h"

void Graph::addFootEdges() {
    for (auto i{nodes.begin()}, end{nodes.end()}; i != end; ++i) {
        for (auto j{i}; j != end; ++j) {
            if (i->first == j->first)
                continue;

            double distance = haversine(
                i->second.stop.getLatitude(), i->second.stop.getLongitude(),
                j->second.stop.getLatitude(), j->second.stop.getLongitude());

            addEdge(i->second, j->second, BY_FOOT_LINE, distance);
            addEdge(j->second, i->second, BY_FOOT_LINE, distance);
        }
    }
}

void Graph::setStart(const double &lat, const double &lon) {
    nodes[START_NODE] = Node(Stop(START_NODE_NAME, START_NODE, "", lat, lon));

    for (auto i{nodes.begin()}, end{nodes.end()}; i != end; ++i) {
        double distance = haversine(i->second.stop.getLatitude(),
                                    i->second.stop.getLongitude(), lat, lon);

        addEdge(START_NODE, i->first, BY_FOOT_LINE, distance);
    }
}

void Graph::setEnd(const double &lat, const double &lon) {
    nodes[END_NODE] = Node(Stop(END_NODE_NAME, END_NODE, "", lat, lon));

    for (auto i{nodes.begin()}, end{nodes.end()}; i != end; ++i) {
        double distance = haversine(i->second.stop.getLatitude(),
                                    i->second.stop.getLongitude(), lat, lon);

        if (i->second.adj.back().dest == END_NODE)
            i->second.adj.pop_back();

        addEdge(i->first, END_NODE, BY_FOOT_LINE, distance);
    }
}

// Add edge from source to destination with a certain weight
void Graph::addEdge(Node &src, Node &dest, const std::string &code,
                    const double &distance) {
    src.adj.push_back({dest.stop.getCode(), distance, code});
}

// Add edge from source to destination with a certain weight
void Graph::addEdge(const std::string &src, const std::string &dest,
                    const std::string &code, const double &distance) {
    addEdge(nodes[src], nodes[dest], code, distance);
}

void Graph::readStops() {
    std::ifstream stopsFile{STOPS_FILE_PATH};

    if (stopsFile.fail())
        return;

    std::string line;

    getline(stopsFile, line);

    while (!stopsFile.eof()) {
        std::vector<std::string> parsedLine;
        getline(stopsFile, line);
        if (line.empty())
            continue;

        parsedLine = split(line, ',');

        Stop stop = Stop(parsedLine.at(1), parsedLine.at(0), parsedLine.at(2),
                         stod(parsedLine.at(3)), stod(parsedLine.at(4)));
        Node node{stop};
        nodes.emplace(stop.getCode(), node);
    }

    stopsFile.close();
}

void Graph::readLines() {
    for (auto &file : std::filesystem::directory_iterator{LINE_FOLDER_PATH}) {
        std::ifstream f{file.path().string()};

        std::string lineCode = split(file.path().generic_string(), '_').at(1);

        if (f.fail())
            continue;

        std::string stops;
        getline(f, stops);
        int nstops = std::stoi(stops);

        std::string stop;
        Node *current = nullptr;
        Node *last = nullptr;

        for (int i{0}; i < nstops; ++i) {
            getline(f, stop);
            current = &getNode(stop);

            if (i > 0) {
                double dist = haversine(
                    current->stop.getLatitude(), current->stop.getLongitude(),
                    last->stop.getLatitude(), last->stop.getLongitude());

                addEdge(*last, *current, lineCode, dist);
            }

            last = current;
        }

        f.close();
    }
}

void Graph::populate() {
    readStops();
    readLines();
    addFootEdges();
}

void Graph::dijkstra(const std::string &src, const std::string &dest,
                     const filter &f) {
    MinHeap<std::string, double> q(nodes.size(), "");

    for (auto i{nodes.begin()}, end{nodes.end()}; i != end; ++i) {
        i->second.dist = INF;
        q.insert(i->first, INF);
        i->second.visited = false;
    }

    nodes[src].dist = 0;
    q.decreaseKey(src, 0);
    nodes[src].pred = src;

    while (q.getSize() > 0) {
        std::string uc = q.removeMin();
        Node &u = getNode(uc);
        u.visited = true;

        if (uc == dest)
            return;

        for (auto e : u.adj) {
            std::string vc = e.dest;
            Node &v = getNode(vc);
            double w = u.dist + e.distance;

            if (!f(u, v, e))
                continue;

            if (!v.visited && w < v.dist) {
                v.line = e.code;
                v.dist = w;
                q.decreaseKey(vc, v.dist);
                v.pred = uc;
            }
        }
    }
}

void Graph::dijkstraCost(const std::string &src, const std::string &dest,
                         const filter &f) {
    MinHeap<std::string, double> q(nodes.size(), "");

    for (auto i{nodes.begin()}, end{nodes.end()}; i != end; ++i) {
        i->second.dist = INF;
        q.insert(i->first, INF);
        i->second.visited = false;
    }

    nodes[src].dist = 0;
    q.decreaseKey(src, 0);
    nodes[src].pred = src;

    while (q.getSize() > 0) {
        std::string uc = q.removeMin();
        Node &u = getNode(uc);
        u.visited = true;
        for (auto e : u.adj) {
            std::string vc = e.dest;
            Node &v = getNode(vc);

            if (!f(u, v, e)) // descriptive variables :D
                continue;

            double w =
                // u.dist + e.distance +
                (v.stop.getZone() != u.stop.getZone()) * 10;

            if (!v.visited && w < v.dist) {
                v.line = e.code;
                v.dist = w;
                q.decreaseKey(vc, v.dist);
                v.pred = uc;
            }
        }
    }
}

void Graph::dijkstraLines(const std::string &src, const std::string &dest,
                          const filter &f) {
    MinHeap<std::string, double> q(nodes.size(), "");

    for (auto i{nodes.begin()}, end{nodes.end()}; i != end; ++i) {
        i->second.dist = INF;
        q.insert(i->first, INF);
        i->second.visited = false;
    }

    nodes[src].dist = 0;
    q.decreaseKey(src, 0);
    nodes[src].pred = src;

    while (q.getSize() > 0) {
        std::string uc = q.removeMin();
        Node &u = getNode(uc);
        u.visited = true;

        for (auto e : u.adj) {

            std::string vc = e.dest;
            Node &v = getNode(vc);

            if (!f(u, v, e))
                continue;

            double w = u.dist + e.distance + (v.line != u.line) * 1000;

            if (!v.visited && w < v.dist) {
                v.line = e.code;
                v.dist = w;
                q.decreaseKey(vc, v.dist);
                v.pred = uc;
            }
        }
    }
}

std::list<Node> Graph::dijkstraPath(const std::string &src,
                                    const std::string &dest, const filter &f) {
    dijkstra(src, dest, f);

    std::list<Node> path{};
    if (nodes[dest].dist == INF)
        return path;

    path.push_back(getNode(dest));
    std::string v = dest;
    while (v != src) {
        v = nodes[v].pred;
        path.push_front(getNode(v));
    }

    return path;
}

std::list<Node> Graph::dijkstraCostPath(const std::string &src,
                                        const std::string &dest,
                                        const filter &f) {
    dijkstraCost(src, dest, f);

    std::list<Node> path{};
    if (nodes[dest].dist == INF)
        return path;

    path.push_back(getNode(dest));
    std::string v = dest;
    while (v != src) {
        v = nodes[v].pred;
        path.push_front(getNode(v));
    }

    return path;
}

std::list<Node> Graph::dijkstraLinesPath(const std::string &src,
                                         const std::string &dest,
                                         const filter &f) {
    dijkstraLines(src, dest, f);

    std::list<Node> path{};
    if (nodes[dest].dist == INF)
        return path;

    path.push_back(getNode(dest));
    std::string v = dest;
    while (v != src) {
        v = nodes[v].pred;
        path.push_front(getNode(v));
    }

    return path;
}

void Graph::bfs(const std::string &src, const std::string &dest,
                const filter &f) {
    for (auto i{nodes.begin()}, end{nodes.end()}; i != end; ++i)
        i->second.visited = false;

    std::queue<std::string> q; // queue of unvisited nodes
    q.push(src);
    nodes[src].visited = true;
    while (!q.empty()) { // while there are still unvisited nodes
        std::string u = q.front();
        q.pop();

        if (u == dest)
            return;

        for (auto e : nodes[u].adj) {
            std::string w = e.dest;

            if (!f(nodes[u], nodes[w], e))
                continue;

            if (!nodes[w].visited) {
                q.push(w);
                nodes[w].line = e.code;
                nodes[w].visited = true;
                nodes[w].pred = u;
            }
        }
    }
}

auto Graph::bfsPath(const std::string &src, const std::string &dest,
                    const filter &f) -> std::list<Node> {
    bfs(src, dest, f);

    std::list<Node> path{};

    path.push_front(getNode(dest));
    std::string v = dest;
    while (v != src) {
        v = nodes[v].pred;
        path.push_front(getNode(v));
    }

    return path;
}