#pragma once

enum class NodeState {
    Unvisited,
    InQueue,
    Processing,
    Visited,
    Obstacle,
    Path,
    Start,
    Target
};

struct Node {
    int row = 0;
    int col = 0;
    NodeState state = NodeState::Unvisited;
    bool obstacle = false;
};
