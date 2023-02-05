#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <bitset>
#include <cmath>
#include "grid.cpp"
#include "game.cpp"

#define SCREEN_SIZE 600

#define BIG_TIC_SIZE 180
#define SMALL_TIC_SIZE 50
#define SYMBOL_SIZE 35
#define FONT_SIZE 100

const int PADDING_BIG = (SCREEN_SIZE - 3*BIG_TIC_SIZE)/2;
const int PADDING_SMALL = (BIG_TIC_SIZE - 3*SMALL_TIC_SIZE)/2;
const int PADDING_SYMBOL = (SMALL_TIC_SIZE - SYMBOL_SIZE)/2;

SDL_Window* window;
SDL_Renderer* rend;
TTF_Font* font;
SDL_Texture* xTurnText;
SDL_Texture* oTurnText;
TicTacToeGame game;

SDL_Window* initWindow() {

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return nullptr;
    }

    auto* window = SDL_CreateWindow( "Ultimate Tic Tac Toe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_SIZE, SCREEN_SIZE, SDL_WINDOW_SHOWN );
    
    return window;
}

void SDL_RenderFillCircle(SDL_Renderer *rend, int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int radiusError = 1 - x;
    while (x >= y)
    {
        SDL_RenderDrawLine(rend, x + x0, y + y0, -x + x0, y + y0);
        SDL_RenderDrawLine(rend, y + x0, x + y0, -y + x0, x + y0);
        SDL_RenderDrawLine(rend, -x + x0, -y + y0, x + x0, -y + y0);
        SDL_RenderDrawLine(rend, -y + x0, -x + y0, y + x0, -x + y0);
        y++;
        if (radiusError < 0)
            radiusError += 2 * y + 1;
        else
        {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}

void drawPolygon(int x, int y, int radius, int n) {
    SDL_Point points[n + 1];
    for (int i = 0; i <= n; i++) {
        double angle = (2*M_PI/n) * i;
        points[i].x = (int) (x + radius*cos(angle));
        points[i].y = (int) (y + radius*sin(angle));
    }

    SDL_RenderDrawLines(rend, points, n + 1);
}

void drawTicTacToeGrid(int x, int y, int gridSize) {
    SDL_RenderDrawLine(rend, x, y + gridSize, x + 3*gridSize, y + gridSize);
    SDL_RenderDrawLine(rend, x, y + 2*gridSize, x + 3*gridSize, y + 2*gridSize);
    SDL_RenderDrawLine(rend, x + gridSize, y, x + gridSize, y + 3*gridSize);
    SDL_RenderDrawLine(rend, x + 2*gridSize, y, x + 2*gridSize, y + 3*gridSize);
}

void drawX(int x, int y, int size) {
    SDL_SetRenderDrawColor(rend, 0xFF, 0x00, 0x00, 0x00);
    SDL_RenderDrawLine(rend, x, y, x + size, y + size);
    SDL_RenderDrawLine(rend, x + size, y, x, y + size);
}

void drawO(int x, int y, int size) {
    SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0xFF, 0x00);
    // SDL_RenderFillCircle(rend, x + size/2, y + size/2, size/2);
    // SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0x00);
    // SDL_RenderFillCircle(rend, x + size/2, y + size/2, size/2 - 1);
    drawPolygon(x + size/2, y + size/2, size/2, 60);
    // SDL_RenderDrawLine(rend, x, y, x + size, y);
    // SDL_RenderDrawLine(rend, x, y, x, y + size);
    // SDL_RenderDrawLine(rend, x + size, y, x + size, y + size);
    // SDL_RenderDrawLine(rend, x, y + size, x + size, y + size);
}


void drawFullGrid() {
    // Fill the surface white
    SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0x00);
    SDL_RenderFillRect(rend, nullptr);

    // Write whose turn it is
    SDL_Rect location{PADDING_BIG, 0, 100, PADDING_BIG + PADDING_SMALL};
    SDL_RenderCopy(rend, game.xTurn ? xTurnText : oTurnText, nullptr, &location);
    
    //Draw the big tic tac toe lines
    SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, 0x00);
    drawTicTacToeGrid(PADDING_BIG, PADDING_BIG, BIG_TIC_SIZE);

    //Draw the small tic tac toe lines
    SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, 0x00);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int x = PADDING_BIG + j*BIG_TIC_SIZE + PADDING_SMALL;
            int y = PADDING_BIG + i*BIG_TIC_SIZE + PADDING_SMALL;
            drawTicTacToeGrid(x, y, SMALL_TIC_SIZE);
        }
    }

    //Draw the symbols and highlight valid next moves
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 3; l++) {
                    int x = PADDING_BIG + j*BIG_TIC_SIZE + PADDING_SMALL + l*SMALL_TIC_SIZE + PADDING_SYMBOL;
                    int y = PADDING_BIG + i*BIG_TIC_SIZE + PADDING_SMALL + k*SMALL_TIC_SIZE + PADDING_SYMBOL;
                    int tileIndex = 27*i + 9*j + 3*k + l;
                    if (game.grid.x[tileIndex]) {
                        drawX(x, y, SYMBOL_SIZE);
                    }
                    if (game.grid.o[tileIndex]) {
                        drawO(x, y, SYMBOL_SIZE);
                    }
                    if (game.validMoves[tileIndex]) {
                        SDL_SetRenderDrawColor(rend, 0x00, 0xFF, 0x00, 0x00);
                        SDL_Rect location{x, y, SYMBOL_SIZE, SYMBOL_SIZE};
                        SDL_RenderFillRect(rend, &location);
                    }
                }
            }
        }
    }

    //Draw the wins, iff you already won
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int x = PADDING_BIG + j*BIG_TIC_SIZE + PADDING_SMALL;
            int y = PADDING_BIG + i*BIG_TIC_SIZE + PADDING_SMALL;

            if (game.xWinners & (1 << (3*i + j))) {
                drawX(x, y, SMALL_TIC_SIZE*3);
            }

            if (game.oWinners & (1 << (3*i + j))) {
                drawO(x, y, SMALL_TIC_SIZE*3);
            }
        }
    }

    
}

int* getTileFromMouseClick(int x, int y) {
    int* ret = new int[4];
    x -= PADDING_BIG + PADDING_SMALL;
    y -= PADDING_BIG + PADDING_SMALL;
    ret[0] = std::min(y/BIG_TIC_SIZE, 2);
    ret[1] = std::min(x/BIG_TIC_SIZE, 2);
    
    x %= BIG_TIC_SIZE;
    y %= BIG_TIC_SIZE;
    ret[2] = std::min(y/SMALL_TIC_SIZE, 2);
    ret[3] = std::min(x/SMALL_TIC_SIZE, 2);

    return ret;
}


int main() {

    window = initWindow();

    if (window == nullptr) {
        std::cout << "Window wasn't able to be initialized" << std::endl;
        return 0;
    }
    
    rend = SDL_CreateRenderer(window, -2, 0);
    
    if (rend == nullptr) {
        std::cout << "Renderer wasn't able to be initialized" << std::endl;
        return 0;
    }

    TTF_Init();
    font = TTF_OpenFont("./resources/OpenSans-Regular.ttf", FONT_SIZE);

    if (font == nullptr) {
        std::cout << "Font wasn't able to be initialized" << std::endl;
        return 0;
    }
    //Create the texts
    SDL_Color black{0, 0, 0};
    SDL_Surface* xTurnTextSurface = TTF_RenderText_Solid(font, "X's Turn", black);
    xTurnText = SDL_CreateTextureFromSurface(rend, xTurnTextSurface);

    SDL_Surface* oTurnTextSurface = TTF_RenderText_Solid(font, "O's Turn", black);
    oTurnText = SDL_CreateTextureFromSurface(rend, oTurnTextSurface);

    SDL_Event e; 
    bool quit = false; 
    while (quit == false) { 
        while (SDL_PollEvent(&e)) { 
            if (e.type == SDL_QUIT) {
                quit = true; 
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int* tindices = getTileFromMouseClick(e.button.x, e.button.y);
                game.makeMove(tindices);
                if (game.bigWinner()) {
                    quit = true;
                    break;
                }
            }
        }

        drawFullGrid();
        SDL_RenderPresent(rend);
    }

    //Destroy stuff
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);

    //Quit SDL subsystems
    SDL_Quit();
    TTF_Quit();
}