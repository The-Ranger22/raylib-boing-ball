#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#define MAX_SPHERE_RES 512

const int height = 800;
const int width  = 800; 
const char* title = "BoingBall";
const int FPS = 60;
const float GRAVITY = 0.002;


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
    for (float x = startPos.x; x < stopPos.x + xStep; x += xStep) {
        Vector3 p1 = {x, startPos.y, depth};
        Vector3 p2 = {x, stopPos.y, depth};
        DrawLine3D(p1, p2, color);
    }
    // Draw Vertical
    for (float y = startPos.y; y < stopPos.y + yStep; y += yStep) {
        Vector3 p1 = {startPos.x, y, depth};
        Vector3 p2 = {stopPos.x, y, depth};
        DrawLine3D(p1, p2, color);
    }
}

void DrawLineGrid(Vector3 centerPos, Vector2 dimensions, Vector2 spacing, Color color){
    Matrix mat = {0.0f, 0.0f, 0.0f, 0.0f};
    DrawRotatedLineGrid(centerPos, dimensions, spacing, color, mat);
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

Vector3 CS_TransformRotateScaleVec3(Vector3 point, Vector3 centerPos, Matrix localTransformMat, float scale, Vector3 axisRotation, float axisRotationAngle){
    Vector3 transformedPoint = Vector3Scale(point, scale);
    transformedPoint = Vector3Transform(transformedPoint, localTransformMat);
    transformedPoint = Vector3RotateByAxisAngle(transformedPoint, axisRotation, axisRotationAngle);
    transformedPoint = Vector3Add(transformedPoint, centerPos);
    return transformedPoint;
}

void CS_DrawSphere(const Vector3 * points, int sectors, int rings, float scale, Vector3 centerPos, Matrix localRotationMat, Vector3 rotationAxis, float rotationAngle, Color color){
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
            
            p1 = CS_TransformRotateScaleVec3(points[adx*rings + bdx], centerPos, localRotationMat, scale, rotationAxis, rotationAngle);
            p2 = CS_TransformRotateScaleVec3(points[(adx+1)*rings + bdx], centerPos, localRotationMat, scale, rotationAxis, rotationAngle);
            p3 = CS_TransformRotateScaleVec3(points[adx*rings + bdx + 1], centerPos, localRotationMat, scale, rotationAxis, rotationAngle);
            p4 = CS_TransformRotateScaleVec3(points[(adx+1)*rings+bdx+1], centerPos, localRotationMat, scale, rotationAxis, rotationAngle);
            
            DrawTriangle3D(p1, p2, p3, aColor);
            DrawTriangle3D(p3, p2, p4, aColor);
            
            // DrawLine3D(p1, p2, BLACK);
            // DrawLine3D(p1, p3, BLACK);
            // DrawLine3D(p3, p4, BLACK);
            // DrawLine3D(p4, p2, BLACK);

        }
    }
}


int processPhysics(Vector3* pos, Vector3* velocity, float refreshedYVelocity, Vector3* rotation, float mass, float* rotationStep, BoundingBox obj, BoundingBox top, BoundingBox bottom, BoundingBox left, BoundingBox right){
    int impact = 0;
    if(CheckCollisionBoxes(obj, top)){
        velocity->y = -0.001;
        impact = 1;
    } else if (CheckCollisionBoxes(obj, bottom)){
        velocity->y = refreshedYVelocity;
        impact = 1;
    }
    if(CheckCollisionBoxes(obj, left) || CheckCollisionBoxes(obj, right)){
        velocity->x = -(velocity->x);
        *rotationStep = -(*rotationStep);
        impact = 1;
    }
    pos->x += velocity->x;
    pos->y += velocity->y;
    velocity->y -= GRAVITY;
    return impact;
}

int main(){
    // Init window
    InitWindow(width, height, title);
    InitAudioDevice();

    char thudFilename[] = "./resources/windy-thud.wav";
    Sound thud = LoadSound(thudFilename);
    SetSoundVolume(thud, 0.5f);
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
    camera.fovy = 10.0f;
    camera.projection = CAMERA_ORTHOGRAPHIC;
    camera.position = (Vector3){0.0f, 0.0f, -1.75f};
    //define grid position & dimensions
    const Vector3 gridPos = Vector3Add(sceneOrigin, (Vector3){0.0f, 0.5f, 1.0f});
    const Vector2 gridDims = {4.0f, 3.0f};
    const Vector2 gridSpacing = {7.5f, 5.0f};

    //define sphere dimensions
    Vector3 sphereBuff[512];
    const float sphereRadius = 0.4f;
    const int sphereRings = 12;
    const int sphereDegrees = 15;
    const int sphereSectors = CS_BuildSphere(sphereBuff, 512, sphereDegrees, sphereRings);
    const float sphereScale = 1.25f;
    Vector3 spherePos = Vector3Add(sceneOrigin, (Vector3){-0.5f, 0.0f, 0.0f});

    Vector3 movementVector = {0.02f, 0.02f, 0.0f};
    Quaternion bounds;
    bounds.w = -4.0f;
    bounds.x =  4.0f;
    bounds.y = -3.0f + 0.5f;
    bounds.z =  3.0f + 0.5f;
    BoundingBox sphereBB;
    BoundingBox bb_bottom = {
        (Vector3){bounds.w, bounds.y, 0.0f},
        (Vector3){bounds.x, bounds.y, 0.0f}
    };
    BoundingBox bb_top = {
        (Vector3){bounds.w, bounds.z, 0.0f},
        (Vector3){bounds.x, bounds.z, 0.0f}
    };
    BoundingBox bb_left = {
        (Vector3){bounds.x, bounds.y, 0.0f},
        (Vector3){bounds.x, bounds.z, 0.0f}
    };
    BoundingBox bb_right = {
        (Vector3){bounds.w, bounds.y, 0.0f},
        (Vector3){bounds.w, bounds.z, 0.0f}
    };
    // Sphere Mesh
    
    const Vector3 panelPos = {1.0f, 1.0f, 0.0f};
    const Vector2 panelDims = {0.3f, 0.3f};
    Vector3 rotationVec = {0.0f, 0.1f, 0.0f};
    float angle = 0.0f;

    Vector3 testPoints[] = {
        {1.5f, 1.5f, 0.0f},
        {1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 0.0f},
        {0.5f, 0.5f, 0.0f}
    };
    float rotationStep = 1.5f;
    
    Vector3 axisRotation = {0.0f, 0.0f, 1.0f};
    float axisRotationAngle = 15.0f*DEG2RAD;
    bool dispFPS = false;
    bool isPaused = false;
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_TAB)) ToggleFullscreen();
        if (IsKeyPressed(KEY_LEFT_ALT)) dispFPS = !dispFPS;
        if (IsKeyPressed(KEY_SPACE)) isPaused = !isPaused;
        BeginDrawing();
            ClearBackground(GRAY);
            if (dispFPS) DrawFPS(50, 50);
            char posText[32];
            sprintf(posText, "X: %.3f Y: %.3f");
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
                if (!isPaused){
                    int impact = processPhysics(&spherePos, &movementVector, 0.09f, &rotationVec, 1.0f, &rotationStep, sphereBB, bb_top, bb_bottom, bb_left, bb_right);
                    if (impact) {
                        PlaySound(thud);
                    }
                    angle += rotationStep;
                }
                // DrawBoundingBox(sphereBB, (CheckCollisionBoxes(bb_top, sphereBB) ? RED : GREEN));
                // DrawBoundingBox(bb_top, GREEN);
                // DrawBoundingBox(bb_bottom, GREEN);
                // DrawBoundingBox(bb_left, GREEN);
                // DrawBoundingBox(bb_right, GREEN);
                CS_DrawSphere(sphereBuff, sphereSectors, sphereRings, sphereScale, spherePos, MatrixRotate(rotationVec, angle*DEG2RAD), axisRotation, axisRotationAngle, GREEN);
                // DrawText(posText, 50, 50, 16, GREEN);
                //DrawTriangleStrip3D(testPoints, 4, RED); 
            EndMode3D();
        EndDrawing();
        // gcurr += gstep;
    }
    CloseAudioDevice();
    CloseWindow();
}