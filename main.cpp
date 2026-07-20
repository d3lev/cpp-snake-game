#include <iostream>
#include <raylib.h>
#include <deque>
#include <cmath>
#include <raymath.h>

using namespace std;

Color red = {168, 10, 10, 255};
Color snakeWhite = {220, 220, 210, 255};
Color darkRed = {84, 3, 3, 255};

int cellSize = 25;
int cellCount = 20;
int offset = 50;

double lastUpdTime = 0;

bool elementInDeque(Vector2 element, deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if(Vector2Equals(deque[i], element)) {
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdTime >= interval) {
        lastUpdTime  = currentTime;
        return true;
    }
    return false;
}


class Snake {
    public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;

    
    void Draw() {
        for (unsigned int i = 0; i < body.size(); i++) {
            int x = round(body[i].x);
            int y = round(body[i].y);
            DrawRectangle(offset + x * cellSize, offset + y * cellSize, cellSize, cellSize, snakeWhite);
        }
    }
    
    void Update() {

        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true) {
            addSegment = false;
        } else {
            body.pop_back();
        }
    }

    void Reset() {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Food {
    public:
    
    Vector2 position;

    Vector2 GenerateRandomCell() {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x,y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody) {
        Vector2 position = GenerateRandomCell();
        while (elementInDeque(position, snakeBody)) {
            position = GenerateRandomCell();
        }
        return position;
    }
    
    Food(deque<Vector2> snakeBody) {
        position = GenerateRandomPos(snakeBody);
    }
    
    void Draw() {
        DrawRectangle(offset + position.x * cellSize, offset + position.y * cellSize, cellSize, cellSize, darkRed);   
    }
    
};

class Game {
    public: 
        Snake snake;
        Food food = Food(snake.body);
        bool running = true;
        int score = 0;
        Texture2D gameOverImage = LoadTexture("graphics/guy.png");

        void Draw() {
            food.Draw();
            snake.Draw();
            if (!running) {
                DrawGameOverOverlay();
            }
        }

        void DrawGameOverOverlay() {

            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));
 
            const char* msg = "GAME OVER!";
            int fontSize = 50;
            int textWidth = MeasureText(msg, fontSize);
            DrawText(msg, GetScreenWidth()/2 - textWidth/2, GetScreenHeight()/2 - 140, fontSize, snakeWhite);
 
            const char* subMsg = "Press W/A/S/D or an arrow key to restart";
            int subFontSize = 20;
            int subWidth = MeasureText(subMsg, subFontSize);
            DrawText(subMsg, GetScreenWidth()/2 - subWidth/2, GetScreenHeight()/2 - 80, subFontSize, snakeWhite);
 
            DrawTexture(gameOverImage, GetScreenWidth()/2 - gameOverImage.width/2, GetScreenHeight()/2 - 20, WHITE);
        }


        void Update() {
            if (running) {
            snake.Update();
            CheckFoodCollision();
            CheckWallCollision();
            CheckBodyCollision();
            }
        }

        void CheckFoodCollision() {
            if (Vector2Equals(snake.body[0], food.position)) {

                food.position = food.GenerateRandomPos(snake.body);
                snake.addSegment = true;
                score++;
            }
        }

        void CheckWallCollision() {
            if (snake.body[0].x == cellCount || snake.body[0].x == -1) {
                GameOver();
            }
            if (snake.body[0].y == cellCount || snake.body[0].y == -1) {
                GameOver();
            }
        }

        void CheckBodyCollision() {
            deque<Vector2> headlessBody = snake.body;
            headlessBody.pop_front();
            if (elementInDeque(snake.body[0], headlessBody)) {
                GameOver();
            }
        }

        void GameOver() {
            snake.Reset();
            food.position = food.GenerateRandomPos(snake.body);
            running = false;
            score = 0;
        }
};
int main () {
    
    cout << "starting the game..." << endl;
    InitWindow(2*offset + cellSize*cellCount, 2*offset + cellSize*cellCount, "SIMPLE SNAKE");
    SetTargetFPS(60);

    Game game;
    
    while(WindowShouldClose() == false) {
        BeginDrawing();

        if (eventTriggered(0.15)) {
            game.Update();
        }

        if((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && game.snake.direction.y != 1) {
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if((IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) && game.snake.direction.y != -1) {
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && game.snake.direction.x != 1) {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && game.snake.direction.x != -1) {
            game.snake.direction = {1, 0};
            game.running = true;
        }


        ClearBackground(red);
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize*cellCount + 10, (float)cellSize*cellCount + 10}, 5, darkRed);
        DrawText("SIMPLE SNAKE", offset - 5, 15, 30, darkRed);
        DrawText(TextFormat("%i", game.score), offset - 5 , offset + cellSize*cellCount + 10, 30, darkRed);
        game.Draw();

        EndDrawing();
    }
    
    UnloadTexture(game.gameOverImage);
    CloseWindow();
    cout << "GOODBYE :<" << endl;
    return 0;
}