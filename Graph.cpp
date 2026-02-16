#include "Graph.hpp"

#include <algorithm>
#include <cmath>

namespace {
constexpr double kDiagonalCost = 1.41421356237;
}

Graph::Graph(const int rows, const int cols) : rows_(rows), cols_(cols) {
    nodes_.resize(rows_ * cols_);
    for (int r = 0; r < rows_; ++r)
        for (int c = 0; c < cols_; ++c) {
            const int idx = Index(r, c);
            nodes_[idx].row = r;
            nodes_[idx].col = c;
        }
}

int Graph::GetRows() const {
    return rows_;
}

int Graph::GetCols() const {
    return cols_;
}

int Graph::Index(const int row, const int col) const {
    return row * cols_ + col;
}

bool Graph::InBounds(const int row, const int col) const {
    return row >= 0 && row < rows_ && col >= 0 && col < cols_;
}

Node& Graph::GetNode(int index) {
    return nodes_[index];
}

const Node& Graph::GetNode(int index) const {
    return nodes_[index];
}

const std::vector<Node>& Graph::GetNodes() const {
    return nodes_;
}

void Graph::SetDiagonal(bool enabled) {
    diagonalEnabled_ = enabled;
}

bool Graph::IsDiagonalEnabled() const {
    return diagonalEnabled_;
}

std::vector<Neighbor> Graph::GetNeighbors(int index) const {
    const Node& node = nodes_[index];
    std::vector<Neighbor> neighbors;

    for (int i = 0; i < 4; ++i) {
        constexpr int dc[4] = {0, 0, -1, 1};
        constexpr int dr[4] = {-1, 1, 0, 0};
        const int nr = node.row + dr[i];
        const int nc = node.col + dc[i];
        if (!InBounds(nr, nc))
            continue;
        const int nidx = Index(nr, nc);
        neighbors.push_back({nidx, 1.0});
    }

    if (diagonalEnabled_)
        for (int i = 0; i < 4; ++i) {
            constexpr int ddc[4] = {-1, 1, -1, 1};
            constexpr int ddr[4] = {-1, -1, 1, 1};
            const int nr = node.row + ddr[i];
            const int nc = node.col + ddc[i];
            if (!InBounds(nr, nc))
                continue;
            const int nidx = Index(nr, nc);
            neighbors.push_back({nidx, kDiagonalCost});
        }

    return neighbors;
}

void Graph::ResetStatesKeepObstacles(int startIdx, int targetIdx) {
    for (auto& node : nodes_)
        if (node.obstacle)
            node.state = NodeState::Obstacle;
        else
            node.state = NodeState::Unvisited;
    ApplySpecialStates(startIdx, targetIdx);
}

void Graph::ResetAll(const int startIdx, const int targetIdx) {
    for (auto& node : nodes_) {
        node.obstacle = false;
        node.state = NodeState::Unvisited;
    }
    ApplySpecialStates(startIdx, targetIdx);
}

void Graph::ClearObstacles(const int startIdx, const int targetIdx) {
    for (auto& node : nodes_) {
        node.obstacle = false;
        node.state = NodeState::Unvisited;
    }
    ApplySpecialStates(startIdx, targetIdx);
}

void Graph::ToggleObstacle(const int index, const int startIdx, const int targetIdx) {
    if (index == startIdx || index == targetIdx)
        return;
    Node& node = nodes_[index];
    node.obstacle = !node.obstacle;
    node.state = node.obstacle ? NodeState::Obstacle : NodeState::Unvisited;
}

void Graph::SetStart(const int index, int& startIdx) {
    if (index == startIdx)
        return;

    nodes_[startIdx].state = nodes_[startIdx].obstacle ? NodeState::Obstacle : NodeState::Unvisited;
    nodes_[startIdx].obstacle = false;
    startIdx = index;
    nodes_[startIdx].obstacle = false;
    nodes_[startIdx].state = NodeState::Start;
}

void Graph::SetTarget(const int index, int& targetIdx) {
    if (index == targetIdx)
        return;

    nodes_[targetIdx].state = nodes_[targetIdx].obstacle ? NodeState::Obstacle : NodeState::Unvisited;
    nodes_[targetIdx].obstacle = false;
    targetIdx = index;
    nodes_[targetIdx].obstacle = false;
    nodes_[targetIdx].state = NodeState::Target;
}

void Graph::ApplySpecialStates(const int startIdx, const int targetIdx) {
    if (startIdx >= 0 && startIdx < static_cast<int>(nodes_.size())) {
        nodes_[startIdx].obstacle = false;
        nodes_[startIdx].state = NodeState::Start;
    }
    if (targetIdx >= 0 && targetIdx < static_cast<int>(nodes_.size())) {
        nodes_[targetIdx].obstacle = false;
        nodes_[targetIdx].state = NodeState::Target;
    }
}
