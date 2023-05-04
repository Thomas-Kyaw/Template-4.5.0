#include "raylib.h"
#include "raymath.h"
#include<stdlib.h>
#include<stdbool.h>
#include<stdio.h>

// Global Variables
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define CHAR_WIDTH 32
#define CHAR_HEIGHT 32

#define JUMP_HEIGHT 9

#define GROUND_HEIGHT 500
#define GRAVITY 0.3f


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

    Player* player = (Player*)malloc(sizeof(Player));
    // Load sprite sheet texture
    player->run_animation = LoadTexture("assets/Run(32x32).png");
    player->idle_animation = LoadTexture("assets/Idle(32x32).png");
    player->jump_animation = LoadTexture("assets/Jump(32x32).png");
    player->double_jump_animation = LoadTexture("assets/DoubleJump(32x32).png");

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

// Define animation variables
int currentFrame = 0;
int frameCounter = 0;
int frameSpeed = 5;

// Define character variables
player->position.x = 5;
player->position.y = 500;
player->speed = 2.0f;
player->is_moving = false;
player->is_jumping = false;
player->velocity.x = 0;
player->velocity.y = 0;

// Disable ESC key quit
SetExitKey(KEY_NULL);

bool game_running = true;


bool can_double_jump = false;
int num_jumps = 0;


DrawStartScreen(&game_running);

// Main game loop
while (game_running && !WindowShouldClose())
{   
    // Reset jump variables if player is on the ground
    if (player->position.y >= GROUND_HEIGHT) {
        num_jumps = 0;
        can_double_jump = true;
    }
    // Update animation frame
    frameCounter++;
    if (frameCounter >= frameSpeed)
    {
        frameCounter = 0;
        currentFrame++;
        if (currentFrame > 11) 
            currentFrame = 0;
    }

    // Handle keyboard input
    if (IsKeyDown(KEY_RIGHT)){
        player->position.x += player->speed;
        player->is_moving = true;
    }
    else if (IsKeyDown(KEY_LEFT)){
        // Don't let the player go off the left side of the screen
        if (player->position.x - player->speed >= 0) {
            player->position.x -= player->speed;
        }
        player->is_moving = true;
    }
    else {
        player->is_moving = false;
    }

    if (IsKeyPressed(KEY_SPACE)) {
        if (num_jumps == 0) {
            player->velocity.y = -JUMP_HEIGHT;
            num_jumps++;
            can_double_jump = true;
        }
        else if (num_jumps == 1 && can_double_jump) {
            player->velocity.y = -JUMP_HEIGHT;
            num_jumps++;
            can_double_jump = false;
        }
    }

    // Update player position
    player->velocity.y += GRAVITY;
    player->position.y += player->velocity.y;

    if (player->position.y >= GROUND_HEIGHT) {
        player->position.y = GROUND_HEIGHT;
        player->velocity.y = 0.0f;
        num_jumps = 0;
        can_double_jump = true;
    }

    // Draw current animation frame and character position
    BeginDrawing();
    ClearBackground(SKYBLUE);    
    if(player->is_jumping){
        DrawTexture(player->jump_animation, player->position.x, player->position.y, WHITE);
    }
    else if(player->is_moving){
        DrawTextureRec(player->run_animation, frames[currentFrame], player->position, WHITE);
    }
    else{
        DrawTextureRec(player->idle_animation, frames[currentFrame], player->position, WHITE);
    }
    EndDrawing();
} 

    // Cleanup
    UnloadTexture(player->idle_animation);
    UnloadTexture(player->run_animation);
    UnloadTexture(player->jump_animation);
    free(player);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
