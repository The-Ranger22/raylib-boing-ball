#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#define MAX_SPHERE_RES 256

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
    points[0] = Vector3Add(Vector3Transform((Vector3){-dimension.x, -dimension.y, 0.0f}, rotationMatrix), centerPos);
    points[1] = Vector3Add(Vector3Transform((Vector3){-dimension.x, +dimension.y, 0.0f}, rotationMatrix), centerPos);
    points[2] = Vector3Add(Vector3Transform((Vector3){+dimension.x, -dimension.y, 0.0f}, rotationMatrix), centerPos);
    points[3] = Vector3Add(Vector3Transform((Vector3){+dimension.x, +dimension.y, 0.0f}, rotationMatrix), centerPos);
    Vector3 pointsReversed[4];
    for (int i = 0; i < 4; i++){
        pointsReversed[i] = points[3-i];
    }
    // DrawTriangle3D(points[0], points[1], points[2],  color);
    // DrawTriangle3D(points[2], points[0], points[1],  color);
    // DrawTriangle3D(points[1], points[2], points[3],  color);
    // DrawTriangle3D(points[3], points[1], points[2],  color);

    DrawTriangleStrip3D(points, 4, color);
    DrawTriangleStrip3D(pointsReversed, 4, color);

    if (drawLines) {
        DrawLine3D(points[0], points[1], BLACK);
        DrawLine3D(points[1], points[2], BLACK);
        DrawLine3D(points[2], points[3], BLACK);
        DrawLine3D(points[3], points[1], BLACK);
        DrawLine3D(points[0], points[2], BLACK);
    }
}

void CS_DrawSphere(int degrees, int rings, float scale, Vector3 centerPos, Matrix rotationMat, Color color){
    Vector3 points[MAX_SPHERE_RES];
    if (360 % degrees != 0) {
        printf("Value `%d` is not a factor of 360. Returning without drawing sphere.\n", degrees);
        return;
    }
    if ((360/degrees)*rings > MAX_SPHERE_RES) {
        printf("Number of points generated too high. Returning without drawing sphere.\n");
    }

    /*
        [
            a1, b1, c1, d1,
            a2, b2, c2, d2,
            a3, b3, c3, d3,
            a4, b4, c4, d4
        ]
        vs.
        [
            a1, a2, a3, a4,
            b1, b2, b3, b4,
            c1, c2, c3, c4,
            d1, d2, d3, d4
        ]
    */
    unsigned int c = 0;
    unsigned int sectors = 360/degrees;
    unsigned int total_num_points = sectors * rings;
    float step = 1.0f/(float)rings;
    for (float v = 0; v <= 1; v += step) {
        float x = cos(v*PI);
        float y = sin(v*PI);
        points[c] = (Vector3){x, y, 0.0f};
        c++;
    }
    for (unsigned int idx = 1; idx < sectors; idx++){
        for (unsigned jdx = 0; jdx < rings; jdx++){
            // printf("indexer: %d\n", idx*rings + jdx);
            points[idx*rings + jdx] = Vector3RotateByAxisAngle(points[jdx], (Vector3){0.0f, 0.1f, 0.0f}, degrees * (float) idx * DEG2RAD);
        }
    }
    // printf("total_points: %d\n", total_num_points);
    // for 
    // Draw Points
    for (unsigned int adx = 0; adx < total_num_points; adx++){
        DrawPoint3D(points[adx], color);
    }
    

//    for () 
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
    const Vector2 panelDims = {0.3f, 0.3f};
    const Vector3 rotationVec = {0.1f, 0.9f, 0.0f};
    float angle = 0.0f;


    Vector3 testPoints[] = {
        {1.5f, 1.5f, 0.0f},
        {1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 0.0f},
        {0.5f, 0.5f, 0.0f}
    };

    while (!WindowShouldClose()) {
        // if (gcurr <= glower || gcurr >= gupper){
        //     gstep *= -1.0f;
        // }
        BeginDrawing();
            ClearBackground(GRAY);
            DrawFPS(50, 50);
            BeginMode3D(camera);
                DrawLineGrid(
                    gridPos,
                    gridDims,
                    gridSpacing,
                    MAGENTA
                );
                // DrawSphereEx(spherePos, sphereRadius, sphereRings, sphereSlices, RED);
                // DrawSphereWires(spherePos, sphereRadius, sphereRings, sphereSlices, WHITE);
                // DrawSphere(ballPos, 50.0f, RED);
                // DrawPoint3D(ballPos, RED);
                CS_DrawPanel3D(panelPos, panelDims, MatrixRotate(rotationVec, angle*DEG2RAD), BLUE, true);
                CS_DrawSphere(30, 8, 1.0f, spherePos, MatrixRotate(rotationVec, angle*DEG2RAD), GREEN);
                //DrawTriangleStrip3D(testPoints, 4, RED); 
                angle += 1.0f; 
            EndMode3D();
        EndDrawing();
        // gcurr += gstep;
    }
    CloseWindow();
}