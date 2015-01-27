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

#include "space.h"
#include "matrix.h"
#include "stereo.h"

tShip ship;
float rotation_buffer[ROT_BUFFER][16];

void update_texture(int horizontal,int vertical)
{
	spSelectRenderTarget(ship.texture);
	spSetZSet(0);
	spSetZTest(0);
	spSetAlphaTest(0);
	spBlitSurface(ship.texture->w/2,ship.texture->h/2,0,ship.original_texture);
	
	int color;
	color = 255*ship.power/100 >> SP_ACCURACY;
	spFloodFill(144*ship.texture->w/512,164*ship.texture->w/512,spGetRGB(color,color,color));
		
	if (horizontal > 0)
		color = 196;
	else
		color = 0;
	if (vertical > 0)
		color += 63;
	spFloodFill( 51*ship.texture->w/512,218*ship.texture->w/512,spGetRGB(color,color,color));
	
	if (horizontal)
		color = 63;
	else
		color = 0;
	if (vertical < 0)
		color += 196;
	spFloodFill(144*ship.texture->w/512, 56*ship.texture->w/512,spGetRGB(color,color,color));

	if (horizontal < 0)
		color = 196;
	else
		color = 0;
	if (vertical > 0)
		color += 63;
	spFloodFill(236*ship.texture->w/512,218*ship.texture->w/512,spGetRGB(color,color,color));
	spSetAlphaTest(1);
}

pShip getShip()
{
	return &ship;
}

void handle_ship_input(int steps)
{
	if (spGetInput()->button[SP_BUTTON_UP])
	{
		ship.power += steps*1024;
		if (ship.power > SP_ONE*100)
			ship.power = SP_ONE*100;
	}
	if (spGetInput()->button[SP_BUTTON_DOWN])
	{
		ship.power -= steps*1024;
		if (ship.power < 0)
			ship.power = 0;
	}
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
	update_texture(spGetInput()->axis[0],spGetInput()->axis[1]);
}

void update_ship(int steps)
{
	//Calculate d* from power and matrix.z
	ship.dx = spMul( -spFloatToFixed(ship.rotation[ 8]),ship.power)/1024;
	ship.dy = spMul( -spFloatToFixed(ship.rotation[ 9]),ship.power)/1024;
	ship.dz = spMul( -spFloatToFixed(ship.rotation[10]),ship.power)/1024;
	
	ship.x += ship.dx * steps;
	ship.y += ship.dy * steps;
	ship.z += ship.dz * steps;
	while (ship.x < -MAX_SPACE)
		ship.x += 2*MAX_SPACE;
	while (ship.x >= MAX_SPACE)
		ship.x -= 2*MAX_SPACE;
	while (ship.y < -MAX_SPACE)
		ship.y += 2*MAX_SPACE;
	while (ship.y >= MAX_SPACE)
		ship.y -= 2*MAX_SPACE;
	while (ship.z < -MAX_SPACE)
		ship.z += 2*MAX_SPACE;
	while (ship.z >= MAX_SPACE)
		ship.z -= 2*MAX_SPACE;
}

void init_ship()
{
	ship.original_texture = spLoadSurface( "./data/ship_texture.png" );
	ship.texture = spCreateSurface(ship.original_texture->w,ship.original_texture->h);
	update_texture(0,0);
	ship.mesh = spMeshLoadObjSize("./data/ship.obj", ship.texture, 65535, SP_ONE/6);
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

void draw_space(Uint16 color)
{
	spPushModelView();
	spTranslate(-ship.x,-ship.y,-ship.z+spFloatToFixed(-1.5f));
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
	spTranslate(ship.x, ship.y, ship.z);
	spMulMatrix(neg_rotation);
	spMulMatrix(rotation);
	spMesh3D( ship.mesh, 0);
	spPopModelView();
}

void draw_map(Uint16 color)
{
	spPushModelView();
	spTranslate(spFloatToFixed(-0.55f), spFloatToFixed(-0.25f), spFloatToFixed(-Z0+3.2f));
	Sint32 rotation[16];
	int x,y;
	spRotateY( SP_PI/11);
	spRotateX(-SP_PI/22);
	for (x = 0; x < 4; x++)
		for (y = 0; y < 4; y++)
			rotation[x+y*4] = spFloatToFixed(ship.rotation[y+x*4]);
	spMulMatrix(rotation);
	Uint16 dark = spGetRGB(
		spGetRFromColor(color)/2,
		spGetGFromColor(color)/2,
		spGetBFromColor(color)/2);
	
	Sint32 e = SP_ONE/12;
	//left
	spLine3D( -e, -e, -e, -e, -e,  e, dark);
	spLine3D( -e, -e,  e, -e,  e,  e, dark);
	spLine3D( -e,  e,  e, -e,  e, -e, dark);
	spLine3D( -e,  e, -e, -e, -e, -e, dark);
	//right
	spLine3D(  e, -e, -e,  e, -e,  e, dark);
	spLine3D(  e, -e,  e,  e,  e,  e, dark);
	spLine3D(  e,  e,  e,  e,  e, -e, dark);
	spLine3D(  e,  e, -e,  e, -e, -e, dark);
	//connect
	spLine3D( -e, -e, -e,  e, -e, -e, dark);
	spLine3D( -e, -e,  e,  e, -e,  e, dark);
	spLine3D( -e,  e,  e,  e,  e,  e, dark);
	spLine3D( -e,  e, -e,  e,  e, -e, dark);
	
	Sint32 X = spMul(ship.x/MAX_SPACE_INT,e);
	Sint32 Y = spMul(ship.y/MAX_SPACE_INT,e);
	Sint32 Z = spMul(ship.z/MAX_SPACE_INT,e);
	spEllipse3D(X,Y,Z,SP_ONE/128,SP_ONE/128,color);
	spLine3D( -e,  Y,  Z,  e,  Y,  Z, color);
	spLine3D(  X, -e,  Z,  X,  e,  Z, color);
	spLine3D(  X,  Y, -e,  X,  Y,  e, color);
	
	/*spLine3D( X, Y, Z,
		 X-spFloatToFixed(ship.rotation[ 8])/8,
		 Y-spFloatToFixed(ship.rotation[ 9])/8,
		 Z-spFloatToFixed(ship.rotation[10])/8, color);*/
	
	spPopModelView();
}
