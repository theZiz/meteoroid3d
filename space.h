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

#ifndef _SPACE_H
#define _SPACE_H

#include <sparrow3d.h>

#define MAX_SPACE SP_ONE*8
#define MAX_SPACE_INT 8
#define ROT_BUFFER 64

typedef struct sShip *pShip;
typedef struct sShip
{
	Sint32 x, y, z;
	Sint32 dx,dy,dz;
	Sint32 power;
	float rotation[16];
	SDL_Surface* original_texture;
	SDL_Surface* texture;
	spModelPointer mesh;
} tShip;

void update_ship(int steps);
void handle_ship_input(int steps);
void draw_space(Uint16 color);
void draw_map(Uint16 color);
pShip getShip();
int get_score();
int get_lives();
void init_game();
void finish_game();
void init_ship();
void finish_ship();

#endif
