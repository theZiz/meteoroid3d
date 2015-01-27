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
#include <string.h>
#include <sparrow3d.h>
#include "glasses.h"
#include "stereo.h"
#include "matrix.h"

typedef struct sShip *pShip;
typedef struct sShip
{
	Sint32 x, y, z;
	Sint32 dx,dy,dz;
	Sint32 power;
	float rotation[16];
	spModelPointer mesh;
} tShip;

#define ROT_BUFFER 256
tShip ship;
float rotation_buffer[ROT_BUFFER][16];

void update_ship(int steps)
{
	//Calculate d* from power and matrix.z
	ship.dx = spMul(spFloatToFixed(ship.rotation[ 2]),ship.power);
	ship.dy = spMul(spFloatToFixed(ship.rotation[ 6]),ship.power);
	ship.dz = spMul(spFloatToFixed(ship.rotation[10]),ship.power);
	
	ship.x += ship.dx * steps;
	ship.y += ship.dy * steps;
	ship.z += ship.dz * steps;
}

void init_ship()
{
	ship.mesh = spMeshLoadObjSize("./data/ship.obj", spLoadSurface( "./data/ship_texture.png" ), 65535, SP_ONE/6);
	ship.x = 0;
	ship.y = 0;
	ship.z = 0;
	memset(ship.rotation,0,sizeof(float)*16);
	ship.rotation[0] = ship.rotation[5] = ship.rotation[10] = ship.rotation[15] = 1.0f;
	int i;
	for (i = 0; i < ROT_BUFFER; i++)
	{
		memset(rotation_buffer[i],0,sizeof(float)*16);
		rotation_buffer[i][0] = rotation_buffer[i][5] = rotation_buffer[i][10] = rotation_buffer[i][15] = 1.0f;
	}
}

void draw_ship(Uint16 color)
{
	ship.mesh->color = color;
	Sint32 neg_rotation[16];
	Sint32 rotation[16];
	int x,y;
	for (x = 0; x < 4; x++)
		for (y = 0; y < 4; y++)
		{
			neg_rotation[x+y*4] = spFloatToFixed(rotation_buffer[0][y+x*4]);
			rotation[x+y*4] = spFloatToFixed(ship.rotation[x+y*4]);
		}
	spMulMatrix(neg_rotation);
	spMulMatrix(rotation);
	spMesh3DwithPos( ship.x, ship.y, ship.z, ship.mesh, 0);
}

void draw_test(int eye,Uint16 color,spFontPointer font)
{
	spIdentity();
	spResetZBuffer();
	spClearTarget( 0 );

	int px,py,pz,w;
	spProjectPoint3D(spFloatToFixed(-0.7f), spFloatToFixed(0.4f), spFloatToFixed(-Z0+3.2f),&px,&py,&pz,&w,1);
	char buffer[256];
	sprintf(buffer,"Power: %i\n",spFixedToInt(ship.power));
	spFontDraw(px,py-font->maxheight/2,pz,buffer,font);
	
	spSetAlphaTest(0);
	spTranslate(-ship.x,-ship.y,-ship.z+spFloatToFixed(-1.5f));
	draw_ship(color);
	spSetAlphaTest(1);
}

int calc_test(Uint32 steps)
{
	if (spGetInput()->button[SP_BUTTON_START])
		return 1;
	update_ship(steps);
	int i,j;
	for (i = 0; i < steps; i++)
	{
		if (spGetInput()->axis[0])
			rotateY(ship.rotation,spFixedToFloat(128*-spGetInput()->axis[0]));
		if (spGetInput()->axis[1])
			rotateX(ship.rotation,spFixedToFloat(128*spGetInput()->axis[1]));
		for (j = 0; j < ROT_BUFFER-1; j++)
			memcpy(rotation_buffer[j],rotation_buffer[j+1],sizeof(float)*16);
		memcpy(rotation_buffer[j],ship.rotation,sizeof(float)*16);
	}
	return 0;
}

int main(int argc, char **argv)
{
	spInitCore();
	init_stereo();
	show_glasses();
	init_ship();
	spSetZSet(1);
	spSetZTest(1);
	stereo_loop(draw_test,calc_test);
	spMeshDelete(ship.mesh);
	spQuitCore();
	return 0;
}
