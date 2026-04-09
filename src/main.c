#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_surface.h>

#include "renderer-naive.h"
#include "renderer-arrays.h"
#include "renderer-intrinsic.h"
#include "constants.h"

typedef struct AppDataS {
  SDL_Window*   win;
  SDL_Renderer* rndr;
  float         scale_factor;
  SDL_Texture*  tex;

} AppDataT;

int main(int argc, char* args[]) {
  AppDataT app_data = {NULL, NULL, kDefaultScaleFactor};

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed initializing SDL: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer(kAppName, kWindowWidth, kWindowHeight,
                                   SDL_WINDOW_MAXIMIZED, &app_data.win,
                                   &app_data.rndr)) {
    SDL_Log("Failed creating window/renderer: %s", SDL_GetError());
    return EXIT_FAILURE;
  }
  SDL_SetRenderLogicalPresentation(app_data.rndr, kWindowWidth, kWindowHeight,
                                   SDL_LOGICAL_PRESENTATION_DISABLED);

  app_data.tex = SDL_CreateTexture(app_data.rndr, SDL_PIXELFORMAT_ARGB8888, 
                                   SDL_TEXTUREACCESS_STREAMING, kWindowWidth, 
                                   kWindowHeight);

  if(!app_data.tex) {
    SDL_Log("Failed creating texture: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

#if   defined VERSION_NAIVE
    SDL_Log("Running naive version");
#elif defined VERSION_ARRAYS
    SDL_Log("Running array version");
#else
    SDL_Log("Running intrinsic version");
#endif

  SDL_Surface* surface = NULL;
  SDL_Event    event;
  bool         quit = false;

  SDL_SetRenderDrawColor(app_data.rndr, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(app_data.rndr);

  while (!quit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        quit = true;
        break;
      }

      else if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.scancode) {
          case SDL_SCANCODE_EQUALS:
            app_data.scale_factor += kScaleFactorDelta;
            break;
          case SDL_SCANCODE_MINUS:
            app_data.scale_factor -= kScaleFactorDelta;
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

    if(SDL_LockTextureToSurface(app_data.tex, NULL, &surface)) {

#if   defined VERSION_NAIVE
      render_mandelbrot_naive(surface, app_data.scale_factor);
#elif defined VERSION_ARRAYS
      render_mandelbrot_arrays(surface, app_data.scale_factor);
#else
      render_mandelbrot_intrinsic(surface, app_data.scale_factor);
#endif

      SDL_UnlockTexture(app_data.tex);
    }
    else {
      SDL_Log("Failed to lock texture: %s", SDL_GetError());
      return EXIT_FAILURE;
    }

    SDL_RenderTexture(app_data.rndr, app_data.tex, NULL, NULL);

    const float fps = 1000 / ((float)(SDL_GetTicks() - last_ticks));
    SDL_Log("%f", fps);
    SDL_SetRenderDrawColor(app_data.rndr, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugTextFormat(app_data.rndr, 10, 10, "FPS: %.1f", fps);

    SDL_RenderPresent(app_data.rndr);
  }

  SDL_DestroyWindow(app_data.win);
  SDL_Quit();

  return EXIT_SUCCESS;
}
