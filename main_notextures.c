#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_video.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include "linear_alg.h"

#define PI 3.14159f
#define WWIDTH 1200
#define WHEIGHT 900
#define MWIDTH 24
#define MHEIGHT 24

int map[MWIDTH][MHEIGHT]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


typedef struct Player {
    Vector2 position;
    Vector2 direction;
    Vector2 screen;
} Player;

SDL_Color *colors;

void castRays(SDL_Renderer *renderer, SDL_Renderer *renderer2D, Player *player, int mapScaler, int offset);
void initColors();
// typedef struct ScaledProjection {
//     int width;
//     int height;
//     int scale;
// } ScaledProjection;



int main(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING)==-1) {
        printf("Couldn't initialize SDL");
    }
    initColors();
    int scale = 1;
    int mapScaler = 32;
    // ScaledProjection scaledProjection = {WWIDTH/scale, WHEIGHT/scale, scale};
    int fps = 60;  
     
    SDL_Event keyboardEvent;
    Uint32 delayMs = (double)(((double)1.0f / (double)fps))*645;
    const Uint8 *pressedKeys = SDL_GetKeyboardState(NULL); 
    // Player player = {WWIDTH/2, WHEIGHT/2, 90, 0};
    Vector2 initialPos = {MWIDTH/2, MHEIGHT/2};
    Vector2 initialDir = {0, -1};
    Vector2 screen = {1, 0}; //perpendicolare alla direzione e di stessa lunghezza, così da avere fov=90°
    // Vector2 initialPos = {22, 12};
    // Vector2 initialDir = {-1, 0};
    // Vector2 screen = {0, 0.66}; //perpendicolare alla direzione e di stessa lunghezza, così da avere fov=90°
    Player player = {initialPos, initialDir, screen};
    SDL_Window *window;
    SDL_Window *window2D;
    SDL_Renderer *renderer;
    SDL_Renderer *renderer2D;
    // uint8_t **map = createRectMap(mapSize, mapSize);
    window =  SDL_CreateWindow("Dumb Raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WWIDTH, WHEIGHT, SDL_WINDOW_OPENGL);
    // window2D =  SDL_CreateWindow("Dumb Raycaster in 2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WWIDTH, WHEIGHT, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // renderer2D = SDL_CreateRenderer(window2D, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); 
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    // initColors();
    // SDL_RenderSetScale(renderer, mapScaler, mapScaler);
    int isGameRunning = 1;
    Uint32 currentFrame = 0;
    Uint32 prevFrame = currentFrame;
    Vector2i positionInMap;
    // int jmpOffset = 10;
    while (isGameRunning) {
        Uint32 lastTick = SDL_GetTicks();
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer2D, 0, 0, 0, 255);
        SDL_RenderClear(renderer2D);
        while (SDL_PollEvent(&keyboardEvent)) { 
            if (keyboardEvent.type == SDL_QUIT) {
                isGameRunning = 0;
            }
            if (keyboardEvent.type == SDL_MOUSEMOTION) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawLine(renderer, 0, 0, keyboardEvent.motion.x, keyboardEvent.motion.y);
                
                // printf("x:%d - y: %d", keyboardEvent.motion.x, keyboardEvent.motion.y);
            }

        }
        if (pressedKeys[SDL_SCANCODE_LEFT]) {
            rotateVector(&player.direction, -0.014); //circa 0.8°
            rotateVector(&player.screen, -0.014);
        } 
        if (pressedKeys[SDL_SCANCODE_RIGHT]) {
            rotateVector(&player.direction, 0.014); //circa 0.8°
            rotateVector(&player.screen, 0.014);
        }    
        if (pressedKeys[SDL_SCANCODE_UP]) {
            player.position.x+=player.direction.x/10;
            player.position.y+=player.direction.y/10;
            positionInMap.x = (int)floor(player.position.x+player.direction.x);
            positionInMap.y = (int)floor(player.position.y+player.direction.y);
            if (map[positionInMap.y][positionInMap.x]!=0) {
                player.position.x-=player.direction.x/10;
                player.position.y-=player.direction.y/10;
            }
            // if (map[positionInMap.x][positionInMap.y]!=0) {
            //     player.position.x = positionInMap.x;
            // }

        } 
        if (pressedKeys[SDL_SCANCODE_DOWN]) {
            player.position.x-=player.direction.x/10;
            player.position.y-=player.direction.y/10;
            positionInMap.x = (int)floor(player.position.x);
            positionInMap.y = (int)floor(player.position.y);
            if (map[positionInMap.y][positionInMap.x]!=0) {
                player.position.x+=player.direction.x/10;
                player.position.y+=player.direction.y/10;
            }
            // if (map[positionInMap.x][positionInMap.y]!=0) {
            //     player.position.x = positionInMap.x;
            // }
        } 
        if (pressedKeys[SDL_SCANCODE_SPACE]) {
            if (currentFrame==0) {
                prevFrame = currentFrame;
                currentFrame+=2;
            }
        }
        if (currentFrame>0 && prevFrame < currentFrame) {
            prevFrame = currentFrame; 
            currentFrame+=2;
        }
        if (currentFrame == 50 || (currentFrame>0 && prevFrame > currentFrame)) {
            prevFrame = currentFrame;
            currentFrame-=2;
        }        
        // SDL_SetRenderDrawColor(renderer2D, 255, 255, 255, 255);
        // for (int i = 0; i < MHEIGHT; i++) {
        //     for (int j = 0; j < MWIDTH; j++) {
        //         if (map[i][j]!=0) {
        //             SDL_Rect rect = {j*30, i*30, 30, 30};
        //             SDL_RenderDrawRect(renderer2D, &rect);
        //         }
        //     }
        // }
        //INIZIO render 2D di prova
        // SDL_RenderDrawPointF(renderer2D, player.position.x*30, player.position.y*30);
        // SDL_RenderDrawLineF(renderer2D, player.position.x*30, player.position.y*30, player.position.x*30 + player.direction.x*30 , player.position.y*30 + player.direction.y*30 );
        // SDL_RenderDrawLineF(renderer2D, player.position.x*30+player.direction.x*30, player.position.y*30 + player.direction.y*30 , player.position.x*30+player.direction.x*30+player.screen.x*30, player.position.y*30+player.direction.y*30+player.screen.y*30);
        // SDL_RenderDrawLineF(renderer2D, player.position.x*30+player.direction.x*30, player.position.y*30 + player.direction.y*30 , player.position.x*30+player.direction.x*30-player.screen.x*30, player.position.y*30+player.direction.y*30-player.screen.y*30);
        //FINE render 2D di prova
        castRays(renderer, renderer2D, &player, mapScaler, currentFrame);
        SDL_RenderPresent(renderer);
        SDL_RenderPresent(renderer2D);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        Uint32 currentDelay = SDL_TICKS_PASSED(lastTick, SDL_GetTicks());
        SDL_Delay(delayMs - currentDelay);
    }
}

void castRays(SDL_Renderer *renderer, SDL_Renderer *renderer2D, Player *player, int mapScaler, int offset) {
    const int wcenter = WWIDTH/2 + offset;
    for (int i = 0; i < WWIDTH; i++) {
        double cameraX = (double)((double)2 * i / (double)WWIDTH) - 1;
        //vettore che rappresenta la direzione del raggio
        Vector2 ray;
        ray.x = player->direction.x + player->screen.x * cameraX;
        ray.y = player->direction.y + player->screen.y * cameraX;
        //vettore che rappresenta il raggio vero e proprio
        Vector2 tmp;
        tmp.x = ray.x + player->position.x;
        tmp.y = ray.y + player->position.y;
        //vettore che rappresenta la distanza percorsa per ogni unita' in X e Y
        Vector2 unaryLength;
        unaryLength.x = ray.x!=0 ? sqrt(1 + pow(ray.y/ray.x, 2)) : DBL_MAX;
        unaryLength.y = ray.y!=0 ? sqrt(1 + pow(ray.x/ray.y, 2)) : DBL_MAX;
        //vettore che rappresenta la posizione corrente nella tilemap
        Vector2i currentPosInMap;
        currentPosInMap.x = floor(tmp.x);
        currentPosInMap.y = floor(tmp.y);
        Vector2 currentLength;
        Vector2i direction;
        //calcolo della distanza del punto dal bordo orizzontale e verticale
        if (ray.x<0) {
            direction.x = -1;
            currentLength.x = (tmp.x - currentPosInMap.x) * unaryLength.x;
        } else {
            direction.x = 1;
            currentLength.x = (currentPosInMap.x+1.0 - tmp.x) * unaryLength.x;
        }
        if (ray.y<0) {
            direction.y = -1;
            currentLength.y = (tmp.y - currentPosInMap.y) * unaryLength.y;
        } else {
            direction.y = 1;
            currentLength.y = (currentPosInMap.y+1.0 - tmp.y) * unaryLength.y;
        }
        int side;
        //dda
        while (map[currentPosInMap.y][currentPosInMap.x]==0) {
            if (currentLength.x < currentLength.y) {
                currentPosInMap.x += direction.x;
                currentLength.x += unaryLength.x;
                side=1;
            } else {
                currentPosInMap.y += direction.y;
                currentLength.y += unaryLength.y;
                side=2;
            }
        }
        double wallDist;
        if (side==1) {
            wallDist = currentLength.x-unaryLength.x;
        } else {
            wallDist = currentLength.y - unaryLength.y;
        }
        // wallDist = wallDist*cos(dotProduct(player->screen, ray)/(norm(ray)*norm(player->screen)));
        int halfWallHeight = floor((WHEIGHT/wallDist)/2);
        //INIZIO RENDER 3D
        SDL_SetRenderDrawColor(renderer, 0, 0, 150, 255);
        SDL_RenderDrawLine(renderer, i, 0, i, wcenter-halfWallHeight);
        // SDL_SetRenderDrawColor(renderer, colors[map[currentPosInMap.y][currentPosInMap.x]-1].r/side, colors[map[currentPosInMap.y][currentPosInMap.x]-1].g/side, colors[map[currentPosInMap.y][currentPosInMap.x]-1].b/side, 255);
        SDL_SetRenderDrawColor(renderer, 255/side, 255/side, 255/side, 255);
        SDL_RenderDrawLine(renderer, i, wcenter-halfWallHeight, i, wcenter+halfWallHeight);
        SDL_RenderDrawLine(renderer, i, wcenter+halfWallHeight, i, wcenter+halfWallHeight*2.5); //riflesso
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 130);
        SDL_RenderDrawLine(renderer, i, wcenter+halfWallHeight, i, WHEIGHT-1);
        //FINE RENDER 3D
        // SDL_RenderDrawLineF(renderer2D, player->position.x*30, player->position.y*30, (player->position.x+wallDist*ray.x)*30, (player->position.y+wallDist*ray.y)*30);


        // SDL_RenderDrawLineF(renderer, player->position.x*30, player->position.y*30, tmp.x*30+currentLength.x*30*direction.x, tmp.y*30+currentLength.y*30*direction.y);
    }
}

void initColors() {
    colors = malloc(4*sizeof(SDL_Color));
    colors[0].r = 255;
    colors[0].g = 0;
    colors[0].b = 0;
    colors[1].r = 10;
    colors[1].g = 250;
    colors[1].b = 0;
    colors[2].r = 0;
    colors[2].g = 10;
    colors[2].b = 250;
    colors[3].r = 255;
    colors[3].g = 255;
    colors[3].b = 255;
    for (int i = 0; i < 4; i++) {
        colors[i].a = 255;
    }
}