#include "renderer-arrays.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>

#include "constants.h"

typedef struct {
  float data[8];

} m256;

inline m256 mm256_add_ps(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_sub_ps(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_mul_ps(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_set1_ps(float a) __attribute__((always_inline));

inline m256 mm256_set_ps(float e7, float e6, float e5, float e4, float e3,
                         float e2, float e1, float e0)
    __attribute__((always_inline));

inline m256 mm256_div_ps(m256 a, m256 b) __attribute__((always_inline));

void render_mandelbrot_arrays(SDL_Surface* surface, Transform tr) {
  m256 sc_fac    = mm256_set1_ps(tr.scale_factor);
  m256 p0_re_inc = mm256_set_ps(7, 6, 5, 4, 3, 2, 1, 0);
  m256 const_2   = mm256_set1_ps(2.f);

  for (float p_y = 0; p_y < kWindowHeight; p_y++) {
    for (float p_x = 0; p_x < kWindowWidth; p_x += 8) {
      m256 p0_re = mm256_set1_ps(p_x - kXOffset + tr.origin_x);
      m256 p0_im = mm256_set1_ps(kYOffset - p_y + tr.origin_y);

      p0_re = mm256_add_ps(p0_re, p0_re_inc);
      p0_re = mm256_div_ps(p0_re, sc_fac);
      p0_im = mm256_div_ps(p0_im, sc_fac);

      m256 p_re = p0_re;
      m256 p_im = p0_im;

      Uint8 p_iter[8] = {};
      char  p_mask    = ~0;

      for (int iter = 0; iter < kMandelMaxIter; ++iter) {
        m256 p_re_sq = mm256_mul_ps(p_re, p_re);
        m256 p_im_sq = mm256_mul_ps(p_im, p_im);

        p_im = mm256_mul_ps(p_re, p_im);
        p_im = mm256_mul_ps(const_2, p_im);
        p_im = mm256_add_ps(p_im, p0_im);

        p_re = mm256_sub_ps(p_re_sq, p_im_sq);
        p_re = mm256_add_ps(p_re, p0_re);

        m256 p_r2 = mm256_add_ps(p_re_sq, p_im_sq);

        for (int i = 0; i < 8; ++i) {
          if (p_r2.data[i] > kMandelMaxMagnitude) p_mask &= ~(1 << i);
        }

        if (!p_mask) break;

        for (int i = 0; i < 8; ++i) {
          if (p_mask & (1 << i)) p_iter[i]++;
        }
      }

      for (int i = 0; i < 8; ++i)
        SDL_WriteSurfacePixel(surface, p_x+i, p_y, p_iter[i], p_iter[i],
                              p_iter[i], SDL_ALPHA_OPAQUE);
    }
  }
}

m256 mm256_add_ps(m256 a, m256 b) {
  m256 res = {.data = 0};

  for (int i = 0; i < 8; ++i) res.data[i] = a.data[i] + b.data[i];

  return res;
}

m256 mm256_sub_ps(m256 a, m256 b) {
  m256 res = {.data = 0};

  for (int i = 0; i < 8; ++i) res.data[i] = a.data[i] - b.data[i];

  return res;
}

m256 mm256_mul_ps(m256 a, m256 b) {
  m256 res = {.data = 0};

  for (int i = 0; i < 8; ++i) res.data[i] = a.data[i] * b.data[i];

  return res;
}

m256 mm256_set1_ps(float a) {
  m256 res = {.data = 0};

  for (int i = 0; i < 8; ++i) res.data[i] = a;

  return res;
}

m256 mm256_set_ps(float e7, float e6, float e5, float e4, float e3, float e2,
                  float e1, float e0) {
  m256 res = {.data = {e0, e1, e2, e3, e4, e5, e6, e7}};
  return res;
}

m256 mm256_div_ps(m256 a, m256 b) {
  m256 res = {.data = 0};

  for (int i = 0; i < 8; ++i) res.data[i] = a.data[i] / b.data[i];

  return res;
}
