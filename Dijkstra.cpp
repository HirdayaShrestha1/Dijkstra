#include "Dijkstra.hpp"

#include <algorithm>
#include <limits>

void Dijkstra::Start(Graph& graph, const int startIdx, const int targetIdx) {
    const int total = static_cast<int>(graph.GetNodes().size());
    distances_.assign(total, std::numeric_limits<double>::infinity());
    previous_.assign(total, -1);
    while (!queue_.empty())
        queue_.pop();
    completed_ = false;
    reachedTarget_ = false;

    graph.ResetStatesKeepObstacles(startIdx, targetIdx);

    distances_[startIdx] = 0.0;
    queue_.push({startIdx, 0.0});
    if (graph.GetNode(startIdx).state != NodeState::Start)
        graph.GetNode(startIdx).state = NodeState::Start;
    if (graph.GetNode(targetIdx).state != NodeState::Target)
        graph.GetNode(targetIdx).state = NodeState::Target;
}

void Dijkstra::Reset() {
    while (!queue_.empty())
        queue_.pop();
    distances_.clear();
    previous_.clear();
    completed_ = false;
    reachedTarget_ = false;
}

bool Dijkstra::Step(Graph& graph, const int startIdx, const int targetIdx) {
    if (completed_)
        return false;

    if (queue_.empty())
        return completed_ = true, false;

    auto [index, distance] = queue_.top();
    queue_.pop();

    if (distance != distances_[index])
        return false;

    if (index != startIdx && index != targetIdx)
        graph.GetNode(index).state = NodeState::Processing;

    if (index == targetIdx)
        return reachedTarget_ = true, completed_ = true, true;

    for (const auto&[index1, cost] : graph.GetNeighbors(index)) {
        Node& neighborNode = graph.GetNode(index1);
        if (neighborNode.obstacle)
            continue;

        if (const double candidate = distance + cost; candidate < distances_[index1]) {
            distances_[index1] = candidate;
            previous_[index1] = index;
            queue_.push({index1, candidate});

            if (index1 != startIdx && index1 != targetIdx)
                neighborNode.state = NodeState::InQueue;
        }
    }

    if (index != startIdx && index != targetIdx) {
        graph.GetNode(index).state = NodeState::Visited;
    }

    return false;
}

bool Dijkstra::IsCompleted() const {
    return completed_;
}

bool Dijkstra::ReachedTarget() const {
    return reachedTarget_;
}

std::vector<int> Dijkstra::BuildPath(const int targetIdx) const {
    std::vector<int> path;
    if (!reachedTarget_ || targetIdx < 0 || targetIdx >= static_cast<int>(previous_.size())) {
        return path;
    }

    int current = targetIdx;
    while (current != -1) {
        path.push_back(current);
        current = previous_[current];
    }

    std::ranges::reverse(path);
    return path;
}
