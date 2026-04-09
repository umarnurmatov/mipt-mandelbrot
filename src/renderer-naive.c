#include "renderer-naive.h"

#include <SDL3/SDL_pixels.h>
#include <stdio.h>

#include <SDL3/SDL_surface.h>

#include "constants.h"

void render_mandelbrot_naive(SDL_Surface* surface, float scale_factor) {
  for (float p_y = 0; p_y < kWindowHeight; p_y++) {
    for (float p_x = 0; p_x < kWindowWidth; p_x++) {
      float p0_re = (p_x - kXOffset) / scale_factor,
            p0_im = (kYOffset - p_y) / scale_factor;

      float p_re = 0, p_im = 0;

      Uint8 iter = 0;
      // (a + i*b)*(a + i*b) = a^2 - b^2 + i*(2ab)
      for (; iter < kMandelMaxIter; ++iter) {
        float p_re_sq = p_re * p_re;
        float p_im_sq = p_im * p_im;

        p_im = 2 * p_im * p_re + p0_im;
        p_re = p_re_sq - p_im_sq + p0_re;

        if (p_re * p_re + p_im * p_im > 4) break;
      }

      SDL_WriteSurfacePixel(surface, p_x, p_y, iter, iter, iter, SDL_ALPHA_OPAQUE);
    }
  }
}
