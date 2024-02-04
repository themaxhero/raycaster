#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define LEVEL_BLOCK_SIZE 64
#define LEVEL_ROW_SIZE 8
#define LEVEL_COL_SIZE 8
#define PLAYER_INITIAL_X 350
#define PLAYER_INITIAL_Y 250
#define PLAYER_SIZE 16
#define LEVEL_SIZE 64
#define VIEW_ANGLE 90.0
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct Player {
  Rectangle boundaries;
  Vector2 origin;
  float rotation;
  float bodyRotation;
} Player;

const char* level1[] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 1, 0, 0, 0, 0, 1,
  1, 0, 1, 0, 0, 0, 0, 1,
  1, 0, 1, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 1, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
};

int setupPlayer(Player *player){
  player->boundaries = (Rectangle) {PLAYER_INITIAL_X, PLAYER_INITIAL_Y, PLAYER_SIZE, PLAYER_SIZE};
  player->origin.x = ((float) player->boundaries.width / 2);
  player->origin.y = ((float) player->boundaries.height / 2);
  player->rotation = 0;
  player->bodyRotation = 0;
}

int main(int argc, char *argv[]){
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Test");

  float deltaTime;
  Player *player = malloc(sizeof(Player));
  setupPlayer(player);
  Vector2 leftEdge;
  Vector2 rightEdge;
  Vector2 leftUnit;
  Vector2 rightUnit;
  int playerGridX;
  int playerGridY;
  int playerGridIndex;

  while (!WindowShouldClose()) {
    deltaTime = GetFrameTime();
    leftEdge.x = player->boundaries.x + (-SCREEN_WIDTH * cos(player->rotation - (VIEW_ANGLE / 2)));
    leftEdge.y = player->boundaries.y + (-SCREEN_HEIGHT * sin(player->rotation - (VIEW_ANGLE / 2)));
    rightEdge.x = player->boundaries.x + (-SCREEN_WIDTH * cos(player->rotation + (VIEW_ANGLE / 2)));
    rightEdge.y = player->boundaries.y + (-SCREEN_HEIGHT * sin(player->rotation + (VIEW_ANGLE / 2)));
    leftUnit = Vector2Normalize(leftEdge);
    rightUnit = Vector2Normalize(rightEdge);
    playerGridX = (int)(player->boundaries.x / LEVEL_BLOCK_SIZE);
    playerGridY = (int)(player->boundaries.y / LEVEL_BLOCK_SIZE);
    playerGridIndex = (playerGridY * LEVEL_COL_SIZE) + playerGridX;

    if (IsKeyDown(KEY_A)){
      player->rotation -= 7.5 * deltaTime;
      player->bodyRotation -= 430.0 * deltaTime;
    } else if (IsKeyDown(KEY_D)){
      player->rotation += 7.5 * deltaTime;
      player->bodyRotation += 430.0 * deltaTime;
    }

    if (IsKeyDown(KEY_W)){
      player->boundaries.x -= 100 * cos(player->rotation) * deltaTime;
      player->boundaries.y -= 100 * sin(player->rotation) * deltaTime;
    } else if (IsKeyDown(KEY_S)) {
      player->boundaries.x += 100 * cos(player->rotation) * deltaTime;
      player->boundaries.y += 100 * sin(player->rotation) * deltaTime;
    }

    BeginDrawing();

      ClearBackground(BLACK);

      for(int i = 0; i < LEVEL_SIZE; i++){
        int iGridX = (i % LEVEL_ROW_SIZE);
        int iGridY = (int)(i / LEVEL_COL_SIZE);
        int blockX = LEVEL_BLOCK_SIZE * iGridX;
        int blockY = LEVEL_BLOCK_SIZE * iGridY;
        if (level1[i]){
          // Drawing Level Blocks
          DrawRectangle(blockX, blockY, LEVEL_BLOCK_SIZE, LEVEL_BLOCK_SIZE, RED);
          DrawRectangleLines(blockX, blockY, LEVEL_BLOCK_SIZE, LEVEL_BLOCK_SIZE, RAYWHITE);
          DrawText(
              TextFormat("%d", i),
              blockX + (int)(LEVEL_BLOCK_SIZE / 2) - 8,
              blockY + (int)(LEVEL_BLOCK_SIZE / 2) - 8,
              16,
              RAYWHITE
          );

          // Casting the Rays
          
          // Ray source
          Vector2 sourcePoint = Vector2Clamp(
              (Vector2){player->boundaries.x, player->boundaries.y},
              (Vector2){blockX, blockY},
              (Vector2){blockX+LEVEL_BLOCK_SIZE,blockY+LEVEL_BLOCK_SIZE}
          );

          // Position of Player inside the grid

          // Is player positioned after the block we're currently verifying?
          int playerBeforeBlock = i < playerGridIndex;

          int forStart =
            (!playerBeforeBlock * playerGridIndex+1) + (playerBeforeBlock * i);

          int forEnd =
            (!playerBeforeBlock * i) + (playerBeforeBlock * LEVEL_SIZE);

          printf(
            "block: %d, playerIndex: %d, playerBeforeBlock?: %d, forStart: %d, forEnd: %d\n",
            i,
            playerGridIndex,
            playerBeforeBlock,
            forStart,
            forEnd
          );

          bool obstructed = false;
          for(int j = forStart; j < forEnd; j++){
            if (obstructed){
              printf("obstructed by %d\n", j - 1);
              break;
            }
            if (i == j) continue;

            int jx, jy;
            jx = (j % LEVEL_ROW_SIZE);
            jy = (int)(i / LEVEL_COL_SIZE);

            if (jx - playerGridX == 0 && jy - playerGridY == 0) continue;

            //if (playerAfterSourceBlock && (playerGridX - iGridX) > (playerGridX - jx)) continue;
            //if (playerAfterSourceBlock && (playerGridY - iGridY) > (playerGridY - jy)) continue;

            obstructed = obstructed || !!level1[j];
          }
          if (!obstructed)
            DrawLine(sourcePoint.x, sourcePoint.y, player->boundaries.x, player->boundaries.y, YELLOW);
        } else {
          DrawRectangleLines(blockX, blockY, LEVEL_BLOCK_SIZE, LEVEL_BLOCK_SIZE, GRAY);
          DrawText(
              TextFormat("%d", i),
              blockX + (int)(LEVEL_BLOCK_SIZE / 2) - 8,
              blockY + (int)(LEVEL_BLOCK_SIZE / 2) - 8,
              16,
              GRAY
          );
        }
      }

      // Draws the Player
      DrawRectanglePro(player->boundaries, player->origin, player->bodyRotation, RAYWHITE);

      // Draw the line that points towards the direction that the player is looking at.
      DrawLine(
          player->boundaries.x,
          player->boundaries.y,
          player->boundaries.x + (-25 * cos(player->rotation)),
          player->boundaries.y + (-25 * sin(player->rotation)),
          GREEN
      );

      // Draw the boundaries of player's view cone
      // DrawLine(player->boundaries.x, player->boundaries.y, leftEdge.x, leftEdge.y, MAGENTA);
      // DrawLine(player->boundaries.x, player->boundaries.y, rightEdge.x, rightEdge.y, MAGENTA);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
