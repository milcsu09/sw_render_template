#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>


#define WW 800
#define WH 600

#define RW 800
#define RH 600


#define PI 3.1415927f
#define PI_2 1.5707963f
#define PI_4 0.7853982f
#define TAU 6.2831853f

#define DEG2RAD 0.0174533f
#define RAD2DEG 57.2957795f

#define RAD(d) ((d) * DEG2RAD)
#define DEG(d) ((d) * RAD2DEG)


typedef float f32;
typedef double f64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef size_t usize;
typedef ssize_t isize;


typedef struct
{
  f32 x, y;
} v2;


v2
v2_add (v2 a, v2 b)
{
  v2 v;

  v.x = a.x + b.x;
  v.y = a.y + b.y;

  return v;
}


v2
v2_sub (v2 a, v2 b)
{
  v2 v;

  v.x = a.x - b.x;
  v.y = a.y - b.y;

  return v;
}


v2
v2_mul (v2 a, f32 scalar)
{
  v2 v;

  v.x = a.x * scalar;
  v.y = a.y * scalar;

  return v;
}


v2
v2_div (v2 a, f32 scalar)
{
  v2 v;

  v.x = a.x / scalar;
  v.y = a.y / scalar;

  return v;
}


f32
v2_dot (v2 a, v2 b)
{
  return a.x * b.x + a.y * b.y;
}


f32
v2_length (v2 a)
{
  return sqrtf (v2_dot (a, a));
}


v2
v2_normalize (v2 a)
{
  f32 length = v2_length (a);

  if (length == 0.0f)
    {
      v2 v;

      v.x = 0.0f;
      v.y = 0.0f;

      return v;
    }

  return v2_div (a, length);
}


typedef struct
{
  f32 x, y, z;
} v3;


v3
v3_add (v3 a, v3 b)
{
  v3 v;

  v.x = a.x + b.x;
  v.y = a.y + b.y;
  v.z = a.z + b.z;

  return v;
}


v3
v3_sub (v3 a, v3 b)
{
  v3 v;

  v.x = a.x - b.x;
  v.y = a.y - b.y;
  v.z = a.z - b.z;

  return v;
}


v3
v3_mul (v3 a, f32 scalar)
{
  v3 v;

  v.x = a.x * scalar;
  v.y = a.y * scalar;
  v.z = a.z * scalar;

  return v;
}


v3
v3_div (v3 a, f32 scalar)
{
  v3 v;

  v.x = a.x / scalar;
  v.y = a.y / scalar;
  v.z = a.z / scalar;

  return v;
}


f32
v3_dot (v3 a, v3 b)
{
  return a.x * b.x + a.y * b.y;
}


f32
v3_length (v3 a)
{
  return sqrtf (v3_dot (a, a));
}


v3
v3_normalize (v3 a)
{
  f32 length = v3_length (a);

  if (length == 0.0f)
    {
      v3 v;

      v.x = 0.0f;
      v.y = 0.0f;
      v.z = 0.0f;

      return v;
    }

  return v3_div (a, length);
}


struct state
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  u32 *pixels;

  f64 dt;
  u64 last_tick;
} state;


void
r_frame_begin ()
{
  memset (state.pixels, 0, RW * RH * sizeof (u32));
}


void
r_frame_end ()
{
  void *pixels;
  int pitch;

  SDL_LockTexture (state.texture, NULL, &pixels, &pitch);

  for (usize y = 0; y < RH; ++y)
    memcpy (&((u8 *)pixels)[y * pitch], &state.pixels[y * RW], RW * sizeof (u32));

  SDL_UnlockTexture (state.texture);

  SDL_RenderCopyEx (state.renderer, state.texture, NULL, NULL, 0.0, NULL, SDL_FLIP_NONE);

  SDL_RenderPresent (state.renderer);
}


void
r_pixel (i32 x, i32 y, u32 color)
{
  state.pixels[y * RW + x] = color;
}


int
main (void)
{
  const char *title = "template";
  int window_x = SDL_WINDOWPOS_CENTERED;
  int window_y = SDL_WINDOWPOS_CENTERED;
  int window_flags = SDL_WINDOW_SHOWN;

  state.window = SDL_CreateWindow (title, window_x, window_y, WW, WH, window_flags);

  if (!state.window)
    {
      fprintf (stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError ());
      return 1;
    }

  state.renderer = SDL_CreateRenderer (state.window, -1, SDL_RENDERER_ACCELERATED);

  if (!state.renderer)
    {
      fprintf (stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError ());
      return 1;
    }

  state.texture = SDL_CreateTexture (state.renderer, SDL_PIXELFORMAT_RGB888,
                                     SDL_TEXTUREACCESS_STREAMING, RW, RH);

  if (!state.texture)
    {
      fprintf (stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError ());
      return 1;
    }

  state.pixels = calloc (RW * RH, sizeof (u32));

  state.last_tick = SDL_GetPerformanceCounter ();

  while (true)
    {
      u64 current_tick = SDL_GetPerformanceCounter ();
      u64 frequency = SDL_GetPerformanceFrequency ();

      state.dt = (f32)(current_tick - state.last_tick) / frequency;
      state.last_tick = current_tick;

      SDL_Event event;

      while (SDL_PollEvent (&event))
        switch (event.type)
          {
          case SDL_QUIT:
            goto quit;
          default:
            break;
          }

      r_frame_begin ();

      r_frame_end ();
    }

quit:
  free (state.pixels);
  SDL_DestroyTexture (state.texture);
  SDL_DestroyRenderer (state.renderer);
  SDL_DestroyWindow (state.window);

  return 0;
}

