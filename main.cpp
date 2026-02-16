#define SDL_MAIN_HANDLED
#include "SDL3/SDL.h"

#include <algorithm>
#include <random>

#include "Dijkstra.hpp"
#include "Graph.hpp"
#include "Renderer.hpp"

namespace {
constexpr int kWindowSize = 1000;
constexpr int kGridRows = 25;
constexpr int kGridCols = 25;
constexpr int kDefaultDelayMs = 50;
constexpr int kMinDelayMs = 1;
constexpr int kMaxDelayMs = 500;
constexpr int kDelayStepMs = 5;
constexpr int kBlockCount = 6;
constexpr int kBlockMinSize = 2;
constexpr int kBlockMaxSize = 6;
constexpr int kNoiseDivisor = 12;
}

void AddRandomBlocks(Graph& graph, const int startIdx, const int targetIdx, const unsigned int seed) {
    const int rows = graph.GetRows();
    const int cols = graph.GetCols();

    std::mt19937 rng(seed);
    std::uniform_int_distribution rowDist(0, rows - 1);
    std::uniform_int_distribution colDist(0, cols - 1);
    std::uniform_int_distribution sizeDist(kBlockMinSize, kBlockMaxSize);

    auto placeObstacle = [&](const int r, const int c) {
        const int idx = graph.Index(r, c);
        if (idx == startIdx || idx == targetIdx)
            return;

        Node& node = graph.GetNode(idx);
        node.obstacle = true;
        node.state = NodeState::Obstacle;
    };

    for (int i = 0; i < kBlockCount; ++i) {
        const int blockH = sizeDist(rng);
        const int blockW = sizeDist(rng);
        const int startR = rowDist(rng);
        const int startC = colDist(rng);

        for (int r = startR; r < std::min(rows, startR + blockH); ++r)
            for (int c = startC; c < std::min(cols, startC + blockW); ++c)
                placeObstacle(r, c);
    }

    const int noiseCount = rows * cols / kNoiseDivisor;
    for (int i = 0; i < noiseCount; ++i)
        placeObstacle(rowDist(rng), colDist(rng));
}

enum class AppState {
    Idle,
    Running,
    Finished
};

class SdlSystem {
public:
    SdlSystem() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("SDL_Init failed: %s", SDL_GetError());
            ok_ = false;
        }
    }

    ~SdlSystem() {
        SDL_Quit();
    }

    [[nodiscard]] bool Ok() const {
        return ok_;
    }

private:
    bool ok_ = true;
};

class SdlWindowRenderer {
public:
    SdlWindowRenderer(const char* title, const int width, const int height) {
        if (!SDL_CreateWindowAndRenderer(title, width, height, SDL_WINDOW_RESIZABLE, &window_, &renderer_)) {
            SDL_Log("SDL_Create failed: %s", SDL_GetError());
            return;
        }
        ok_ = true;
    }

    ~SdlWindowRenderer() {
        if (renderer_)
            SDL_DestroyRenderer(renderer_);
        if (window_)
            SDL_DestroyWindow(window_);
    }

    [[nodiscard]] bool Ok() const {
        return ok_;
    }

    [[nodiscard]] SDL_Window* Window() const {
        return window_;
    }

    [[nodiscard]] SDL_Renderer* Renderer() const {
        return renderer_;
    }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    bool ok_ = false;
};

int main() {
    if (const SdlSystem sdl; !sdl.Ok())
        return 1;

    const SdlWindowRenderer windowRenderer("Dijkstra", kWindowSize, kWindowSize);
    if (!windowRenderer.Ok())
        return 1;

    Graph graph(kGridRows, kGridCols);
    int startIdx = 0;
    int targetIdx = graph.Index(kGridRows - 1, kGridCols - 1);
    graph.ResetAll(startIdx, targetIdx);
    AddRandomBlocks(graph, startIdx, targetIdx, SDL_GetTicks());
    graph.ResetStatesKeepObstacles(startIdx, targetIdx);

    Renderer renderer(windowRenderer.Renderer());
    Dijkstra dijkstra;

    AppState state = AppState::Idle;
    int delayMs = kDefaultDelayMs;
    Uint32 lastStepTicks = SDL_GetTicks();
    Uint32 lastPathTicks = SDL_GetTicks();
    std::vector<int> path;
    size_t pathIndex = 0;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE)
                    running = false;
                else if (event.key.key == SDLK_SPACE) {
                    if (state != AppState::Running) {
                        dijkstra.Start(graph, startIdx, targetIdx);
                        path.clear();
                        pathIndex = 0;
                        state = AppState::Running;
                        lastStepTicks = SDL_GetTicks();
                    }
                } else if (event.key.key == SDLK_R) {
                    graph.ResetAll(startIdx, targetIdx);
                    AddRandomBlocks(graph, startIdx, targetIdx, SDL_GetTicks());
                    graph.ResetStatesKeepObstacles(startIdx, targetIdx);
                    dijkstra.Reset();
                    path.clear();
                    pathIndex = 0;
                    state = AppState::Idle;
                } else if (event.key.key == SDLK_C) {
                    graph.ClearObstacles(startIdx, targetIdx);
                    dijkstra.Reset();
                    path.clear();
                    pathIndex = 0;
                    state = AppState::Idle;
                } else if (event.key.key == SDLK_D) {
                    if (state != AppState::Running) {
                        graph.SetDiagonal(!graph.IsDiagonalEnabled());
                        graph.ResetStatesKeepObstacles(startIdx, targetIdx);
                    }
                } else if (event.key.key == SDLK_UP) {
                    delayMs = std::max(kMinDelayMs, delayMs - kDelayStepMs);
                } else if (event.key.key == SDLK_DOWN) {
                    delayMs = std::min(kMaxDelayMs, delayMs + kDelayStepMs);
                }
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (state == AppState::Running)
                    continue;

                const int mouseX = static_cast<int>(event.button.x);
                const int mouseY =static_cast<int>(event.button.y);
                int renderW = 0;
                int renderH = 0;
                SDL_GetRenderOutputSize(windowRenderer.Renderer(), &renderW, &renderH);

                const int cellSize = std::max(1, std::min(renderW / kGridCols, renderH / kGridRows));
                const int gridW = cellSize * kGridCols;
                const int gridH = cellSize * kGridRows;
                const int offsetX = (renderW - gridW) / 2;
                const int offsetY = (renderH - gridH) / 2;

                const int col = (mouseX - offsetX) / cellSize;

                if (const int row = (mouseY - offsetY) / cellSize; graph.InBounds(row, col)) {
                    const int index = graph.Index(row, col);
                    if (event.button.button == SDL_BUTTON_LEFT)
                        graph.ToggleObstacle(index, startIdx, targetIdx);
                    else if (event.button.button == SDL_BUTTON_RIGHT) {
                        if (const SDL_Keymod mod = SDL_GetModState(); mod & SDL_KMOD_SHIFT)
                            graph.SetTarget(index, targetIdx);
                        else
                            graph.SetStart(index, startIdx);
                        graph.ResetStatesKeepObstacles(startIdx, targetIdx);
                    }
                    dijkstra.Reset();
                    path.clear();
                    pathIndex = 0;
                    state = AppState::Idle;
                }
            }
        }

        const Uint32 now = SDL_GetTicks();
        if (state == AppState::Running) {
            if (now - lastStepTicks >= static_cast<Uint32>(delayMs)) {
                const bool reached = dijkstra.Step(graph, startIdx, targetIdx);
                lastStepTicks = now;
                if (dijkstra.IsCompleted()) {
                    state = AppState::Finished;
                    if (reached && dijkstra.ReachedTarget()) {
                        path = dijkstra.BuildPath(targetIdx);
                        pathIndex = 0;
                        lastPathTicks = now;
                    }
                }
            }
        } else if (state == AppState::Finished && !path.empty()) {
            if (pathIndex < path.size() && now - lastPathTicks >= static_cast<Uint32>(delayMs)) {
                if (const int idx = path[pathIndex]; idx != startIdx && idx != targetIdx)
                    graph.GetNode(idx).state = NodeState::Path;
                ++pathIndex;
                lastPathTicks = now;
            }
        }

        renderer.Render(graph, startIdx, targetIdx);
        SDL_Delay(1);
    }

    return 0;
}
