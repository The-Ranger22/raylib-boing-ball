#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#define MAX_SPHERE_RES 512

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
    for (float v = 0.0f; v <= 1; v += step) {
        float x = cos((v * 180 - 90) * DEG2RAD);
        float y = sin((v * 180 - 90) * DEG2RAD);
        Vector3 p = {x, y, 0.0f};
        points[c] = p;
        points[total_num_points+c] = p;
        c++;
    }
    for (unsigned int idx = 1; idx < sectors; idx++){
        for (unsigned jdx = 0; jdx < rings; jdx++){
            // printf("indexer: %d\n", idx*rings + jdx);
            Vector3 point = Vector3RotateByAxisAngle(points[jdx], (Vector3){0.0f, 0.1f, 0.0f}, (degrees) * (float) idx * DEG2RAD);
            point = Vector3Scale(point, scale);
            point = Vector3Transform(point, rotationMat);
            point = Vector3Add(point, centerPos);
            points[idx*rings + jdx] = point;
        }
    }
    for (unsigned jdx = 0; jdx < rings; jdx++){
        // printf("indexer: %d\n", idx*rings + jdx);
        Vector3 point = points[jdx];
        point = Vector3Scale(point, scale);
        point = Vector3Transform(point, rotationMat);
        point = Vector3Add(point, centerPos);
        points[jdx] = point;
        points[total_num_points + jdx] = point;
    }
    printf("total_points: %d\n", total_num_points);
    // for 
    // Draw Points
    for (unsigned int adx = 0; adx < total_num_points; adx++){
        DrawPoint3D(points[adx], BLACK);
    }
    for (unsigned int adx = 0; adx < sectors; adx++) {
        
        
        for (unsigned int bdx = 0; bdx < rings - 1; bdx++){
            // DrawLine3D(points[adx*sectors+bdx], points[(adx+1)*sectors+bdx])

            /*
                adx*rings + bdx
                adx*rings + bdx + 1
                (adx+1)*rings + bdx 
                (adx+1)*rings + bdx + 1
            */
            Color aColor;
            if (adx % 2 == 0){
                aColor = (adx*rings + bdx) % 2 == 0 ? (RED) : (WHITE);
            } else {
                aColor = (adx*rings + bdx) % 2 == 0 ? (WHITE) : (RED);
            } 

            DrawTriangle3D(points[adx*rings + bdx], points[(adx+1)*rings+bdx], points[adx*rings+bdx+1], aColor);
            DrawTriangle3D(points[(adx)*rings+bdx+1], points[(adx+1)*rings+bdx], points[(adx+1)*rings+bdx+1], aColor);
            
            DrawLine3D(points[adx*rings + bdx], points[(adx+1)*rings+bdx], BLACK);
            DrawLine3D(points[adx*rings + bdx], points[(adx)*rings+bdx + 1], BLACK);
            // DrawLine3D(points[(adx+1)*rings + bdx], points[(adx)*rings+bdx+1], BLACK);
            DrawLine3D(points[adx*rings + bdx+1], points[(adx+1)*rings+bdx+1], BLACK);
            DrawLine3D(points[(adx+1)*rings + bdx + 1], points[(adx+1)*rings+bdx], BLACK);

            // printf("[----]\n");
            // printf("p1: %d p2 %d\n", adx*rings + bdx, (adx+1)*rings + bdx); // E1(n1, n2)
            // printf("p1: %d p2 %d\n", adx*rings + bdx,   (adx)*rings + bdx + 1); // E2(n1, n3)
            // printf("p1: %d p2 %d\n", (adx+1)*rings + bdx, (adx)*rings + bdx + 1); // E3(n2, n3)
            // printf("p1: %d p2 %d\n", adx*rings + bdx + 1, (adx+1)*rings + bdx + 1); // E4(n3, n4)
            // printf("p1: %d p2 %d\n", (adx+1)*rings + bdx + 1, (adx+1)*rings + bdx); // E5(n2, n4)
        }
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
    Vector3 spherePos = Vector3Add(sceneOrigin, (Vector3){-0.5f, 0.0f, 0.0f});
    
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
                CS_DrawSphere(15, 12, 0.5f, spherePos, MatrixRotate(rotationVec, angle*DEG2RAD), GREEN);
                //DrawTriangleStrip3D(testPoints, 4, RED); 
                angle += 1.0f;
            EndMode3D();
        EndDrawing();
        // gcurr += gstep;
    }
    CloseWindow();
}