#include <chrono>
#include <iostream>
#include <limits>
#include <string>

#include "../includes/Menu.h"
#include "../includes/Utils.h"
#include "../includes/constants.h"

void UserInterface::optionsMenu(
    const std::vector<std::pair<std::string, Menu>> &options) {
    for (int i{1}; i < options.size(); ++i) {
        std::cout << "(" << i << ") " << options.at(i).first << std::endl;
    }

    std::cout << "(0) " << options.front().first << "\n\n";

    unsigned option = getUnsignedInput("Please insert option: ");

    if (option < options.size())
        currentMenu = options.at(option).second;
    else
        errorMessage = "Invalid option!\n";
}

unsigned long UserInterface::getUnsignedInput(std::string prompt,
                                              unsigned long min,
                                              unsigned long max) {
    std::string input;
    unsigned long number;
    bool done = false;

    do {
        input = getStringInput(prompt);

        try {
            number = stoul(input);
            done = true;
        } catch (std::invalid_argument) {
            errorMessage = "Invalid input!\n";
            done = false;
        }
    } while (!done || !inRange(number, min, max));

    return number;
}

double UserInterface::getDoubleInput(std::string prompt, double min,
                                     double max) {
    std::string input;
    double number;
    bool done = false;

    do {
        input = getStringInput(prompt);

        try {
            number = stod(input);
            done = true;
        } catch (std::invalid_argument) {
            errorMessage = "Invalid input!\n";
            done = false;
        }
    } while (!done || !inRange(number, min, max));

    return number;
}

std::string UserInterface::getStringInput(std::string prompt) {
    std::cout << RED_TEXT << errorMessage << RESET_FORMATTING << prompt;
    errorMessage = "";

    std::string input{};

    std::getline(std::cin, input);
    normalizeInput(input);

    if (std::cin.eof())
        exit();

    return input;
}

bool UserInterface::inRange(unsigned long n, unsigned long min,
                            unsigned long max) {
    bool b = (n <= max) && (n >= min);

    if (!b)
        errorMessage = "Value outside allowed range!\n";

    return b;
}

bool UserInterface::inRange(double n, double min, double max) {
    bool b = (n <= max) && (n >= min);

    if (!b)
        errorMessage = "Value outside allowed range!\n";

    return b;
}

void UserInterface::show(Graph &graph, Trip &trip) {
    std::cout << CLEAR_SCREEN << PROGRAM_NAME << '\n' << std::endl;

    switch (currentMenu) {
    case MAIN:
        mainMenu(trip);
        break;
    case SOURCE:
        sourceMenu(graph, trip);
        break;
    case DESTINATION:
        destinationMenu(graph, trip);
        break;
    case STRATEGY:
        strategyMenu(trip);
        break;
    case WALK_DISTANCE:
        walkDistanceMenu(trip);
        break;
    case NIGHT:
        nightMenu(trip);
        break;
    case TOGGLE_LINES:
        toggleLinesMenu(trip);
        break;
    case TOGGLE_STOPS:
        toggleStopsMenu(trip);
        break;
    case PLAN:
        planMenu(graph, trip);
        break;
    case EXIT:
        throw Exit();
        break;
    }
}

void UserInterface::exit() {
    std::cout << CLEAR_SCREEN << std::flush;
    std::cout << "Shutting down..." << std::endl;
}

void UserInterface::mainMenu(Trip &trip) {
    std::cout << trip << '\n';

    optionsMenu({{"Exit", EXIT},
                 {"Set starting point", SOURCE},
                 {"Set destination point", DESTINATION},
                 {"Set strategy", STRATEGY},
                 {"Set max walking distance", WALK_DISTANCE},
                 {"Set scheduling preferences", NIGHT},
                 {"Close/open lines", TOGGLE_LINES},
                 {"Close/open stops", TOGGLE_STOPS},
                 {"Calculate path", PLAN}});
}

void UserInterface::sourceMenu(Graph &graph, Trip &trip) {
    unsigned opt = getUnsignedInput("(1) Coordinates\n"
                                    "(2) Stop\n"
                                    "(0) Go back\n\n"
                                    "Enter departure by: ",
                                    0, 2);

    switch (opt) {
    case 1: {
        double depLatitude =
            getDoubleInput("Insert your latitude value: ", -90., 90.);

        double depLongitude =
            getDoubleInput("Insert your longitude value: ", -180., 180.);

        graph.setStart(depLatitude, depLongitude);
        trip.setSource("START");
        break;
    }
    case 2:
        trip.setSource(getStringInput("Insert the departure's stop code: "));
        break;
    }

    currentMenu = MAIN;
}

void UserInterface::destinationMenu(Graph &graph, Trip &trip) {
    unsigned opt = getUnsignedInput("(1) Coordinates\n"
                                    "(2) Stop\n"
                                    "(0) Go back\n\n"
                                    "Enter departure by: ",
                                    0, 2);

    switch (opt) {
    case 1: {
        double destLatitude = getDoubleInput(
            "Insert the destination's latitude value: ", -90., 90.);

        double destLongitude = getDoubleInput(
            "Insert the destination's longitude value: ", -180., 180.);

        graph.setEnd(destLatitude, destLongitude);
        trip.setDestination("END");
        break;
    }
    case 2:
        trip.setDestination(
            getStringInput("Insert the destination's stop code: "));
        break;
    }

    currentMenu = MAIN;
}

void UserInterface::walkDistanceMenu(Trip &trip) {
    trip.setMaxWalkDistance(getDoubleInput(
        "Insert a distance you'd be comfortable walking (m): ", 0));

    currentMenu = MAIN;
}

void UserInterface::nightMenu(Trip &trip) {
    int opt = getUnsignedInput("(1) Avoid night only lines\n"
                               "(2) Avoid day only lines\n"
                               "(3) Don't avoid any lines\n\n"
                               "Choose your scheduling preference: ",
                               1, 3);

    trip.setDay(opt & 0b01);
    trip.setNight(opt & 0b10);

    currentMenu = MAIN;
}

void UserInterface::strategyMenu(Trip &trip) {
    int opt = getUnsignedInput("(1) Minimize distance\n"
                               "(2) Minimize cost\n"
                               "(3) Minimize bus changes\n"
                               "(4) Minimize stops\n\n"
                               "Choose how you'd like to plan your trip: ",
                               1, 4);

    trip.setStrategy(static_cast<Strategy>(opt - 1));

    currentMenu = MAIN;
}

void UserInterface::toggleLinesMenu(Trip &trip) {
    if (trip.getClosedLines().size()) {
        std::cout << "Currently closed lines:\n";

        for (const std::string &line : trip.getClosedLines())
            std::cout << '\t' << line << '\n';

        std::cout << '\n';
    }

    while (true) {
        std::string input = getStringInput("Enter line to toggle: ");

        if (input == "")
            break;

        trip.toggleLine(input);

        std::cout << '\n';
    }

    currentMenu = MAIN;
}

void UserInterface::toggleStopsMenu(Trip &trip) {
    if (trip.getClosedStops().size()) {
        std::cout << "Currently closed stops:\n";

        for (const std::string &stop : trip.getClosedStops())
            std::cout << '\t' << stop << '\n';

        std::cout << '\n';
    }

    while (true) {
        std::string input = getStringInput("Enter stop to toggle: ");

        if (input == "")
            break;

        trip.toggleStop(input);

        std::cout << '\n';
    }

    currentMenu = MAIN;
}

void UserInterface::planMenu(Graph &graph, Trip &trip) {
    if (!trip.hasDestination() || !trip.hasSource()) {
        currentMenu = MAIN;
        errorMessage = "Starting point or destination point not set!\n";
        return;
    } else if (!graph.getNodes().contains(trip.getDestination()) ||
               !graph.getNodes().contains(trip.getSource())) {
        currentMenu = MAIN;
        errorMessage = "Starting point or destination point do not exist!\n";
        return;
    }

    std::list<Node> path{};

    switch (trip.getStrategy()) {
    case MIN_COST:
        // zone changes
        path = graph.dijkstraCostPath(trip.getSource(), trip.getDestination(),
                                      trip.getFilter());
        break;

    case MIN_CHANGES:
        // bus changes
        path = graph.dijkstraLinesPath(trip.getSource(), trip.getDestination(),
                                       trip.getFilter());
        break;

    case MIN_DISTANCE:
        // normal dijkstra
        path = graph.dijkstraPath(trip.getSource(), trip.getDestination(),
                                  trip.getFilter());
        break;

    case MIN_STOPS:
        // stop changes (bfs)
        path = graph.bfsPath(trip.getSource(), trip.getDestination(),
                             trip.getFilter());
        break;
    }

    if (path.size())
        for (const Node &node : path)
            std::cout << node.stop.getCode() << "\t- " << node.line << "\t- "
                      << node.stop.getName() << std::endl;
    else
        std::cout << "No path found between those points...\n";

    getStringInput("\nPress Enter to continue...\n");

    currentMenu = MAIN;
}
