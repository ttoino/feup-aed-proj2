#ifndef AED_PROJ2_2021_TRIP_H
#define AED_PROJ2_2021_TRIP_H

#include "filters.h"

#include <ostream>
#include <set>
#include <string>

/**
 * @brief Holds the possible ways to calculate a path.
 */
enum Strategy {
    /**
     * @brief Generates a path with the least amount of distance.
     */
    MIN_DISTANCE,
    /**
     * @brief Generates a path passing through the minimum amount of zones.
     */
    MIN_COST,
    /**
     * @brief Generates a path passing through the minimum amount of lines.
     */
    MIN_CHANGES,
    /**
     * @brief Generates a path passing through the minimum amount of stops.
     */
    MIN_STOPS
};

class Trip {

    /**
     * @brief The source of the trip.
     */
    std::string source{""};

    /**
     * @brief The destination of the trip.
     */
    std::string destination{""};

    /**
     * @brief The strategies to be applied to the calculation of the trip:
     * defaults to shortest distance.
     */
    Strategy strategy{MIN_DISTANCE};

    /**
     * @brief The maximum length the user is willing to travel on foot: defaults
     * to zero.
     */
    double maxWalkDistance{0};

    /**
     * @brief A boolean that defines if the user wants to travel during the day:
     * defaults to true.
     */
    bool day{true};

    /**
     * @brief A boolean that defines if the user wants to travel during the
     * night: defaults to true.
     */
    bool night{true};

    /**
     * @brief A set with codes to all of the closed lines.
     */
    std::set<std::string> closedLines{};

    /**
     * @brief A set with codes to all of the closed stops.
     */
    std::set<std::string> closedStops{};

public:
    /**
     * @brief Checks if the user specified a source from which to start the
     * trip.
     * @return A boolean confirming if the trip has a designated source.
     */
    bool hasSource() const;

    /**
     * @brief Checks if the user specified a destination for the trip.
     * @return A boolean confirming if the trip has a designated destination.
     */
    bool hasDestination() const;

    /**
     * @brief Get the source of the trip.
     * @return The code to the source of the trip.
     */
    std::string getSource() const;

    /**
     * @brief Get the destination of the trip.
     * @return The code to the destination of the trip.
     */
    std::string getDestination() const;

    /**
     * @brief Get the strategy to be applied to the calculation of the route.
     * @return The strategy (enum) chosen.
     */
    Strategy getStrategy() const;

    /**
     * @brief Get the maximum amount of distance the user is willing to travel
     * on foot.
     * @return A double that represents the maximum distance.
     */
    double getMaxWalkDistance() const;

    /**
     * @brief Get the setting the user defined regarding travelling during the
     * day.
     * @return A boolean that specifies if the trip will be done during the day
     * or not.
     */
    bool getDay() const;

    /**
     * @brief Get the setting the user defined regarding travelling during the
     * night.
     * @return A boolean that specifies if the trip will be done during the
     * night or not.
     */
    bool getNight() const;

    /**
     * @brief Get all of the closed lines.
     * @return A set with all of the closed lines.
     */
    std::set<std::string> getClosedLines() const;

    /**
     * @brief Get all of the closed stops.
     * @return A set with all of the closed stops.
     */
    std::set<std::string> getClosedStops() const;

    /**
     * @brief Get the filter that will restrict the paths to be considered.
     * @return A filter (regarding walking distance, day/night travelling, ...)
     */
    filter getFilter() const;

    /**
     * @brief Set the source of the trip.
     * @param The new code of the source of the trip.
     */
    void setSource(const std::string &source);

    /**
     * @brief Set the destination of the trip.
     * @param The new code of the destination of the trip.
     */
    void setDestination(const std::string &destination);

    /**
     * @brief Set the strategy of the trip.
     * @param The new strategy to be applied to the calculation of the trip.
     */
    void setStrategy(const Strategy &strategy);

    /**
     * @brief Set whether or not the trip can be done during the day.
     * @param The new boolean that specifies if the trip can be done during the
     * day.
     */
    void setDay(const bool &day);

    /**
     * @brief Set whether or not the trip can be done during the night.
     * @param The new boolean that specifies if the trip can be done during the
     * night.
     */
    void setNight(const bool &night);

    /**
     * @brief Set the maximum distance the user is willing to walk.
     * @param The double that represents the distance.
     */
    void setMaxWalkDistance(const double &dist);

    /**
     * @brief Toggle the availability of a line.
     * @param The code of the line to be toggled.
     */
    void toggleLine(const std::string &line);

    /**
     * @brief Toggle the availability of a stop.
     * @param The code of the stop to be toggled.
     */
    void toggleStop(const std::string &stop);

    /**
     * @brief Overloading of the << operator: Provides clarity regarding the
     * settings chosen for the trip.
     * @param out The output of the terminal.
     * @param trip The trip to be done.
     * @return The new custom output of the terminal.
     */
    friend std::ostream &operator<<(std::ostream &out, const Trip &trip);
};

#endif