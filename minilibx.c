
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minilib.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lrosa-do <lrosa-do@student.42lisboa>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/15 18:20:43 by lrosa-do          #+#    #+#             */
/*   Updated: 2022/07/02 11:18:57 by lrosa-do         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include <SDL2/SDL_surface.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <strings.h>
#include "mlx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"


#define MAX_PIXEL_NB	200000
#define UNIQ_BPP	4
#define USEASSERT 1

#define FONT_WIDTH	10
#define	FONT_HEIGHT	20
#define MLX_MAX_EVENT 32

/*****************************************************************
 * EVENT DEFINITIONS
 *****************************************************************/

#define NoEventMask		    	0L
#define KeyPressMask			(1L<<0)
#define KeyReleaseMask			(1L<<1)
#define ButtonPressMask			(1L<<2)
#define ButtonReleaseMask		(1L<<3)
#define EnterWindowMask			(1L<<4)
#define LeaveWindowMask			(1L<<5)
#define PointerMotionMask		(1L<<6)
#define PointerMotionHintMask		(1L<<7)
#define Button1MotionMask		(1L<<8)
#define Button2MotionMask		(1L<<9)
#define Button3MotionMask		(1L<<10)
#define Button4MotionMask		(1L<<11)
#define Button5MotionMask		(1L<<12)
#define ButtonMotionMask		(1L<<13)
#define KeymapStateMask			(1L<<14)
#define ExposureMask			(1L<<15)
#define VisibilityChangeMask		(1L<<16)
#define StructureNotifyMask		(1L<<17)
#define ResizeRedirectMask		(1L<<18)
#define SubstructureNotifyMask		(1L<<19)
#define SubstructureRedirectMask	(1L<<20)
#define FocusChangeMask			(1L<<21)
#define PropertyChangeMask		(1L<<22)
#define ColormapChangeMask		(1L<<23)
#define OwnerGrabButtonMask		(1L<<24)



#define KeyPress		2
#define KeyRelease		3
#define ButtonPress		4
#define ButtonRelease		5
#define MotionNotify		6
#define EnterNotify		7
#define LeaveNotify		8
#define FocusIn			9
#define FocusOut		10
#define KeymapNotify		11
#define Expose			12
#define GraphicsExpose		13
#define NoExpose		14
#define VisibilityNotify	15
#define CreateNotify		16
#define DestroyNotify		17
#define UnmapNotify		18
#define MapNotify		19
#define MapRequest		20
#define ReparentNotify		21
#define ConfigureNotify		22
#define ConfigureRequest	23
#define GravityNotify		24
#define ResizeRequest		25
#define CirculateNotify		26
#define CirculateRequest	27
#define PropertyNotify		28
#define SelectionClear		29
#define SelectionRequest	30
#define SelectionNotify		31
#define ColormapNotify		32
#define ClientMessage		33
#define MappingNotify		34
#define GenericEvent		35
#define LASTEvent		   36




typedef struct	s_event_list
{
	int		mask;
    int		event;
	int		(*hook)();
	void	*param;
	int use;
}	t_event_list;


typedef struct {
  SDL_Surface *image;
  stbtt_bakedchar glyphs[128];
  int height;
} font_t;


typedef struct CoreData
{
int         quit ;
SDL_Window  *window;
SDL_Renderer *renderer;
SDL_Surface *screen;
font_t *fnt;
SDL_Joystick *joystick;
t_event_list        events[MLX_MAX_EVENT];
int SCREEN_WIDTH;
int SCREEN_HEIGHT;
t_event_list		hook_loop;
t_event_list		hook_mouse;
t_event_list		hook_keys;

int numEvents;
} CoreData;


CoreData *core;

//****************************************************************************
//UTIL
//*****************************************************************************
void set_pixel_color(void *dstPtr, SDL_Color color)
{

            ((unsigned char *)dstPtr)[0] = color.r;
            ((unsigned char *)dstPtr)[1] = color.g;
            ((unsigned char *)dstPtr)[2] = color.b;
            ((unsigned char *)dstPtr)[3] = color.a;

}

// Get color with alpha applied, alpha goes from 0.0f to 1.0f
SDL_Color color_alpha(SDL_Color color, float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    else if (alpha > 1.0f) alpha = 1.0f;

    return (SDL_Color){color.r, color.g, color.b, (unsigned char)(255.0f*alpha)};
}

// Get src alpha-blended into dst color with tint
SDL_Color color_alpha_blend(SDL_Color dst, SDL_Color src, SDL_Color tint)
{
    SDL_Color out = (SDL_Color){255,255,255,255};

    // Apply color tint to source color
    src.r = (unsigned char)(((unsigned int)src.r*(unsigned int)tint.r) >> 8);
    src.g = (unsigned char)(((unsigned int)src.g*(unsigned int)tint.g) >> 8);
    src.b = (unsigned char)(((unsigned int)src.b*(unsigned int)tint.b) >> 8);
    src.a = (unsigned char)(((unsigned int)src.a*(unsigned int)tint.a) >> 8);
    if (src.a == 0) out = dst;
    else if (src.a == 255) out = src;
    else
    {
        unsigned int alpha = (unsigned int)src.a + 1;     // We are shifting by 8 (dividing by 256), so we need to take that excess into account
        out.a = (unsigned char)(((unsigned int)alpha*256 + (unsigned int)dst.a*(256 - alpha)) >> 8);

        if (out.a > 0)
        {
            out.r = (unsigned char)((((unsigned int)src.r*alpha*256 + (unsigned int)dst.r*(unsigned int)dst.a*(256 - alpha))/out.a) >> 8);
            out.g = (unsigned char)((((unsigned int)src.g*alpha*256 + (unsigned int)dst.g*(unsigned int)dst.a*(256 - alpha))/out.a) >> 8);
            out.b = (unsigned char)((((unsigned int)src.b*alpha*256 + (unsigned int)dst.b*(unsigned int)dst.a*(256 - alpha))/out.a) >> 8);
        }
    }
    return out;
}
unsigned char *load_file_data(const char *fileName, unsigned int *bytesRead)
{
   unsigned char *data = NULL;
    *bytesRead = 0;

    if (fileName != NULL)
    {
        SDL_RWops* file= SDL_RWFromFile(fileName, "rb");


        if (file != NULL)
        {
            int size =(int) SDL_RWsize(file);

            if (size > 0)
            {
                data = (unsigned char *)SDL_malloc(size*sizeof(unsigned char));

                unsigned int count = (unsigned int) SDL_RWread(file, data, sizeof(unsigned char), size);
                *bytesRead = count;
            }
            else SDL_LogWarn(0, "FILEIO: [%s] Failed to read file", fileName);
            SDL_RWclose(file);
        }
        else SDL_LogWarn(0, "FILEIO: [%s] Failed to open file", fileName);
    }
    return data;
}

//****************************************************************************
//FONT
//*****************************************************************************


font_t *load_font_from_data( const void *data, int ptsize)
{
  int i;
  font_t *self = (font_t *)malloc(sizeof(font_t));
  if (!self)
    return (NULL);
  stbtt_fontinfo font;
  if ( !stbtt_InitFont(&font, data, 0) )
  {
    SDL_Log("could not load font\n" );
    return ((void*)0);
  }

  /* Get height and scale */
  int ascent, descent, lineGap;
  stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
  float scale = stbtt_ScaleForMappingEmToPixels(&font, ptsize);
  self->height = (ascent - descent + lineGap) * scale + 0.5;

  /* Init image */
  int w = 128, h = 128;



    unsigned char* pixels =NULL;
retry:
{
    pixels = (unsigned char*)malloc(w*h);


    float s = stbtt_ScaleForMappingEmToPixels(&font, 1) /    stbtt_ScaleForPixelHeight(&font, 1);
    int res = stbtt_BakeFontBitmap(data, 0, ptsize * s, pixels, w, h, 0, 128, self->glyphs);


    if (res < 0)
    {
        w <<= 1;
        h <<= 1;
        free(pixels);
        goto retry;
    }
}

/*
self->image = SDL_CreateRGBSurfaceWithFormat(0,w,h,8,SDL_PIXELFORMAT_RGB332);
//SDL_FillRect(self->image,NULL,SDL_MapRGB(self->image->format,0,0,0));

SDL_Rect rect=(SDL_Rect){0,0,w,h};
int bytesPerPixel=1;


    for (int y = 0; y < h; y++)
    {
          unsigned char *dst =((unsigned char *)self->image->pixels) + y*(int)w*bytesPerPixel;
          unsigned char *src =((unsigned char *)pixels) + y  * w  * 1;
          int pitch =(int)w*bytesPerPixel;
          memcpy(dst,src,pitch);
    }

*/




//self->image = SDL_CreateRGBSurface(0,  w, h, 32 , 0, 0, 0, 0);
self->image = SDL_CreateRGBSurfaceWithFormat(0,w,h,24,SDL_PIXELFORMAT_BGR24);
SDL_SetSurfaceBlendMode(self->image,SDL_BLENDMODE_NONE);
SDL_Rect rect  =(SDL_Rect){0,0,w,h};
SDL_Rect srcRec=(SDL_Rect){0,0,w,h};


int strideSrc = self->image->pitch;
int bytesPerPixelSrc = self->image->format->BytesPerPixel;

int strideFnt = w * 1;
int bytesPerPixelFnt= strideFnt/w;


SDL_Log(" %d %d \n",strideSrc,bytesPerPixelSrc);
SDL_Log(" %d %d \n",strideFnt,bytesPerPixelFnt);





        unsigned char *pSrcBase = (unsigned char *)self->image->pixels + ((int)srcRec.y * w + (int)srcRec.x) * bytesPerPixelSrc;
        unsigned char *pFntBase = (unsigned char *)pixels + ((int)srcRec.y * w + (int)srcRec.x) * bytesPerPixelFnt;


        SDL_Color pixel =(SDL_Color){255,255,255,255};

        for (int y = 0; y < (int)srcRec.h; y++)
        {
                 unsigned char *pSrc = pSrcBase;
                 unsigned char *pFnt = pFntBase;

                 for (int x = 0; x < (int)srcRec.w; x++)
                {

                    Uint8 c = ((unsigned char *)pFnt)[0];
                    if (c!=0)
                        pixel =(SDL_Color){255,255,255,255};
                    else
                        pixel =(SDL_Color){0,0,0,0};

                    set_pixel_color(pSrc, pixel);
                    pSrc += bytesPerPixelSrc;
                    pFnt += bytesPerPixelFnt;
                }
            pSrcBase += strideSrc;
            pFntBase += strideFnt;
        }

SDL_SetColorKey(self->image,SDL_TRUE,0);
//SDL_SetColorKey(self->image,1,0);
//SDL_SaveBMP(self->image,"font.bmp");



//now i can free the pixels ;)
free(pixels);



  int scaledAscent = ascent * scale + 0.5;
  for (i = 0; i < 128; i++)
  {
    self->glyphs[i].yoff += scaledAscent;
  }
  return self;
}


font_t *load_font(const char *filename)
{

unsigned int fileSize = 0;
unsigned char *fileData = load_file_data(filename, &fileSize);
if (fileData)
{
font_t *self = load_font_from_data(fileData, fileSize);
free(fileData);
return self;
}
return (NULL);
}

font_t *font_intern()
{
  #include "font_ttf.h"
  int ptsize =11;
  return load_font_from_data(font_ttf, ptsize);
}


void free_font(font_t * font)
{
SDL_FreeSurface(font->image);
free(font);
}


void font_blit(SDL_Surface *buffer, font_t *self, const char *str, int dx, int dy)
{
  const char *p = str;
  int x = dx;
  int y = dy;

  while (*p)
  {
    if (*p == '\n')
    {
      x = dx;
      y += self->height;
    } else
     {
      stbtt_bakedchar *g = &self->glyphs[(int) (*p & 127)];
      int w = g->x1 - g->x0;
      int h = g->y1 - g->y0;

  SDL_Rect SrcR;
  SDL_Rect DestR;

  SrcR.x = g->x0;
  SrcR.y = g->y0;
  SrcR.w = w;
  SrcR.h = h;

  DestR.x = x + g->xoff;
  DestR.y = y + g->yoff;
  DestR.w = w;
  DestR.h = h;



      SDL_BlitSurface(self->image, &SrcR, buffer, &DestR);

      x += g->xadvance;
    }
    p++;
  }
}



//****************************************************************************
//MLX CORE
//*****************************************************************************

void	*mlx_init()
{

core = (CoreData*) calloc(1,sizeof(CoreData));
if (!core)
    return ((void*)0);
bzero(core,sizeof(CoreData));
core->quit =0;
core->numEvents=0;


for (int i=0;i<MLX_MAX_EVENT;i++)
{
    core->events[i].param=NULL;
    core->events[i].hook=NULL;
    core->events[i].use =0;
    core->events[i].event =0;
    core->events[i].mask=0;
}

core->hook_loop.param=NULL;
core->hook_loop.hook=NULL;
core->hook_loop.use =0;

core->hook_keys.use =0;
core->hook_keys.param=NULL;
core->hook_keys.hook=NULL;


core->hook_mouse.use=0;
core->hook_mouse.param=NULL;
core->hook_mouse.hook=NULL;

core->joystick =NULL;
core->renderer=NULL;
core->screen=NULL;
core->window=NULL;


SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS |  SDL_INIT_JOYSTICK |  SDL_INIT_GAMECONTROLLER  );

SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
return core;
}


void	mlx_free(void *mlx)
{
  (void)mlx;
  #if  (defined USEASSERT)
  SDL_assert(mlx != NULL);
  #endif
  if (core->fnt)
  {
  free_font(core->fnt);
  }

 for (int i=0;i<MLX_MAX_EVENT;i++)
{
    core->events[i].param=NULL;
    core->events[i].hook=NULL;
    core->events[i].use =0;
}

  SDL_free(mlx);
  SDL_Quit();
}

int	mlx_destroy_display(void *mlx_ptr)
{
  (void)mlx_ptr;
   #if  (defined USEASSERT)
          SDL_assert(core != NULL);
          SDL_assert(core->renderer != NULL);
    #endif


  SDL_DestroyRenderer(core->renderer);
  core->renderer=NULL;
 return (0);
}

int mlx_loop_end (void *mlx_ptr)
{
  (void)mlx_ptr;
  core->quit =1;

}

int	mlx_destroy_window(void *mlx_ptr, void *win_ptr)
{

(void)mlx_ptr;
(void)win_ptr;
#if  (defined USEASSERT)
        SDL_assert(core != NULL);
        SDL_assert(core->window != NULL);
#endif
  if (!core->renderer)
  {
    SDL_DestroyRenderer(core->renderer);
    core->renderer=NULL;
  }
   SDL_DestroyWindow(core->window);
   return (1);
}
void	*mlx_new_window(void *mlx_ptr, int size_x, int size_y, char *title)
{
    (void)mlx_ptr;
 //   core->window = SDL_CreateWindow(title,0,0,size_x,size_y, SDL_WINDOW_SHOWN |SDL_WINDOW_FULLSCREEN );
   core->window = SDL_CreateWindow(title,SDL_WINDOWPOS_CENTERED,           // initial x position
        SDL_WINDOWPOS_CENTERED,size_x,size_y, SDL_WINDOW_SHOWN  );

    if (!core->window)
        return ((void*)0);
    core->screen = SDL_GetWindowSurface( core->window );
    core->renderer = SDL_CreateSoftwareRenderer(core->screen);
    core->SCREEN_WIDTH  = size_x;
    core->SCREEN_HEIGHT = size_y;
    core->fnt=font_intern();
    SDL_SetSurfaceBlendMode(core->screen,SDL_BLENDMODE_NONE);
    for (int e=0;e<core->numEvents;e++)
    {
        if (core->events[e].use && core->events[e].event ==CreateNotify && core->events[e].mask == VisibilityChangeMask)
        {
            core->events[e].hook(core->hook_keys.param);
            break;
        }
    }



    return (void*)core->window;
}

int	mlx_hook(void *win_ptr, int x_event, int x_mask, int (*funct)(), void *param)
{
    (void)win_ptr;
    core->events[core->numEvents].param=param;
    core->events[core->numEvents].hook=funct;
    core->events[core->numEvents].use =1;
    core->events[core->numEvents].event =x_event;
    core->events[core->numEvents].mask=x_mask;
    core->numEvents++;
    return (1);
}

int	mlx_mouse_hook (void *win_ptr, int (*funct_ptr)(), void *param)
{
    (void)win_ptr;
    core->hook_mouse.hook = funct_ptr;
    core->hook_mouse.mask = 0;
    core->hook_mouse.use  = 1;
    core->hook_mouse.param= param;
    return (1);
}

int	mlx_key_hook (void *win_ptr, int (*funct_ptr)(), void *param)
{
    (void)win_ptr;
    core->hook_keys.hook = funct_ptr;
    core->hook_keys.mask = 0;
    core->hook_keys.use  = 1;
    core->hook_keys.param= param;
    return (1);
}

int	mlx_loop_hook (void *mlx_ptr, int (*funct_ptr)(), void *param)
{
    (void)mlx_ptr;
    core->hook_loop.hook = funct_ptr;
    core->hook_loop.mask = 0;
    core->hook_loop.use  = 1;
    core->hook_loop.param= param;
    return (1);

}


int	mlx_clear_window(void *mlx_ptr, void *win_ptr)
{

     (void)mlx_ptr;
     (void)win_ptr;


#if  (defined USEASSERT)
        SDL_assert(core->window != NULL);
        SDL_assert(core->screen != NULL);
     #endif


     SDL_FillRect(core->screen, NULL, SDL_MapRGBA(core->screen->format,0,0,45,0));
     return (1);
}


int get_os_key(int key)
{

    switch (key)
    {
      case 27:return 65307;break;
      case SDLK_q:return 12;break;
      case SDLK_w:return 13;break;
      case SDLK_e:return 14;break;

      case SDLK_a:return 0;break;
      case SDLK_s:return 1;break;
      case SDLK_d:return 2;break;


      case SDLK_UP  :return 126;break;
      case SDLK_DOWN:return 125;break;
      case SDLK_LEFT:return 123;break;
      case SDLK_RIGHT:return 125;break;

    case SDLK_TAB:return 48;break;

    }


  return key;
}

void add_key_down(int key)
{
   if (core->hook_keys.use)
      core->hook_keys.hook(key, core->hook_keys.param);
  for (int e=0;e<core->numEvents;e++)
  {
      if (core->events[e].use && core->events[e].event ==KeyPress && core->events[e].mask == KeyPressMask)
      {
          core->events[e].hook(key, core->events[e].param);
          break;
      }
  }
}

void add_key_up(int key)
{
  for (int e=0;e<core->numEvents;e++)
  {
      if (core->events[e].use && core->events[e].event ==KeyRelease && core->events[e].mask == KeyReleaseMask)
      {
          core->events[e].hook(key, core->events[e].param);
          break;
      }
  }
}

static void
DrawRect( const int x, const int y, const int w, const int h)
{
    SDL_Rect area;
    area.x = x;
    area.y = y;
    area.w = w;
    area.h = h;
    SDL_RenderFillRect(core->renderer, &area);
}


static void PrintJoystick(SDL_Joystick *joy)
{
    const char *type;
    char guid[64];

    SDL_assert(SDL_JoystickFromInstanceID(SDL_JoystickInstanceID(joy)) == joy);
    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, sizeof (guid));
    switch (SDL_JoystickGetType(joy)) {
    case SDL_JOYSTICK_TYPE_GAMECONTROLLER:
        type = "Game Controller";
        break;
    case SDL_JOYSTICK_TYPE_WHEEL:
        type = "Wheel";
        break;
    case SDL_JOYSTICK_TYPE_ARCADE_STICK:
        type = "Arcade Stick";
        break;
    case SDL_JOYSTICK_TYPE_FLIGHT_STICK:
        type = "Flight Stick";
        break;
    case SDL_JOYSTICK_TYPE_DANCE_PAD:
        type = "Dance Pad";
        break;
    case SDL_JOYSTICK_TYPE_GUITAR:
        type = "Guitar";
        break;
    case SDL_JOYSTICK_TYPE_DRUM_KIT:
        type = "Drum Kit";
        break;
    case SDL_JOYSTICK_TYPE_ARCADE_PAD:
        type = "Arcade Pad";
        break;
    case SDL_JOYSTICK_TYPE_THROTTLE:
        type = "Throttle";
        break;
    default:
        type = "Unknown";
        break;
    }
    SDL_Log("Joystick\n");
    SDL_Log("          name: %s\n", SDL_JoystickName(joy));
    SDL_Log("          type: %s\n", type);
  //  SDL_Log("           LED: %s\n", SDL_JoystickHasLED(joy) ? "yes" : "no");
  // SDL_Log("        rumble: %s\n", SDL_JoystickHasRumble(joy) ? "yes" : "no");
  //  SDL_Log("trigger rumble: %s\n", SDL_JoystickHasRumbleTriggers(joy) ? "yes" : "no");
    SDL_Log("          axes: %d\n", SDL_JoystickNumAxes(joy));
    SDL_Log("         balls: %d\n", SDL_JoystickNumBalls(joy));
    SDL_Log("          hats: %d\n", SDL_JoystickNumHats(joy));
    SDL_Log("       buttons: %d\n", SDL_JoystickNumButtons(joy));
    SDL_Log("   instance id: %d\n", SDL_JoystickInstanceID(joy));
    SDL_Log("          guid: %s\n", guid);
    SDL_Log("       VID/PID: 0x%.4x/0x%.4x\n", SDL_JoystickGetVendor(joy), SDL_JoystickGetProduct(joy));
}

int	mlx_loop (void *mlx_ptr)
{
   (void)mlx_ptr;
   int key_x_down =0;
   int key_y_down =0;

    int doClose = 0; // melhor terminar o loop fora dos eventos ???? fica assim
		while( !core->quit )
		{
      SDL_Event   events;
			while( SDL_PollEvent( &events ) != 0 )
			{


				switch (events.type)
				{
                        case SDL_QUIT:
                        {
                            core->quit = 1;
                            core->hook_loop.hook=NULL;
                            core->hook_loop.use=0;
                            return (1);

                        }
                         case SDL_WINDOWEVENT:
                        {


                              if (events.window.event == SDL_WINDOWEVENT_RESIZED)
                              {

                                  for (int e=0;e<core->numEvents;e++)
                                  {
                                      if (core->events[e].use && core->events[e].event ==ResizeRequest && core->events[e].mask == ResizeRedirectMask)
                                      {
                                          core->events[e].hook(events.window.data1,events.window.data2, core->events[e].param);
                                          return (1);
                                      }
                                  }
                              }

                              else  if (events.window.event ==  SDL_WINDOWEVENT_CLOSE)
                              {
                                              doClose = 1;
                                              core->hook_loop.hook=NULL;
                                              core->hook_loop.use=0;

                              }
                        break;
                        }

                    case SDL_JOYDEVICEADDED:
                    {
                            if (!core->joystick)
                            {
                                core->joystick = SDL_JoystickOpen(events.jdevice.which);
                                if (core->joystick)
                                {
                                    PrintJoystick(core->joystick);
                                }
                            }
                            break;
                    }

                        case SDL_JOYDEVICEREMOVED:
                        {
                            SDL_Log("Joystick device %d removed.\n", (int) events.jdevice.which);
                            if (events.jdevice.which == SDL_JoystickInstanceID(core->joystick))
                            {
                                SDL_JoystickClose(core->joystick);
                                core->joystick = SDL_JoystickOpen(0);
                            }
                            break;
                        }
                        case SDL_JOYAXISMOTION:
                        {

                            if (events.jaxis.which==0)
                            {
                              if (events.jaxis.axis==1)
                              {
                                    if (events.jaxis.value<=-32768 )
                                    {


                                    }

                                      if (events.jaxis.value>=32767 )
                                      {


                                      }

                                 }

                              //**
                               if (events.jaxis.axis==0)
                              {
                                if (events.jaxis.value <= -32768 )
                                {


                                }

                                  if (events.jaxis.value>= 32767)
                                  {


                                  }

                              }

                            }
                        }
                            break;
                        case SDL_JOYHATMOTION:
                            SDL_Log("Joystick %d hat %d value:",events.jhat.which, events.jhat.hat);
                            if (events.jhat.value == SDL_HAT_CENTERED)
                                SDL_Log(" centered");
                            if (events.jhat.value & SDL_HAT_UP)
                            {
                                 add_key_down(SDLK_w);
                                     SDL_Log(" up");
                            }


                            if (events.jhat.value & SDL_HAT_RIGHT)
                            {
                                 SDL_Log(" right");
                                    add_key_down(SDLK_d);
                            }

                            if (events.jhat.value & SDL_HAT_DOWN)
                            {
                                 SDL_Log(" down");
                                   add_key_down(SDLK_s);
                            }

                            if (events.jhat.value & SDL_HAT_LEFT)
                            {
                                SDL_Log(" left");
                                add_key_down(SDLK_a);
                            }

                            SDL_Log("\n");
                            break;
                        case SDL_JOYBALLMOTION:
                        {
                            SDL_Log("Joystick %d ball %d delta: (%d,%d)\n",events.jball.which,events.jball.ball, events.jball.xrel, events.jball.yrel);
                            break;
                        }
                        case SDL_JOYBUTTONDOWN:
                        {
                            SDL_Log("Joystick %d button %d down\n",events.jbutton.which, events.jbutton.button);
                            /* First button triggers a 0.5 second full strength rumble */
                            if (events.jbutton.button == 9)
                                add_key_down(SDLK_ESCAPE);
                            if (events.jbutton.button == 8)
                                add_key_down(SDLK_RETURN);
                            if (events.jbutton.button == 0)
                                add_key_down(SDLK_SPACE);
                            if (events.jbutton.button == 13)
                                add_key_down(SDLK_w);
                            if (events.jbutton.button == 14)
                                add_key_down(SDLK_s);
                            if (events.jbutton.button == 15)
                                add_key_down(SDLK_a);
                            if (events.jbutton.button == 16)
                                add_key_down(SDLK_d);



                            break;
                        }
                        case SDL_JOYBUTTONUP:
                        {
                            SDL_Log("Joystick %d button %d up\n",events.jbutton.which, events.jbutton.button);
                            break;
                        }

                       case SDL_KEYDOWN:
                       {
                           // key.keysym.scancode,key.keysym.sym,key.keysym.mod);
                          // printf("scan:%d  key:%d, \n",events.key.keysym.scancode,events.key.keysym.sym);

                           add_key_down(events.key.keysym.sym);

                       }
                       break;
                       case SDL_KEYUP:
                       {

                            add_key_up(events.key.keysym.sym);

                       }
                       break;

                        case SDL_MOUSEBUTTONDOWN:
                        {


                            if (core->hook_mouse.use)
                            {
                                core->hook_mouse.hook(events.button.x,events.button.y,events.button.button -1, core->hook_mouse.param);
                            }
                            for (int e=0;e<core->numEvents;e++)
                            {
                                if (core->events[e].use && core->events[e].event ==ButtonPress && core->events[e].mask == ButtonPressMask)
                                {
                                    core->events[e].hook(events.button.x,events.button.y,events.button.button -1, core->events[e].param);
                                    break;
                                }
                            }

                        }
                        break;
                        case SDL_MOUSEBUTTONUP:
                        {
                            for (int e=0;e<core->numEvents;e++)
                            {
                                if (core->events[e].use && core->events[e].event ==ButtonRelease && core->events[e].mask == ButtonReleaseMask)
                                {
                                    core->events[e].hook(events.button.x,events.button.y,events.button.button -1, core->events[e].param);
                                    break;
                                }
                            }

                        }
                        break;
                        case SDL_MOUSEMOTION:
                        {
                            for (int e=0;e<core->numEvents;e++)
                            {
                                if (core->events[e].use && core->events[e].event ==MotionNotify && core->events[e].mask == PointerMotionMask)
                                {
                                    core->events[e].hook(events.button.x,events.button.y,events.button.button -1, core->events[e].param);
                                    break;
                                }
                            }
                        }
                        break;
                    }
                break;
			}
      if (doClose == 1)
      {
          for (int e=0;e<core->numEvents;e++)
          {
              if (core->events[e].use && core->events[e].event ==DestroyNotify && core->events[e].mask == StructureNotifyMask)
              {
                core->events[e].hook(core->events[e].param);
                break;
              }
          }
          return (0);

      } else
      {


          #if  (defined USEASSERT)
            SDL_assert(core->window != NULL);
         #endif

          if (core->hook_loop.use)
              {

                  core->hook_loop.hook(core->hook_loop.param);
              }

         if (core->joystick)
         {


            SDL_SetRenderDrawColor(core->renderer, 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE);


            int x, y;
            x = (((int) SDL_JoystickGetAxis(core->joystick, 0)) );
            if (x>=300)
            {
                          add_key_down(SDLK_d);
            }
            if (x<=-300)
            {
                          add_key_down(SDLK_a);
            }


            y = (((int) SDL_JoystickGetAxis(core->joystick, 1)) );
            if (y<=-300)
            {
                          add_key_down(SDLK_w);
            }
            if (y>=300)
            {
                          add_key_down(SDLK_s);

            }
        }





            SDL_UpdateWindowSurface( core->window );


      }

     }
    return (0);
}


void	*mlx_new_image(void *mlx_ptr,int width,int height)
{
    (void)mlx_ptr;
    SDL_Surface *buffer = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_BGRA32);
    // SDL_Surface *buffer = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_ARGB8888);

    if (!buffer)
        return ((void*)0);
    SDL_SetSurfaceBlendMode(buffer,SDL_BLENDMODE_NONE);
    SDL_SetColorKey(buffer,SDL_TRUE,0xFF000000);


    return (void*)buffer;
}

int	mlx_destroy_image(void *mlx_ptr, void *img_ptr)
{
    (void)mlx_ptr;
    if (!img_ptr)
        return (0);

#if  (defined USEASSERT)
    SDL_assert(img_ptr != NULL);
#endif


    SDL_Surface *buffer = (SDL_Surface*)img_ptr;
    if (!buffer)
        return (0);
#if  (defined USEASSERT)
    SDL_assert(buffer != NULL);
#endif
    SDL_FreeSurface(buffer);
    return (1);
}

int	mlx_put_image_to_window(void *mlx_ptr, void *win_ptr, void *img_ptr, int x, int y)
{
    (void)mlx_ptr;
    (void)win_ptr;
#if  (defined USEASSERT)
    SDL_assert(core->screen != NULL);
    SDL_assert(img_ptr != NULL);
  #endif


    if (!img_ptr)
        return (0);
    SDL_Surface *buffer = (SDL_Surface*)img_ptr;
    if (!buffer)
        return (0);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = buffer->w;
    rect.h = buffer->h;

    SDL_BlitSurface( buffer,NULL, core->screen,  &rect );

    return (1);
}

char	*mlx_get_data_addr(void *img_ptr, int *bits_per_pixel,int *size_line, int *endian)
{
#if  (defined USEASSERT)
    SDL_assert(img_ptr != NULL);
  #endif

    if (!img_ptr)
        return (0);

    SDL_Surface *buffer = (SDL_Surface*)img_ptr;
    if (!buffer)
        return (0);
    *bits_per_pixel=buffer->format->BitsPerPixel;
    //*bits_per_pixel=buffer->format->BytesPerPixel;
    *size_line     =buffer->pitch;
    *endian=0;
    return (char*)buffer->pixels;
}

int	mlx_pixel_put(void *mlx_ptr, void *win_ptr, int x, int y, int color)
{
    (void)mlx_ptr;
    (void)win_ptr;

    SDL_Surface *surface = (SDL_Surface*)core->screen;
    if (!surface)
        return (0);
    Uint8 *target_pixel = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
   *(Uint32 *)target_pixel = color;

  //   Uint8 *target = surface->pixels + (x * surface->format->BitsPerPixel / 8 + y * surface->pitch);//
//	*(Uint32 *)target = color;

    return 1;
}



///dumy :P

int	mlx_do_key_autorepeatoff(void *mlx_ptr)
{
    (void)mlx_ptr;
     return (0);

}

int	mlx_do_key_autorepeaton(void *mlx_ptr)
{
        (void)mlx_ptr;
     return (0);
}


int	mlx_do_sync(void *mlx_ptr)
{
     (void)mlx_ptr;
     return (0);
}


int		mlx_mouse_move(void *mlx_ptr, void *win_ptr, int x, int y)
{

	return (0);
}

int		mlx_mouse_hide(void *mlx_ptr, void *win_ptr)
{
	   (void)mlx_ptr;
     (void)win_ptr;
     return (0);

}

int		mlx_mouse_show(void *mlx_ptr, void *win_ptr)
{
	   (void)mlx_ptr;
     (void)win_ptr;
     return (0);

}


int		mlx_mouse_get_pos(void *mlx_ptr, void *win_ptr, int *win_x_return, int *win_y_return)
{
	   (void)mlx_ptr;
     (void)win_ptr;
     return (0);
}
int		mlx_get_screen_size(void *mlx_ptr, int *sizex, int *sizey)
{
(void)mlx_ptr;


SDL_DisplayMode dm;

if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
{
     SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
     return 1;
}


*sizex = dm.w;
*sizey = dm.h;

return (0);
}


//*****************************************************************************************************************


void	*mlx_png_file_to_image(void *mlx_ptr ,char *filename, int *width,int *height)
{
   (void)mlx_ptr;

    int comp;
    int w;
    int h;

    unsigned int fileSize = 0;
    unsigned char *fileData = load_file_data(filename, &fileSize);
    unsigned char *data = stbi_load_from_memory(fileData, fileSize, &w, &h, &comp, 4);
#if  (defined USEASSERT)
    SDL_assert(data != NULL);
  #endif

    if (!data)
        return ((void*)0);




    int depth =0;
    int pitch =0;
    Uint32 pixel_format = SDL_PIXELFORMAT_RGBA32;


    if (comp == 2)
    {

        depth = 2;
        pitch = 2 * w;

        pixel_format = SDL_PIXELFORMAT_RGBA4444;



    }else if (comp == 3)
    {
        depth = 24;
        pitch = 3 * w; // 3 bytes per pixel * pixels per row

        pixel_format = SDL_PIXELFORMAT_RGB24;

    }else if (comp == 4)
    {
        depth = 32;
        pitch = 4 * w;

        pixel_format = SDL_PIXELFORMAT_RGBA32;

    }


    SDL_Surface* buffer = SDL_CreateRGBSurfaceWithFormatFrom((void*)data,  w, h,   depth, pitch, pixel_format);

    SDL_free(data);

    *width  = w;
    *height = h;

    //SDL_Surface *buffer = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA8888);
    if (!buffer)
        return ((void*)0);

    return (void*) buffer;
}



typedef struct mlx_image
{
int width;
int height;
int line_len;
int endian;
int bpp;
char *buffer;
}mlx_image;

int	mlx_int_str_str(char *str,char *find,int len)
{
  int	len_f;
  int	pos;
  char	*s;
  char	*f;

  len_f = strlen(find);
  if (len_f>len)
    return (-1);
  pos = 0;
  while (*(str+len_f-1))
    {
      s = str;
      f = find;
      while (*(f++) == *(s++))
        if (!*f)
          return (pos);
      str ++;
      pos ++;
    }
  return (-1);
}



int	mlx_int_str_str_cote(char *str,char *find,int len)
{
  int	len_f;
  int	pos;
  char	*s;
  char	*f;
  int	cote;

  len_f = strlen(find);
  if (len_f>len)
    return (-1);
  cote = 0;
  pos = 0;
  while (*(str+len_f-1))
    {
      if (*str=='"')
	cote = 1-cote;
      if (!cote)
	{
	  s = str;
	  f = find;
	  while (*(f++) == *(s++))
	    if (!*f)
	      return (pos);
	}
      str ++;
      pos ++;
    }
  return (-1);
}


char	**mlx_int_str_to_wordtab(char *str)
{
  char	**tab;
  int	pos;
  int	nb_word;
  int	len;

  len = strlen(str);
  nb_word = 0;
  pos = 0;
  while (pos<len)
  {
    while (*(str+pos)==' ' || *(str+pos)=='\t')
      pos ++;
    if (*(str+pos))
      nb_word ++;
    while (*(str+pos) && *(str+pos)!=' ' && *(str+pos)!='\t')
      pos ++;
  }
  if (!(tab = SDL_malloc((1+nb_word)*sizeof(*tab))))
    return ((char **)0);
  nb_word = 0;
  pos = 0;
  while (pos<len)
    {
      while (*(str+pos)==' ' || *(str+pos)=='\t')
	{
	  *(str+pos) = 0;
	  pos ++;
	}
      if (*(str+pos))
	{
	  tab[nb_word] = str+pos;
	  nb_word ++;
	}
      while (*(str+pos) && *(str+pos)!=' ' && *(str+pos)!='\t')
	pos ++;
    }
  tab[nb_word] = 0;
  return (tab);
}


typedef struct  s_xpm_col
{
  int           name;
  int           col;
} t_xpm_col;


struct  s_col_name
{
  char  *name;
  int   color;
};


int mlx_destroy_mlximage(mlx_image *img)
{
SDL_free(img->buffer);
SDL_free(img);
return 0;
}

mlx_image *mlx_new_mlximage(int w, int h)
{
  mlx_image *img = SDL_malloc(sizeof(mlx_image));
  bzero(img,sizeof(mlx_image));
  img->width=w;
  img->height=h;
  img->buffer=SDL_malloc(sizeof(char)*w*h*4);
  bzero(img->buffer,sizeof(char)*w*h*4);
  return img;
}

void	mlx_set_pixel(mlx_image *img, char *data, int opp, int col, int x)
{
  *((unsigned int *)(data+4*x)) = col;
}


#include "mlx_rgb.c"


#define	RETURN	{ if (colors) SDL_free(colors); if (tab) SDL_free(tab); \
		  if (colors_direct) SDL_free(colors_direct); \
                  if (img) mlx_destroy_mlximage( img);   \
                  return ((void *)0); }




char	*mlx_int_get_line(char *ptr,int *pos,int size)
{
  int	pos2;
  int	pos3;
  int	pos4;

  if ((pos2 = mlx_int_str_str(ptr+*pos,"\"",size-*pos))==-1)
    return ((char *)0);
  if ((pos3 = mlx_int_str_str(ptr+*pos+pos2+1,"\"",size-*pos-pos2-1))==-1)
    return ((char *)0);
  *(ptr+*pos+pos2) = 0;
  *(ptr+*pos+pos2+1+pos3) = 0;
  pos4 = *pos+pos2+1;
  *pos += pos2+pos3+2;
  return (ptr+pos4);
}


char	*mlx_int_static_line(char **xpm_data,int *pos,int size)
{
  static char	*copy = 0;
  static int	len = 0;
  int		len2;
  char		*str;

  str = xpm_data[(*pos)++];
  if ((len2 = SDL_strlen(str))>len)
    {
      if (copy)
	      SDL_free(copy);
      if (!(copy = SDL_malloc(len2+1)))
	        return ((char *)0);
      len = len2;
    }

  SDL_strlcpy(copy, str, len2+1);
  return (copy);
}


int	mlx_int_get_col_name(char *str,int size)
{
  int	result;

  result = 0;
  while (size--)
    result = (result<<8)+*(str++);
  return (result);
}

int	mlx_int_get_text_rgb(char *name, char *end)
{
  int	i;
  char	buff[64];

  if (*name == '#')
    return (strtol(name+1,0,16));
  if (end)
    {
      SDL_snprintf(buff, 64, "%s %s", name, end);
      name = buff;
    }
  i = 0;
  while (mlx_col_name[i].name)
    {
      if (!SDL_strcasecmp(mlx_col_name[i].name, name))
	return (mlx_col_name[i].color);
      i ++;
    }
  return (0);
}





void	*mlx_int_parse_xpm(void *info,int info_size,char *(*f)())
{
  int	pos;
  char	*line;
  char	**tab;
  char	*data;

  int	nc;
  int	opp;
  int	cpp;
  int	col;
  int	rgb_col;
  int	col_name;
  int	method;
  int	x;
  int	i;
  int	j;
  mlx_image	*img;
  t_xpm_col	*colors;
  int		*colors_direct;
  int	width;
  int	height;

  colors = 0;
  colors_direct = 0;
  img = 0;
  tab = 0;
  pos = 0;
  if (!(line = f(info,&pos,info_size)) ||
      !(tab = mlx_int_str_to_wordtab(line)) || !(width = SDL_atoi(tab[0])) ||
      !(height = SDL_atoi(tab[1])) || !(nc = SDL_atoi(tab[2])) ||
      !(cpp = SDL_atoi(tab[3])) )
    RETURN;
  SDL_free(tab);
  tab = 0;

  method = 0;
  if (cpp<=2)
    {
      method = 1;
      if (!(colors_direct = SDL_malloc((cpp==2?65536:256)*sizeof(int))))
	RETURN;
    }
  else
    if (!(colors = SDL_malloc(nc*sizeof(*colors))))
      RETURN;



  i = nc;
  while (i--)
    {
      if (!(line = f(info,&pos,info_size)) ||
	  !(tab = mlx_int_str_to_wordtab(line+cpp)) )
	RETURN;
      j = 0;
      while (tab[j] && SDL_strcmp(tab[j++],"c"));

      if (!tab[j])
	RETURN;

      rgb_col = mlx_int_get_text_rgb(tab[j], tab[j+1]);

      if (method)
	colors_direct[mlx_int_get_col_name(line,cpp)] = rgb_col;

      else
          {
            colors[i].name = mlx_int_get_col_name(line,cpp);
            colors[i].col = rgb_col;
          }
      SDL_free(tab);
      tab = 0;
    }

  if (!(img = mlx_new_mlximage(width,height)))
    RETURN;
  opp = 4;



  i = height;
  data = img->buffer;
  while (i--)
    {
      if (!(line = f(info,&pos,info_size)))
	RETURN;
      x = 0;
      while (x<width)
	{
	  col = 0;
	  col_name = mlx_int_get_col_name(line+cpp*x,cpp);
	  if (method)
	    col = colors_direct[col_name];
	  else
	    {
	      j = nc;
	      while (j--)
		if (colors[j].name==col_name)
		  {
		    col = colors[j].col;
		    j = 0;
		  }
	    }

	  if (col==-1)
	    col = 0xFF000000;
	  mlx_set_pixel(img, data, opp, col, x);
	  x ++;
	}
      data += img->width*4;
    }

  if (colors)
      SDL_free(colors);
  if (colors_direct)
      SDL_free(colors_direct);
  return (img);
}


void	mlx_int_file_get_rid_comment(char *ptr, int size)
{
  int	com_begin;
  int	com_end;

  while ((com_begin = mlx_int_str_str_cote(ptr,"/*",size))!=-1)
    {
      com_end = mlx_int_str_str(ptr+com_begin+2,"*/",size-com_begin-2);
      SDL_memset(ptr+com_begin,' ',com_end+4);
    }
  while ((com_begin = mlx_int_str_str_cote(ptr,"//",size))!=-1)
    {
      com_end = mlx_int_str_str(ptr+com_begin+2,"\n",size-com_begin-2);
      SDL_memset(ptr+com_begin,' ',com_end+3);
    }
}





char    *mlx_xpm_get_data_addr(mlx_image *img_ptr, int *bits_per_pixel, int *size_line)
{
  *bits_per_pixel = UNIQ_BPP*8;
  *size_line = img_ptr->width*UNIQ_BPP;
  return (img_ptr->buffer);
}

void	*mlx_xpm_file_to_image_internal(char *file,int *width,int *height)
{
  unsigned int	size;
  char	*ptr;
  mlx_image	*img;

  ptr =(char*)load_file_data(file, &size);
  if (!ptr)
      return ((void *)0);

    mlx_int_file_get_rid_comment(ptr, size);

    if ((img = mlx_int_parse_xpm(ptr,size,mlx_int_get_line)))
      {
        *width = img->width;
        *height = img->height;
      }
    SDL_free(ptr);
    return (void*)img;
}


void	*mlx_xpm_file_to_image(void *mlx_ptr,char *file,int *width,int *height)
{
   (void)mlx_ptr;
    mlx_image	*img;

    img = mlx_xpm_file_to_image_internal(file,width,height);
    if (!img)
        return ((void *)0);
    int bits_per_pixel;
    int size_line;
    int w=*width;
    int h=*height;
   char* pixels =mlx_xpm_get_data_addr (img, &bits_per_pixel, &size_line);

  //copy pixel by pixel , and so, we free the pixel memory :(
    //SDL_Surface  *surface =SDL_CreateRGBSurfaceWithFormatFrom((void*)pixels, img->width,img->height,bits_per_pixel,size_line,SDL_PIXELFORMAT_BGRA32);



  //SDL_Surface  *surface =SDL_CreateRGBSurface(0,w,h,32,0,0,0,0);
  SDL_Surface  *surface =SDL_CreateRGBSurfaceWithFormat(0,img->width,img->height,32 ,SDL_PIXELFORMAT_BGRA32);
  SDL_SetSurfaceBlendMode(surface,SDL_BLENDMODE_NONE);
  SDL_Rect rect=(SDL_Rect){0,0,w,h};
  int bytesPerPixel=4;//mlx user 4 bgra 4 * 8bytes per color

    for (int y = 0; y < h; y++)
    {
      //fast line by line copy , (pitch copy or row copy)
          memcpy(((unsigned char *)surface->pixels) + y*(int)w*bytesPerPixel,
          ((unsigned char *)pixels) + ((y + (int)rect.y)*w + (int)rect.x)*bytesPerPixel,
            (int)w*bytesPerPixel);
    }

  //free the pixel that was copy to surface
   mlx_destroy_mlximage(img);

    if (!surface)
        return ((void *)0);
    SDL_SetColorKey(surface,SDL_TRUE,0xFF000000);
    return (void*)surface;
  }




void	*mlx_xpm_to_image(void *mlx_ptr, char **xpm_data,int *width, int *height)
{
  mlx_image	*img;
  if ((img = mlx_int_parse_xpm(xpm_data,0,mlx_int_static_line)))
    {
      *width = img->width;
      *height = img->height;
    }
    int bits_per_pixel;
    int size_line;
    int w=*width;
    int h=*height;
    char* pixels =mlx_xpm_get_data_addr (img, &bits_per_pixel, &size_line);
    //SDL_Surface  *surface =SDL_CreateRGBSurfaceWithFormatFrom((void*)pixels, img->width,img->height,bits_per_pixel,size_line,SDL_PIXELFORMAT_BGRA32);

  SDL_Surface  *surface =SDL_CreateRGBSurfaceWithFormat(0,img->width,img->height,32 ,SDL_PIXELFORMAT_BGRA32);


  SDL_SetSurfaceBlendMode(surface,SDL_BLENDMODE_NONE);
  SDL_Rect rect=(SDL_Rect){0,0,w,h};
  int bytesPerPixel=4;

    for (int y = 0; y < h; y++)
    {
          memcpy(((unsigned char *)surface->pixels) + y*(int)w*bytesPerPixel,
          ((unsigned char *)pixels) + ((y + (int)rect.y)*w + (int)rect.x)*bytesPerPixel,
          (int)w*bytesPerPixel);
    }

   mlx_destroy_mlximage(img);
    SDL_SetColorKey(surface,SDL_TRUE,0xFF000000);
    return (void*)surface;
}



//****************************************************************************
//STRING
//*****************************************************************************
int	mlx_string_put(void *mlx_ptr, void *win_ptr, int x, int y, int color,char *string)
{
    (void)mlx_ptr;
    (void)win_ptr;

    Uint8  r;
    Uint8  g;
    Uint8  b;


    r = (Uint8)(color>>16) & 0xff;
    g = (Uint8)(color>>8) & 0xff;
    b = (Uint8)(color & 0xff);

    SDL_SetSurfaceColorMod(core->fnt->image,r,g,b);
    font_blit(core->screen,core->fnt,(const char*)string,x,y);

return (1);
}

void	mlx_set_font(void *mlx_ptr, void *win_ptr, char *name)
{
   (void)mlx_ptr;
   (void)win_ptr;
   free_font(core->fnt);
   core->fnt = load_font(name);
}

//****************************************************************************
//EXTRA
//*****************************************************************************

void mlx_joystick_rumble(int duration_ms)
{
  if (core->joystick)
      SDL_JoystickRumble(core->joystick, 0xFFFF, 0xFFFF, (Uint32)duration_ms);
}

void free_mlx(void *mlx_ptr)
{
   (void)mlx_ptr;
}

