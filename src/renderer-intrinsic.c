#include "renderer-intrinsic.h"
#include <stdio.h>

#include "constants.h"
#include "immintrin.h"

void render_mandelbrot_intrinsic(SDL_Renderer* rndr, float scale_factor) {

  __m256 sc_fac    = _mm256_set1_ps(scale_factor);
  __m256 p0_re_inc = _mm256_set_ps(7, 6, 5, 4, 3, 2, 1, 0);
  __m256 const_2   = _mm256_set1_ps(2.f);

  for (float p_y = 0; p_y < kWindowHeight; p_y++) {
    for (float p_x = 0; p_x < kWindowWidth; p_x += 8) {
      __m256 p0_re = _mm256_set1_ps(p_x - kXOffset);
      __m256 p0_im = _mm256_set1_ps(kYOffset - p_y);

      p0_re = _mm256_add_ps(p0_re, p0_re_inc);
      p0_re = _mm256_div_ps(p0_re, sc_fac);
      p0_im = _mm256_div_ps(p0_im, sc_fac);

      __m256 p_re = p0_re;
      __m256 p_im = p0_im;

      Uint8 p_iter[8] = {};
      char  p_mask    = ~0;

      for (int iter = 0; iter < kMandelMaxIter; ++iter) {
        __m256 p_re_sq = _mm256_mul_ps(p_re, p_re);
        __m256 p_im_sq = _mm256_mul_ps(p_im, p_im);

        p_im = _mm256_mul_ps(p_re, p_im);
        p_im = _mm256_mul_ps(const_2, p_im);
        p_im = _mm256_add_ps(p_im, p0_im);

        p_re = _mm256_sub_ps(p_re_sq, p_im_sq);
        p_re = _mm256_add_ps(p_re, p0_re);

        __m256 p_r2 = _mm256_add_ps(p_re_sq, p_im_sq);

        for (int i = 0; i < 8; ++i) {
          if (p_r2[i] > kMandelMaxMagnitude) p_mask &= ~(1 << i);
        }

        if (!p_mask) break;

        for (int i = 0; i < 8; ++i) {
          if (p_mask & (1 << i)) p_iter[i]++;
        }
      }

      for(int i = 0; i < 8; ++i) {
        // SDL_SetRenderDrawColor(rndr, p_iter[i], p_iter[i], p_iter[i], SDL_ALPHA_OPAQUE);
        // SDL_RenderPoint(rndr, p_x+i, p_y);
        printf(NULL, "%d", p_iter[i]);
      }
    }
  }
}
