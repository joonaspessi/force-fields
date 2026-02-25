#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>

#define GRID_COLS 20
#define GRID_ROWS 15
#define CELL_SIZE 40
#define PADDING 40

Vector2 field_uniform(float x, float y);
void draw_grid(Vector2 (*field)[GRID_COLS]);
void draw_arrow(Vector2 center, Vector2 dir, float max_len, Color color);
float field_max_magnitude(Vector2 (*field)[GRID_COLS]);

typedef Vector2 (*FieldFunc)(float x, float y);

int main(void) {

    InitWindow(PADDING * 2 + GRID_COLS * CELL_SIZE,
               PADDING * 2 + GRID_ROWS * CELL_SIZE, "force-fields");
    SetTargetFPS(60);

    Vector2 field[GRID_ROWS][GRID_COLS] = {0};

    FieldFunc current_field = field_uniform;

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            float x = (float)c / GRID_COLS * 2.0f - 1.0f;
            float y = (float)r / GRID_ROWS * 2.0f - 1.0f;
            field[r][c] = current_field(x, y);
        }
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        draw_grid(field);
        DrawFPS(730, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void draw_grid(Vector2 (*field)[GRID_COLS]) {
    float max_mag = field_max_magnitude(field);
    if (max_mag < 0.001f)
        max_mag = 1.0f;

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            int x = c * CELL_SIZE + PADDING;
            int y = r * CELL_SIZE + PADDING;

            DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, LIGHTGRAY);

            Vector2 center = {x + CELL_SIZE / 2.0f, y + CELL_SIZE / 2.0f};
            Vector2 scaled = Vector2Scale(field[r][c], 1.0f / max_mag);

            draw_arrow(center, scaled, CELL_SIZE - 4, DARKGRAY);
        }
    }
}

Vector2 field_uniform(float x, float y) {
    (void)x;
    (void)y;

    return (Vector2){1.0f, 0.5f};
}

void draw_arrow(Vector2 center, Vector2 dir, float max_len, Color color) {
    float mag = Vector2Length(dir);

    if (mag < 0.001f) {
        return;
    }

    Vector2 norm = Vector2Normalize(dir);
    float len = max_len * fminf(mag, 1.0f);

    Vector2 tip = Vector2Add(center, Vector2Scale(norm, len * 0.5f));
    Vector2 tail = Vector2Add(center, Vector2Scale(norm, -len * 0.5f));

    DrawLineEx(tail, tip, 2.0f, color);
    
    // Arrow head
    float head_len = len * 0.3f;
    float head_half = len * 0.15f;
    Vector2 perp = {-norm.y, norm.x};

    Vector2 base = Vector2Add(tip, Vector2Scale(norm, -head_len));
    Vector2 left = Vector2Add(base, Vector2Scale(perp, head_half));
    Vector2 right = Vector2Add(base, Vector2Scale(perp, -head_half));

    DrawTriangle(tip, right, left, color);
}

float field_max_magnitude(Vector2 (*field)[GRID_COLS]) {
    float max_mag = 0.0f;

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            float mag = Vector2Length(field[r][c]);
            if (mag < max_mag) {
                max_mag = mag;
            }
        }
    }
    return max_mag;
}

