#include "renderer-intrinsic.h"

#include <stdio.h>

#include "constants.h"
#include "immintrin.h"

void render_mandelbrot_intrinsic(SDL_Surface* surface, Transform tr) {
  const __m256 sc_fac    = _mm256_set1_ps(tr.scale_factor);
  const __m256 p0_re_inc = _mm256_set_ps(7, 6, 5, 4, 3, 2, 1, 0);
  const __m256 const_2   = _mm256_set1_ps(2.f);
  const __m256 p_r2_max  = _mm256_set1_ps(kMandelMaxMagnitude);

  for (float p_y = 0; p_y < kWindowHeight; p_y++) {
    for (float p_x = 0; p_x < kWindowWidth; p_x += 8) {
      __m256 p0_re = _mm256_set1_ps(p_x - kXOffset + tr.origin_x);
      __m256 p0_im = _mm256_set1_ps(kYOffset - p_y + tr.origin_y);

      p0_re = _mm256_add_ps(p0_re, p0_re_inc);
      p0_re = _mm256_div_ps(p0_re, sc_fac);
      p0_im = _mm256_div_ps(p0_im, sc_fac);

      __m256 p_re = p0_re;
      __m256 p_im = p0_im;

      __m256 p_iter = _mm256_set1_ps(0);
      __m256 p_mask = _mm256_set1_ps(0);

      for (int iter = 0; iter < kMandelMaxIter; ++iter) {
        __m256 p_re_sq = _mm256_mul_ps(p_re, p_re);
        __m256 p_im_sq = _mm256_mul_ps(p_im, p_im);

        p_im = _mm256_mul_ps(p_re, p_im);
        p_im = _mm256_mul_ps(const_2, p_im);
        p_im = _mm256_add_ps(p_im, p0_im);

        p_re = _mm256_sub_ps(p_re_sq, p_im_sq);
        p_re = _mm256_add_ps(p_re, p0_re);

        __m256 p_r2 = _mm256_add_ps(p_re_sq, p_im_sq);

        p_mask       = _mm256_cmp_ps(p_r2, p_r2_max, _CMP_LE_OS);
        int32_t mask = _mm256_movemask_ps(p_mask);

        if (!mask) break;

        __m256 p_iter_inc = _mm256_add_ps(p_iter, _mm256_set1_ps(1));
        p_iter            = _mm256_blendv_ps(p_iter, p_iter_inc, p_mask);
      }

      for (int i = 0; i < 8; ++i) {
        SDL_WriteSurfacePixel(surface, p_x + i, p_y, p_iter[i],
                              p_iter[i], p_iter[i],
                              SDL_ALPHA_OPAQUE);
      }
    }
  }
}
