#pragma once

#include <vector>
#include "Node.hpp"

struct Neighbor {
    int index = -1;
    double cost = 1.0;
};

class Graph {
public:
    Graph(int rows, int cols);

    int GetRows() const;
    int GetCols() const;
    int Index(int row, int col) const;
    bool InBounds(int row, int col) const;

    Node& GetNode(int index);
    const Node& GetNode(int index) const;
    const std::vector<Node>& GetNodes() const;

    void SetDiagonal(bool enabled);
    bool IsDiagonalEnabled() const;

    std::vector<Neighbor> GetNeighbors(int index) const;

    void ResetStatesKeepObstacles(int startIdx, int targetIdx);
    void ResetAll(int startIdx, int targetIdx);
    void ClearObstacles(int startIdx, int targetIdx);
    void ToggleObstacle(int index, int startIdx, int targetIdx);
    void SetStart(int index, int& startIdx);
    void SetTarget(int index, int& targetIdx);

private:
    int rows_ = 0;
    int cols_ = 0;
    bool diagonalEnabled_ = false;
    std::vector<Node> nodes_;

    void ApplySpecialStates(int startIdx, int targetIdx);
};
