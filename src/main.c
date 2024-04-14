#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
const int height = 800;
const int width  = 800; 
const char* title = "BoingBall";
const int FPS = 60;


struct Axis {
    Vector3 p1;
    Vector3 p2;
};

struct Triangle {

};



void DrawRotatedLineGrid(Vector3 centerPos, Vector2 dimensions, Vector2 spacing, Color color, Matrix rotation){
    if (spacing.x <= 0.0f || spacing.y <= 0.0f){
        spacing.x = 0.1f;
        spacing.y = 0.1f;
    }
    Vector2 startPos = {centerPos.x - dimensions.x, centerPos.y - dimensions.y};
    Vector2 stopPos = {centerPos.x + dimensions.x, centerPos.y + dimensions.y};
    float depth = centerPos.z;
    float xStep = dimensions.x/spacing.x;
    float yStep = dimensions.y/spacing.y;
    // printf("xStep: %.2f yStep: %.2f\n", xStep, yStep);
    // Draw Horizontal
    for (float x = startPos.x; x <= stopPos.x + xStep; x += xStep) {
        Vector3 p1 = {x, startPos.y, depth};
        Vector3 p2 = {x, stopPos.y, depth};
        DrawLine3D(p1, p2, color);
    }
    // Draw Vertical
    for (float y = startPos.y; y <= stopPos.y + yStep; y += yStep) {
        Vector3 p1 = {startPos.x, y, depth};
        Vector3 p2 = {stopPos.x, y, depth};
        DrawLine3D(p1, p2, color);
    }
}

void DrawLineGrid(Vector3 centerPos, Vector2 dimensions, Vector2 spacing, Color color){
    Matrix mat = {0.0f, 0.0f, 0.0f, 0.0f};
    DrawRotatedLineGrid(centerPos, dimensions, spacing, color, mat);
}

void DrawCheckeredSphere(Vector3 centerPos, int rings, int slices, Vector3 rotation, Color c1, Color c2){
    // 
}

void CS_DrawPanel3D(Vector3 centerPos, Vector2 dimension, Matrix rotationMatrix, Color color, bool drawLines){
    Vector3 points[4];
    points[0] = Vector3Transform((Vector3){centerPos.x - dimension.x, centerPos.y - centerPos.y, centerPos.z}, rotationMatrix);
    points[1] = Vector3Transform((Vector3){centerPos.x - dimension.x, centerPos.y + centerPos.y, centerPos.z}, rotationMatrix);
    points[2] = Vector3Transform((Vector3){centerPos.x + dimension.x, centerPos.y - centerPos.y, centerPos.z}, rotationMatrix);
    points[3] = Vector3Transform((Vector3){centerPos.x + dimension.x, centerPos.y + centerPos.y, centerPos.z}, rotationMatrix);

    DrawTriangle3D(points[0], points[1], points[2],  color);
    DrawTriangle3D(points[1], points[2], points[3],  color);

    if (drawLines) {
        DrawLine3D(points[0], points[1], BLACK);
        DrawLine3D(points[1], points[2], BLACK);
        DrawLine3D(points[2], points[3], BLACK);
        DrawLine3D(points[3], points[1], BLACK);
        DrawLine3D(points[0], points[2], BLACK);
    }
}

int main(){
    // Init window
    InitWindow(width, height, title);
    if (!IsWindowReady()) {
        printf("Window failed to initialize correctly.");
    }
    SetTargetFPS(FPS);
    ClearBackground(GRAY);

    const Vector3 sceneOrigin = {0.0f, 0.0f, 0.0f}; // Everything will be relative to the scene origin
    // Setup camera
    const Vector3 cameraPos = {0.0f, 0.0f, -5.0f};
    Camera camera = {
        cameraPos,// Position
        sceneOrigin, // Target/LookAt
        {0.0f, 1.0f, 0.0f}, // Up
        45.0f,
        CAMERA_PERSPECTIVE
    };
    //define grid position & dimensions
    const Vector3 gridPos = Vector3Add(sceneOrigin, (Vector3){0.0f, 0.5f, 1.0f});
    const Vector2 gridDims = {1.5f, 1.0f};
    const Vector2 gridSpacing = {7.5f, 5.0f};

    //define sphere dimensions
    const float sphereRadius = 0.4f;
    const int sphereRings = 10;
    const int sphereSlices = 8;
    Vector3 spherePos = Vector3Add(sceneOrigin, (Vector3){0.0f, 0.0f, 0.0f});
    
    // Sphere Mesh

    const Vector3 panelPos = {1.0f, 1.0f, 0.0f};
    const Vector2 panelDims = {0.05f, 0.05f};
    float angle = 0.0f;

    while (!WindowShouldClose()) {
        // if (gcurr <= glower || gcurr >= gupper){
        //     gstep *= -1.0f;
        // }
        BeginDrawing();
            ClearBackground(GRAY);
            BeginMode3D(camera);
                DrawLineGrid(
                    gridPos,
                    gridDims,
                    gridSpacing,
                    MAGENTA
                );

                DrawSphereEx(spherePos, sphereRadius, sphereRings, sphereSlices, RED);
                DrawSphereWires(spherePos, sphereRadius, sphereRings, sphereSlices, WHITE);
                // DrawSphere(ballPos, 50.0f, RED);
                // DrawPoint3D(ballPos, RED);
                
                CS_DrawPanel3D(panelPos, panelDims, MatrixRotateY(angle*DEG2RAD), BLUE, true);
                
                angle += 0.5f; 
            EndMode3D();
        EndDrawing();
        // gcurr += gstep;
    }
    CloseWindow();
}