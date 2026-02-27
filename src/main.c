#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

// GRID
#define GRID_COLS 50
#define GRID_ROWS 50
#define CELL_SIZE 20
#define PADDING 40

// PARTICLES
#define MAX_PARTICLE 4096
#define PARTICLE_SPEED 150.0f
#define PARTICLE_LIFE 3.0f
#define SPAWN_RATE 20

// Vector field functions
Vector2 field_uniform(float x, float y);
Vector2 field_radial(float x, float y);
Vector2 field_vortex(float x, float y);
Vector2 field_spiral(float x, float y);
Vector2 field_saddle(float x, float y);
float field_max_magnitude(Vector2 (*field)[GRID_COLS]);
typedef Vector2 (*FieldFunc)(float x, float y);
typedef enum {
    FIELD_UNIFORM,
    FIELD_RADIAL,
    FIELD_VORTEX,
    FIELD_SPIRAL,
    FIELD_SADDLE,
    FIELD_COUNT,
} FieldType;
FieldFunc field_funcs[FIELD_COUNT] = {field_uniform, field_radial, field_vortex,
                                      field_spiral, field_saddle};
const char *field_names[FIELD_COUNT] = {"Uniform", "Radial", "Vortex", "Spiral",
                                        "Saddle"};
Vector2 screen_to_norm(Vector2 pos) {
    float nx = (pos.x - PADDING) / (GRID_COLS * CELL_SIZE) * 2.0f - 1.0f;
    float ny = (pos.y - PADDING) / (GRID_ROWS * CELL_SIZE) * 2.0f - 1.0f;
    return (Vector2){nx, ny};
}

typedef struct {
    Vector2 pos;
    float age;
    float life;
    bool active;
} Particle;

void draw_grid(Vector2 (*field)[GRID_COLS]);
void draw_arrow(Vector2 center, Vector2 dir, float max_len, Color color);

int main(void) {

    InitWindow(PADDING * 2 + GRID_COLS * CELL_SIZE,
               PADDING * 2 + GRID_ROWS * CELL_SIZE, "force-fields");
    SetTargetFPS(60);

    Vector2 field[GRID_ROWS][GRID_COLS] = {0};
    FieldType current_type = FIELD_RADIAL;
    Particle particles[MAX_PARTICLE] = {0};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_SPACE)) {
            current_type = (current_type + 1) % FIELD_COUNT;
        }

        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                float x = (float)c / GRID_COLS * 2.0f - 1.0f;
                float y = (float)r / GRID_ROWS * 2.0f - 1.0f;
                field[r][c] = field_funcs[current_type](x, y);
            }
        }

        draw_grid(field);
        DrawText(field_names[current_type], PADDING, 10, 20, DARKGRAY);
        DrawFPS(PADDING * 2 + GRID_COLS * CELL_SIZE - PADDING - CELL_SIZE * 4,
                PADDING / 2);
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

            Vector2 v = field[r][c];
            float mag = Vector2Length(v);
            float t = mag / max_mag;

            // hsv: hue 240 (blue) -> 0 (red), full saturation, full value
            Color color = ColorFromHSV(240.0f * (1.0f - t), 0.9f, 0.9f);

            Vector2 center = {x + CELL_SIZE / 2.0f, y + CELL_SIZE / 2.0f};
            Vector2 scaled = Vector2Scale(field[r][c], 1.0f / max_mag);
            draw_arrow(center, scaled, CELL_SIZE - 4, color);
        }
    }
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
            if (mag > max_mag) {
                max_mag = mag;
            }
        }
    }
    return max_mag;
}

Vector2 field_uniform(float x, float y) {
    (void)x;
    (void)y;

    return (Vector2){1.0f, 0.5f};
}

Vector2 field_radial(float x, float y) { return (Vector2){x, y}; }
Vector2 field_vortex(float x, float y) { return (Vector2){-y, x}; }
Vector2 field_spiral(float x, float y) { return (Vector2){x - y, y + x}; }
Vector2 field_saddle(float x, float y) { return (Vector2){x, -y}; }
