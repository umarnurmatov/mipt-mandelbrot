#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

static const char* kAppName            = "mandelbrot";
static const int   kWindowWidth        = 1366;
static const int   kWindowHeight       = 768;
static const int   kMandelMaxIter      = 100;

static const float kXOffset            = 800;
static const float kYOffset            = kWindowHeight / 2;
static const float kDefaultScaleFactor = 350.f;
static const float kScaleFactorDelta   = 10.f;

typedef struct AppDataS {
  SDL_Window*   win;
  SDL_Renderer* rndr;
  float         scale_factor;

} AppDataT;

static AppDataT gAppData = {NULL, NULL, kDefaultScaleFactor};

void render_mandelbrot(AppDataT* app_data);

int main(int argc, char* args[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer(kAppName, kWindowWidth, kWindowHeight,
                                   SDL_WINDOW_FULLSCREEN, &gAppData.win,
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

    render_mandelbrot(&gAppData);

    const float fps = 1000 / ((float)(SDL_GetTicks() - last_ticks));
    SDL_SetRenderDrawColor(gAppData.rndr, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugTextFormat(gAppData.rndr, 10, 10, "FPS: %.1f", fps);

    SDL_RenderPresent(gAppData.rndr);
  }

  SDL_DestroyWindow(gAppData.win);
  SDL_Quit();

  return EXIT_SUCCESS;
}

void render_mandelbrot(AppDataT* app_data) {
  for (float p_y = 0; p_y < kWindowHeight; p_y++) {
    for (float p_x = 0; p_x < kWindowWidth; p_x++) {
      float p0_re = (p_x - kXOffset) / app_data->scale_factor,
            p0_im = (kYOffset - p_y) / app_data->scale_factor;

      float p_re = 0, p_im = 0;

      float p_re_prev = p0_re, p_im_prev = p0_im;

      int iter = 0;
      // (a + i*b)*(a + i*b) = a^2 - b^2 + i*(2ab)
      for (; iter < kMandelMaxIter; ++iter) {
        p_re = p_re_prev * p_re_prev - p_im_prev * p_im_prev + p0_re;
        p_im = 2 * p_re_prev * p_im_prev + p0_im;

        if (p_re * p_re + p_im * p_im > 10) break;

        p_re_prev = p_re;
        p_im_prev = p_im;
      }

      SDL_SetRenderDrawColor(gAppData.rndr, iter, iter, iter, SDL_ALPHA_OPAQUE);
      SDL_RenderPoint(gAppData.rndr, p_x, p_y);
    }
  }
}
