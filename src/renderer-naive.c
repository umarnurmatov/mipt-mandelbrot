#include "renderer-naive.h"

#include "constants.h"

void render_mandelbrot_naive(SDL_Renderer* rndr, float scale_factor) {
  for (float p_y = 0; p_y < kWindowHeight; p_y++) {
    for (float p_x = 0; p_x < kWindowWidth; p_x++) {
      float p0_re = (p_x - kXOffset) / scale_factor,
            p0_im = (kYOffset - p_y) / scale_factor;

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

      SDL_SetRenderDrawColor(rndr, iter, iter, iter, SDL_ALPHA_OPAQUE);
      SDL_RenderPoint(rndr, p_x, p_y);
    }
  }
}
