#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>


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


#define WW 800
#define WH 600

#define RW 800
#define RH 600


#define PI 3.14159265359f
#define PI_2 (PI / 2.0f)
#define PI_4 (PI / 4.0f)
#define TAU (PI * 2.0f)

#define RAD(d) ((d) * (PI / 180.0f))
#define DEG(d) ((d) * (180.0f / PI))


static struct
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  u32 *pixels;

  bool quit;

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
      fprintf (stderr, "SDL_CreateTexturefailed: %s\n", SDL_GetError ());
      return 1;
    }

  state.pixels = calloc (RW * RH, sizeof (u32));

  state.last_tick = SDL_GetPerformanceCounter ();

  f32 x = 0;

  while (!state.quit)
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
            state.quit = true;
            break;
          default:
            break;
          }

      r_frame_begin ();

      r_frame_end ();
    }

  free (state.pixels);
  SDL_DestroyTexture (state.texture);
  SDL_DestroyRenderer (state.renderer);
  SDL_DestroyWindow (state.window);

  return 0;
}


