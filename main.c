#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

// Actual game render resolution
const int RENDER_WIDTH  = 320;
const int RENDER_HEIGHT = 180;

// Window resolution, this value will be updated later with the monitor
// resolution. The rendered image will be upscaled to fit this size
int SCREEN_WIDTH  = 320;
int SCREEN_HEIGHT = 180;

bool FULLSCREEN = true;
bool USE_SHADER = true;
bool SHOW_FPS = true;

int main(){
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Template");

  int target_fps = GetMonitorRefreshRate(GetCurrentMonitor());
  SetTargetFPS(target_fps);
  
  SetExitKey(KEY_NULL);

  // Set the actual window resolution, and set fullscreen
  SCREEN_WIDTH = GetMonitorWidth(GetCurrentMonitor());
  SCREEN_HEIGHT= GetMonitorHeight(GetCurrentMonitor());
  SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  if (FULLSCREEN) ToggleFullscreen();

  RenderTexture2D render_target = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);
  Shader shader = LoadShader(0, "shader.frag");
  SetShaderValue(
    shader,
    GetShaderLocation(shader, "resolution"),
    (float[2]){(float)render_target.texture.width, (float)render_target.texture.height},
    SHADER_UNIFORM_VEC2
  );

  // ---------- Initialization ----------
  Rectangle rect = {0, 20, 10, 10};

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    SCREEN_WIDTH = GetScreenWidth();
    SCREEN_HEIGHT= GetScreenHeight();

    if (IsKeyPressed(KEY_F11) || ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER))) {
      ToggleFullscreen();
    }

    // ---------- Game Logic ----------
    rect.x += 50 * dt;
    if (rect.x >= RENDER_WIDTH) rect.x = -10;

    // Drawing Logic
    // Actual game drawing (to the 320x180 render)
    char fps_text[10];
    snprintf(fps_text, sizeof(fps_text), "FPS: %d", GetFPS());
    BeginTextureMode(render_target);

      ClearBackground(WHITE);
      if (SHOW_FPS) DrawText(fps_text, 2, 2, 8, BLACK);
      
      // ---------- Start Drawing Here ----------
      DrawRectangleRec(rect, BLACK);

    EndTextureMode();

    // Upscale and draw the render texture to the screen
    float scale = fminf(
      (float)SCREEN_WIDTH / render_target.texture.width, 
      (float)SCREEN_HEIGHT / render_target.texture.height
    );

    // Where to draw in the actual window
    Rectangle dest_rect = {
      (SCREEN_WIDTH - render_target.texture.width * scale) * 0.5f,
      (SCREEN_HEIGHT- render_target.texture.height * scale) * 0.5f,
      render_target.texture.width * scale,
      render_target.texture.height * scale,
    };

    // What area to upscale into dest_rect
    Rectangle source_rect = {
      0.0f,
      0.0f,
      (float)render_target.texture.width,
      (float)-render_target.texture.height,
    }; //    ^ this is inverted because render textures have an inverted y-axis compared to just drawing in raylib

    BeginDrawing();
    if (USE_SHADER) BeginShaderMode(shader);

      ClearBackground(BLACK);
      DrawTexturePro(
        render_target.texture,
        source_rect,
        dest_rect,
        (Vector2){0, 0},
        0.0f,
        WHITE
      );

    if (USE_SHADER) EndShaderMode();
    EndDrawing();
  }

  UnloadRenderTexture(render_target);
  UnloadShader(shader);
  CloseWindow();
  return 0;
}
