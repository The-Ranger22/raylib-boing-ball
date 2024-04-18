#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#define MAX_SPHERE_RES 512

const int height = 800;
const int width  = 800; 
const char* title = "BoingBall";
const int FPS = 60;
const float GRAVITY = 0.001;


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


float map_into_range(float inp, Vector2 input_range, Vector2 output_range){
    return output_range.x + ((output_range.y - output_range.x)/(input_range.y - input_range.x)) * (inp - input_range.x);
}

int CS_BuildSphere(Vector3 * points, int size, int degrees, int rings){
    if (360 % degrees != 0) {
        printf("Value `%d` is not a factor of 360. Returning without drawing sphere.\n", degrees);
        return -1;
    }
    if ((360/degrees)*(rings) > MAX_SPHERE_RES) {
        printf("Number of points generated too high. Returning without drawing sphere.\n");
        return -2;
    }
    unsigned int c = 0;
    unsigned int sectors = 360/degrees;
    unsigned int total_num_points = sectors * rings;
    if (total_num_points > size){
        printf("Vector3 Buffer is not big enought.");
        return -3;
    }
    float step = 1.0f/((float)(rings));
    for (int i = 0; i < rings; i++) {
        float v = map_into_range((float) i, (Vector2){0.0f, (float)(rings-1)}, (Vector2){0.0f, 1.0f});
        float x = cos((v * 180 - 90) * DEG2RAD);
        float y = sin((v * 180 - 90) * DEG2RAD);
        Vector3 p = {x, y, 0.0f};
        points[c] = p;
        points[total_num_points+c] = p;
        c++;
        printf("rings: %d, step: %.6f, v: %.6f, x: %.6f, y: %.6f\n", rings, step, v, x, y);
    }
    for (unsigned int idx = 1; idx < sectors; idx++){
        // printf("[------]\n");
        for (unsigned jdx = 0; jdx < rings; jdx++){
            // printf("indexer: %d\n", idx*rings + jdx);
            // printf("Degrees: %.2f\n", degrees*(float) idx);
            points[idx*rings + jdx] = Vector3RotateByAxisAngle(points[jdx], (Vector3){0.0f, 0.1f, 0.0f}, (degrees) * (float) idx * DEG2RAD);
        }
    }
    for (unsigned jdx = 0; jdx < rings; jdx++){
        // printf("indexer: %d\n", idx*rings + jdx);
        points[total_num_points + jdx] = points[jdx];
    }
    return sectors;
}

Vector3 CS_TransformRotateScaleVec3(Vector3 point, Vector3 centerPos, Matrix transformMat, float scale){
    Vector3 transformedPoint = Vector3Scale(point, scale);
    transformedPoint = Vector3Transform(transformedPoint, transformMat);
    transformedPoint = Vector3Add(transformedPoint, centerPos);
    return transformedPoint;
}

void CS_DrawSphere(const Vector3 * points, int sectors, int rings, float scale, Vector3 centerPos, Matrix rotationMat, Color color){
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
            Vector3 p1, p2, p3, p4;
            
            p1 = CS_TransformRotateScaleVec3(points[adx*rings + bdx]    , centerPos, rotationMat, scale);
            p2 = CS_TransformRotateScaleVec3(points[(adx+1)*rings + bdx], centerPos, rotationMat, scale);
            p3 = CS_TransformRotateScaleVec3(points[adx*rings + bdx + 1], centerPos, rotationMat, scale);
            p4 = CS_TransformRotateScaleVec3(points[(adx+1)*rings+bdx+1], centerPos, rotationMat, scale);
            
            DrawTriangle3D(p1, p2, p3, aColor);
            DrawTriangle3D(p3, p2, p4, aColor);
            
            DrawLine3D(p1, p2, BLACK);
            DrawLine3D(p1, p3, BLACK);
            DrawLine3D(p3, p4, BLACK);
            DrawLine3D(p4, p2, BLACK);

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


void processPhysics(Vector3* pos, Vector3* velocity, Vector3* rotation, float mass, BoundingBox obj, BoundingBox top, BoundingBox bottom, BoundingBox left, BoundingBox right){
    if(CheckCollisionBoxes(obj, top) || CheckCollisionBoxes(obj, bottom)){
        velocity->y = -(velocity->y);
    }
    if(CheckCollisionBoxes(obj, left) || CheckCollisionBoxes(obj, right)){
        velocity->x = -(velocity->x);
        rotation->x = -(rotation->x);
        rotation->y = -(rotation->y);
        rotation->z = -(rotation->z);
    }
    pos->x += velocity->x;
    pos->y += velocity->y;
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
    Vector3 sphereBuff[512];
    const float sphereRadius = 0.4f;
    const int sphereRings = 12;
    const int sphereDegrees = 15;
    const int sphereSectors = CS_BuildSphere(sphereBuff, 512, sphereDegrees, sphereRings);
    const float sphereScale = 0.35f;
    Vector3 spherePos = Vector3Add(sceneOrigin, (Vector3){-0.5f, 0.0f, 0.0f});
    for (unsigned int idx = 0; idx < sphereRings*2; idx++){
        printf("[%3d] x: %.2f y: %.2f z: %.2f\n", idx, sphereBuff[idx].x, sphereBuff[idx].y, sphereBuff[idx].z);
    }

    Vector3 movementVector = {0.005f, 0.005f, 0.0f};
    Quaternion bounds = {-1.0f, -1.0f, 5.0f, 5.0f};
    BoundingBox sphereBB;
    BoundingBox bb_bottom = {
        (Vector3){-1.2f, -0.41f, 0.0f},
        (Vector3){ 1.2f, -0.41f, 0.0f}
    };
    BoundingBox bb_top = {
        (Vector3){-1.2f, 0.9f, 0.0f},
        (Vector3){ 1.2f, 0.9f, 0.0f}
    };
    BoundingBox bb_left = {
        (Vector3){1.2f, -0.41f, 0.0f},
        (Vector3){1.2f, 0.9f, 0.0f}
    };
    BoundingBox bb_right = {
        (Vector3){-1.2f, -0.41f, 0.0f},
        (Vector3){-1.2f, 0.9f, 0.0f}
    };


    

    // Sphere Mesh
    
    const Vector3 panelPos = {1.0f, 1.0f, 0.0f};
    const Vector2 panelDims = {0.3f, 0.3f};
    const Vector3 rotationVec = {0.0f, 0.1f, 0.0f};
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
                // CS_DrawPanel3D(panelPos, panelDims, MatrixRotate(rotationVec, angle*DEG2RAD), BLUE, true);
                sphereBB = (BoundingBox){
                    (Vector3){spherePos.x-sphereScale, spherePos.y-sphereScale, 0.0f},
                    (Vector3){spherePos.x+sphereScale, spherePos.y+sphereScale, 0.0f}
                };
                processPhysics(&spherePos, &movementVector, &rotationVec, 1.0f, sphereBB, bb_top, bb_bottom, bb_left, bb_right);
                DrawBoundingBox(sphereBB, (CheckCollisionBoxes(bb_top, sphereBB) ? RED : GREEN));
                DrawBoundingBox(bb_top, GREEN);
                DrawBoundingBox(bb_bottom, GREEN);
                DrawBoundingBox(bb_left, GREEN);
                DrawBoundingBox(bb_right, GREEN);
                CS_DrawSphere(sphereBuff, sphereSectors, sphereRings, sphereScale, spherePos, MatrixRotate(rotationVec, angle*DEG2RAD), GREEN);
                
                //DrawTriangleStrip3D(testPoints, 4, RED); 
                angle += 1.0f;
            EndMode3D();
        EndDrawing();
        // gcurr += gstep;
    }
    CloseWindow();
}