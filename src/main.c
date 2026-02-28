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

// VECTOR FIELDS
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

void draw_grid(Vector2 (*field)[GRID_COLS]);
void draw_arrow(Vector2 center, Vector2 dir, float max_len, Color color);

// PARTICLES
#define MAX_PARTICLES 500
#define PARTICLE_SPEED 150.0f
#define PARTICLE_LIFE 300.0f
#define SPAWN_RATE 1
typedef struct {
    Vector2 pos;
    float age;
    float life;
    bool active;
} Particle;
void spawn_particle(Particle particles[], Vector2 pos, float life);
void draw_particles(Particle particles[]);
void update_particles(Particle particles[], FieldType type, float dt);

int main(void) {

    InitWindow(PADDING * 2 + GRID_COLS * CELL_SIZE,
               PADDING * 2 + GRID_ROWS * CELL_SIZE, "force-fields");
    SetTargetFPS(60);

    Vector2 field[GRID_ROWS][GRID_COLS] = {0};
    FieldType current_type = FIELD_RADIAL;
    Particle particles[MAX_PARTICLES] = {0};

    bool show_grid = true;
    bool show_particles = true;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        BeginDrawing();
        ClearBackground(RAYWHITE);

        FieldType prev_type = current_type;

        if (IsKeyPressed(KEY_SPACE)) {
            current_type = (current_type + 1) % FIELD_COUNT;
        }

        if (current_type != prev_type) {
            for (int i = 0; i < MAX_PARTICLES; i++) {
                particles[i].active = false;
            }
        }

        if (IsKeyPressed(KEY_G))
            show_grid = !show_grid;
        if (IsKeyPressed(KEY_P))
            show_particles = !show_particles;

        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                float x = (float)c / GRID_COLS * 2.0f - 1.0f;
                float y = (float)r / GRID_ROWS * 2.0f - 1.0f;
                field[r][c] = field_funcs[current_type](x, y);
            }
        }

        // PARTICLES
        if (show_particles) {
            for (int i = 0; i < SPAWN_RATE; i++) {
                float px =
                    PADDING + (float)rand() / RAND_MAX * GRID_COLS * CELL_SIZE;
                float py =
                    PADDING + (float)rand() / RAND_MAX * GRID_ROWS * CELL_SIZE;
                spawn_particle(particles, (Vector2){px, py}, PARTICLE_LIFE);
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse = GetMousePosition();
                for (int i = 0; i < 10; i++) {
                    float jx =
                        mouse.x + ((float)rand() / RAND_MAX - 0.5f) * 20.0f;
                    float jy =
                        mouse.y + ((float)rand() / RAND_MAX - 0.5f) * 20.0f;
                    spawn_particle(particles, (Vector2){jx, jy}, PARTICLE_LIFE);
                }
            }

            // AGE PARTICLES
            update_particles(particles, current_type, dt);
        }
        if (show_grid)
            draw_grid(field);
        if (show_particles)
            draw_particles(particles);
        DrawText(TextFormat("%s | SPACE: cycle | G: grid | P: particles",
                            field_names[current_type]),
                 PADDING, 10, 20, DARKGRAY);
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

void spawn_particle(Particle particles[], Vector2 pos, float life) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].pos = pos;
            particles[i].age = 0.0f;
            particles[i].life = life;
            particles[i].active = true;
            return;
        }
    }
}

void draw_particles(Particle particles[]) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active)
            continue;
        float t = particles[i].age / particles[i].life;
        unsigned char alpha = (unsigned char)(255.0f * (1.0f - t));
        Color color = {50, 120, 220, alpha};
        DrawCircleV(particles[i].pos, 3.0f, color);
    }
}

void update_particles(Particle particles[], FieldType type, float dt) {
    float x_min = PADDING;
    float x_max = PADDING + GRID_COLS * CELL_SIZE;
    float y_min = PADDING;
    float y_max = PADDING + GRID_ROWS * CELL_SIZE;

    for (int i = 0; i < MAX_PARTICLES; ++i) {
        if (!particles[i].active)
            continue;

        particles[i].age += dt;
        if (particles[i].age >= particles[i].life) {
            particles[i].active = false;
            continue;
        }

        Vector2 n = screen_to_norm(particles[i].pos);
        Vector2 force = field_funcs[type](n.x, n.y);
        particles[i].pos.x += force.x * PARTICLE_SPEED * dt;
        particles[i].pos.y += force.y * PARTICLE_SPEED * dt;

        if (particles[i].pos.x < x_min || particles[i].pos.x > x_max ||
            particles[i].pos.y < y_min || particles[i].pos.y > y_max) {
            particles[i].active = false;
        }
    }
}
