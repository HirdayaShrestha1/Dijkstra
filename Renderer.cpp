#include "Renderer.hpp"

#include <algorithm>

Renderer::Renderer(SDL_Renderer* renderer) : renderer_(renderer) {}

void Renderer::Render(const Graph& graph, const int startIdx, const int targetIdx) const {
    (void)startIdx;
    (void)targetIdx;
    int renderW = 0;
    int renderH = 0;
    SDL_GetRenderOutputSize(renderer_, &renderW, &renderH);

    const int rows = graph.GetRows();
    const int cols = graph.GetCols();

    const int cellSize = std::max(1, std::min(renderW / cols, renderH / rows));
    const int gridW = cellSize * cols;
    const int gridH = cellSize * rows;
    const int offsetX = (renderW - gridW) / 2;
    const int offsetY = (renderH - gridH) / 2;

    SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
    SDL_RenderClear(renderer_);

    for (const Node& node : graph.GetNodes()) {
        auto [r, g, b, a] = GetColor(node.state);
        SDL_SetRenderDrawColor(renderer_, r, g, b, a);

        SDL_FRect rect;
        rect.x = static_cast<float>(offsetX + node.col * cellSize);
        rect.y = static_cast<float>(offsetY + node.row * cellSize);
        rect.w = static_cast<float>(cellSize);
        rect.h = static_cast<float>(cellSize);
        SDL_RenderFillRect(renderer_, &rect);

        SDL_SetRenderDrawColor(renderer_, 30, 30, 30, 255);
        SDL_RenderRect(renderer_, &rect);
    }

    SDL_RenderPresent(renderer_);
}

SDL_Color Renderer::GetColor(const NodeState state) {
    switch (state) {
        case NodeState::Unvisited:
            return {255, 255, 255, 255};
        case NodeState::InQueue:
            return {0, 120, 255, 255};
        case NodeState::Processing:
            return {255, 220, 0, 255};
        case NodeState::Visited:
            return {0, 200, 0, 255};
        case NodeState::Obstacle:
            return {220, 0, 0, 255};
        case NodeState::Path:
            return {128, 0, 200, 255};
        case NodeState::Start:
            return {0, 200, 200, 255};
        case NodeState::Target:
            return {255, 140, 0, 255};
        default:
            return {255, 255, 255, 255};
    }
}
