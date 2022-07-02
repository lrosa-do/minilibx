#include <stdio.h>
#include <stdlib.h>
#include	"mlx.h"


#define	WIN1_SX		640
#define	WIN1_SY		480
#define	IM1_SX		42
#define	IM1_SY		42
#define	IM3_SX		242
#define	IM3_SY		242

void	*mlx;
void	*win1;
void    *win2;
void    *win3;
void    *im1;
void	*im2;
void	*im3;
void	*im4;
int	bpp1;
int	bpp2;
int	bpp3;
int	bpp4;
int	sl1;
int	sl2;
int	sl3;
int	sl4;
int	endian1;
int	endian2;
int	endian3;
int	endian4;
char	*data1;
char	*data2;
char	*data3;
char	*data4;
int	xpm1_x;
int	xpm1_y;

int	local_endian;


typedef struct s_bitmap
{
	void			*buffer;
	char			*addr;
	int				bpp;
	int				line_len;
	int				endian;
	int				width;
	int				height;
}t_bitmap;



void	*mlx;
void	*mlx_win;
t_bitmap    *screen;
int mouse_x;
int mouse_y;


t_bitmap    *images[10];


int hook_keys(int key, void* s)
{
    (void)s;
    printf(" %d  \n",key);
    return 0;
}


int hook_mouse(int x, int y, int button,void* s)
{
    (void)s;
    printf(" %d %d %d  \n",x,y,button);
    return 0;
}



int hook_loop(void* s)
{
    (void)s;

    mlx_clear_window(mlx,win1);

	mlx_put_image_to_window(mlx, mlx_win, images[0]->buffer, 10,10);

	mlx_put_image_to_window(mlx, mlx_win, images[1]->buffer, 50,10);

    mlx_string_put(0,0,100,100,0xffff0000,"Luis Santos");

    return 0;
}


t_bitmap	*mlx_load_bitmap(void *mlx, char *filename)
{
	t_bitmap	*bitmap;

	bitmap = (t_bitmap *)malloc(sizeof(t_bitmap));
	if (!bitmap)
		return (NULL);
	bitmap->buffer = mlx_xpm_file_to_image(mlx, filename, &bitmap->width, &bitmap->height);
	if (!bitmap->buffer)
        return ((void *)0);
	bitmap->addr = mlx_get_data_addr(bitmap->buffer, &bitmap->bpp,&bitmap->line_len, &bitmap->endian);
	printf("Load  %d %d %d \n", bitmap->bpp, bitmap->line_len, bitmap->endian);
	return (bitmap);
}


int	main()
{
  int	a;

  printf("MinilibX Test Program\n");
  a = 0x11223344;
  if (((unsigned char *)&a)[0] == 0x11)
    local_endian = 1;
  else
    local_endian = 0;
  printf(" => Local Endian : %d\n",local_endian);

  printf(" => Connection ...");
  if (!(mlx = mlx_init()))
    {
      printf(" !! KO !!\n");
      exit(1);
    }

  printf(" => Window1 %dx%d \"Title 1\" ...",WIN1_SX,WIN1_SY);
  if (!(win1 = mlx_new_window(mlx,WIN1_SX,WIN1_SY,"Title1")))
    {
      printf(" !! KO !!\n");
      exit(1);
    }
  printf("OK\n");

 images[0]  = mlx_load_bitmap(mlx,"images/gem_0.xpm");
images[1]  = mlx_load_bitmap(mlx,"images/Steps.xpm");

mlx_loop_hook(mlx, &hook_loop,0);
mlx_mouse_hook(mlx,hook_mouse,0);
mlx_key_hook(mlx,hook_keys,0);

  mlx_loop(mlx);

  printf("Free\n");
  mlx_free(mlx);

  printf("Exit\n");
}
