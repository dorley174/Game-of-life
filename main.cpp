#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <vector>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 800;

class GameOfLife {
public:
    GameOfLife(int rows, int cols)
        : rows(rows), cols(cols), grid(rows, std::vector<bool>(cols, false)) {}

    void toggleCell(int row, int col) {
        if (isValid(row, col)) grid[row][col] = !grid[row][col];
    }

    void update() {
        std::vector<std::vector<bool>> newGrid = grid;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int liveNeighbors = countLiveNeighbors(r, c);
                if (grid[r][c]) {
                    newGrid[r][c] = (liveNeighbors == 2 || liveNeighbors == 3);
                } else {
                    newGrid[r][c] = (liveNeighbors == 3);
                }
            }
        }
        grid = newGrid;
    }

    void render(SDL_Renderer* renderer, int cellSize) {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                SDL_Rect cell = { c * cellSize, r * cellSize, cellSize, cellSize };
                if (grid[r][c]) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                else SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &cell);
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
                SDL_RenderDrawRect(renderer, &cell);
            }
        }
    }

    int getRows() const { return rows; }
    int getCols() const { return cols; }

private:
    int rows, cols;
    std::vector<std::vector<bool>> grid;

    bool isValid(int r, int c) {
        return r >= 0 && r < rows && c >= 0 && c < cols;
    }

    int countLiveNeighbors(int r, int c) {
        int count = 0;
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) continue;
                int nr = r + dr, nc = c + dc;
                if (isValid(nr, nc) && grid[nr][nc]) ++count;
            }
        }
        return count;
    }
};

int main(int argc, char* argv[]) {
    int gridRows = 50;
    int gridCols = 50;
    int speedMs = 200;

    if (argc >= 2) gridRows = gridCols = std::stoi(argv[1]);
    if (argc >= 3) speedMs = std::stoi(argv[2]);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    GameOfLife game(gridRows, gridCols);
    int cellSize = DEFAULT_WINDOW_WIDTH / gridCols;

    bool running = true;
    bool paused = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) paused = !paused;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                int col = x / cellSize;
                int row = y / cellSize;
                game.toggleCell(row, col);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        game.render(renderer, cellSize);
        SDL_RenderPresent(renderer);

        if (!paused) game.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(speedMs));
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
