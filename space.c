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
#include <string.h>
#include <stdlib.h>

#define BUCKET 5000

tShip ship;
float rotation_buffer[ROT_BUFFER][16];
spParticleBunchPointer particle_bunch = NULL;
spModelPointer middle_mesh;
#define STONE_COUNT 32
spModelPointer stone_mesh[STONE_COUNT*3];
Uint16 mom_color;
spSound *laser,*explosion;

typedef struct sStone *pStone;
typedef struct sStone
{
	Sint32 x, y, z;
	Sint32 dx,dy,dz;
	Sint32 rot_x,rot_y;
	int mesh;
	int size;
	pStone next;
} tStone;

typedef struct sBullet *pBullet;
typedef struct sBullet
{
	Sint32 x, y, z;
	Sint32 dx,dy,dz;
	Sint32 age;
	pBullet next;
} tBullet;

pStone first_stone = NULL;
pBullet first_bullet = NULL;
int score = 0;
int lives = 0;
int shake = 0;

int get_score()
{
	return score;
}

int get_lives()
{
	return lives;
}

int power_feedback( spParticleBunchPointer bunch, Sint32 action, Sint32 extra_data)
{
	int i;
	if (action == SP_PARTICLE_DRAW)
	{
		int* ptr = (int*)(&bunch);
		int r = spGetRFromColor(mom_color);
		int g = spGetGFromColor(mom_color);
		int b = spGetBFromColor(mom_color);
		for (i = 0; i < bunch->count; i++)
		{
			if (bunch->particle[i].status >= 64)
				continue;
			spSetAlphaPattern(128-bunch->particle[i].status*2,*ptr);
			Uint16 color = spGetRGB(r-bunch->particle[i].status*r/128,g-bunch->particle[i].status*g/128,b-bunch->particle[i].status*b/128);
			spEllipse3D(bunch->particle[i].x-ship.x,bunch->particle[i].y-ship.y,bunch->particle[i].z-ship.z,bunch->particle[i].data.reserved[0],bunch->particle[i].data.reserved[0],color);
		}
		spDeactivatePattern();
	}
	else
	if (action == SP_PARTICLE_UPDATE)
	{
		for (i = 0; i < bunch->count; i++)
			bunch->particle[i].status += extra_data;
		if (bunch->age >= 64)
			return 1;
	}
	return 0;
}

int meteroid_feedback( spParticleBunchPointer bunch, Sint32 action, Sint32 extra_data)
{
	int i;
	if (action == SP_PARTICLE_DRAW)
	{
		int* ptr = (int*)(&bunch);
		int r = spGetRFromColor(mom_color);
		int g = spGetGFromColor(mom_color);
		int b = spGetBFromColor(mom_color);
		for (i = 0; i < bunch->count; i++)
		{
			if (bunch->particle[i].status >= 2000)
				continue;
			//spSetAlphaPattern(500-bunch->particle[i].status,*ptr);
			Uint16 color = spGetRGB(r-bunch->particle[i].status*r/2000,g-bunch->particle[i].status*g/2000,b-bunch->particle[i].status*b/2000);
			spEllipse3D(bunch->particle[i].x-ship.x,bunch->particle[i].y-ship.y,bunch->particle[i].z-ship.z,SP_ONE/64,SP_ONE/64,color);
		}
		//spDeactivatePattern();
	}
	else
	if (action == SP_PARTICLE_UPDATE)
	{
		for (i = 0; i < bunch->count; i++)
		{
			bunch->particle[i].status += extra_data;
			bunch->particle[i].x += bunch->particle[i].dx * extra_data;
			bunch->particle[i].y += bunch->particle[i].dy * extra_data;
			bunch->particle[i].z += bunch->particle[i].dz * extra_data;
			while (bunch->particle[i].x < -MAX_SPACE)
				bunch->particle[i].x += 2*MAX_SPACE;
			while (bunch->particle[i].x >= MAX_SPACE)
				bunch->particle[i].x -= 2*MAX_SPACE;
			while (bunch->particle[i].y < -MAX_SPACE)
				bunch->particle[i].y += 2*MAX_SPACE;
			while (bunch->particle[i].y >= MAX_SPACE)
				bunch->particle[i].y -= 2*MAX_SPACE;
			while (bunch->particle[i].z < -MAX_SPACE)
				bunch->particle[i].z += 2*MAX_SPACE;
			while (bunch->particle[i].z >= MAX_SPACE)
				bunch->particle[i].z -= 2*MAX_SPACE;
		}
		if (bunch->age >= 2000)
			return 1;
	}
	return 0;
}

void update_texture(int horizontal,int vertical,int steps)
{
	spSelectRenderTarget(ship.texture);
	spSetZSet(0);
	spSetZTest(0);
	spSetAlphaTest(0);
	spBlitSurface(ship.texture->w/2,ship.texture->h/2,0,ship.original_texture);
	
	int big_color;
	big_color = 255*ship.power/100 >> SP_ACCURACY;
	spFloodFill(144*ship.texture->w/512,164*ship.texture->w/512,spGetRGB(big_color,big_color,big_color));
	
	int left_color;
	if (horizontal > 0)
		left_color = 196;
	else
		left_color = 0;
	if (vertical > 0)
		left_color += 63;
	spFloodFill( 51*ship.texture->w/512,218*ship.texture->w/512,spGetRGB(left_color,left_color,left_color));
	
	int up_color;
	if (horizontal)
		up_color = 63;
	else
		up_color = 0;
	if (vertical < 0)
		up_color += 196;
	spFloodFill(144*ship.texture->w/512, 56*ship.texture->w/512,spGetRGB(up_color,up_color,up_color));

	int right_color;
	if (horizontal < 0)
		right_color = 196;
	else
		right_color = 0;
	if (vertical > 0)
		right_color += 63;
	spFloodFill(236*ship.texture->w/512,218*ship.texture->w/512,spGetRGB(right_color,right_color,right_color));
	spSetAlphaTest(1);
	
	if (steps)
	{
		int i;
		int points[4] = {0,0,0,0};
		for (i = 0; i < steps; i++)
		{
			if ((rand() & 511) < big_color)
				points[0]++;
			if ((rand() & 1023) < left_color)
				points[1]++;
			if ((rand() & 1023) < up_color)
				points[2]++;
			if ((rand() & 1023) < right_color)
				points[3]++;
		}
		int sum = points[0] + points[1] + points[2] + points[3];
		if (sum == 0)
			return;
		Sint32 rotation[16];
		for (i = 0; i < 16; i++)
			rotation[i] = spFloatToFixed(ship.rotation[i]);
		spParticleBunchPointer bunch = spParticleCreate(sum,power_feedback,&particle_bunch);
		int j;
		i = 0;
		int sum_part = 0;
		for (j = 0; j < 4; j++)
		{
			sum_part += points[j];
			Sint32 x,y,z;
			switch (j)
			{
				case 0:
					x = ship.mesh->texPoint[69].x;
					y = ship.mesh->texPoint[69].y;
					z = ship.mesh->texPoint[69].z;
					break;
				case 1:
					x = ship.mesh->texPoint[61].x;
					y = ship.mesh->texPoint[61].y;
					z = ship.mesh->texPoint[61].z;
					break;
				case 2:
					x = ship.mesh->texPoint[49].x;
					y = ship.mesh->texPoint[49].y;
					z = ship.mesh->texPoint[49].z;
					break;
				case 3:
					x = ship.mesh->texPoint[55].x;
					y = ship.mesh->texPoint[55].y;
					z = ship.mesh->texPoint[55].z;
					break;
				
			}
			int r = SP_ONE/64;
			if (j == 0)
				r = SP_ONE/32-SP_ONE/128;
			for (; i < sum_part; i++)
			{
				int rz = -rand()%steps;
				bunch->particle[i].status = rz;
				rz *= ship.power/16384;
				bunch->particle[i].data.reserved[0] = r;
				bunch->particle[i].x = ship.x +
					+ spMul(rotation[ 0],x)
					+ spMul(rotation[ 4],y)
					+ spMul(rotation[ 8],z+rz);
				bunch->particle[i].y = ship.y
					+ spMul(rotation[ 1],x)
					+ spMul(rotation[ 5],y)
					+ spMul(rotation[ 9],z+rz);
				bunch->particle[i].z = ship.z
					+ spMul(rotation[ 2],x)
					+ spMul(rotation[ 6],y)
					+ spMul(rotation[10],z+rz);
			}
		}
	}
}

pShip getShip()
{
	return &ship;
}

int middle_rotation = 0;
int rotation_acceleration = 0;

void shoot()
{
	spSoundPlay(laser,-1,0,0,0);
	pBullet bullet = (pBullet)malloc(sizeof(tBullet));
	bullet->next = first_bullet;
	first_bullet = bullet;
	bullet->x = ship.x-spFloatToFixed(ship.rotation[ 8])/4;
	bullet->y = ship.y-spFloatToFixed(ship.rotation[ 9])/4;
	bullet->z = ship.z-spFloatToFixed(ship.rotation[10])/4;
	bullet->age = 0;
	bullet->dx = -spFloatToFixed(ship.rotation[ 8])/64;
	bullet->dy = -spFloatToFixed(ship.rotation[ 9])/64;
	bullet->dz = -spFloatToFixed(ship.rotation[10])/64;
}

int shoot_wait = 100;
int shoot_next = 100;

void handle_ship_input(int steps)
{
	int i,j;
	if (spGetInput()->button[SP_BUTTON_LEFT] || spGetInput()->button[SP_BUTTON_RIGHT])
	{
		for (i = 0; i < steps; i++)
		{
			if (shoot_wait >= shoot_next)
			{
				shoot();
				shoot_wait -= shoot_next;
				shoot_next += 30;
			}
			shoot_wait++;
		}
	}
	else
	{
		shoot_next = 100;
		shoot_wait = 100;
	}
	if (spGetInput()->button[SP_BUTTON_UP])
	{
		ship.power += steps*4096;
		if (ship.power > SP_ONE*100)
			ship.power = SP_ONE*100;
	}
	if (spGetInput()->button[SP_BUTTON_DOWN])
	{
		ship.power -= steps*4096;
		if (ship.power < 0)
			ship.power = 0;
	}
	for (i = 0; i < steps; i++)
	{
		int input = 0;
		if (spGetInput()->button[SP_BUTTON_L])
		{
			rotateZ(ship.rotation,spFixedToFloat(rotation_acceleration));
			if (rotation_acceleration < 256)
				rotation_acceleration++;
			input = 1;
		}
		if (spGetInput()->button[SP_BUTTON_R])
		{
			rotateZ(ship.rotation,-spFixedToFloat(rotation_acceleration));
			if (rotation_acceleration < 256)
				rotation_acceleration++;
			input = 1;
		}
		if (spGetInput()->analog_axis[0])
		{
			rotateY(ship.rotation,spFixedToFloat(spMul(rotation_acceleration,-spGetInput()->analog_axis[0])));
			if (rotation_acceleration < 256)
				rotation_acceleration++;
			input = 1;
		}
		if (spGetInput()->axis[1])
		{
			if (get_flip_direction())
				rotateX(ship.rotation,spFixedToFloat(spMul(rotation_acceleration,-spGetInput()->analog_axis[1])));
			else
				rotateX(ship.rotation,spFixedToFloat(spMul(rotation_acceleration, spGetInput()->analog_axis[1])));
			if (rotation_acceleration < 256)
				rotation_acceleration++;
			input = 1;
		}
		if (input == 0)
			rotation_acceleration = 0;
		for (j = 0; j < ROT_BUFFER-1; j++)
			memcpy(rotation_buffer[j],rotation_buffer[j+1],sizeof(float)*16);
		/*printf("%f\n",
			rotation_buffer[0][ 0]*rotation_buffer[0][ 5]*rotation_buffer[0][10] +
			rotation_buffer[0][ 1]*rotation_buffer[0][ 6]*rotation_buffer[0][ 8] +
			rotation_buffer[0][ 2]*rotation_buffer[0][ 4]*rotation_buffer[0][ 9] -
			rotation_buffer[0][ 2]*rotation_buffer[0][ 5]*rotation_buffer[0][ 8] -
			rotation_buffer[0][ 6]*rotation_buffer[0][ 9]*rotation_buffer[0][ 0] -
			rotation_buffer[0][10]*rotation_buffer[0][ 1]*rotation_buffer[0][ 4] );*/
		memcpy(rotation_buffer[j],ship.rotation,sizeof(float)*16);
	}
	update_texture(spGetInput()->axis[0],spGetInput()->axis[1],steps);
	spParticleUpdate(&particle_bunch,steps);
}

int stone_bucket = BUCKET;

void add_stone(Sint32 x,Sint32 y,Sint32 z,int size)
{
	pStone stone = (pStone)malloc(sizeof(tStone));
			stone->next = first_stone;
			first_stone = stone;
			stone->dx = (rand() & 255)-128;
			stone->dy = (rand() & 255)-128;
			stone->dz = (rand() & 255)-128;
			stone->x = x + stone->dx*10;
			stone->y = y + stone->dy*10;
			stone->z = z + stone->dz*10;
			stone->size = size;
			stone->mesh = rand()%STONE_COUNT;
}

void wums(Sint32 x,Sint32 y,Sint32 z,Sint32 count,int r)
{
	spParticleBunchPointer bunch = spParticleCreate(count,meteroid_feedback,&particle_bunch);
	int i;
	for (i = 0; i < bunch->count; i++)
	{
		bunch->particle[i].x = x;
		bunch->particle[i].y = y;
		bunch->particle[i].z = z;
		if (r)
		{
			bunch->particle[i].x += rand()%(2*r)-r;
			bunch->particle[i].y += rand()%(2*r)-r;
			bunch->particle[i].z += rand()%(2*r)-r;
		}
		bunch->particle[i].status = 0;
		bunch->particle[i].dx = (rand() & 255)-128;
		bunch->particle[i].dy = (rand() & 255)-128;
		bunch->particle[i].dz = (rand() & 255)-128;
	}	
}

void delete_stone(pStone stone,pStone before)
{
	spSoundPlay(explosion,-1,0,0,0);
	score += (25 << stone->size);
	if (before)
		before->next = stone->next;
	else
		first_stone = stone->next;
	if (stone->size < 2)
	{
		printf("Added\n");
		add_stone(stone->x,stone->y,stone->z,stone->size+1);
		add_stone(stone->x,stone->y,stone->z,stone->size+1);
	}
	free(stone);
}

void update_ship(int steps)
{
	if (shake > 0)
		shake -= steps;
	if (shake < 0)
		shake = 0;
	middle_rotation += steps*8;
	
	//Calculate d* from power and matrix.z
	ship.dx = spMul( -spFloatToFixed(ship.rotation[ 8]),ship.power)/16384;
	ship.dy = spMul( -spFloatToFixed(ship.rotation[ 9]),ship.power)/16384;
	ship.dz = spMul( -spFloatToFixed(ship.rotation[10]),ship.power)/16384;
	
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
		
	//Stones
	int rest_steps = steps;
	while (rest_steps > 0)
	{
		int one = 10;
		rest_steps-=10;
		if (rest_steps < 0)
		{
			one += rest_steps;
			rest_steps = 0;
		}
	
		stone_bucket += one;
		while (stone_bucket > BUCKET)
		{
			stone_bucket -= BUCKET;
			add_stone(0,0,0,rand()%3);
		}
		pStone stone = first_stone;
		pStone before = NULL;
		while (stone)
		{
			stone->x += stone->dx * one;
			stone->y += stone->dy * one;
			stone->z += stone->dz * one;
			stone->rot_x += stone->dx * one;
			stone->rot_y += stone->dy * one;
			while (stone->x < -MAX_SPACE)
				stone->x += 2*MAX_SPACE;
			while (stone->x >= MAX_SPACE)
				stone->x -= 2*MAX_SPACE;
			while (stone->y < -MAX_SPACE)
				stone->y += 2*MAX_SPACE;
			while (stone->y >= MAX_SPACE)
				stone->y -= 2*MAX_SPACE;
			while (stone->z < -MAX_SPACE)
				stone->z += 2*MAX_SPACE;
			while (stone->z >= MAX_SPACE)
				stone->z -= 2*MAX_SPACE;
			
			Sint32 dx = stone->x-ship.x;
			Sint32 dy = stone->y-ship.y;
			Sint32 dz = stone->z-ship.z;
			Sint32 r = SP_ONE >> stone->size+1;
			pStone next = stone->next;
			if (shake == 0 && spMul(dx,dx)+spMul(dy,dy)+spMul(dz,dz) < spMul(r+spFloatToFixed(0.15f),r+spFloatToFixed(0.15f)))
			{
				printf("Wums of %i at you!\n",stone->size);
				wums(stone->x,stone->y,stone->z,1 << 8-stone->size,r);
				delete_stone(stone,before);
				lives--;
				shake = 500;
			}
			else
				before = stone;
			stone = next;
		}
		
		pBullet bullet = first_bullet;
		pBullet previous = NULL;
		while (bullet)
		{
			bullet->age += one;
					
			bullet->x += bullet->dx * one;
			bullet->y += bullet->dy * one;
			bullet->z += bullet->dz * one;

			while (bullet->x < -MAX_SPACE)
				bullet->x += 2*MAX_SPACE;
			while (bullet->x >= MAX_SPACE)
				bullet->x -= 2*MAX_SPACE;
			while (bullet->y < -MAX_SPACE)
				bullet->y += 2*MAX_SPACE;
			while (bullet->y >= MAX_SPACE)
				bullet->y -= 2*MAX_SPACE;
			while (bullet->z < -MAX_SPACE)
				bullet->z += 2*MAX_SPACE;
			while (bullet->z >= MAX_SPACE)
				bullet->z -= 2*MAX_SPACE;
				
			int kill_bullet = 0;
			
			stone = first_stone;
			before = NULL;
			
			if (spMul(bullet->x,bullet->x)+spMul(bullet->y,bullet->y)+spMul(bullet->z,bullet->z) < spMul(spFloatToFixed(0.3f),spFloatToFixed(0.3f)))
			{
				kill_bullet = 1;
				wums(bullet->x,bullet->y,bullet->z,16,0);
			}
			else
			{
				while (stone)
				{	
					Sint32 dx = stone->x-bullet->x;
					Sint32 dy = stone->y-bullet->y;
					Sint32 dz = stone->z-bullet->z;
					Sint32 r = SP_ONE >> stone->size+1;
					if (spMul(dx,dx)+spMul(dy,dy)+spMul(dz,dz) < spMul(r,r))
					{
						//Wums
						printf("Wums of %i\n",stone->size);
						wums(stone->x,stone->y,stone->z,1 << 8-stone->size,r);
						delete_stone(stone,before);
						kill_bullet = 1;
						break;
					}
					before = stone;
					stone = stone->next;
				}
			}
			if (kill_bullet || bullet->age > 900)
			{
				if (previous)
					previous->next = bullet->next;
				else
					first_bullet = bullet->next;
				pBullet next = bullet->next;
				free(bullet);
				bullet = next;
				continue;
			}
			previous = bullet;	
			bullet = bullet->next;
		}
	}
}


void draw_space(Uint16 color)
{
	if (shake)
		spTranslate(rand() & 2047,rand() & 2047,0);
	Uint16 dark = spGetRGB(
		spGetRFromColor(color)/2,
		spGetGFromColor(color)/2,
		spGetBFromColor(color)/2);
	mom_color = color;
	spPushModelView();
	spTranslate(-ship.x,-ship.y,-ship.z+spFloatToFixed(-2.0f));
	ship.mesh->color = color;
	Sint32 neg_rotation[16];
	Sint32 rotation[16];
	int x,y,z;
	for (x = 0; x < 4; x++)
		for (y = 0; y < 4; y++)
		{
			neg_rotation[x+y*4] = spFloatToFixed(rotation_buffer[0][y+x*4]);
			rotation[x+y*4] = spFloatToFixed(ship.rotation[x+y*4]);
		}
	spTranslate(ship.x, ship.y, ship.z);
	spTranslate(0, -SP_ONE/5, 0);
	spRotateX(SP_PI/16);
	spMulMatrix(neg_rotation);

	spSetZTest(0);
	spSetZSet(0);
	for (x = -MAX_SPACE; x <= MAX_SPACE; x+=SP_ONE)
		for (y = -MAX_SPACE; y <= MAX_SPACE; y+=SP_ONE)
			for (z = -MAX_SPACE; z <= MAX_SPACE; z+=SP_ONE)
			{
				if ( (abs(x) == MAX_SPACE) && (abs(y) == MAX_SPACE) && (abs(z) == MAX_SPACE))
					spEllipse3D(x-ship.x,y-ship.y,z-ship.z,SP_ONE/8,SP_ONE/8,dark);
				else
				if (spMax(spMax(abs(x),abs(y)),abs(z)) == MAX_SPACE)
					spEllipse3D(x-ship.x,y-ship.y,z-ship.z,SP_ONE/18,SP_ONE/18,dark);
			}


	spSetZSet(1);
	spParticleDraw(particle_bunch);
	spSetZTest(1);
	spPushModelView();
	spTranslate(-ship.x, -ship.y, -ship.z);
	spRotateX(middle_rotation);
	spRotateY(spMul(middle_rotation,spSin(middle_rotation)));
	spRotateZ(spMul(middle_rotation,spCos(middle_rotation)));
	middle_mesh->color = color;
	spSetLightPosition( 0, 0, 0, 0);
	spUpdateLight( 0 );
	spMesh3D(middle_mesh,0);
	spPopModelView();
	
	for (x = 0; x < STONE_COUNT*3; x++)
		stone_mesh[x]->color = color;
		
	pStone stone = first_stone;
	int hit = 1;
	Sint32 bx = spFloatToFixed(ship.rotation[ 8]);
	Sint32 by = spFloatToFixed(ship.rotation[ 9]);
	Sint32 bz = spFloatToFixed(ship.rotation[10]);
	while (stone)
	{
		spPushModelView();
		spTranslate(stone->x-ship.x,stone->y-ship.y,stone->z-ship.z);
		spRotateX( stone->rot_x );
		spRotateY( stone->rot_y );
		spMesh3D(stone_mesh[stone->size*STONE_COUNT+stone->mesh],0);
		spPopModelView();
		//Would I hit?
		if (hit == 1)
		{
			Sint32 cx = stone->x-ship.x;
			Sint32 cy = stone->y-ship.y;
			Sint32 cz = stone->z-ship.z;
			Sint32 g = spMul(cx,bx) + spMul(cy,by) + spMul(cz,bz);
			Sint32 dx = -cx + spMul(bx,g);
			Sint32 dy = -cy + spMul(by,g);
			Sint32 dz = -cz + spMul(bz,g);
			Sint32 d2 = spSquare(dx) + spSquare(dy) + spSquare(dz);
			if (g < 0 && d2 < (SP_ONE >> stone->size*2+2))
				hit = 3;
		}
		stone = stone->next;
	}
	pBullet bullet = first_bullet;
	while (bullet)
	{
		spLine3D(
			bullet->x-ship.x,
			bullet->y-ship.y,
			bullet->z-ship.z,
			bullet->x-bullet->dx*16-ship.x,
			bullet->y-bullet->dy*16-ship.y,
			bullet->z-bullet->dz*16-ship.z,color);
		bullet = bullet->next;
	}
	spMulMatrix(rotation);
	int px,py,pz,w;
	spProjectPoint3D(0,0,-SP_ONE*12,&px,&py,&pz,&w,1);
	w = spFixedToInt(spGetSizeFactor()*10);
	spLine(px-w/2,py,pz,px-w,py,pz,color);
	spLine(px+w/2,py,pz,px+w,py,pz,color);
	spLine(px,py-w/2,pz,px,py-w,pz,color);
	spLine(px,py+w/2,pz,px,py+w,pz,color);
	spEllipseBorder(px,py,pz,w*8/10,w*8/10,hit,hit,color);
	spMesh3D( ship.mesh, 0);
	spPopModelView();
}

void draw_map(Uint16 color)
{
	spSetZTest(0);
	spSetZSet(0);
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
	
	spRectangle3D(0,0,0,SP_ONE/64,SP_ONE/64,color);
	
	/*spLine3D( X, Y, Z,
		 X-spFloatToFixed(ship.rotation[ 8])/8,
		 Y-spFloatToFixed(ship.rotation[ 9])/8,
		 Z-spFloatToFixed(ship.rotation[10])/8, color);*/
	
	pStone stone = first_stone;
	while (stone)
	{
		X = spMul(stone->x/MAX_SPACE_INT,e);
		Y = spMul(stone->y/MAX_SPACE_INT,e);
		Z = spMul(stone->z/MAX_SPACE_INT,e);
		spEllipse3D(X,Y,Z,SP_ONE/256,SP_ONE/256,color);
		stone = stone->next;
	}

	spPopModelView();
	spSetZTest(1);
	spSetZSet(1);
}

void init_game()
{
	score = 0;
	lives = 3;
	shake = 0;
	stone_bucket = BUCKET;
	update_texture(0,0,0);
	ship.x = 0;
	ship.y = 0;
	ship.z = MAX_SPACE/2;
	ship.power = 0;
	memset(ship.rotation,0,sizeof(float)*16);
	ship.rotation[0] = ship.rotation[5] = ship.rotation[10] = ship.rotation[15] = 1.0f;
	int i;
	for (i = 0; i < ROT_BUFFER; i++)
	{
		memset(rotation_buffer[i],0,sizeof(float)*16);
		rotation_buffer[i][0] = rotation_buffer[i][5] = rotation_buffer[i][10] = rotation_buffer[i][15] = 1.0f;
	}	
}

void init_ship()
{
	explosion = spSoundLoad("./data/explosion.wav");
	laser = spSoundLoad("./data/laser.wav");
	ship.original_texture = spLoadSurface( "./data/ship_texture.png" );
	ship.texture = spCreateSurface(ship.original_texture->w,ship.original_texture->h);
	ship.mesh = spMeshLoadObjSize("./data/ship.obj", ship.texture, 65535, SP_ONE/6);
	middle_mesh = spMeshLoadObjSize("./data/middle.obj", NULL , 65535, SP_ONE/6);
	int i,j,k;
	for (k = 0; k < 3; k++)
	{
		int and;
		switch (k)
		{
			case 0:
				and = 8191;
				break;
			case 1:
				and = 4095;
				break;
			case 2:
				and = 2047;
				break;
		}
		for (i = 0; i < STONE_COUNT; i++)
		{
			stone_mesh[i+k*STONE_COUNT] = spMeshLoadObjSize("./data/stone.obj", NULL , 65535, SP_ONE >> 2+k);
			for (j = 0; j < stone_mesh[i+k*STONE_COUNT]->pointCount; j++)
			{
				stone_mesh[i+k*STONE_COUNT]->point[j].x += rand() & and;
				stone_mesh[i+k*STONE_COUNT]->point[j].y += rand() & and;
				stone_mesh[i+k*STONE_COUNT]->point[j].z += rand() & and;
			}
		}
	}
	spUsePrecalculatedNormals(1);
	spSetLight( 1 );
	spSetLightColor( 0, SP_ONE / 2, SP_ONE / 2, SP_ONE / 2);
	spSetAmbientLightColor( SP_ONE / 2, SP_ONE / 2, SP_ONE / 2);
}

void finish_game()
{
	while (first_stone)
	{
		pStone next = first_stone->next;
		free(first_stone);
		first_stone = next;
	}
	while (first_bullet)
	{
		pBullet next = first_bullet->next;
		free(first_bullet);
		first_bullet = next;
	}	
}

void finish_ship()
{
	spSoundDelete(explosion);
	spSoundDelete(laser);
	spMeshDelete(ship.mesh);
	spMeshDelete(middle_mesh);
	int i;
	for (i = 0; i < STONE_COUNT; i++)
		spMeshDelete(stone_mesh[i]);
	spParticleDelete(&particle_bunch);
	spSetLight( 0 );
}
