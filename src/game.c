#include "raylib.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct wall{
    float position_x;
    float position_y;
    int width;
    int height;
    int *cell;
    int cell_size;
}Wall;

//Rectlist for collision check
typedef struct RectList{
    Rectangle *rect;
    int size;
}RectList;

void GameInit();
void GameUpdate();
void GameDraw();
void GameLoop(){GameUpdate(); GameDraw();}
void Reset();

void DrawTileGrid();
void DrawTileMap();
void DrawCoins();
void DrawPlayer();
void DrawScoreText();

void UpdateScreen();
void UpdatePlayer();
void UpdateCoin();

void RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity);
Rectangle GetCollisionRectangle(Rectangle* rect, Vector2* size);
RectList*   RectangleListFromTiles(Rectangle *rect, Wall *wall);
void HandleRectangleTileCollision(Rectangle *rectangle, Vector2 *velocity, RectList *tileList);



//game variables
#define MAP_W 20
#define MAP_H 12
int screenWidth = 32*MAP_W;
int screenHeight = 32*MAP_H;
const int gameWidth = 32*MAP_W;
const int gameHeight = 32*MAP_H;

Rectangle player = {32.0f * 2, 32.0f * 8, 32.0f, 32.0f};

//Coin Varaibles
#define COIN_COUNT 10
Rectangle coins[COIN_COUNT] = {0};
bool visible[COIN_COUNT] = {0}; //making coins visible invisible when the player touches
int points = 0;
int time = 0;  // For animation

Wall map;
int tiles[] = {
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
            1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
            1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
            1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
            1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,
            1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,
            1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
            1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
            1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
            1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

int main(void){
    //InitWindow(640, 384, "classic game: platformer");
    GameInit();
    SetTargetFPS(60);
    while (!WindowShouldClose()){
        GameLoop();
    }
    //CloseWindow();
    return 0;
}

void GameInit() {
    map.position_x = 0.0f;
    map.position_y = 0.0f;
    map.width = MAP_W;
    map.height = MAP_H;
    map.cell_size = 32;
    map.cell = tiles;
    Reset();
}

void Reset(){
    const float size = 32.0f;
    player = (Rectangle){size * 2, size * 6, size, size};
    points = 0;
    time = 0;
    
    coins[0] = (Rectangle){size * 1.5f, size * 8, 10.0f, 10.0f};
    coins[1] = (Rectangle){size * 3.5f, size * 6, 10.0f, 10.0f};
    coins[2] = (Rectangle){size * 4.5f, size * 6, 10.0f, 10.0f};
    coins[3] = (Rectangle){size * 5.5f, size * 6, 10.0f, 10.0f};
    coins[4] = (Rectangle){size * 8.5f, size * 3, 10.0f, 10.0f};
    coins[5] = (Rectangle){size * 9.5f, size * 3, 10.0f, 10.0f};
    coins[6] = (Rectangle){size * 10.5f, size * 3, 10.0f, 10.0f};
    coins[7] = (Rectangle){size * 14.5f, size * 4, 10.0f, 10.0f};
    coins[8] = (Rectangle){size * 15.5f, size * 4, 10.0f, 10.0f};
    coins[9] = (Rectangle){size * 17.5f, size * 2, 10.0f, 10.0f};
    
    for (int i = 0; i < COIN_COUNT; i++){
        visible[i] = true;
    }
}
void GameUpdate(){

    UpdatePlayer();
    UpdateCoin();

}
void UpdatePlayer(){
    const float MAX_SPEED = 6.0f;
    const float ACCELERATION = 0.1f;
    static int x_input = 0;
    static int y_input = 0;
    static Vector2 velocity = {0};
    static Vector2 previousVelocity = {0};
    
    
    // INPUT
    x_input = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    y_input = (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W));
    
    // HORIZONTAL SPEED
    velocity.x += (x_input * MAX_SPEED - velocity.x) * ACCELERATION;
    if (velocity.x < -MAX_SPEED){
        velocity.x = -MAX_SPEED;
    }
    else if (velocity.x > MAX_SPEED){
        velocity.x = MAX_SPEED;
    }

        // TOP-DOWN
        velocity.y += (y_input * MAX_SPEED - velocity.y) * ACCELERATION;
        if (velocity.y < -MAX_SPEED){
            velocity.y = -MAX_SPEED;
        }
        else if (velocity.y > MAX_SPEED){
            velocity.y = MAX_SPEED;
        }

    RectangleCollisionUpdate(&player, &velocity);
    //player.x += velocity.x;
    //player.y += velocity.y;
}

void UpdateCoin(){
    for (int i = 0; i < COIN_COUNT; i++){
        if (visible[i]){
            if (CheckCollisionRecs(coins[i], player)){
                visible[i] = false;
                points += 1;
            }
        }
    }
    
    if (IsKeyPressed(KEY_ENTER)){
        Reset();
    }
}

void GameDraw(){
    BeginDrawing();
        DrawRectangle(0, 0, gameWidth, gameHeight, SKYBLUE); // Background
        DrawTileMap();
        DrawTileGrid();
        DrawScoreText();
        DrawCoins();
        DrawPlayer();
        ClearBackground(BLACK);
    EndDrawing();
}
void DrawTileMap(){
    for (int y = 0; y < map.height; y++){
        for (int x = 0; x < map.width; x++){
            int cell_index = x + y * map.width;
            int tile_type = map.cell[cell_index];
            if (tile_type){
                float cell_x = (map.position_x + map.cell_size * x);
                float cell_y = (map.position_y + map.cell_size * y);
                DrawRectangle((int)cell_x, (int)cell_y, map.cell_size, map.cell_size, LIME);
                // check tile above
                if (cell_index - map.width >= 0 && !map.cell[cell_index - map.width]){
                    DrawLineEx((Vector2){cell_x, cell_y + 3}, (Vector2){cell_x + map.cell_size, cell_y + 3}, 6.0f, GREEN);
                }
            }
        }
    }
}

void DrawTileGrid(){
     Color gridColor = (Color){255,255,255,25};
    
    for (int y = 0; y < map.height + 1; y++){
        int startX = map.position_x;
        int endX = map.position_x + map.width * map.cell_size;
        int currentY = map.position_y + map.cell_size * y;
        DrawLine(startX, currentY, endX, currentY, gridColor);
    }
    for (int x = 0; x < map.width + 1; x++){
        int startY = map.position_y;
        int endY = map.position_y + map.height * map.cell_size;
        int currentX = map.position_x + map.cell_size * x;
        DrawLine(currentX, startY, currentX, endY, gridColor);
    }
}

void DrawPlayer(){
        DrawRectangle((int)player.x, (int)player.y, (int)player.width, (int)player.height, WHITE);
        DrawRectangleLinesEx(player, 2, BLACK);
        
        //Drawing Face
        static int dirX = 0;
        dirX = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)) * 4;
        Vector2 leftFootPos = (Vector2){player.x + 12 + dirX, player.y + 4};
        Vector2 rightFootPos = (Vector2){player.x + 20 + dirX, player.y + 4};
        Vector2 leftHandPos = leftFootPos;
        leftHandPos.y += 8;
        Vector2 rightHandPos = rightFootPos;
        rightHandPos.y += 8;
        DrawLineEx(leftFootPos, leftHandPos, 2.0f, BLACK);
        DrawLineEx(rightFootPos, rightHandPos, 2.0f, BLACK);
}

void DrawCoins(){
    time += 1;
    
    for (int i = 0; i < COIN_COUNT; i++){
        if (visible[i]){
            Rectangle coin = coins[i];
            //sin function for coing animation effect
            float y = (float)sin(2 * PI * (time / 60.0f * 0.5) + (coin.x * 5)) * 4; // pseudo random offset floating
            float x = (float)sin(2 * PI * (time / 60.0f * 2)) * 4;
            DrawRectangle((int)(coin.x + 4 + x * 0.5), (int)(coin.y + y), (int)(coin.width - 4 - x), (int)coin.height, GOLD);
        }
    }
}

void DrawScoreText(){
    const char *text;
    if (points == COIN_COUNT){
        text = TextFormat("Pres 'ENTER' to restart!");
    }
    else{
        text = TextFormat("Score: %d", points);
    }
    
    const int size = 24;
    int x_pos = gameWidth /2 - MeasureText(text, size) / 2;
    int y_pos = 48;
    
    DrawText(text, x_pos, y_pos+1, size, BLACK);
    DrawText(text, x_pos, y_pos, size, WHITE);
    
}
void RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity){
    Rectangle colArea = GetCollisionRectangle(rect, velocity);
    RectList *tiles = RectangleListFromTiles(&colArea, &map);
    
    HandleRectangleTileCollision(rect, velocity, tiles);
    // free allocated RectList memory
    MemFree(tiles->rect);
    MemFree(tiles);
}
Rectangle GetCollisionRectangle(Rectangle* rect, Vector2* size) {
    Rectangle collisionRect;
    if (size->x > 0) {
        collisionRect.x = rect->x;
        collisionRect.width = rect->width + size->x;
    } else {
        collisionRect.x = rect->x + size->x;
        collisionRect.width = rect->width - size->x;
    }
    if (size->y > 0) {
        collisionRect.y = rect->y;
        collisionRect.height = rect->height + size->y;
    } else {
        collisionRect.y = rect->y + size->y;
        collisionRect.height = rect->height - size->y;
    }
    return collisionRect;
}
RectList* RectangleListFromTiles(Rectangle *rect, Wall *grid){
    float offsetX = rect->x - grid->position_x;
    float offsetY = rect->y - grid->position_y;
    float offsetXPlusWidth = rect->x - grid->position_x + rect->width;
    float offsetYPlusHeight = rect->y - grid->position_y + rect->height;
    
    // compensate for flooring
    if (offsetX < 0.0f){offsetX -= grid->cell_size;}
    if (offsetY < 0.0f){offsetY -= grid->cell_size;}
    if (offsetXPlusWidth < 0.0f){offsetXPlusWidth -= grid->cell_size;}
    if (offsetYPlusHeight < 0.0f){offsetYPlusHeight -= grid->cell_size;}
    
    // grid coordinates
    int startX = (int)(offsetX / grid->cell_size);
    int startY = (int)(offsetY / grid->cell_size);
    int sizeX = (int)(offsetXPlusWidth / grid->cell_size) - startX + 1;
    int sizeY = (int)(offsetYPlusHeight / grid->cell_size) - startY + 1;
    
    // Allocate memory for rectList
RectList *rectList = malloc(sizeof(RectList));

// Allocate memory for rectList->rect
rectList->rect = malloc(sizeof(Rectangle) * sizeX * sizeY);

// Check for allocation errors
if (rectList == NULL || rectList->rect == NULL) {
    printf("Error: Memory allocation failed.\n");
    exit(EXIT_FAILURE);
}
    rectList->size = 0;
    
    for (int y = startY; y < startY + sizeY; y++){
        if (y >= 0 && y < grid->height){
            for (int x = startX; x < startX + sizeX; x++){
                if (x >= 0 && x < grid->width){
                    int tile = grid->cell[x + y * grid->width];
                    if (tile){
                        rectList->rect[rectList->size] = (Rectangle){
                            grid->position_x + x * grid->cell_size,
                            grid->position_y + y * grid->cell_size,
                            grid->cell_size,
                            grid->cell_size
                        };
                        rectList->size += 1;
                    }
                }
            }
        }
    }
    return rectList;
}
void HandleRectangleTileCollision(Rectangle *rectangle, Vector2 *velocity, RectList *tileList){
    Rectangle *currentRect = rectangle;
    Rectangle *tile;
    Rectangle testRect = (Rectangle){currentRect->x + velocity->x, currentRect->y, currentRect->width, currentRect->height};
    
    // Solve X axis collisions
    for (int i = 0; i < tileList->size; i++){
        tile = &tileList->rect[i]; // Get next tile
        if (CheckCollisionRecs(testRect, *tile)) {
            // Colliding to the right
            if (velocity->x > 0.0f) {
                // Adjust velocity
                velocity->x = (tile->x - currentRect->width) - currentRect->x;
            }
            // Colliding to the left
            else if (velocity->x < 0.0f) {
                velocity->x = (tile->x + tile->width) - currentRect->x;
            }
        }
    }
    // Set testRect to resolved X position
    testRect.x = currentRect->x + velocity->x;
    
    // Move on Y axis
    // Set testRect to test Y position
    testRect.y += velocity->y;
    for (int i = 0; i < tileList->size; i++){
        tile = &tileList->rect[i];
        if (CheckCollisionRecs(testRect, *tile)) {
            // Colliding downwards
            if (velocity->y > 0.0f) {
                velocity->y = (tile->y - currentRect->height) - currentRect->y;
            }
            // Colliding upwards
            else if (velocity->y < 0.0f) {
                velocity->y = (tile->y + tile->height) - currentRect->y;
            }
        }
    }
    
    // Update rectangle position with resolved velocity
    rectangle->x += velocity->x;
    rectangle->y += velocity->y;
}
