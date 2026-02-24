#include "raylib.h"
#include <stdlib.h>

#define GRID_COLS 20
#define GRID_ROWS 15
#define CELL_SIZE 40
#define PADDING 40

void draw_grid(Color (*grid)[GRID_COLS]);

int main(void) {

    InitWindow(PADDING * 2 + GRID_COLS * CELL_SIZE,
               PADDING * 2 + GRID_ROWS * CELL_SIZE, "force-fields");
    SetTargetFPS(60);

    Color grid[GRID_ROWS][GRID_COLS];
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            grid[r][c] = RAYWHITE;
        }
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                grid[r][c] =
                    (Color){rand() % 256, rand() % 256, rand() % 256, 255};
            }
        }

        draw_grid(grid);
        DrawFPS(730, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void draw_grid(Color (*grid)[GRID_COLS]) {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            int x = c * CELL_SIZE + PADDING;
            int y = r * CELL_SIZE + PADDING;

            DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, grid[r][c]);
            DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, LIGHTGRAY);
        }
    }
}
