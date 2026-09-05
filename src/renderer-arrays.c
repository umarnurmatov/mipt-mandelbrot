#include "renderer-arrays.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>

#include "constants.h"

typedef union {
  float   fpnt[8];
  int32_t intgr[8];

} m256;

inline m256 mm256_add_ps(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_add_epi32(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_sub_ps(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_mul_ps(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_set1_ps(float a) __attribute__((always_inline));

inline m256 mm256_set1_epi32(int32_t a) __attribute__((always_inline));

inline m256 mm256_set_ps(float e7, float e6, float e5, float e4, float e3,
                         float e2, float e1, float e0)
    __attribute__((always_inline));

inline m256 mm256_div_ps(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_cmple_ps(m256 a, m256 b) __attribute__((always_inline));

inline int32_t mm256_movemask_ps(m256 a) __attribute__((always_inline));

inline m256 mm256_blendv_ps(m256 a, m256 b, m256 mask)
    __attribute__((always_inline));

inline m256 mm256_blendv_epi32(m256 a, m256 b, m256 mask)
    __attribute__((always_inline));

void render_mandelbrot_arrays(SDL_Surface* surface, Transform tr) {
  const m256 sc_fac    = mm256_set1_ps(tr.scale_factor);
  const m256 p0_re_inc = mm256_set_ps(7, 6, 5, 4, 3, 2, 1, 0);
  const m256 const_2   = mm256_set1_ps(2.f);
  const m256 p_r2_max  = mm256_set1_ps(kMandelMaxMagnitude);

  for (float p_y = 0; p_y < kWindowHeight; p_y++) {
    for (float p_x = 0; p_x < kWindowWidth; p_x += 8) {

      m256 p0_re = mm256_set1_ps(p_x - kXOffset + tr.origin_x);
      m256 p0_im = mm256_set1_ps(kYOffset - p_y - tr.origin_y);

      p0_re = mm256_add_ps(p0_re, p0_re_inc);
      p0_re = mm256_div_ps(p0_re, sc_fac);
      p0_im = mm256_div_ps(p0_im, sc_fac);

      m256 p_re = p0_re;
      m256 p_im = p0_im;

      m256 p_iter = {.intgr = 0};
      m256 p_mask = {.intgr = 0};

      for (int iter = 0; iter < kMandelMaxIter; ++iter) {
        m256 p_re_sq = mm256_mul_ps(p_re, p_re);
        m256 p_im_sq = mm256_mul_ps(p_im, p_im);

        p_im = mm256_mul_ps(p_re, p_im);
        p_im = mm256_mul_ps(const_2, p_im);
        p_im = mm256_add_ps(p_im, p0_im);

        p_re = mm256_sub_ps(p_re_sq, p_im_sq);
        p_re = mm256_add_ps(p_re, p0_re);

        m256 p_r2 = mm256_add_ps(p_re_sq, p_im_sq);

        p_mask       = mm256_cmple_ps(p_r2, p_r2_max);
        int32_t mask = mm256_movemask_ps(p_mask);

        if (!mask) break;

        m256 p_iter_inc = mm256_add_epi32(p_iter, mm256_set1_epi32(1));
        p_iter          = mm256_blendv_epi32(p_iter_inc, p_iter, p_mask);
      }

      for (int i = 0; i < 8; ++i)
        SDL_WriteSurfacePixel(surface, (int)p_x + i, (int)p_y,
                              (Uint8)p_iter.intgr[i], (Uint8)p_iter.intgr[i],
                              (Uint8)p_iter.intgr[i], SDL_ALPHA_OPAQUE);
    }
  }
}

m256 mm256_add_ps(m256 a, m256 b) {
  m256 res = {.fpnt = 0};

  for (int i = 0; i < 8; ++i) res.fpnt[i] = a.fpnt[i] + b.fpnt[i];

  return res;
}

inline m256 mm256_add_epi32(m256 a, m256 b) {
  m256 res = {.intgr = 0};

  for (int i = 0; i < 8; ++i) res.intgr[i] = a.intgr[i] + b.intgr[i];

  return res;
}

m256 mm256_sub_ps(m256 a, m256 b) {
  m256 res = {.fpnt = 0};

  for (int i = 0; i < 8; ++i) res.fpnt[i] = a.fpnt[i] - b.fpnt[i];

  return res;
}

m256 mm256_mul_ps(m256 a, m256 b) {
  m256 res = {.fpnt = 0};

  for (int i = 0; i < 8; ++i) res.fpnt[i] = a.fpnt[i] * b.fpnt[i];

  return res;
}

m256 mm256_set1_ps(float a) {
  m256 res = {.fpnt = 0};

  for (int i = 0; i < 8; ++i) res.fpnt[i] = a;

  return res;
}

m256 mm256_set1_epi32(int32_t a) {
  m256 res = {.intgr = 0};

  for (int i = 0; i < 8; ++i) res.intgr[i] = a;

  return res;
}

m256 mm256_set_ps(float e7, float e6, float e5, float e4, float e3, float e2,
                  float e1, float e0) {
  m256 res = {.fpnt = {e0, e1, e2, e3, e4, e5, e6, e7}};
  return res;
}

m256 mm256_div_ps(m256 a, m256 b) {
  m256 res = {.fpnt = 0};

  for (int i = 0; i < 8; ++i) res.fpnt[i] = a.fpnt[i] / b.fpnt[i];

  return res;
}

inline m256 mm256_cmple_ps(m256 a, m256 b) {
  m256 res = {.intgr = 0};

  for (int i = 0; i < 8; ++i) res.intgr[i] = a.fpnt[i] <= b.fpnt[i] ? ~0u : 0u;

  return res;
}

inline int32_t mm256_movemask_ps(m256 a) {
  int32_t res = 0;

  for (int i = 0; i < 8; ++i)
    if (a.intgr[i] >> 31) res |= 1 << i;

  return res;
}

inline m256 mm256_blendv_ps(m256 a, m256 b, m256 mask) {
  m256 res = {.fpnt = 0};

  for (int i = 0; i < 8; ++i)
    res.fpnt[i] = mask.intgr[i] ? a.fpnt[i] : b.fpnt[i];

  return res;
}

inline m256 mm256_blendv_epi32(m256 a, m256 b, m256 mask) {
  m256 res = {.intgr = 0};

  for (int i = 0; i < 8; ++i)
    res.intgr[i] = mask.intgr[i] ? a.intgr[i] : b.intgr[i];

  return res;
}
