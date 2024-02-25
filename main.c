#include "raylib.h"
#include "raymath.h"
#include "settings.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
  Vector2 pos;
  Vector2 dir;
  int moveProgress;
  Vector2 nextDir;
} SnakeHead;

typedef struct {
  Vector2 pos;
  Vector2 dir;
} SnakePart;

void HandleKeypress(float dt, SnakeHead *head) {
  if (IsKeyDown(KEY_RIGHT)) {
    head->nextDir = (Vector2){+1, 0};
  } else if (IsKeyDown(KEY_LEFT)) {
    head->nextDir = (Vector2){-1, 0};
  } else if (IsKeyDown(KEY_UP)) {
    head->nextDir = (Vector2){0, -1};
  } else if (IsKeyDown(KEY_DOWN)) {
    head->nextDir = (Vector2){0, +1};
  }
}

bool CheckCollision(Vector2 a, Vector2 b) {
  return fabsf(a.x - b.x) < PIECE_SIZE && fabsf(a.y - b.y) < PIECE_SIZE;
}

void UpdateApplePosition(Vector2 *pos, SnakeHead head, SnakePart *snakeBody,
                         int snakeBodySize) {
  while (true) {
    pos->x = GetRandomValue(0, GAME_SIZE - 1) * PIECE_SIZE;
    pos->y = GetRandomValue(0, GAME_SIZE - 1) * PIECE_SIZE;

    if (CheckCollision(*pos, head.pos)) {
      continue;
    }

    for (int i = 0; i < snakeBodySize; i++) {
      if (CheckCollision(*pos, snakeBody[i].pos)) {
        continue;
      }
    }

    break;
  }
}

void DrawScore(int score) {
  char text[10];
  snprintf(text, sizeof text, "Score %d", score);
  DrawText(text, 15, 15, 20, WHITE);
}

void HandleMove(SnakeHead *snakeHead, SnakePart *snakeBody, int snakeBodySize,
                float dt) {
  int posChange = dt * SNAKE_SPEED;
  int remainingMovement = PIECE_SIZE - snakeHead->moveProgress;

  if (posChange >= remainingMovement) {
    for (int i = snakeBodySize; i > 0; i--) {
      snakeBody[i].pos = Vector2Add(
          snakeBody[i].pos, Vector2Scale(snakeBody[i].dir, remainingMovement));
      snakeBody[i].dir = snakeBody[i - 1].dir;
    }
    snakeBody[0].pos = Vector2Add(
        snakeBody[0].pos, Vector2Scale(snakeBody[0].dir, remainingMovement));
    snakeBody[0].dir = snakeHead->dir;
    snakeHead->pos = Vector2Add(
        snakeHead->pos, Vector2Scale(snakeHead->dir, remainingMovement));
    snakeHead->dir = snakeHead->nextDir;
    snakeHead->moveProgress = 0;
  } else {
    for (int i = snakeBodySize; i >= 0; i--) {
      snakeBody[i].pos = Vector2Add(snakeBody[i].pos,
                                    Vector2Scale(snakeBody[i].dir, posChange));
    }
    snakeHead->pos =
        Vector2Add(snakeHead->pos, Vector2Scale(snakeHead->dir, posChange));
    snakeHead->moveProgress += posChange;
  }
}

void NewSnakePart(SnakePart *snakeBody, int *snakeBodySize, float dt) {
  Vector2 pos = Vector2Subtract(
      snakeBody[*snakeBodySize - 1].pos,
      Vector2Scale(snakeBody[*snakeBodySize - 1].dir, dt * SNAKE_SPEED));
  pos.x = round(pos.x);
  pos.y = round(pos.y);
  snakeBody[*snakeBodySize] = (SnakePart){pos, {0, 0}};
  (*snakeBodySize)++;
}

const Vector2 size = {PIECE_SIZE, PIECE_SIZE};

void DrawHead(SnakeHead snakeHead) {
  DrawCircleV(Vector2AddValue(snakeHead.pos, PIECE_SIZE / 2.), PIECE_SIZE / 2.,
              DARKGREEN);
  // Render rect to connect circle
  Vector2 rectOffset = (Vector2){0, 0};
  Vector2 rectSize = (Vector2){PIECE_SIZE / 2., PIECE_SIZE};
  if (snakeHead.dir.x == 1) {
  } else if (snakeHead.dir.x == -1) {
    rectOffset.x = PIECE_SIZE / 2.;
  } else if (snakeHead.dir.y == 1) {
    rectSize = (Vector2){PIECE_SIZE, PIECE_SIZE / 2.};
  } else if (snakeHead.dir.y == -1) {
    rectSize = (Vector2){PIECE_SIZE, PIECE_SIZE / 2.};
    rectOffset.y = PIECE_SIZE / 2.;
  }

  DrawRectangleV(Vector2Add(snakeHead.pos, rectOffset), rectSize, DARKGREEN);

  // Draw eyes
  Vector2 relativeEyePosL = {PIECE_SIZE - 25, 20};
  Vector2 relativeEyePosR = {PIECE_SIZE - 25, PIECE_SIZE - 20};
  if (snakeHead.dir.x == 1) {
  } else if (snakeHead.dir.x == -1) {
    relativeEyePosL.x = 25;
    relativeEyePosL.y = PIECE_SIZE - 20;
    relativeEyePosR.x = 25;
    relativeEyePosR.y = 20;
  } else if (snakeHead.dir.y == 1) {
    relativeEyePosL = (Vector2){PIECE_SIZE - 20, PIECE_SIZE - 25};
    relativeEyePosR = (Vector2){20, PIECE_SIZE - 25};
  } else if (snakeHead.dir.y == -1) {
    relativeEyePosL = (Vector2){20, 25};
    relativeEyePosR = (Vector2){PIECE_SIZE - 20, 25};
  }
  DrawCircleV(Vector2Add(snakeHead.pos, relativeEyePosL), EYE_SIZE, GRAY);
  DrawCircleV(Vector2Add(snakeHead.pos, relativeEyePosR), EYE_SIZE, GRAY);

  // Draw tongue
  Vector2 relativeTonguePos;
  Vector2 tongueSize = {TONGUE_SIZE_X, TONGUE_SIZE_Y};
  if (snakeHead.dir.x == 1) {
    relativeTonguePos =
        (Vector2){PIECE_SIZE - 15, PIECE_SIZE / 2. - TONGUE_SIZE_Y / 2.};
  } else if (snakeHead.dir.x == -1) {
    relativeTonguePos =
        (Vector2){15 - TONGUE_SIZE_X, PIECE_SIZE / 2. - TONGUE_SIZE_Y / 2.};
  } else if (snakeHead.dir.y == 1) {
    tongueSize = (Vector2){TONGUE_SIZE_Y, TONGUE_SIZE_X};
    relativeTonguePos =
        (Vector2){PIECE_SIZE / 2. - TONGUE_SIZE_Y / 2., PIECE_SIZE - 15};
  } else if (snakeHead.dir.y == -1) {
    tongueSize = (Vector2){TONGUE_SIZE_Y, TONGUE_SIZE_X};
    relativeTonguePos =
        (Vector2){PIECE_SIZE / 2. - TONGUE_SIZE_Y / 2., 15 - TONGUE_SIZE_X};
  }
  DrawRectangleV(Vector2Add(snakeHead.pos, relativeTonguePos), tongueSize, RED);
}

void DrawTail(SnakePart tail) {
  DrawCircleV(Vector2AddValue(tail.pos, PIECE_SIZE / 2.), PIECE_SIZE / 2.,
              DARKGREEN);

  // Render rect to connect circle
  Vector2 rectOffset = (Vector2){0, 0};
  Vector2 rectSize = (Vector2){30, 60};
  if (tail.dir.x == 1) {
    rectOffset.x = PIECE_SIZE / 2.;
  } else if (tail.dir.x == -1) {
  } else if (tail.dir.y == 1) {
    rectSize = (Vector2){60, 30};
    rectOffset.y = PIECE_SIZE / 2.;
  } else if (tail.dir.y == -1) {
    rectSize = (Vector2){60, 30};
  }
  DrawRectangleV(Vector2Add(tail.pos, rectOffset), rectSize, DARKGREEN);
}

void DrawApple(Vector2 pos) {
  DrawCircleV(Vector2AddValue(pos, PIECE_SIZE / 2.), PIECE_SIZE / 2., RED);
  DrawRectangleV(
      Vector2Add(pos, (Vector2){PIECE_SIZE / 2. - APPLE_ROD_SIZE_X / 2., 10}),
      (Vector2){APPLE_ROD_SIZE_X, APPLE_ROD_SIZE_Y}, DARKBROWN);
}

void Draw(SnakeHead snakeHead, SnakePart *snakeBody, int snakeBodySize,
          int score, Vector2 applePos) {
  BeginDrawing();
  ClearBackground(BLACK);

  DrawHead(snakeHead);
  for (int i = 0; i < snakeBodySize - 1; i++) {
    DrawRectangleV(snakeBody[i].pos, size, DARKGREEN);
  }
  DrawTail(snakeBody[snakeBodySize - 1]);
  DrawApple(applePos);
  DrawScore(score);

  EndDrawing();
}

int main(void) {
  InitWindow(GAME_SIZE * PIECE_SIZE, GAME_SIZE * PIECE_SIZE, "Cnake Game");
  SetTargetFPS(TARGET_FPS);

  SnakeHead snakeHead = {{120, 60}, {1, 0}, 0, {1, 0}};
  SnakePart snakeBody[SNAKE_SIZE_LIMIT] = {{{60, 60}, {1, 0}}};
  int snakeBodySize = 1;

  Vector2 applePos;
  UpdateApplePosition(&applePos, snakeHead, snakeBody, snakeBodySize);

  int score = 0;
  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    HandleKeypress(dt, &snakeHead);

    HandleMove(&snakeHead, snakeBody, snakeBodySize, dt);

    if (CheckCollision(snakeHead.pos, applePos)) {
      score++;
      NewSnakePart(snakeBody, &snakeBodySize, dt);
      UpdateApplePosition(&applePos, snakeHead, snakeBody, snakeBodySize);
    }

    Draw(snakeHead, snakeBody, snakeBodySize, score, applePos);
  }

  CloseWindow();

  return 0;
}
