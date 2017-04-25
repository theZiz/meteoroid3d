 /* This file is part of meteoroid3d.
  * meteoroid3d is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * meteoroid3d is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with meteoroid3d.  If not, see <http://www.gnu.org/licenses/>
  * 
  * For feedback and questions about my Files and Projects please mail me,
  * Alexander Matthes (Ziz) , zizsdl_at_googlemail.com */

#ifndef _STEREO_H
#define _STEREO_H

#include <sparrow3d.h>

#define DISTANCE 0.06f
#define Z0 5.0f
#define MIN_Z (-SP_ONE*3/2)
#define MAX_Z (-SP_ONE*10)

void resize( Uint16 w, Uint16 h );
void init_stereo();
int stereo_loop ( void ( *draw )( int, Uint16, spFontPointer ), int ( *calc )( Uint32 steps ));
void set_color(int lr,int lg,int lb,int rr,int rg,int rb);
int get_brightness();
void set_brightness(int b);
void set_flipped(int f);
int get_flipped();
void set_alltime(int value);
int get_alltime();
int get_volume();
void set_volume(int v);
int get_flip_direction();
void set_flip_direction(int v);
void save_stereo();
#endif
