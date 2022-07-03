/*
** mlx.h for MinilibX in
**
** Made by Charlie Root
** Login   <ol@epitech.net>
**
** Started on  Mon Jul 31 16:37:50 2000 Charlie Root
** Last update Tue May 15 16:23:28 2007 Olivier Crouzet
** Started on  07/06 17:00:50 2022 Charlie Root

** Port to SDL2 By Luis Santos AKA DJOKER
*/

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minilibx SDL2 Port                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lrosa-do <lrosa-do@student.42lisboa>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/15 18:20:43 by lrosa-do          #+#    #+#             */
/*   Updated: 2022/06/25 09:18:57 by lrosa-do         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MLX_H
#define	MLX_H


typedef enum
{
    KEY_UNKNOWN = 0,

    KEY_RETURN = '\r',
    KEY_ESCAPE = '\x1B',
    KEY_BACKSPACE = '\b',
    KEY_TAB = '\t',
    KEY_SPACE = ' ',
    KEY_EXCLAIM = '!',
    KEY_QUOTEDBL = '"',
    KEY_HASH = '#',
    KEY_PERCENT = '%',
    KEY_DOLLAR = '$',
    KEY_AMPERSAND = '&',
    KEY_QUOTE = '\'',
    KEY_LEFTPAREN = '(',
    KEY_RIGHTPAREN = ')',
    KEY_ASTERISK = '*',
    KEY_PLUS = '+',
    KEY_COMMA = ',',
    KEY_MINUS = '-',
    KEY_PERIOD = '.',
    KEY_SLASH = '/',
    KEY_0 = '0',
    KEY_1 = '1',
    KEY_2 = '2',
    KEY_3 = '3',
    KEY_4 = '4',
    KEY_5 = '5',
    KEY_6 = '6',
    KEY_7 = '7',
    KEY_8 = '8',
    KEY_9 = '9',
    KEY_COLON = ':',
    KEY_SEMICOLON = ';',
    KEY_LESS = '<',
    KEY_EQUALS = '=',
    KEY_GREATER = '>',
    KEY_QUESTION = '?',
    KEY_AT = '@',

    /*
       Skip uppercase letters
     */

    KEY_LEFTBRACKET = '[',
    KEY_BACKSLASH = '\\',
    KEY_RIGHTBRACKET = ']',
    KEY_CARET = '^',
    KEY_UNDERSCORE = '_',
    KEY_BACKQUOTE = '`',
    KEY_a = 'a',
    KEY_b = 'b',
    KEY_c = 'c',
    KEY_d = 'd',
    KEY_e = 'e',
    KEY_f = 'f',
    KEY_g = 'g',
    KEY_h = 'h',
    KEY_i = 'i',
    KEY_j = 'j',
    KEY_k = 'k',
    KEY_l = 'l',
    KEY_m = 'm',
    KEY_n = 'n',
    KEY_o = 'o',
    KEY_p = 'p',
    KEY_q = 'q',
    KEY_r = 'r',
    KEY_s = 's',
    KEY_t = 't',
    KEY_u = 'u',
    KEY_v = 'v',
    KEY_w = 'w',
    KEY_x = 'x',
    KEY_y = 'y',
    KEY_z = 'z'
}KeyCode;


void	*mlx_init();
/*
**  needed before everything else.
**  return (void *)0 if failed
*/

void	mlx_free(void *mlx);

/*
** Basic actions
*/

void	*mlx_new_window(void *mlx_ptr, int size_x, int size_y, char *title);
/*
**  return void *0 if failed
*/
int	mlx_clear_window(void *mlx_ptr, void *win_ptr);
int	mlx_pixel_put(void *mlx_ptr, void *win_ptr, int x, int y, int color);
/*
**  origin for x & y is top left corner of the window
**  y down is positive
**  color is 0x00RRGGBB
*/


/*
** Image stuff
*/

void	*mlx_new_image(void *mlx_ptr,int width,int height);
/*
**  return void *0 if failed
**  obsolete : image2 data is stored using bit planes
**  void	*mlx_new_image2(void *mlx_ptr,int width,int height);
*/
char	*mlx_get_data_addr(void *img_ptr, int *bits_per_pixel,int *size_line, int *endian);
/*
**  endian : 0 = sever X is little endian, 1 = big endian
**  for mlx_new_image2, 2nd arg of mlx_get_data_addr is number_of_planes
*/
int	mlx_put_image_to_window(void *mlx_ptr, void *win_ptr, void *img_ptr, int x, int y);
int	mlx_get_color_value(void *mlx_ptr, int color);


/*
** dealing with Events
*/

int	mlx_mouse_hook (void *win_ptr, int (*funct_ptr)(), void *param);
int	mlx_key_hook (void *win_ptr, int (*funct_ptr)(), void *param);
int	mlx_expose_hook (void *win_ptr, int (*funct_ptr)(), void *param);

int	mlx_loop_hook (void *mlx_ptr, int (*funct_ptr)(), void *param);
int	mlx_loop (void *mlx_ptr);
int mlx_loop_end (void *mlx_ptr);

/*
**  hook funct are called as follow :
**
**   expose_hook(void *param);
**   key_hook(int keycode, void *param);
**   mouse_hook(int button, int x,int y, void *param);
**   loop_hook(void *param);
**
*/


/*
**  Usually asked...
*/

int	mlx_string_put(void *mlx_ptr, void *win_ptr, int x, int y, int color,
		       char *string);
void	mlx_set_font(void *mlx_ptr, void *win_ptr, char *name);
void	*mlx_xpm_to_image(void *mlx_ptr, char **xpm_data,
			  int *width, int *height);
void	*mlx_xpm_file_to_image(void *mlx_ptr, char *filename,
			       int *width, int *height);
int	mlx_destroy_window(void *mlx_ptr, void *win_ptr);

int	mlx_destroy_image(void *mlx_ptr, void *img_ptr);

int	mlx_destroy_display(void *mlx_ptr);

/*
**  generic hook system for all events, and minilibX functions that
**    can be hooked. Some macro and defines from X11/X.h are needed here.
*/

int	mlx_hook(void *win_ptr, int x_event, int x_mask, int (*funct)(), void *param);

int	mlx_do_key_autorepeatoff(void *mlx_ptr);
int	mlx_do_key_autorepeaton(void *mlx_ptr);
int	mlx_do_sync(void *mlx_ptr);

int	mlx_mouse_get_pos(void *mlx_ptr, void *win_ptr, int *x, int *y);
int	mlx_mouse_move(void *mlx_ptr, void *win_ptr, int x, int y);
int	mlx_mouse_hide(void *mlx_ptr, void *win_ptr);
int	mlx_mouse_show(void *mlx_ptr, void *win_ptr);

int	mlx_get_screen_size(void *mlx_ptr, int *sizex, int *sizey);


//extra
void	*mlx_png_file_to_image(void *mlx_ptr, char *file, int *width, int *height);
void mlx_joystick_rumble(int duration_ms);

#endif /* MLX_H */
