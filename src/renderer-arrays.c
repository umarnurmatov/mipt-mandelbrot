#include "renderer-arrays.h"

#include "SDL3/SDL.h"
#include "constants.h"

typedef struct {
  float data[8];

} m256;

inline m256 mm256_add_ps(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_sub_ps(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_mul_ps(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_set1_ps(float a) __attribute__((always_inline));

inline m256 mm256_set_ps(float e7, float e6, float e5, float e4, float e3, float e2,
                  float e1, float e0) __attribute__((always_inline));

inline m256 mm256_div_ps(m256 a, m256 b) __attribute__((always_inline));

inline m256 mm256_blend_ps(m256 a, m256 b, const char imm8) __attribute__((always_inline));

void render_mandelbrot_arrays(SDL_Renderer* rndr, float scale_factor) {

  m256 sc_fac = mm256_set1_ps(scale_factor);
  m256 p0_re_inc = mm256_set_ps(7, 6, 5, 4, 3, 2, 1, 0);
  for (float p_y = 0; p_y < kWindowHeight; p_y++) {
    for (float p_x = 0; p_x < kWindowWidth; p_x += 8) {

      m256 p0_re = mm256_set1_ps(p_x - kXOffset);
      m256 p0_im = mm256_set1_ps(kYOffset - p_y); 

      p0_re = mm256_add_ps(p0_re, p0_re_inc);
      p0_re = mm256_div_ps(p0_re, sc_fac);
      p0_im = mm256_div_ps(p0_im, sc_fac);

      m256 p_re = mm256_set1_ps(0.f);
      m256 p_im = mm256_set1_ps(0.f);

      m256 p_re_prev = p0_re;
      m256 p_im_prev = p0_im;

      m256 const_2 = mm256_set1_ps(2.f);
      
      Uint8 p_iter[8] = {};
      char blend_mask = ~0;

      m256 p_im_sq = {.data=0};
      m256 p_r2 = {.data=0};

      for(int iter = 0; iter < kMandelMaxIter; ++iter) {

        p_re = mm256_mul_ps(p_re_prev, p_re_prev);
        p_im_sq = mm256_mul_ps(p_im_prev, p_im_prev);

        p_re = mm256_sub_ps(p_re, p_im_sq);
        p_re = mm256_add_ps(p_re, p0_re);

        p_im = mm256_mul_ps(p_re_prev, p_im_prev);
        p_im = mm256_mul_ps(const_2, p_im);
        p_im = mm256_add_ps(p_im, p0_im);

        p_r2 = p_re;
        p_r2 = mm256_mul_ps(p_r2, p_r2);
        p_im_sq = mm256_mul_ps(p_im, p_im);
        p_r2 = mm256_add_ps(p_r2, p_im_sq);

        for(int i = 0; i < 8; ++i) {
          if(p_r2.data[i] > 5) blend_mask &= ~(1 << i);
        }

        if(!blend_mask) break;

        for(int i = 0; i < 8; ++i) {
          if(blend_mask & (1 << i)) p_iter[i]++;
        }

        p_re_prev = mm256_blend_ps(p_re, p_re_prev, blend_mask);
        p_im_prev = mm256_blend_ps(p_im, p_im_prev, blend_mask);
      }

      for(int i = 0; i < 8; ++i) {
        SDL_SetRenderDrawColor(rndr, p_iter[i], p_iter[i], p_iter[i], SDL_ALPHA_OPAQUE);
        SDL_RenderPoint(rndr, p_x+i, p_y);
      }

      // float p0_re = (p_x - kXOffset) / scale_factor,
      //       p0_im = (kYOffset - p_y) / scale_factor;
      //
      // float p_re = 0, p_im = 0;
      //
      // float p_re_prev = p0_re, p_im_prev = p0_im;
      //
      // int iter = 0;
      // // (a + i*b)*(a + i*b) = a^2 - b^2 + i*(2ab)
      // for (; iter < kMandelMaxIter; ++iter) {
      //   p_re = p_re_prev * p_re_prev - p_im_prev * p_im_prev + p0_re;
      //   p_im = 2 * p_re_prev * p_im_prev + p0_im;
      //
      //   if (p_re * p_re + p_im * p_im > 10) break;
      //
      //   p_re_prev = p_re;
      //   p_im_prev = p_im;
      // }
      //
      // SDL_SetRenderDrawColor(rndr, iter, iter, iter, SDL_ALPHA_OPAQUE);
      // SDL_RenderPoint(rndr, p_x, p_y);
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

m256 mm256_blend_ps(m256 a, m256 b, const char imm8) {
  char mask = 0x01;
  m256 res = {.data = 0};

  for(int i = 0; i < 8; ++i) {
      res.data[i] = imm8 & mask ? a.data[i] : b.data[i];
      mask <<= 1;
  }

  return res;
}
