#include "raylib.h"
#include "raymath.h"
#include<stdlib.h>
#include<stdbool.h>
#include<stdio.h>
#include <math.h>

// Global Variables

#define MAP_W 20
#define MAP_H 12
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 384
#define CHAR_WIDTH 32
#define CHAR_HEIGHT 32

typedef struct Player {
    Texture2D run_animation;
    Texture2D idle_animation;
    Texture2D jump_animation;
    Texture2D double_jump_animation;
    Vector2 position;
    Vector2 velocity;
    float speed;
    bool is_moving;
    bool is_jumping;
} Player;

typedef struct wall{
    float position_x;
    float position_y;
    int width;
    int height;
    int *cell;
    int cell_size;
}Wall;
void DrawTileMap(Texture2D wall);

void DrawTileGrid();

void DrawCoins();

void player_update();

void UpdateCoin();

void Reset();

Wall map;
Player player;

#define COIN_COUNT 106
Rectangle coins[COIN_COUNT] = {0};
bool visible[COIN_COUNT] = {0};
int points = 0;
int time_a = 0;       // For animation 

int tiles[] = {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1,
            1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1,
            1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1,
            1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,
            1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
Rectangle frames[12] = {
            { 0, 0, CHAR_WIDTH, CHAR_HEIGHT },
            { CHAR_WIDTH, 0, CHAR_WIDTH, CHAR_HEIGHT },
            { CHAR_WIDTH*2, 0, CHAR_WIDTH, CHAR_HEIGHT },
            { CHAR_WIDTH*3, 0, CHAR_WIDTH, CHAR_HEIGHT },
            { CHAR_WIDTH*4, 0, CHAR_WIDTH, CHAR_HEIGHT },
            { CHAR_WIDTH*5, 0, CHAR_WIDTH, CHAR_HEIGHT },
            { CHAR_WIDTH*6, 0, CHAR_WIDTH, CHAR_HEIGHT },
            { CHAR_WIDTH*7, 0, CHAR_WIDTH, CHAR_HEIGHT },
            { CHAR_WIDTH*8, 0, CHAR_WIDTH, CHAR_HEIGHT },
            { CHAR_WIDTH*9, 0, CHAR_WIDTH, CHAR_HEIGHT },
            { CHAR_WIDTH*10, 0, CHAR_WIDTH, CHAR_HEIGHT },
            { CHAR_WIDTH*11, 0, CHAR_WIDTH, CHAR_HEIGHT }
};

void DrawStartScreen(bool* game_running)
{
    Music bing_chilling_song = LoadMusicStream("assets/Oogway_Ascends.mp3");
    PlayMusicStream(bing_chilling_song);
    SetMusicVolume(bing_chilling_song, 0.5f);
    bool playing_music = true;

    while (*game_running)
    {
        // Check for quit button press
        if (WindowShouldClose())
        {
            *game_running = false;
            break;
        }

        if (IsKeyPressed(KEY_ENTER)) 
            break;

        if (IsKeyPressed(KEY_SPACE))
        {
            if (playing_music){
                PauseMusicStream(bing_chilling_song);
                playing_music = false;
            }
            else{
                ResumeMusicStream(bing_chilling_song);
                playing_music = true;
            }
        }

        // Update music stream
        UpdateMusicStream(bing_chilling_song);

        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("Press ENTER to start game", GetScreenWidth()/2 - MeasureText("Press ENTER to start game", 20)/2, GetScreenHeight()/2 - 50, 20, WHITE);
        DrawText("Press SPACE to pause/resume music", 10, 420, 20, MAROON);
        DrawText("-Music-", 720, 400, 20, RAYWHITE);
        DrawText("Oogway Ascends by Hams Zimmer", 562, 430, 15, RAYWHITE);
            
        EndDrawing();
    }
}

void game_over(bool* game_running, int score){
    Music game_over_sad_song = LoadMusicStream("assets/game_over.mp3");
    PlayMusicStream(game_over_sad_song);
    SetMusicVolume(game_over_sad_song, 1);
    while(*game_running){
        while(!WindowShouldClose()){
            *game_running = false;
            break;
        }
        UpdateMusicStream(game_over_sad_song);
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("Game Over", GetScreenWidth()/2 - MeasureText("Game Over",30)/2, GetScreenHeight()/2 - 50, 30, WHITE);

        EndDrawing();
    }  
}

int main(void)
{
    // Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sprite Test");
    InitAudioDevice();
    SetTargetFPS(60);

    player.run_animation = LoadTexture("assets/Run(32x32).png");
    player.idle_animation = LoadTexture("assets/Idle(32x32).png");
   Texture2D wall = LoadTexture("assets/Tile_72.png");

// Define animation variables
int currentFrame = 0;
int frameCounter = 0;
int frameSpeed = 5;

// Disable ESC key quit
SetExitKey(KEY_NULL);

bool game_running = true;


bool can_double_jump = false;
int num_jumps = 0;

map.position_x = 0.0f;
map.position_y = 0.0f;
map.width = 20;
map.height = 12;
map.cell_size = 32;
// Allocate memory for the cell array
map.cell = (int*) malloc(sizeof(int) * map.width * map.height);
// Initialize cell values
for (int i = 0; i < map.width * map.height; i++) {
    map.cell[i] = tiles[i]; // Set all cells to 0 initially
}

DrawStartScreen(&game_running);
Reset();

// Main game loop
while (game_running && !WindowShouldClose())
{   
    
    // Update animation frame
    frameCounter++;
    if (frameCounter >= frameSpeed)
    {
        frameCounter = 0;
        currentFrame++;
        if (currentFrame > 11) 
            currentFrame = 0;
    }
    // Draw current animation frame and character position
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTileGrid();  
    DrawTileMap(wall);  
    DrawCoins();
    EndDrawing();
} 

    // Cleanup
    // Free the memory allocated for the cell array
    free(map.cell);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
void Reset(){
    const float s = 32.0f;
    //player = (Rectangle){s * 2, s * 6, s, s};
    points = 0;
    time_a = 0;

    // Place coins on empty tiles
    int count = 0;
    for (int i = 0; i <  MAP_W * MAP_H; i++) {
        if (tiles[i] == 0){
            coins[count] = (Rectangle){(i % MAP_W) * s + s / 2 - 5, (i / MAP_W) * s + s / 2 - 5, 10.0f, 10.0f};
            visible[count] = true;
            count += 1;
            if (count == COIN_COUNT) {
                break;
            }
        }
    }
}

void DrawTileMap(Texture2D wall){
    for (int y = 0; y < map.height; y++){
        for (int x = 0; x < map.width; x++){
            int cell_index = x + y * map.width;
            int tile_type = map.cell[cell_index];
            if (tiles[cell_index] == 1){
                float cell_x = map.position_x + (map.cell_size * x);
                float cell_y = map.position_y + (map.cell_size * y);
                Rectangle dest = {(int)cell_x, (int)cell_y, map.cell_size, map.cell_size};
                DrawTextureRec(wall, dest, (Vector2){cell_x, cell_y}, WHITE);
        }
    }
}
}

void DrawTileGrid(){
     Color gridColor = (Color){0,0,255,255};
    
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

void DrawCoins(){
    for (int i = 0; i < COIN_COUNT; i++){
        if (visible[i]){
            bool drawCoin = true;
            for (int j = 0; j < 240; j++) {
                if (tiles[j] == 0 && CheckCollisionRecs(coins[i], (Rectangle){j % 20 * 32, j / 20 * 32, 32, 32})) {
                    drawCoin = false;
                    break;
                }
            }
            if (drawCoin) {
                DrawRectangle(coins[i].x, coins[i].y, coins[i].width, coins[i].height, YELLOW);
            }
        }
    }
}


void player_update(){
    const float maxSpeed = 6.0f;
    const float acceleration = 0.1f;
    static int x_direction = 0;
    static int y_direction = 0;
    static Vector2 velocity = {0};

    x_direction = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    y_direction = (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W));

    //pressing D and A speed
    velocity.x += (x_direction * maxSpeed - velocity.x) * acceleration;
    if(velocity.x < -maxSpeed){
        velocity.x = -maxSpeed;
    }
    else if(velocity.x > maxSpeed){
        velocity.x = maxSpeed;
    }
    //pressing W and S speed
    velocity.y += (y_direction * maxSpeed - velocity.y) * acceleration;
    if(velocity.y < -maxSpeed){
        velocity.y = -maxSpeed;
    }
    else if(velocity.y > maxSpeed){
        velocity.y = maxSpeed;
    }
    //player collision with walls
}

