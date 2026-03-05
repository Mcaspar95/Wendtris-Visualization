#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>
#include <limits>
#include <algorithm>
#include <random>
#include <string>

// Include matplotlib-cpp (download from https://github.com/lava/matplotlib-cpp) :contentReference[oaicite:1]{index=1}
#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

struct Request {
    int prize;
    int demand;
    std::pair<int,int> location;

    Request(int p, int d, std::pair<int,int> loc) : prize(p), demand(d), location(loc) {}
};

double calculate_distance(std::pair<int,int> loc1, std::pair<int,int> loc2) {
    return std::sqrt(std::pow(loc1.first - loc2.first, 2) + std::pow(loc1.second - loc2.second, 2));
}

class DeliverySet {
private:
    std::pair<int,int> depot;
    std::vector<Request> requests;
    int capacity;
    std::vector<Request> accepted_requests;
    std::mt19937 rng;

public:
    DeliverySet(int cap) : depot({0,0}), capacity(cap) {
        requests.push_back(Request(5,2,{-8,5}));
        requests.push_back(Request(8,3,{7,-2}));
        requests.push_back(Request(10,1,{2,8}));
        requests.push_back(Request(6,4,{-5,-9}));
        requests.push_back(Request(7,2,{9,4}));

        std::random_device rd;
        rng = std::mt19937(rd());
    }

    void simulate_periods(int periods) {
        std::uniform_int_distribution<int> dist(0, requests.size() - 1);

        for (int period = 1; period <= periods; ++period) {
            int idx = dist(rng);
            Request& chosen_request = requests[idx];

            std::cout << "\nPeriod " << period
                      << ": Offered Request - Prize: " << chosen_request.prize
                      << ", Demand: " << chosen_request.demand
                      << ", Location: (" << chosen_request.location.first << ", " << chosen_request.location.second << ")\n";

            std::string decision;
            std::cout << "Do you want to accept this request? (yes/no): ";
            std::cin >> decision;

            std::transform(decision.begin(), decision.end(), decision.begin(), ::tolower);

            if (decision == "yes") {
                if (capacity >= chosen_request.demand) {
                    std::cout << "Accepted! Updating capacity.\n";
                    capacity -= chosen_request.demand;
                    accepted_requests.push_back(chosen_request);
                } else {
                    std::cout << "Error: Insufficient capacity. Residual: " << capacity
                              << ", Demand: " << chosen_request.demand << "\n";
                }
            } else {
                std::cout << "Rejected.\n";
            }
            std::cout << "Residual Capacity: " << std::max(capacity, 0) << "\n";
        }
    }

    int display_accepted_requests() {
        std::cout << "\nAccepted Requests:\n";
        int cumulated_prize = 0;
        for (auto& req : accepted_requests) {
            std::cout << "Prize: " << req.prize
                      << ", Demand: " << req.demand
                      << ", Location: (" << req.location.first
                      << ", " << req.location.second << ")\n";
            cumulated_prize += req.prize;
        }
        std::cout << "Cumulated Prize: " << cumulated_prize << "\n";
        return cumulated_prize;
    }

    std::pair<std::vector<std::pair<int,int>>, double> calculate_tsp_tour() {
        std::vector<std::pair<int,int>> locations;
        for (auto& req : accepted_requests) {
            locations.push_back(req.location);
        }

        // remove duplicates
        std::sort(locations.begin(), locations.end());
        locations.erase(std::unique(locations.begin(), locations.end()), locations.end());

        double min_cost = std::numeric_limits<double>::max();
        std::vector<std::pair<int,int>> optimal_tour;

        std::sort(locations.begin(), locations.end());
        do {
            double tour_cost = calculate_distance(depot, locations[0]);
            for (size_t i = 0; i + 1 < locations.size(); ++i) {
                tour_cost += calculate_distance(locations[i], locations[i + 1]);
            }
            tour_cost += calculate_distance(locations.back(), depot);

            if (tour_cost < min_cost) {
                min_cost = tour_cost;
                optimal_tour = {depot};
                optimal_tour.insert(optimal_tour.end(), locations.begin(), locations.end());
                optimal_tour.push_back(depot);
            }
        } while (std::next_permutation(locations.begin(), locations.end()));

        return {optimal_tour, min_cost};
    }

    void plot_result(const std::vector<std::pair<int,int>>& tsp_tour) {
        // Prepare vectors for scatter/line plot
        std::vector<double> xs, ys;
        for (auto& p : tsp_tour) {
            xs.push_back(p.first);
            ys.push_back(p.second);
        }

        std::vector<double> depot_x { (double) depot.first };
        std::vector<double> depot_y { (double) depot.second };

        // Plot path
        plt::figure();
        plt::plot(xs, ys, "b-o");           // tour path
        plt::scatter(depot_x, depot_y, 100); // depot

        // Labels
        plt::title("Optimal TSP Tour");
        plt::xlabel("X-coordinate");
        plt::ylabel("Y-coordinate");
        plt::legend();
        plt::grid(true);

        plt::show();
    }
};

int main() {
    DeliverySet delivery_set(10);
    delivery_set.simulate_periods(5);

    int cum_prize = delivery_set.display_accepted_requests();

    auto [tour, cost] = delivery_set.calculate_tsp_tour();
    std::cout << "\nTotal TSP Cost: " << cost << "\n";
    std::cout << "Final Score: " << cum_prize - cost << "\n";

    delivery_set.plot_result(tour);

    return 0;
}
