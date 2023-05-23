/*
    Pardon for my bad English in this code if it gets confusing sometimes. I forgot most of enligsh grammar.
*/
#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include <stdlib.h> // for rand() and srand()
#include <time.h>   // for time()

typedef struct map{
    float x;
    float y;
    int width;     
    int height;      
    int cell_size;      
    int *cell;  // array of tiles
}Map;

// List of Rectangles to check for a collision.
typedef struct{
    Rectangle *rect;
    int size;
}RectList;

Color PEACH = {255, 229, 180, 100};
void GameInit();
void GameUpdate();
void GameDraw(Texture2D *wall);
void Reset();

void DrawStartScreen();
void PauseScreen();
void VictoryScreen();
void GameOverScreen();

void DrawTileMap(Texture2D *wall);
void DrawCoins();
void DrawPlayer();
void DrawScoreText();
void DrawHealthText();

void UpdatePlayer();
void UpdateCoin();

void        RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity);
Rectangle   RectangleUpdateDimensions(Rectangle *rect, Vector2 *size);
RectList*   RectangleListFromTiles(Rectangle *rect, Map *grid);
void ResolveRectangleTileCollision(Rectangle* playerRect, Vector2* playerVelocity, RectList* collidableTiles);

#define MAP_W 20
#define MAP_H 12
#define NUM_TILES (MAP_H * MAP_W)
int screenWidth = 32*MAP_W;
int screenHeight = 32*MAP_H;
const int gameWidth = 32*MAP_W;
const int gameHeight = 32*MAP_H;

Rectangle player = {32.0f, 32.0f, 32.0f, 32.0f};
int health = 5;

#define COIN_COUNT 15
Rectangle coins[COIN_COUNT] = {0};
bool visible[COIN_COUNT] = {0};
int points = 0;
int time_a = 0;       // For animation

Map map;
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
bool is_gameover = false;

Sound coin_sound;

int main(void){
    GameInit();
    InitAudioDevice();
    SetTargetFPS(60);
    DrawStartScreen();
    Texture2D wall = LoadTexture("assets/Wall.png");
    Texture2D enemy_texture = LoadTexture("assets/Run(32x32).png");
    Sound hit_sound = LoadSound("assets/oof.mp3");


    //sorry but I'm too deep in this to add more functions that will frick it all up


    Rectangle frameRecs[12] = {0}; //Rectangles for enemy sprite animation
    for (int i = 0; i < 12; i++) {  
        frameRecs[i] = (Rectangle){i * 32, 0, 32, 32};
    }
    Rectangle frameRecs2[12] = {0};
    for (int i = 0; i < 12; i++) {
    frameRecs2[i] = (Rectangle){i * 32, 0, 32, 32};
    }
    Rectangle frameRecs3[12] = {0};
    for (int i = 0; i < 12; i++) {
    frameRecs3[i] = (Rectangle){i * 32, 0, 32, 32};
    }
    Rectangle frameRecs4[12] = {0};
    for (int i = 0; i < 12; i++) {
    frameRecs4[i] = (Rectangle){i * 32, 0, 32, 32};
    }

    // Declare sprite and starting and target positions for enemy 1
    Rectangle enemy1 = {32, 320, 32, 32};  // Current position
    Vector2 enemy1_startPos = {32, 320};        // Starting position
    Vector2 enemy1_targetPos = {160, 320};       // Target position
    Vector2 enemy1_currentTarget = enemy1_targetPos;    // Current target position
    Vector2 enemy1_currentPos = enemy1_startPos;        // Current position
    Vector2 velocity1 = {0};               // Velocity vector

    // Declare sprite and starting and target positions for enemy 2
    Rectangle enemy2 = {224, 320, 32, 32}; // Current position
    Vector2 enemy2_startPos = {224, 320};  // Starting position
    Vector2 enemy2_targetPos = {224, 224};
    Vector2 enemy2_currentTarget = enemy2_targetPos;    // Current target position
    Vector2 enemy2_currentPos = enemy2_startPos;        // Current position
    Vector2 velocity2 = {0};               // Velocity vector

    // Declare sprite and starting and target positions for enemy 3
    Rectangle enemy3 = {416, 96, 32, 32}; // Current position
    Vector2 enemy3_startPos = {416, 96};  // Starting position
    Vector2 enemy3_targetPos = {544, 96};
    Vector2 enemy3_currentTarget = enemy3_targetPos;    // Current target position
    Vector2 enemy3_currentPos = enemy3_startPos;        // Current position
    Vector2 velocity3 = {0};               // Velocity vector

    // Declare sprite and starting and target positions for enemy 4
    Rectangle enemy4 = {544, 160, 32, 32}; // Current position
    Vector2 enemy4_startPos = {544, 160};  // Starting position
    Vector2 enemy4_targetPos = {416, 160};
    Vector2 enemy4_currentTarget = enemy4_targetPos;    // Current target position
    Vector2 enemy4_currentPos = enemy4_startPos;        // Current position
    Vector2 velocity4 = {0};               // Velocity vector

    const float speed = 2.0f;
    // Calculate velocity vector required to move from current position to target position
    float distance1 = Vector2Distance(enemy1_currentPos, enemy1_currentTarget);
    velocity1 = Vector2Scale(Vector2Subtract(enemy1_currentTarget, enemy1_currentPos), speed/distance1);

    float distance2 = Vector2Distance(enemy2_currentPos, enemy2_currentTarget);
    velocity2 = Vector2Scale(Vector2Subtract(enemy2_currentTarget, enemy2_currentPos), speed/distance2);

    float distance3 = Vector2Distance(enemy3_currentPos, enemy3_currentTarget);
    velocity3 = Vector2Scale(Vector2Subtract(enemy3_currentTarget, enemy3_currentPos), speed/distance3);

    float distance4 = Vector2Distance(enemy4_currentPos, enemy4_currentTarget);
    velocity4 = Vector2Scale(Vector2Subtract(enemy4_currentTarget, enemy4_currentPos), speed/distance4);

    // Set up timer for sprite animation
    double frameTime = 0.0;
    const double animSpeed = 0.1;
    int currentFrame = 0;

    //Timers to gain vulnerablity after getting hit by enemy
    bool isCooldownActive = false;
    float cooldownDuration = 2.0f;  // Downtime duration in seconds
    float cooldownTimer = 0.0f;     // Timer to track the cooldown duration

    // Disable ESC key quit
    SetExitKey(KEY_NULL);
    while (game_running && !WindowShouldClose()){
        if(IsKeyPressed(KEY_ESCAPE)){
            is_paused = !is_paused;
        }
        if(!is_paused){
            
            GameUpdate();
            // Update enemy position
            enemy1.x += velocity1.x;
            enemy1.y = enemy1_currentPos.y; // Set y-coordinate to a fixed value

            enemy2.x = enemy2_currentPos.x; // Set x-coordinate to a fixed value
            enemy2.y += velocity2.y; 

            enemy3.x += velocity3.x;
            enemy3.y = enemy3_currentPos.y; // Set y-coordinate to a fixed value

            enemy4.x += velocity4.x;
            enemy4.y = enemy4_currentPos.y; // Set y-coordinate to a fixed value

            // Update sprite animation frame
            frameTime += GetFrameTime();
            if (frameTime >= animSpeed) {
            frameTime = 0.0;
            currentFrame = (currentFrame + 1) % 12;
            }
        }

        GameDraw(&wall);

        if(!is_paused){
            DrawTextureRec(enemy_texture, frameRecs[currentFrame], (Vector2){enemy1.x, enemy1.y}, RAYWHITE);
            DrawTextureRec(enemy_texture, frameRecs2[currentFrame], (Vector2){enemy2.x, enemy2.y}, RAYWHITE);
            DrawTextureRec(enemy_texture, frameRecs3[currentFrame], (Vector2){enemy3.x, enemy3.y}, RAYWHITE);
            DrawTextureRec(enemy_texture, frameRecs4[currentFrame], (Vector2){enemy4.x, enemy4.y}, RAYWHITE);
        }
        // Check if sprite has reached its target position
        if (!is_paused && Vector2Distance((Vector2){enemy1.x, enemy1.y}, enemy1_currentTarget) < speed / 2) // checking with speed/2 instead of speed to make sure it doesnt go off the tile box.
        {
            enemy1.x = enemy1_currentTarget.x;
            enemy1_currentPos = enemy1_currentTarget;
            enemy1_currentTarget = (enemy1_currentTarget.x == enemy1_targetPos.x) ? enemy1_startPos : enemy1_targetPos; // Toggle target position
            distance1 = Vector2Distance(enemy1_currentPos, enemy1_currentTarget);
            velocity1 = Vector2Scale(Vector2Subtract(enemy1_currentTarget, enemy1_currentPos), speed/distance1);
        }
        if (!is_paused && Vector2Distance((Vector2){enemy2.x, enemy2.y}, enemy2_currentTarget) < speed / 2) {
            enemy2.y = enemy2_currentTarget.y;
            enemy2_currentPos = enemy2_currentTarget;
            enemy2_currentTarget = (enemy2_currentTarget.y == enemy2_targetPos.y) ? enemy2_startPos : enemy2_targetPos; // Toggle target position
            distance2 = Vector2Distance(enemy2_currentPos, enemy2_currentTarget);
            velocity2 = Vector2Scale(Vector2Subtract(enemy2_currentTarget, enemy2_currentPos), speed/distance2);
        }
        if (!is_paused && Vector2Distance((Vector2){enemy3.x, enemy3.y}, enemy3_currentTarget) < speed / 2) {
            enemy3.x = enemy3_currentTarget.x;
            enemy3_currentPos = enemy3_currentTarget;
            enemy3_currentTarget = (enemy3_currentTarget.x == enemy3_targetPos.x) ? enemy3_startPos : enemy3_targetPos; // Toggle target position
            distance3 = Vector2Distance(enemy3_currentPos, enemy3_currentTarget);
            velocity3 = Vector2Scale(Vector2Subtract(enemy3_currentTarget, enemy3_currentPos), speed/distance3);
        }
        if (!is_paused && Vector2Distance((Vector2){enemy4.x, enemy4.y}, enemy4_currentTarget) < speed / 2) {
            enemy4.x = enemy4_currentTarget.x;
            enemy4_currentPos = enemy4_currentTarget;
            enemy4_currentTarget = (enemy4_currentTarget.x == enemy4_targetPos.x) ? enemy4_startPos : enemy4_targetPos; // Toggle target position
            distance4 = Vector2Distance(enemy4_currentPos, enemy4_currentTarget);
            velocity4 = Vector2Scale(Vector2Subtract(enemy4_currentTarget, enemy4_currentPos), speed/distance4);
        }
        //Check player rectangle collision between the player and enemy1 2 3
        if(!is_paused && CheckCollisionRecs(player, enemy1) || CheckCollisionRecs(player, enemy2) || CheckCollisionRecs(player, enemy3) || CheckCollisionRecs(player, enemy4)){
           if (!isCooldownActive) {
            health -= 1;
            PlaySound(hit_sound);
            isCooldownActive = true;
            cooldownTimer = cooldownDuration;
            }
            DrawRectangle((int)player.x, (int)player.y, (int)player.width, (int)player.height, RED);
        }
        if (isCooldownActive) {
            cooldownTimer -= GetFrameTime();

            // Check if the cooldown duration has elapsed
            if (cooldownTimer <= 0.0f) {
            isCooldownActive = false;
            }
        }
    }
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
        if (blink_timer >= 30) {
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

    DrawText("- Paused -",  GetScreenWidth()/2 - MeasureText("- Paused -", 40)/2  , 0, 40, RED);

    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();
    const int buttonWidth = 200;
    const int buttonHeight = 50;
    const int buttonSpacing = 20;
    const int buttonsTotalHeight = 2 * buttonHeight + buttonSpacing;
    const int buttonsX = (screenWidth - buttonWidth) / 2; //centering? the box
    const int buttonsY = (screenHeight - buttonsTotalHeight) / 2; //centering the box
    
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

void VictoryScreen() {
    Music victory_music = LoadMusicStream("assets/victory_music.mp3");
    PlayMusicStream(victory_music);

    while (IsMusicStreamPlaying(victory_music)) {
        if (WindowShouldClose()) {
            game_running = false;
            return;
        }

        ClearBackground(PINK);

        // Set background color and text color
        Color bg_color = { 100, 149, 237, 200 };  // Cornflower Blue
        Color text_color = WHITE;

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
        } else if (CheckCollisionPointRec(GetMousePosition(), exit_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            game_running = false;
            return;
        }

        EndDrawing();
    }

    StopMusicStream(victory_music);
    UnloadMusicStream(victory_music);
}

void GameOverScreen(){
    Music gameover_music = LoadMusicStream("assets/gameover.mp3");
    PlayMusicStream(gameover_music);    
    const char *text;
    text = TextFormat("Your Score was: %d", points);
    while (IsMusicStreamPlaying(gameover_music)){
        if (WindowShouldClose()) {
            game_running = false;
            return;
        } 
        ClearBackground(PINK);
        // Set background color and text color
        Color bg_color = { 255, 255, 255, 200 };
        Color text_color = BLACK;

        // Draw game over message and buttons
        DrawRectangle(0, 0, screenWidth, screenHeight, bg_color);
        DrawText("!! Game Over !!", GetScreenWidth() / 2 - MeasureText("!! Game over !!", 40) / 2, GetScreenHeight() / 2 - 70, 40, text_color);
        DrawText(text, GetScreenWidth() / 2 - MeasureText(text, 40) / 2, GetScreenHeight() / 2 - 130, 40, text_color);

        Rectangle restart_button = { GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 + 10, 120, 50 };
        DrawRectangleRec(restart_button, BLUE);
        DrawText("Restart", restart_button.x + 15, restart_button.y + 15, 20, WHITE);

        Rectangle exit_button = { GetScreenWidth() / 2 + 30, GetScreenHeight() / 2 + 10, 120, 50 };
        DrawRectangleRec(exit_button, RED);
        DrawText("Exit", exit_button.x + 25, exit_button.y + 15, 20, WHITE);
        // Update the audio stream
        UpdateMusicStream(gameover_music);

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
    StopMusicStream(gameover_music);
    UnloadMusicStream(gameover_music);    
}

void GameInit() {
    bool game_running = true;
    InitWindow(screenWidth, screenHeight, "単純わけねぇねくそゲーム");
    map.x = 0.0f;
    map.y = 0.0f;
    map.width = MAP_W;
    map.height = MAP_H;
    map.cell_size = 32;
    map.cell = tiles;
    //to ensure scores, health, coin and player are set to original positions when program starts
    Reset();
}

void Reset(){
    const float size = 32.0f;
    player = (Rectangle){size , size , size, size};
    points = 0;
    time_a = 0;
    health = 5;

    // Place coins on empty tiles
    int count = 0;
    for (int i = 0; i < MAP_W * MAP_H; i++) {
        if (tiles[i] == 0){
            //i % MAP_W to get column to calculate x AND i / MAP_W to get row to calculate y, multiply by the cell size to get desired x and y coordinates....
            coins[count] = (Rectangle){(i % MAP_W) * size + size / 2 - 5, (i / MAP_W) * size + size / 2 - 5, 10.0f, 10.0f};
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
    if(!is_victory) {
        // game logic
        if(IsKeyPressed(KEY_ESCAPE)) {
            is_paused = !is_paused;
        }
    } else {
        is_paused = false; // should not pause when victory is achieved
    }
}

void UpdatePlayer(){
    const float maxSpeed = 5.0f;
    const float acceleration = 0.1f;
    static int directionX = 0;
    static int directionY = 0;
    static Vector2 velocity = {0}; //static will retain its value after calculation below across function calls. if no static every GameUpdate() loop it will be initialized to 0
    
    
    // INPUT
    directionX = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));// to compansate for pressing two keys of opposite direction at the same time. 1 or 0
    directionY = (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W));//to compansate for pressing two keys at the same time. 1 or 0
    
    // A D keys speed
    velocity.x += (directionX * maxSpeed - velocity.x) * acceleration;
    //making sure velocity deoesnt exceed maxSpeed
    if (velocity.x < -maxSpeed){
        velocity.x = -maxSpeed;
    }
    else if (velocity.x > maxSpeed){
        velocity.x = maxSpeed;
    }
    
    // W S keys speed
    velocity.y += (directionY * maxSpeed - velocity.y) * acceleration;
    if (velocity.y < -maxSpeed){
        velocity.y = -maxSpeed;
    }
    else if (velocity.y > maxSpeed){
        velocity.y = maxSpeed;
    }
    RectangleCollisionUpdate(&player, &velocity);
}

void UpdateCoin(){
    coin_sound = LoadSound("assets/coin_sound.mp3");
    SetSoundVolume(coin_sound, 0.1f);
    for (int i = 0; i < COIN_COUNT; i++){
        if (visible[i]) //two arrays Rec coins[] and bool visible[] one to check collision, one for not drawing.
        {
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

void GameDraw(Texture2D *wall)
{
    Color bcg_color = {155, 255, 124, 100};
    if(points == COIN_COUNT){
        is_victory = true;
    }
    if(health == 0){
        is_gameover = true;
    }
    BeginDrawing();
    if(!is_paused) //stops drawing everything when pausescreen is called
    {  
        ClearBackground(WHITE);
        DrawRectangle(0, 0, gameWidth, gameHeight, bcg_color); // Background
        DrawTileMap(wall);
        DrawScoreText();
        DrawCoins();
        DrawPlayer();
        DrawHealthText();
        if(is_victory) {
            VictoryScreen();
            is_victory = false;
        }
        if(is_gameover){
            GameOverScreen();
            is_gameover = false;
        }
    }
    else{
        ClearBackground(SKYBLUE);
        PauseScreen(); 
    }
    EndDrawing();
}

void DrawTileMap(Texture2D* wallTexture) 
{
    for (int row = 0; row < map.height; row++) {
        for (int col = 0; col < map.width; col++) {
            int cellIndex = col + row * map.width; //calculating which cell to draw. pretty self-explanatory. chose 1D array so I have to do this instead of something like map.cell[][].
            int cellValue = map.cell[cellIndex];

            if (cellValue) {
                float cellX = map.cell_size * col;
                float cellY = map.cell_size * row;

                Rectangle destinationRect = { (int)cellX, (int)cellY, map.cell_size, map.cell_size }; //if no texture you dont need tis just call DrawRec() and you can draw solid color rectangle.
                DrawTextureRec(*wallTexture, destinationRect, (Vector2){ cellX, cellY }, WHITE);
            }
        }
    }
}
    
void DrawPlayer() {
    //drawing player rectangle and the border
    DrawRectangle((int)player.x, (int)player.y, (int)player.width, (int)player.height, PEACH);
    DrawRectangleLinesEx(player, 2, BLACK);
    
    static bool isEyesMoving = true; // flag for tracking eye state
    if (!is_paused) {
        // Draw eyes
        static int eyeHorizontalOffset = 0;
        // Calculate eye positions based on player position and eye horizontal offset
        eyeHorizontalOffset = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A)) * 4; // +4 or -4 or 0 depending on D and A
        Vector2 leftEyeTop = { player.x + 12 + eyeHorizontalOffset, player.y + 6 };
        Vector2 rightEyeTop = { player.x + 20 + eyeHorizontalOffset, player.y + 6 };
        Vector2 leftEyeBottom = leftEyeTop;
        leftEyeBottom.y += 8;
        Vector2 rightEyeBottom = rightEyeTop;
        rightEyeBottom.y += 8;
        // Draw eye lines
        DrawLineEx(leftEyeTop, leftEyeBottom, 4.0f, BLACK);
        DrawLineEx(rightEyeTop, rightEyeBottom, 4.0f, BLACK);

        isEyesMoving = true;
    }
    else {
        isEyesMoving = false;
    }
    
    static bool isMouthOpen = false; //mouth state
    static int mouthHeight = 4; // initial mouth height
    static int frameCount = 0; // frame counter
    
    // Draw mouth animation or sorta animation
    if (isMouthOpen) {
        DrawRectangle((int)player.x + 10, (int)player.y + 20, 12, mouthHeight, BLACK);
    } else {
        DrawRectangle((int)player.x + 10, (int)player.y + 20, 12, 4, BLACK);
    }
    
    // Update mouth state
    if (frameCount == 3) //open close speed sorta
    {
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
}


void DrawCoins(){
    time_a += 1;

    for (int j = 0; j < COIN_COUNT; j++) {
        if (visible[j]) {
            Rectangle coin = coins[j];
            int index = (int)(coin.y / map.cell_size) * MAP_W + (int)(coin.x / map.cell_size);//just another way of calculating index through 1D map array
            if (tiles[index] == 0) {
                float x = (float)((index % MAP_W) * map.cell_size); //to get column, multiply by cell size to get x position
                float y = (float)((index / MAP_W) * map.cell_size); //to get row, multiply by cell size to get y position

                float coin_x = x + map.cell_size / 2 - coin.width / 2; //to draw the coin rectangle relatively at the center of the cell
                float coin_y = y + map.cell_size / 2 - coin.height / 2;
                DrawRectangle((int)(coin_x), (int)(coin_y), (int)(coin.width), (int)coin.height, GOLD);
            }
        }
    }
}

void DrawScoreText(){
    const char *text;

    text = TextFormat("SCORE: %d", points);
    
    const int size = 24;
    int x = gameWidth /2 - MeasureText(text, size) / 2; //center of the screen and subtract with size of the text divied by 2 to really center the text on x coordinate.
    int y = 5; //meh

    DrawText(text, x, y, size, BLACK);
    
}

void DrawHealthText()
{
    const char *text;
    text = TextFormat("HEALTH: %d", health);
    DrawText(text, 5, 5, 24, BLACK);
}

void RectangleCollisionUpdate(Rectangle *player, Vector2 *velocity){
    Rectangle collisionArea = RectangleUpdateDimensions(player, velocity);
    RectList *tiles = RectangleListFromTiles(&collisionArea, &map);
    
    ResolveRectangleTileCollision(player, velocity, tiles);
    // free allocated RectList memory
    MemFree(tiles->rect);
    MemFree(tiles);
}

Rectangle RectangleUpdateDimensions(Rectangle* originalRect, Vector2* changeVector) 
// to resolve collision area update between the player rec and the tile map wall.
// this function is used to calculate future collision between where player will be moving in the future
// this function calculates future position with velocity resolves collision
{
    Rectangle newRect;
    newRect.x = originalRect->x + changeVector->x;
    newRect.y = originalRect->y + changeVector->y;
    newRect.width = originalRect->width;
    newRect.height = originalRect->height;
    return newRect;
}

RectList* RectangleListFromTiles(Rectangle* boundingRect, Map* map)
//this function calculate on which tiles the boundingRect aka colliisonArea calculated from player rect is currently in.
//if a tile is currently within 4 tiles the list will be 4 rectangles represeting the tiles
//this list is used with the function below to calculate collision detection check
//Normal CheckCollisionRec() wont work for if you just check what one tile the player is currently colliding as it can be collided with many tiles at the same time
{
    // Calculate the relative coordinates and dimensions of the bounding rectangle
    float relativeX = boundingRect->x; 
    float relativeY = boundingRect->y; 
    float relativeWidth = boundingRect->x + boundingRect->width; //x boundary or width 
    float relativeHeight = boundingRect->y + boundingRect->height; //y boundary or height

    // Calculate grid/map index for the tiles
    int startTileX = (int)(relativeX / map->cell_size);
    int startTileY = (int)(relativeY / map->cell_size);
    int numTilesX = (int)(relativeWidth / map->cell_size) - startTileX + 1;
    int numTilesY = (int)(relativeHeight / map->cell_size) - startTileY + 1;

    // Allocate memory for the tile list
    RectList* tileList = MemAlloc(sizeof(RectList));
    tileList->rect = MemAlloc(sizeof(Rectangle) * numTilesX * numTilesY);
    tileList->size = 0;

    // Iterate through the tiles within the specified bounding rectangle
    for (int y = startTileY; y < startTileY + numTilesY; y++) {
        if (y >= 0 && y < map->height) {
            for (int x = startTileX; x < startTileX + numTilesX; x++) {
                if (x >= 0 && x < map->width) {
                    // Get the tile index from the map's cell array
                    int tileIndex = map->cell[x + y * map->width];//same thing with the calculations in above functions.getting thru 1D array
                    if (tileIndex) {
                        // Create a rectangle representing the tile's position and size
                        tileList->rect[tileList->size] = (Rectangle) {
                            x * map->cell_size,
                            y * map->cell_size,
                            map->cell_size,
                            map->cell_size
                        };
                        tileList->size += 1;
                    }
                }
            }
        }
    }

    // Return the resulting list of tiles
    return tileList;
}

void ResolveRectangleTileCollision(Rectangle* playerRect, Vector2* playerVelocity, RectList* collidableTiles) 
// this the real collision check
//
{
    Rectangle* collidableTile;
    Rectangle futurePlayerRect = (Rectangle) {playerRect->x + playerVelocity->x, playerRect->y, playerRect->width, playerRect->height};
    
    // Solve X axis separately
    for (int i = 0; i < collidableTiles->size; i++) 
    {
        collidableTile = &collidableTiles->rect[i]; // collidable tile index 
        if (CheckCollisionRecs(futurePlayerRect, *collidableTile)) {
            // moving to the right
            if (playerVelocity->x > 0.0f) {
                // adjust velocity 
                playerVelocity->x = (collidableTile->x - playerRect->width) - playerRect->x;
                // (collidableTile->x - playerRect->width) the point where the player's right edge would intersect with the collidable tile if there were no collision.
                // This difference represents the penetration depth, or how far the player is overlapping into the collidable tile
                // setting playerVelocity->x to the penetration depth, the player will be moved back by that amount, resolving the collision and preventing further penetration into the collidable tile.
            }
            // moving to the left
            else if (playerVelocity->x < 0.0f) {
                playerVelocity->x = (collidableTile->x + collidableTile->width) - playerRect->x;
                // this is similar to the above calculation but this time the player is moving to the left. as we know, player rectangle starts from top left x coordinate. 
                // So this time (collidableTile->x + collidableTile->width) to calculate the penetrating depth to the collidable tile
                // setting playerVelocity->x to the penetration depth, the player will be moved forward by that amount, resolving the collision and preventing further penetration into the collidable tile.
            }
        }
    }
    // set futurePlayerRect to X position
    futurePlayerRect.x = playerRect->x + playerVelocity->x;
    // set futurePlayerRect on Y position
    futurePlayerRect.y += playerVelocity->y;
    
    // move on Y axis
    for (int i = 0; i < collidableTiles->size; i++) 
    {
        collidableTile = &collidableTiles->rect[i];
        if (CheckCollisionRecs(futurePlayerRect, *collidableTile)) {
            // moving down
            if (playerVelocity->y > 0.0f) {
                playerVelocity->y = (collidableTile->y - playerRect->height) - playerRect->y;//the point where the player's down edge would intersect with the collidable tile if there were no collision.
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
