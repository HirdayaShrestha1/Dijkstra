#pragma once

#include "Graph.hpp"
#include "SDL3/SDL.h"

class Renderer {
public:
    explicit Renderer(SDL_Renderer* renderer);

    void Render(const Graph& graph, int startIdx, int targetIdx) const;

private:
    SDL_Renderer* renderer_ = nullptr;

    static SDL_Color GetColor(NodeState state);
};
