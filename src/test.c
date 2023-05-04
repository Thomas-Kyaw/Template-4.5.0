#include "raylib.h"
#include "math.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

bool PLATFORMER = true; // toggle to top-down movement with TAB

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
int scale = 1;
Vector2 vpOffset = (Vector2){0.0f, 0.0f};

Rectangle player = {32.0f * 2, 32.0f * 8, 32.0f, 32.0f};

#define COIN_COUNT 10
Rectangle coins[COIN_COUNT] = {0};
bool visible[COIN_COUNT] = {0};
int points = 0;
int time = 0;       // For animation

Grid map;
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
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

int main(void){
    InitWindow(screenWidth, screenHeight, "classic game: platformer");
    GameInit();
    SetTargetFPS(60);
    while (!WindowShouldClose()){
        GameLoop();
    }
    CloseWindow();
    return 0;
}

void GameInit() {
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
    time = 0;
    
    coins[0] = (Rectangle){s * 1.5f, s * 8, 10.0f, 10.0f};
    coins[1] = (Rectangle){s * 3.5f, s * 6, 10.0f, 10.0f};
    coins[2] = (Rectangle){s * 4.5f, s * 6, 10.0f, 10.0f};
    coins[3] = (Rectangle){s * 5.5f, s * 6, 10.0f, 10.0f};
    coins[4] = (Rectangle){s * 8.5f, s * 3, 10.0f, 10.0f};
    coins[5] = (Rectangle){s * 9.5f, s * 3, 10.0f, 10.0f};
    coins[6] = (Rectangle){s * 10.5f, s * 3, 10.0f, 10.0f};
    coins[7] = (Rectangle){s * 14.5f, s * 4, 10.0f, 10.0f};
    coins[8] = (Rectangle){s * 15.5f, s * 4, 10.0f, 10.0f};
    coins[9] = (Rectangle){s * 17.5f, s * 2, 10.0f, 10.0f};
    
    for (int i = 0; i < COIN_COUNT; i++){visible[i] = true;}
}

void GameUpdate(){
    
    //UpdateScreen();// Adapt to resolution
    UpdatePlayer();
    UpdateCoin();
}

/*void UpdateScreen(){
    // Adapt to resolution
    if (IsWindowResized()){
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        scale = MAX(1, MIN((screenWidth/gameWidth), (screenHeight/gameHeight)));
        vpOffset.x = (screenWidth - (gameWidth * scale)) / 2;
        vpOffset.y = (screenHeight - (gameHeight * scale)) / 2;
    }
}*/

void UpdatePlayer(){
    const float MAX_SPEED = 6.0f;
    const float ACCELERATION = 0.1f;
    const float GRAVITY = 0.5f;
    const float JUMP_IMPULSE = -10.0f;
    const int JUMP_BUFFER_TIME = 30;
    static bool isGrounded = false;
    static int jumpBuffer = 0;
    static int horizontalInput = 0;
    static int verticalInput = 0;
    static Vector2 velocity = {0};
    static Vector2 previousVelocity = {0};
    
    
    // INPUT
    horizontalInput = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    verticalInput = (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W));
    if(IsKeyPressed(KEY_TAB)){PLATFORMER = !PLATFORMER;}
    
    // HORIZONTAL SPEED
    velocity.x += (horizontalInput * MAX_SPEED - velocity.x) * ACCELERATION;
    if (velocity.x < -MAX_SPEED){
        velocity.x = -MAX_SPEED;
    }
    else if (velocity.x > MAX_SPEED){
        velocity.x = MAX_SPEED;
    }
    
    // VERTICAL SPEED
    if (PLATFORMER){
        if (isGrounded && jumpBuffer != JUMP_BUFFER_TIME){
            jumpBuffer = JUMP_BUFFER_TIME;
        }
        if (isGrounded && IsKeyPressed(KEY_SPACE)){
            velocity.y = JUMP_IMPULSE;
            jumpBuffer = 0;
        }
        else if (jumpBuffer > 0 && IsKeyPressed(KEY_SPACE)){
            velocity.y = JUMP_IMPULSE;
            jumpBuffer = 0;
        }
        else{
            if (!IsKeyDown(KEY_SPACE) && velocity.y < JUMP_IMPULSE * 0.2){
                velocity.y = JUMP_IMPULSE * 0.2;
            }
            else{
                velocity.y += GRAVITY;
                if (velocity.y > -JUMP_IMPULSE){
                    velocity.y = -JUMP_IMPULSE;
                }
                if (jumpBuffer > 0){
                    jumpBuffer -= 1;
                }
            }
        }
    }
    else{
        // TOP-DOWN
        velocity.y += (verticalInput * MAX_SPEED - velocity.y) * ACCELERATION;
        if (velocity.y < -MAX_SPEED){
            velocity.y = -MAX_SPEED;
        }
        else if (velocity.y > MAX_SPEED){
            velocity.y = MAX_SPEED;
        }
    }
    
    previousVelocity = velocity;   // for ground check
    RectangleCollisionUpdate(&player, &velocity);
    isGrounded = previousVelocity.y > 0.0f && velocity.y <= 0.0001f;  // naive way to check grounded state
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
            Rectangle c = coins[i];
            float y = (float)sin(2 * PI * (time / 60.0f * 0.5) + (c.x * 5)) * 4; // pseudo random offset floating
            float x = (float)sin(2 * PI * (time / 60.0f * 2)) * 4;
            DrawRectangle((int)(c.x + 4 + x * 0.5), (int)(c.y + y), (int)(c.width - 4 - x), (int)c.height, GOLD);
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
    Rectangle colArea = RectangleResize(rect, velocity);
    RectList *tiles = RectangleListFromTiles(&colArea, &map);
    
    RectangleTileCollision(rect, velocity, tiles);
    // free allocated RectList memory
    MemFree(tiles->rect);
    MemFree(tiles);
}

Rectangle RectangleResize(Rectangle *rect, Vector2 *size){ //CalculateCollisionArea
    Rectangle collisionArea;
    if (size->x > 0) {
        collisionArea.x = rect->x;
        collisionArea.width = rect->width + size->x;
    } else {
        collisionArea.x = rect->x + size->x;
        collisionArea.width = rect->width - size->x;
    }
    if (size->y > 0) {
        collisionArea.y = rect->y;
        collisionArea.height = rect->height + size->y;
    } else {
        collisionArea.y = rect->y + size->y;
        collisionArea.height = rect->height - size->y;
    }
    return collisionArea;
}
/*
Rectangle RectangleResize(Rectangle *rect, Vector2 *size){
    Rectangle r;
    if (size->x > 0) {
        r.x = rect->x;
        r.width = rect->width + size->x;
    } else {
        r.x = rect->x + size->x;
        r.width = rect->width - size->x;
    }
    if (size->y > 0) {
        r.y = rect->y;
        r.height = rect->height + size->y;
    } else {
        r.y = rect->y + size->y;
        r.height = rect->height - size->y;
    }
    return r;
}
*/

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
