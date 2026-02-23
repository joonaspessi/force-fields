#include "raylib.h"

int main(void)
{
    InitWindow(800, 600, "force-fields");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Filled + outlined rectangle
        DrawRectangle(50, 50, 200, 120, BLUE);
        DrawRectangleLines(50, 50, 200, 120, DARKBLUE);

        // Circle
        DrawCircle(500, 200, 80, RED);

        // Line
        DrawLine(100, 400, 700, 350, DARKGREEN);

        // Triangle
        DrawTriangle(
            (Vector2){400, 400},
            (Vector2){350, 520},
            (Vector2){450, 520},
            ORANGE
        );

        // FPS counter in top-right
        DrawFPS(730, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
