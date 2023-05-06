#include "raylib.h"
#include "math.h"

//tile collision types
#define EMPTY 0
#define BLOCK 1
typedef struct{
    float x;
    float y;
    int w;      // width
    int h;      // height
    int s;      // cell size (square cells)
    int *cell;  // array of tiles
}Grid;

// List of Rectangles to check for a collision.
typedef struct{
    Rectangle *rect;
    int size;
}RectList;

typedef struct Player {
    Texture2D run_animation;
    Texture2D idle_animation;
    Vector2 position;
    const float maxSpeed;
    const float acceleration;
    Vector2 velocity;
    float speed;
    bool is_moving;
} Player;

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

void        RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity);
Rectangle   RectangleResize(Rectangle *rect, Vector2 *size);
RectList*   RectangleListFromTiles(Rectangle *rect, Grid *grid);
void        RectangleTileCollision(Rectangle *rect, Vector2 *velocity, RectList *list);

#define MAP_W 20
#define MAP_H 12
int screenWidth = 32*MAP_W;
int screenHeight = 32*MAP_H;
const int gameWidth = 32*MAP_W;
const int gameHeight = 32*MAP_H;

Rectangle player = {32.0f * 2, 32.0f * 8, 32.0f, 32.0f};

#define COIN_COUNT 165
Rectangle coins[COIN_COUNT] = {0};
bool visible[COIN_COUNT] = {0};
int points = 0;
int time_a = 0;       // For animation

Grid map;
int tiles[] = {
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
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
    GameInit();
    SetTargetFPS(60);
    Player* player = (Player*)malloc(sizeof(Player));
    // Load sprite sheet texture
    player->run_animation = LoadTexture("assets/Run(32x32).png");
    player->idle_animation = LoadTexture("assets/Idle(32x32).png");
    while (!WindowShouldClose()){
        GameLoop();
    }
    CloseWindow();
    return 0;
}

void GameInit() {
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
    player = (Rectangle){s * 2, s * 6, s, s};
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
}

void UpdatePlayer(){
    const float maxSpd = 6.0f;
    const float acc = 0.1f;
    static int dirX = 0;
    static int dirY = 0;
    static Vector2 vel = {0};
      
    // INPUT
    dirX = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    dirY = (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W));
    
    // player 
    vel.x += (dirX * maxSpd - vel.x) * acc;
    if (vel.x < -maxSpd){
        vel.x = -maxSpd;
    }
    else if (vel.x > maxSpd){
        vel.x = maxSpd;
    }

    vel.y += (dirY * maxSpd - vel.y) * acc;
    if (vel.y < -maxSpd){
        vel.y = -maxSpd;
    }
    else if (vel.y > maxSpd){
        vel.y = maxSpd;
    }
    RectangleCollisionUpdate(&player, &vel);
    //player.x += vel.x;
    //player.y += vel.y;
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
    
    // Render game's viewport
        DrawRectangle(0, 0, gameWidth, gameHeight, SKYBLUE); // Background
        DrawTileMap();
        DrawTileGrid();
        DrawScoreText();
        DrawCoins();
        DrawPlayer();
    
    // Draw the viewport
    BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangle(0, 0, gameWidth, gameHeight, SKYBLUE); // Background
        DrawTileMap();
        DrawTileGrid();
        DrawScoreText();
        DrawCoins();
        DrawPlayer();
    EndDrawing();
}

void DrawTileMap(){
    for (int y = 0; y < map.h; y++){
        for (int x = 0; x < map.w; x++){
            int i = x + y * map.w;
            int tile = map.cell[i];
            if (tile){
                float cellX = (map.x + map.s * x);
                float cellY = (map.y + map.s * y);
                DrawRectangle((int)cellX, (int)cellY, map.s, map.s, LIME);
                // check tile above
                if (i - map.w >= 0 && !map.cell[i - map.w]){
                    DrawLineEx((Vector2){cellX, cellY + 3}, (Vector2){cellX + map.s, cellY + 3}, 6.0f, GREEN);
                }
            }
        }
    }
}

void DrawTileGrid(){
    Color c = (Color){255,255,255,25};
    
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

void DrawPlayer(){
    DrawRectangle((int)player.x, (int)player.y, (int)player.width, (int)player.height, WHITE);
    DrawRectangleLinesEx(player, 2, BLACK);
    
    // Artistic touch
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
    if (points == COIN_COUNT){
        text = TextFormat("Pres 'ENTER' to restart!");
    }
    else{
        text = TextFormat("Score: %d", points);
    }
    
    const int size = 24;
    int x = gameWidth /2 - MeasureText(text, size) / 2;
    int y = 48;
    
    DrawText(text, x, y+1, size, BLACK);
    DrawText(text, x, y, size, WHITE);
    
}

void RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity){
    Rectangle colArea = RectangleResize(rect, velocity);
    RectList *tiles = RectangleListFromTiles(&colArea, &map);
    
    RectangleTileCollision(rect, velocity, tiles);
    // free allocated RectList memory
    MemFree(tiles->rect);
    MemFree(tiles);
}

Rectangle RectangleResize(Rectangle *rect, Vector2 *size){
    Rectangle r = (Rectangle){
        size->x > 0 ? rect->x : rect->x + size->x,
        size->y > 0 ? rect->y : rect->y + size->y,
        size->x > 0 ? rect->width + size->x : rect->width - size->x,
        size->y > 0 ? rect->height + size->y : rect->height - size->y
        };
    return r;
}

RectList* RectangleListFromTiles(Rectangle *rect, Grid *grid){
    float offX = rect->x - grid->x;
    float offY = rect->y - grid->y;
    float offXw = rect->x - grid->x + rect->width;
    float offYh = rect->y - grid->y + rect->height;
    // compensate flooring
    if (offX < 0.0f){offX -= grid->s;}
    if (offY < 0.0f){offY -= grid->s;}
    if (offXw < 0.0f){offXw -= grid->s;}
    if (offYh < 0.0f){offYh -= grid->s;}
    
    // grid coordinates
    int X = (int)(offX / grid->s);
    int Y = (int)(offY / grid->s);
    int sizeX = (int)(offXw / grid->s) - X + 1;
    int sizeY = (int)(offYh / grid->s) - Y + 1;
    
    RectList *list = MemAlloc(sizeof(RectList));
    list->rect = MemAlloc(sizeof(Rectangle) * sizeX * sizeY);
    list->size = 0;
    
    for (int y = Y; y < Y + sizeY; y++){
        if (y >= 0 && y < grid->h){
            for (int x = X; x < X + sizeX; x++){
                if (x >= 0 && x < grid->w){
                    int tile = grid->cell[x + y * grid->w];
                    if (tile){
                        list->rect[list->size] = (Rectangle){
                            grid->x + x * grid->s,
                            grid->y + y * grid->s,
                            grid->s,
                            grid->s
                            };
                        list->size += 1;
                    }
                }
            }
        }
    }
    return list;
}

void RectangleTileCollision(Rectangle *rect, Vector2 *velocity, RectList *list){
    Rectangle *a = rect;
    Rectangle *b;
    Rectangle c = (Rectangle){a->x + velocity->x, a->y, a->width, a->height};
    
    // Solve X axis separately
    for (int i = 0; i < list->size; i++){
        b = &list->rect[i]; // next collision Rectangle
        if (CheckCollisionRecs(c, *b)) {
            // moving to the right
            if (velocity->x > 0.0f) {
                // adjust velocity 
                velocity->x = (b->x - a->width) - a->x;
            }
            // moving to the left
            else if (velocity->x < 0.0f) {
                velocity->x = (b->x + b->width) - a->x;
            }
        }
    }
    // set C to resolved X position
    c.x = a->x + velocity->x;
    
    // move on Y axis
    // set C on test Y position
    c.y += velocity->y;
    for (int i = 0; i < list->size; i++){
        b = &list->rect[i];
        if (CheckCollisionRecs(c, *b)) {
            // moving down
            if (velocity->y > 0.0f) {
                velocity->y = (b->y - a->height) - a->y;
            }
            // moving up
            else if (velocity->y < 0.0f) {
                velocity->y = (b->y + b->height) - a->y;
            }
        }
    }
    
    rect->x += velocity->x;
    
    rect->y += velocity->y;
}

