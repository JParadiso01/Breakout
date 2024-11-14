#include "WinEasy.h"
#include <stdio.h>

//TODO: Make proper game over screen
//TODO: Make ball bouncing on bricks better
//TODO: Fix where ball gets inside of paddle (make it bounce off the side)

typedef struct {
    int x;
    int y;
} Vec2;

typedef struct {
    float x;
    float y;
} Vec2f;

typedef struct {
    Vec2f pos;
    Vec2f speed;
    int radius;
} Ball;

Ball ball;


typedef struct {
    RECT rect;
    WINBOOL broken;
} Brick;

#define MAX_BRICKS 100
Brick bricks[MAX_BRICKS] = {0};
int brick_count = 0;
#define BRICK_HEIGHT 12
#define BRICK_WIDTH 60
#define BRICK_PADDING 40
#define BRICK_SPACE 15

typedef struct {
    RECT rect;
} Paddle;

Paddle paddle;


#define WIDTH  800
#define HEIGHT 600
#define FPS    20

int score = 0;

WINBOOL game_over = 0;

int randRange(int low, int high){
    return ((rand()%(high-low))+low);
}
void InitializeBall(Ball *ball){
    ball->pos.x   = randRange(100, WIDTH/2);
    ball->pos.y   = randRange(HEIGHT/2,400);
    ball->speed.x = 0.2;
    ball->speed.y = 0.2;
    ball->radius  = 6;
}


void InitializeBricks(Brick bricks[MAX_BRICKS], int rows, int cols){
    brick_count = rows*cols;
    if (brick_count > MAX_BRICKS){
        printf("Amount of bricks being created is larger than MAX_BRICKS allows");
        exit(1);
    }
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){

            int l = BRICK_PADDING + (2*BRICK_SPACE) + (j*BRICK_WIDTH);
            int r = BRICK_PADDING + (BRICK_SPACE)   + (j*BRICK_WIDTH) + BRICK_WIDTH;
            int b = BRICK_PADDING + (BRICK_SPACE)   + ((i*2)*BRICK_HEIGHT);
            int t = BRICK_PADDING + (BRICK_SPACE)   + ((i*2)*BRICK_HEIGHT) + BRICK_HEIGHT;


            bricks[(i*cols)+j].rect = (RECT){
                                    .left   =  l,
                                    .right  =  r,
                                    .top    =  t,
                                    .bottom =  b,
                                    };
            bricks[(i*cols)+j].broken = 0;
        }
    }
}

void updateBall(Ball *ball){
    ball->pos.x += ball->speed.x;
    ball->pos.y += ball->speed.y;

    int x = ball->pos.x;
    int y = ball->pos.y;

    //BRICK CHECK
    for (int i = 0; i < brick_count; i++){
        if ((x >= bricks[i].rect.left && x <= bricks[i].rect.right) && (y >= bricks[i].rect.bottom && y <= bricks[i].rect.top) && bricks[i].broken == 0)
        {
            ball->speed.y *= -1;
            bricks[i].broken = 1;
            score += 50;
        }
    }

    //PADDLE CHECK
    if ((x >= paddle.rect.left && x <= paddle.rect.right) && (y >= paddle.rect.bottom && y <= paddle.rect.top)){
        ball->speed.y *= -1;
    }

    //WINDOW BOUNDS
    if(x > WIDTH || x < 0){
        ball->pos.x = (ball->pos.x < 0) ? 0 : WIDTH;
        ball->speed.x *= -1;
    }
    if(y < 0){  
        ball->pos.y = 0;
        //ball->pos.y = (ball->pos.y < 0) ? 0 : HEIGHT;
        ball->speed.y *= -1;
    }

    if (y > HEIGHT){
        game_over = 1;
    }
}

void PrintBricks(Brick brick[MAX_BRICKS]){
    for (int i = 0; i < brick_count; i++){
        printf("top: %ld, bottom: %ld, left: %ld, right: %ld\n", brick[i].rect.top,brick[i].rect.bottom, brick[i].rect.left, brick[i].rect.right);
        printf("Broken: %d\n", brick[i].broken);
    }
}

void DrawGameOverScreen(HDC screen){
    WinEasyDrawText(screen, colors[WHITE], "Game Over", (WIDTH/2), HEIGHT/2-30);
    WinEasyDrawText(screen, colors[WHITE], "Press R to play again", (WIDTH/2), (HEIGHT/2));
    char gameOverScoreText[100];
    sprintf(gameOverScoreText, "Your score was: %d", score);
    WinEasyDrawText(screen, colors[WHITE], gameOverScoreText, (WIDTH/2), (HEIGHT/2)+30);
}

void ResetGame(){
    InitializeBall(&ball);
    for (int i = 0; i < brick_count; i++){
        (&bricks[i])->broken = 0;
    }
    game_over = 0;
    score = 0;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch(msg)
    {   
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        case WM_KEYDOWN:
            switch (wParam)
            {
                //R
                case 0x52:
                    if (game_over){
                        ResetGame();
                    }
                break;
                //<
                case VK_LEFT:
                    if (!(paddle.rect.left <= 0)){
                        paddle.rect.right -= 10;
                        paddle.rect.left -= 10;
                    }
                break;

                //>
                case VK_RIGHT:
                    if (!(paddle.rect.right >= WIDTH)){
                        paddle.rect.right += 10;
                        paddle.rect.left += 10;
                    }
                break;

                default:
                break;
            }
        break;

        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC frontHDC, backHDC;
            HBITMAP backBuffer;
            RECT windowRect = {0,0,0,0};
            clock_t beg_time = clock();
            WinEasyStartBackBuffer(&ps, &hwnd, &frontHDC, &backHDC, &backBuffer, &windowRect);
            SetBkColor(backHDC, WinEasyColorToCOLORREF(colors[DARK_GRAY]));
            SetTextAlign(backHDC, TA_CENTER);
            if (game_over){
                DrawGameOverScreen(backHDC);
            }
            else{
                WinEasyDrawCircle(backHDC, colors[GREEN], ball.pos.x, ball.pos.y, ball.radius);

                for (int i = 0; i < MAX_BRICKS; i++){
                    if (!bricks[i].broken){
                        WinEasyDrawRect(backHDC, colors[RED], bricks[i].rect);
                    }
                }

                WinEasyDrawRect(backHDC, colors[BLUE],paddle.rect);

                updateBall(&ball);

                char scoreText[100];
                sprintf(scoreText, "Score: %d ", score);
                SetTextAlign(backHDC, TA_RIGHT);
                WinEasyDrawText(backHDC, colors[WHITE],scoreText, WIDTH, 0);
            }

            WinEasyCopyBackBuffer(frontHDC, backHDC, windowRect);
            WinEasyEndBackBuffer(ps, hwnd, backHDC, backBuffer);
            InvalidateRect(hwnd, &(RECT){windowRect.top, windowRect.left, windowRect.right, windowRect.bottom}, FALSE);

            clock_t end_time = clock();
            WinEasySleepForFrames(FPS, beg_time, end_time);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    srand(time(NULL));
    paddle.rect = (RECT){
                    .bottom = HEIGHT - 120,
                    .top = HEIGHT - 100,
                    .right = WIDTH/2 + 50,
                    .left = WIDTH/2 - 50 
    };

    InitializeBall(&ball);
    InitializeBricks(bricks, 5, 11);
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
    char ClassName[] = "myWindowClass";
    WinEasyCreateWindowClass(&wc, hInstance, &WndProc, ClassName, WinEasyColorToHBRUSH(colors[DARK_GRAY]));

    // Step 2: Creating the Window
    WinEasyCreateWindow(&hwnd, "Breakout", ClassName, hInstance, WIDTH, HEIGHT, 500, 200);

    ShowWindow(hwnd, nCmdShow);

    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}