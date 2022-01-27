#include <algorithm>
#include <filesystem>
#include <fstream>

#include "../includes/Utils.h"
#include "../includes/constants.h"
#include "../includes/graph.h"

// Constructor: nr nodes and direction (default: undirected)
Graph::Graph(int num, bool dir) : n(num), hasDir(dir), nodes(num) {
    readStops();
    readLines();
}

// Add edge from source to destination with a certain weight
void Graph::addEdge(int &src, int &dest, std::string &code, double distance) {
    if (src < 1 || src > n || dest < 1 || dest > n)
        return;
    nodes[src].adj.push_back({src, dest, distance, code});
    if (!hasDir)
        nodes[dest].adj.push_back({src, dest, distance, code});
}

void Graph::readStops() {
    std::ifstream stopsFile{STOPS_FILE_PATH};

    if (stopsFile.fail())
        return;

    std::string line;
    std::vector<Node> nods;

    getline(stopsFile, line);

    while (!stopsFile.eof()) {
        std::vector<std::string> parsedLine;
        getline(stopsFile, line);
        if (line.empty())
            continue;

        parsedLine = split(line, ',');

        Stop stop = Stop(parsedLine.at(1), parsedLine.at(0), parsedLine.at(2),
                         stod(parsedLine.at(4)), stod(parsedLine.at(3)));
        Node node{std::list<Edge>(), false, 0, -1, stop};
        nods.push_back(node);
    }

    stopsFile.close();

    this->nodes = nods;
}

void Graph::readLines() {
    for (auto &file : std::filesystem::directory_iterator{LINE_FOLDER_PATH}) {
        std::ifstream f{file.path()};

        std::string lineCode = split(file.path().generic_string(), '_').at(1);

        if (f.fail())
            continue;
        std::string line;

        getline(f, line);
        if (line == "0") {
            f.close();
            continue;
        }

        getline(f, line);
        if (line.empty())
            continue;
        std::string stopCode1 = line;
        int src = findNodeIdx(stopCode1);
        Node n1 = nodes.at(src);

        while (f.good()) {
            getline(f, line);
            if (line.empty())
                continue;
            std::string stopCode2 = line;
            int dest = findNodeIdx(stopCode2);
            Node n2 = nodes.at(findNodeIdx(stopCode2));

            double dist =
                haversine(n1.stop.getLatitude(), n1.stop.getLongitude(),
                          n2.stop.getLatitude(), n2.stop.getLongitude());

            addEdge(src, dest, lineCode, dist);
            n1 = n2;
            src = dest;
        }

        f.close();
    }
}

/*

STOP_CODE   STOP_NAME   STOP_ZONE	LONG	LAT
N_DEST      DIST        LINE_CODE
N_DEST      DIST        LINE_CODE
N_DEST      DIST        LINE_CODE
N_DEST      DIST        LINE_CODE

STOP_CODE   STOP_NAME   STOP_ZONE   LONG    LAT
N_DEST      DIST        LINE_CODE
N_DEST      DIST        LINE_CODE
N_DEST      DIST        LINE_CODE

STOP_CODE   STOP_NAME   STOP_ZONE   LONG    LAT
N_DEST      DIST        LINE_CODE
N_DEST      DIST        LINE_CODE

 */

void Graph::print() {
    /*
    for (auto e: nodes) {
        std::cout << e.stop.getName() << " " << e.stop.getCode() << " " <<
    e.stop.getZone() << " " << e.stop.getLatitude() << " " <<
    e.stop.getLongitude() << '\n' << std::flush;
    }
*/
    for (auto e : nodes) {
        for (auto i : e.adj) {
            if (i.code == "202") {
                std::cout << e.stop.getCode() << '\n' << std::flush;
            }
        }
    }
}

int Graph::findNodeIdx(std::string code) {
    auto itr = std::find_if(nodes.cbegin(), nodes.cend(), [&code](Node n1) {
        return (n1.stop.getCode() == code);
    });

    if (itr != nodes.cend()) {
        return std::distance(nodes.cbegin(), itr);
    } else {
        return -1;
    }
}

void Graph::populate() {
    readStops();
    readLines();
}

void Graph::dijkstra(int s) {
    MinHeap<int, double> q(n, -1);

    for (int v = 1; v <= n; v++) {
        nodes[v].dist = INF;
        q.insert(v, INF);
        nodes[v].visited = false;
    }

    nodes[s].dist = 0;
    q.decreaseKey(s, 0);
    nodes[s].pred = s;
    while (q.getSize() > 0) {
        int u = q.removeMin();
        // cout << "Node " << u << " with dist = " << nodes[u].dist << endl;
        nodes[u].visited = true;
        for (auto e : nodes[u].adj) {
            int v = e.dest;
            int w = e.distance;
            if (!nodes[v].visited && nodes[u].dist + w < nodes[v].dist) {
                nodes[v].dist = nodes[u].dist + w;
                q.decreaseKey(v, nodes[v].dist);
                nodes[v].pred = u;
            }
        }
    }
}

int Graph::dijkstra_distance(int a, int b) {
    dijkstra(a);
    if (nodes[b].dist == INF)
        return -1;
    return nodes[b].dist;
}

list<int> Graph::dijkstra_path(int a, int b) {
    dijkstra(a);
    list<int> path;
    if (nodes[b].dist == INF)
        return path;
    path.push_back(b);
    int v = b;
    while (v != a) {
        v = nodes[v].pred;
        path.push_front(v);
    }
    return path;
}