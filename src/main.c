#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <x86intrin.h>

#include "constants.h"
#include "renderer-arrays.h"
#include "renderer-intrinsic.h"
#include "renderer-naive.h"
#include "transform.h"

#ifdef MODE_BENCHMARK
#include <stdio.h>
#define IF_MODE_BENCHMARK(st) st
#else
#define IF_MODE_BENCHMARK(st)
#endif

#if defined VERSION_NAIVE
static const char* version_info = "Running naive version";
#define FUNC_RENDERER render_mandelbrot_naive
#elif defined VERSION_ARRAYS
static const char* version_info = "Running array version";
#define FUNC_RENDERER render_mandelbrot_arrays
#else
static const char* version_info = "Running intrinsic version";
#define FUNC_RENDERER render_mandelbrot_intrinsic
#endif

typedef struct AppDataS {
  SDL_Window*   win;
  SDL_Renderer* rndr;
  Transform     tr;
  SDL_Texture*  tex;

} AppDataT;

int main(int argc, char* argv[]) {
  IF_MODE_BENCHMARK(FILE* file_bench_out          = NULL;
                    if (argc == 2) file_bench_out = fopen(argv[1], "w");
                    else file_bench_out           = fopen("bench_out.txt", "w");

                    if (!file_bench_out) {
                      SDL_Log("Failed to open benchmark output file");
                      return EXIT_FAILURE;
                    })

  AppDataT app_data = {NULL, NULL, {0, 0, kDefaultScaleFactor}, NULL};

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

  if (!app_data.tex) {
    SDL_Log("Failed creating texture: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_Log("%s", version_info);

  SDL_Surface* surface = NULL;
  SDL_Event    event;
  bool         quit = false;

  IF_MODE_BENCHMARK(int iter_cnt = 0;)

  SDL_SetRenderDrawColor(app_data.rndr, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(app_data.rndr);

  while (!quit) {
    IF_MODE_BENCHMARK(iter_cnt++; if (iter_cnt >= kBenchmarkIterCnt) break;)

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        quit = true;
        break;
      }

      else if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.scancode) {
          case SDL_SCANCODE_EQUALS:
            app_data.tr.scale_factor += kScaleFactorDelta;
            break;
          case SDL_SCANCODE_MINUS:
            app_data.tr.scale_factor -= kScaleFactorDelta;
            break;
          case SDL_SCANCODE_W:
            app_data.tr.origin_y += kOriginDelta;
            break;
          case SDL_SCANCODE_S:
            app_data.tr.origin_y -= kOriginDelta;
            break;
          case SDL_SCANCODE_D:
            app_data.tr.origin_x += kOriginDelta;
            break;
          case SDL_SCANCODE_A:
            app_data.tr.origin_x -= kOriginDelta;
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

    if (SDL_LockTextureToSurface(app_data.tex, NULL, &surface)) {
      IF_MODE_BENCHMARK(_mm_lfence(); uint64_t timestamp_cnt_begin = __rdtsc();)

      FUNC_RENDERER(surface, app_data.tr);

      IF_MODE_BENCHMARK(_mm_lfence(); uint64_t timestamp_cnt_end = __rdtsc();
                        fprintf(file_bench_out, "%ld\n",
                                timestamp_cnt_end - timestamp_cnt_begin);)

      SDL_UnlockTexture(app_data.tex);
    } else {
      SDL_Log("Failed to lock texture: %s", SDL_GetError());
      return EXIT_FAILURE;
    }

    SDL_RenderTexture(app_data.rndr, app_data.tex, NULL, NULL);

    const float fps = 1000 / ((float)(SDL_GetTicks() - last_ticks));
    SDL_SetRenderDrawColor(app_data.rndr, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugTextFormat(app_data.rndr, 10, 10, "FPS: %.1f", fps);

    SDL_RenderPresent(app_data.rndr);
  }

  IF_MODE_BENCHMARK(fclose(file_bench_out);)
  SDL_DestroyWindow(app_data.win);
  SDL_Quit();

  return EXIT_SUCCESS;
}
