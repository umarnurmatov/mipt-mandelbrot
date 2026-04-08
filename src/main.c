#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

#include "renderer-naive.h"
#include "renderer-arrays.h"
#include "renderer-intrinsic.h"
#include "constants.h"

typedef struct AppDataS {
  SDL_Window*   win;
  SDL_Renderer* rndr;
  float         scale_factor;

} AppDataT;

int main(int argc, char* args[]) {
  AppDataT gAppData = {NULL, NULL, kDefaultScaleFactor};

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer(kAppName, kWindowWidth, kWindowHeight,
                                   SDL_WINDOW_MAXIMIZED, &gAppData.win,
                                   &gAppData.rndr)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return EXIT_FAILURE;
  }
  SDL_SetRenderLogicalPresentation(gAppData.rndr, kWindowWidth, kWindowHeight,
                                   SDL_LOGICAL_PRESENTATION_DISABLED);

  SDL_Event event;
  bool      quit = false;

  SDL_SetRenderDrawColor(gAppData.rndr, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(gAppData.rndr);

  while (!quit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        quit = true;
        break;
      }

      else if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.scancode) {
          case SDL_SCANCODE_EQUALS:
            gAppData.scale_factor += kScaleFactorDelta;
            break;
          case SDL_SCANCODE_MINUS:
            gAppData.scale_factor -= kScaleFactorDelta;
            break;
          case SDL_SCANCODE_Q:
            quit = true;
            break;
          default:
            break;
        }
      }
    }

    uint64_t last_ticks = SDL_GetTicks();

    render_mandelbrot_naive(gAppData.rndr, gAppData.scale_factor);

    const float fps = 1000 / ((float)(SDL_GetTicks() - last_ticks));
    SDL_Log("%f", fps);
    SDL_SetRenderDrawColor(gAppData.rndr, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugTextFormat(gAppData.rndr, 10, 10, "FPS: %.1f", fps);

    SDL_RenderPresent(gAppData.rndr);
  }

  SDL_DestroyWindow(gAppData.win);
  SDL_Quit();

  return EXIT_SUCCESS;
}
