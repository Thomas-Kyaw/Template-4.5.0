#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include <stdlib.h> // for rand() and srand()
#include <time.h>   // for time()

typedef struct grid{
    float x;
    float y;
    int w;      // width
    int h;      // height
    int s;      // cell size (square cells)
    int *cell;  // array of tiles
}Grid;

// List of Rectangles to check for a collision.
typedef struct rectlist{
    Rectangle *rect;
    int size;
}RectList;

typedef struct balls_lmfao{
    float radius;
    Vector2 center;
}Balls;

Color PEACH = {255, 229, 180, 100};
void GameInit();
void GameUpdate();
void GameDraw(Texture2D *wall);
void Reset();

void DrawStartScreen();
void PauseScreen();
void VictoryScreen();
void GameOverScreen();

void DrawTileGrid();
void DrawTileMap(Texture2D *wall);
void DrawCoins();
void DrawPlayer();
void DrawEnemy1();
void DrawScoreText();
void DrawHealthText();

void UpdatePlayer();
void UpdateCoin();
void UpdateEnemy1Rec();

void        RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity);
Rectangle   RectangleUpdateDimensions(Rectangle *rect, Vector2 *size);
RectList*   RectangleListFromTiles(Rectangle *rect, Grid *grid);
void ResolveRectangleTileCollision(Rectangle* playerRect, Vector2* playerVelocity, RectList* collidableTiles);

#define MAP_W 20
#define MAP_H 12
#define NUM_TILES (MAP_H * MAP_W)
int screenWidth = 32*MAP_W;
int screenHeight = 32*MAP_H;
const int gameWidth = 32*MAP_W;
const int gameHeight = 32*MAP_H;

Rectangle player = {32.0f, 32.0f, 32.0f, 32.0f};
Rectangle enemy1;
Rectangle enemy2;
Rectangle enemy3;
int health = 5;

#define COIN_COUNT 139
Rectangle coins[COIN_COUNT] = {0};
bool visible[COIN_COUNT] = {0};
int points = 0;
int time_a = 0;       // For animation

//ball

Sound coin_sound;
Music victory_music;

#define MOVE_HORIZONTAL 0
#define MOVE_VERTICAL 1

Grid map;
int tiles[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1,
    1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

//setting pause bool
bool  is_paused = false;
bool game_running = true;
bool is_victory = false;

int main(void){
    GameInit();
    InitAudioDevice();
    SetTargetFPS(60);
    DrawStartScreen();
    Texture2D wall = LoadTexture("assets/Wall.png");

    // Disable ESC key quit
    SetExitKey(KEY_NULL);

    while (game_running && !WindowShouldClose()){
        if(IsKeyPressed(KEY_ESCAPE)){
            is_paused = !is_paused;
        }
        if(!is_paused){
            GameUpdate();
        }
        GameDraw(&wall);
    }
    UnloadMusicStream(victory_music);
    UnloadSound(coin_sound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}


void DrawStartScreen()
{
    Music bing_chilling_song = LoadMusicStream("assets/Oogway_Ascends.mp3");
    PlayMusicStream(bing_chilling_song);
    SetMusicVolume(bing_chilling_song, 0.5f);   
    bool playing_music = true;

    int blink_timer = 0; // initialize the blink timer
    bool show_text = true; // start with the text visible

    SetExitKey(KEY_NULL);
    while (game_running)
    {
        // Check for quit button press
        if (WindowShouldClose())
        {
            game_running = false;
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

        // Update blink timer
        blink_timer++;
        if (blink_timer >= 30) { // change the '10' to adjust the blink speed
            show_text = !show_text;
            blink_timer = 0;
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        if (show_text) {
            DrawText("Press ENTER to start game", GetScreenWidth()/2 - MeasureText("Press ENTER to start game", 18)/2, GetScreenHeight()/2 - 40, 18, BLACK);
        }
        DrawText("Press SPACE to pause/resume music", GetScreenWidth()/2 - MeasureText("Press SPACE to pause/resume music", 17)/2 , GetScreenHeight()/2, 17, MAROON);
        DrawText("- Music -",  GetScreenWidth()/2 - MeasureText("- Music -", 17)/2 , GetScreenHeight()/2 + 150, 17, PEACH);
        DrawText("Oogway Ascends by Hams Zimmer", GetScreenWidth()/2 - MeasureText("Oogway Ascends by Hams Zimmer", 14)/2, GetScreenHeight()/2 + 170, 14, PEACH);
            
        EndDrawing();
    }
    UnloadMusicStream(bing_chilling_song);
}

void PauseScreen() {
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();
    const int buttonWidth = 200;
    const int buttonHeight = 50;
    const int buttonSpacing = 20;
    const int buttonsTotalHeight = 2 * buttonHeight + buttonSpacing;
    const int buttonsX = (screenWidth - buttonWidth) / 2;
    const int buttonsY = (screenHeight - buttonsTotalHeight) / 2;
    
    // Draw semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(SKYBLUE, 0.5f));
    
    // Draw buttons
    Rectangle continueButtonRect = { buttonsX, buttonsY, buttonWidth, buttonHeight };
    Rectangle exitButtonRect = { buttonsX, buttonsY + buttonHeight + buttonSpacing, buttonWidth, buttonHeight };
    
    DrawRectangleRec(continueButtonRect, PINK);
    DrawRectangleLinesEx(continueButtonRect, 2, BLACK);
    DrawText("Continue", continueButtonRect.x + buttonWidth / 2 - MeasureText("Continue", 30) / 2, continueButtonRect.y + buttonHeight / 2 - 15, 30, BLACK);
    
    DrawRectangleRec(exitButtonRect, PINK);
    DrawRectangleLinesEx(exitButtonRect, 2, BLACK);
    DrawText("Exit", exitButtonRect.x + buttonWidth / 2 - MeasureText("Exit", 30) / 2, exitButtonRect.y + buttonHeight / 2 - 15, 30, BLACK);
    
    // Check for button clicks
    const bool isMouseLeftPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    const Vector2 mousePos = GetMousePosition();
    
    if (CheckCollisionPointRec(mousePos, continueButtonRect) && isMouseLeftPressed) {
        is_paused = false; // continue game
    }
    
    if (CheckCollisionPointRec(mousePos, exitButtonRect) && isMouseLeftPressed) {
        game_running = false; // exit game
    }
}

void VictoryScreen(){
    victory_music = LoadMusicStream("assets/victory_music.mp3");
    PlayMusicStream(victory_music);
    while (IsMusicStreamPlaying(victory_music)){
        ClearBackground(PINK);
        // Set background color and text color
        Color bg_color = { 255, 255, 255, 200 };
        Color text_color = BLACK;

        // Draw victory message and buttons
        DrawRectangle(0, 0, screenWidth, screenHeight, bg_color);
        DrawText("Congratulations, you won!", GetScreenWidth() / 2 - MeasureText("Congratulations, you won!", 40) / 2, GetScreenHeight() / 2 - 70, 40, text_color);

        Rectangle restart_button = { GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 + 10, 120, 50 };
        DrawRectangleRec(restart_button, BLUE);
        DrawText("Restart", restart_button.x + 15, restart_button.y + 15, 20, WHITE);

        Rectangle exit_button = { GetScreenWidth() / 2 + 30, GetScreenHeight() / 2 + 10, 120, 50 };
        DrawRectangleRec(exit_button, RED);
        DrawText("Exit", exit_button.x + 25, exit_button.y + 15, 20, WHITE);
        // Update the audio stream
        UpdateMusicStream(victory_music);

        // Check for button clicks
        if (CheckCollisionPointRec(GetMousePosition(), restart_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Reset();
            return;
        }
        else if (CheckCollisionPointRec(GetMousePosition(), exit_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            game_running = false;
            return;
        }
        EndDrawing();
    }
    StopMusicStream(victory_music);
    UnloadMusicStream(victory_music);
}

void GameInit() {
    bool game_running = true;
    InitWindow(screenWidth, screenHeight, "classic game: platformer");
    map.x = 0.0f;
    map.y = 0.0f;
    map.w = MAP_W;
    map.h = MAP_H;
    map.s = 32;
    map.cell = tiles;
    Reset();
}

void Reset(){
    const float s = 32.0f;
    player = (Rectangle){s , s , s, s};
    points = 0;
    time_a = 0;

    // Place coins on empty tiles
    int count = 0;
    for (int i = 0; i < MAP_W * MAP_H; i++) {
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

void GameUpdate(){
    UpdatePlayer();
    UpdateCoin();
    UpdateEnemy1Rec();
    if(!is_victory) {
        // game logic
        if(IsKeyPressed(KEY_ESCAPE)) {
            is_paused = !is_paused;
        }
    } else {
        is_paused = false; // don't pause when victory is achieved
    }
}

void UpdatePlayer(){
    const float maxSpd = 6.0f;
    const float acc = 0.1f;
    static int dirX = 0;
    static int dirY = 0;
    static Vector2 vel = {0};
    
    
    // INPUT
    dirX = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A) + IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT));
    dirY = (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W) + IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP));
    
    // LEFT RIGHT A D keys speed
    vel.x += (dirX * maxSpd - vel.x) * acc;
    if (vel.x < -maxSpd){
        vel.x = -maxSpd;
    }
    else if (vel.x > maxSpd){
        vel.x = maxSpd;
    }
    
    // UP DOWN W S keys speed
    vel.y += (dirY * maxSpd - vel.y) * acc;
    if (vel.y < -maxSpd){
        vel.y = -maxSpd;
    }
    else if (vel.y > maxSpd){
        vel.y = maxSpd;
    }
    RectangleCollisionUpdate(&player, &vel);
}

void UpdateCoin(){
    coin_sound = LoadSound("assets/coin_sound.mp3");
    SetSoundVolume(coin_sound, 0.1f);
    for (int i = 0; i < COIN_COUNT; i++){
        if (visible[i]){
            if (CheckCollisionRecs(coins[i], player)){
                visible[i] = false;
                points += 1;
                PlaySound(coin_sound);
            }
        }
    }
    
    if (IsKeyPressed(KEY_ENTER)){
        Reset();
    }
}

void UpdateEnemy1Rec(){

    Vector2 direction = { 1, 0 };  
    float speed = 5;  
    int start_x = 202 % MAP_W;
    int start_y = 202 % MAP_H;
    float startCellX = map.x + map.s * start_x;
    float startCellY = map.y + map.s * start_y;
        
    int end_x = 205 % MAP_W;
    int end_y = 205 % MAP_H;
    float endCellX = map.x + map.s * end_x;
    float endCellY = map.x + map.s * end_y;
    endCellX += map.s;
    
    enemy1.width = 32;
    enemy1.height = 32;
    enemy1.x = direction.x * speed;
    if(enemy1.x >= endCellX){
        direction.x = -1;
    }
    else{
        direction.x = 1;
    }
}

void GameDraw(Texture2D *wall){
    Color bcg_color = {155, 255, 124, 100};
    if(points == COIN_COUNT){
        is_victory = true;
    }
    BeginDrawing();
    if(!is_paused){
        ClearBackground(WHITE);
        DrawRectangle(0, 0, gameWidth, gameHeight, bcg_color); // Background
        DrawTileMap(wall);
        DrawTileGrid();
        DrawScoreText();
        DrawCoins();
        DrawPlayer();
        DrawHealthText();
        DrawEnemy1();
        if(is_victory) {
            VictoryScreen();
            is_victory = false;
        }
        int x = 202 % MAP_W;  // x = 2
        int y = 202 / MAP_W;  // y = 10
        float cellX = map.x + map.s * x;
        float cellY = map.y + map.s * y;
    }
    else{
        ClearBackground(SKYBLUE);
        PauseScreen();
        DrawText("- Paused -",  GetScreenWidth()/2 - MeasureText("- Paused -", 40)/2  , 0, 40, RED);
    }
    EndDrawing();
}

void DrawTileMap(Texture2D *wall){
    for (int y = 0; y < map.h; y++){
        for (int x = 0; x < map.w; x++){
            int i = x + y * map.w;
            int tile = map.cell[i];
            if (tile){
                float cellX = (map.x + map.s * x);
                float cellY = (map.y + map.s * y);
                //DrawRectangle((int)cellX, (int)cellY, map.s, map.s, LIME);
                Rectangle dest = {(int)cellX, (int)cellY, map.s, map.s};
                DrawTextureRec(*wall, dest, (Vector2){cellX, cellY}, WHITE);
            }
        }
    }
}

void DrawTileGrid(){
    Color c = (Color){255,100,100,200};
    
    for (int y = 0; y < map.h + 1; y++){
        int x1 = map.x;
        int x2 = map.x + map.w * map.s;
        int Y = map.y + map.s * y;
        DrawLine(x1, Y, x2, Y, c);
    }
    for (int x = 0; x < map.w + 1; x++){
        int y1 = map.y;
        int y2 = map.y + map.h * map.s;
        int X = map.x + map.s * x;
        DrawLine(X, y1, X, y2, c);
    }
}
    
void DrawPlayer() {
    DrawRectangle((int)player.x, (int)player.y, (int)player.width, (int)player.height, PEACH);
    DrawRectangleLinesEx(player, 2, BLACK);
    
    static bool isEyesMoving = true; // flag for tracking eye state
    if(!is_paused){
        // Draw eyes
        static int dirX = 0;
        dirX = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)) * 4;
        Vector2 L1 = (Vector2){player.x + 12 + dirX, player.y + 4};
        Vector2 R1 = (Vector2){player.x + 20 + dirX, player.y + 4};
        Vector2 L2 = L1;
        L2.y += 8;
        Vector2 R2 = R1;
        R2.y += 8;
        DrawLineEx(L1, L2, 2.0f, BLACK);
        DrawLineEx(R1, R2, 2.0f, BLACK);

        static int dirY = 0;
        dirY = (float)(IsKeyDown(KEY_W) - IsKeyDown(KEY_S)) * 4;
        Vector2 L3 = (Vector2){player.x + 14 + dirX, player.y + 12 + dirY};
        Vector2 R3 = (Vector2){player.x + 18 + dirX, player.y + 12 + dirY};
        Vector2 L4 = L3;
        L4.y -= 8;
        Vector2 R4 = R3;
        R4.y -= 8;
        DrawLineEx(L3, L4, 2.0f, BLACK);
        DrawLineEx(R3, R4, 2.0f, BLACK);

        isEyesMoving = true;
    }
    else{
        isEyesMoving = false;
    }
    
    static bool isMouthOpen = false; // flag for tracking mouth state
    static int mouthHeight = 4; // initial mouth height
    static int frameCount = 0; // frame counter
    
    // Draw mouth
    if (isMouthOpen) {
        DrawRectangle((int)player.x + 10, (int)player.y + 20, 12, mouthHeight, BLACK);
    } else {
        DrawRectangle((int)player.x + 10, (int)player.y + 20, 12, 4, BLACK);
    }
    
    // Update mouth state
    if (frameCount == 20) {
        if (mouthHeight <= 0) {
            isMouthOpen = false;
        } else if (mouthHeight >= 8) {
            isMouthOpen = true;
        }
        mouthHeight += isMouthOpen ? -1 : 1;
        frameCount = 0;
    } else {
        frameCount++;
    }
    
    // Update eye state
    if (isEyesMoving) {
        // Artistic touch
        static int eyeXOffset = 0;
        eyeXOffset = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)) * 2;
        static int eyeYOffset = 0;
        eyeYOffset = (float)(IsKeyDown(KEY_W) - IsKeyDown(KEY_S)) * 2;
        // Draw the pupils
        DrawCircle((int)(player.x + 15 + eyeXOffset), (int)(player.y + 8 + eyeYOffset), 2, BLACK);
        DrawCircle((int)(player.x + 19 + eyeXOffset), (int)(player.y + 8 + eyeYOffset), 2, BLACK);
    } else {
        // Draw the pupils in a fixed position
        DrawCircle((int)(player.x + 15), (int)(player.y + 8), 2, BLACK);
        DrawCircle((int)(player.x + 19), (int)(player.y + 8), 2, BLACK);
    }
}

void DrawCoins(){
    time_a += 1;

    for (int j = 0; j < COIN_COUNT; j++) {
        if (visible[j]) {
            Rectangle c = coins[j];
            int i = (int)(c.y / map.s) * MAP_W + (int)(c.x / map.s);
            if (tiles[i] == 0) {
                float x = (float)((i % MAP_W) * map.s);
                float y = (float)((i / MAP_W) * map.s);

                float coin_x = x + map.s / 2 - c.width / 2;
                float coin_y = y + map.s / 2 - c.height / 2;
                    float coin_y_offset = (float)sin(2 * PI * (time_a / 60.0f * 0.5) + (coin_x * 5)) * 4;
                    float coin_x_offset = (float)sin(2 * PI * (time_a / 60.0f * 2)) * 4;
                    DrawRectangle((int)(coin_x + 4 + coin_x_offset * 0.5), (int)(coin_y + coin_y_offset), (int)(c.width - 4 - coin_x_offset), (int)c.height, GOLD);
                }
        }
    }
}

void DrawScoreText(){
    const char *text;

    text = TextFormat("Score: %d", points);
    
    const int size = 24;
    int x = gameWidth /2 - MeasureText(text, size) / 2;
    int y = 48;

    DrawText(text, x, y, size, BLACK);
    
}

void DrawEnemy1(){
    DrawRectangle((int)enemy1.x, (int)enemy1.y, (int)enemy1.width, (int)enemy1.height, RED);
}

void DrawHealthText()
{
    const char *text;
    text = TextFormat("HEALTH: %d", health);
    DrawText(text, 5, 5, 24, BLACK);
}

void RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity){
    Rectangle collisionArea = RectangleUpdateDimensions(rect, velocity);
    RectList *tiles = RectangleListFromTiles(&collisionArea, &map);
    
    ResolveRectangleTileCollision(rect, velocity, tiles);
    // free allocated RectList memory
    MemFree(tiles->rect);
    MemFree(tiles);
}

Rectangle RectangleUpdateDimensions(Rectangle* originalRect, Vector2* sizeChangeVector) {
    Rectangle newRect;
    if (sizeChangeVector->x > 0) {
        newRect.x = originalRect->x;
        newRect.width = originalRect->width + sizeChangeVector->x;
    } else {
        newRect.x = originalRect->x + sizeChangeVector->x;
        newRect.width = originalRect->width - sizeChangeVector->x;
    }
    if (sizeChangeVector->y > 0) {
        newRect.y = originalRect->y;
        newRect.height = originalRect->height + sizeChangeVector->y;
    } else {
        newRect.y = originalRect->y + sizeChangeVector->y;
        newRect.height = originalRect->height - sizeChangeVector->y;
    }
    return newRect;
}

RectList* RectangleListFromTiles(Rectangle* boundingRect, Grid* tileGrid) {
    float relativeX = boundingRect->x - tileGrid->x;
    float relativeY = boundingRect->y - tileGrid->y;
    float relativeWidth = boundingRect->x - tileGrid->x + boundingRect->width;
    float relativeHeight = boundingRect->y - tileGrid->y + boundingRect->height;
    
    // Compensate for flooring
    if (relativeX < 0.0f) {
        relativeX -= tileGrid->s;
    }
    if (relativeY < 0.0f) {
        relativeY -= tileGrid->s;
    }
    if (relativeWidth < 0.0f) {
        relativeWidth -= tileGrid->s;
    }
    if (relativeHeight < 0.0f) {
        relativeHeight -= tileGrid->s;
    }
    
    // Calculate grid coordinates
    int startTileX = (int)(relativeX / tileGrid->s);
    int startTileY = (int)(relativeY / tileGrid->s);
    int numTilesX = (int)(relativeWidth / tileGrid->s) - startTileX + 1;
    int numTilesY = (int)(relativeHeight / tileGrid->s) - startTileY + 1;
    
    RectList* tileList = MemAlloc(sizeof(RectList));
    tileList->rect = MemAlloc(sizeof(Rectangle) * numTilesX * numTilesY);
    tileList->size = 0;
    
    for (int y = startTileY; y < startTileY + numTilesY; y++) {
        if (y >= 0 && y < tileGrid->h) {
            for (int x = startTileX; x < startTileX + numTilesX; x++) {
                if (x >= 0 && x < tileGrid->w) {
                    int tileIndex = tileGrid->cell[x + y * tileGrid->w];
                    if (tileIndex) {
                        tileList->rect[tileList->size] = (Rectangle) {
                            tileGrid->x + x * tileGrid->s,
                            tileGrid->y + y * tileGrid->s,
                            tileGrid->s,
                            tileGrid->s
                        };
                        tileList->size += 1;
                    }
                }
            }
        }
    }
    return tileList;
}

void ResolveRectangleTileCollision(Rectangle* playerRect, Vector2* playerVelocity, RectList* collidableTiles) {
    Rectangle* collidableTile;
    Rectangle futurePlayerRect = (Rectangle) {playerRect->x + playerVelocity->x, playerRect->y, playerRect->width, playerRect->height};
    
    // Solve X axis separately
    for (int i = 0; i < collidableTiles->size; i++) {
        collidableTile = &collidableTiles->rect[i]; // next collidable tile
        if (CheckCollisionRecs(futurePlayerRect, *collidableTile)) {
            // moving to the right
            if (playerVelocity->x > 0.0f) {
                // adjust velocity 
                playerVelocity->x = (collidableTile->x - playerRect->width) - playerRect->x;
            }
            // moving to the left
            else if (playerVelocity->x < 0.0f) {
                playerVelocity->x = (collidableTile->x + collidableTile->width) - playerRect->x;
            }
        }
    }
    // set futurePlayerRect to resolved X position
    futurePlayerRect.x = playerRect->x + playerVelocity->x;
    
    // move on Y axis
    // set futurePlayerRect on test Y position
    futurePlayerRect.y += playerVelocity->y;
    for (int i = 0; i < collidableTiles->size; i++) {
        collidableTile = &collidableTiles->rect[i];
        if (CheckCollisionRecs(futurePlayerRect, *collidableTile)) {
            // moving down
            if (playerVelocity->y > 0.0f) {
                playerVelocity->y = (collidableTile->y - playerRect->height) - playerRect->y;
            }
            // moving up
            else if (playerVelocity->y < 0.0f) {
                playerVelocity->y = (collidableTile->y + collidableTile->height) - playerRect->y;
            }
        }
    }
    
    playerRect->x += playerVelocity->x;
    playerRect->y += playerVelocity->y;
}

