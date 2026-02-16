#pragma once

#include <queue>
#include <vector>

#include "Graph.hpp"

class Dijkstra {
public:
    void Start(Graph& graph, int startIdx, int targetIdx);
    void Reset();

    bool Step(Graph& graph, int startIdx, int targetIdx);
    bool IsCompleted() const;
    bool ReachedTarget() const;

    std::vector<int> BuildPath(int targetIdx) const;

private:
    struct QueueItem {
        int index = -1;
        double distance = 0.0;
    };

    struct Compare {
        bool operator()(const QueueItem& a, const QueueItem& b) const {
            return a.distance > b.distance;
        }
    };

    std::priority_queue<QueueItem, std::vector<QueueItem>, Compare> queue_;
    std::vector<double> distances_;
    std::vector<int> previous_;
    bool completed_ = false;
    bool reachedTarget_ = false;
};
