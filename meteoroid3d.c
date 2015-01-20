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

Sint32 rotation = 0;
SDL_Surface *sparrow;

void draw_test(int eye,Uint16 color,spFontPointer font)
{
	spIdentity();
	spResetZBuffer();
	spClearTarget( 0 );
	spSetZSet(1);
	spSetZTest(1);
	spSetAlphaTest(1);
	spSetLight(1);
	
	spTranslate(0,0, MIN_Z+( MIN_Z+MAX_Z >> 1 ));
	spBindTexture( sparrow );
	
	spRotateZ(SP_PI/4);

	int i;
	for (i = 0; i < 6; i++)
	{
		Sint32 matrix[16];
		memcpy( matrix, spGetMatrix(), 16 * sizeof( Sint32 ) ); //glPush()
		Sint32 rot = rotation;
		rot += i*SP_PI/3;
		spTranslate(0,spMul(spCos(rot),MIN_Z+MAX_Z >> 4),spMul(spSin(rot),MIN_Z+MAX_Z >> 1));
		spRotateX( rotation );
		spRotateY( rotation );
		spRotateZ( rotation );
		spScale(spFloatToFixed( 0.125 ),spFloatToFixed( 0.125 ),spFloatToFixed( 0.125 ));
		spQuadTex3D( -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, sparrow->h - 1,
					 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, 0,
					 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), sparrow->w - 1, 0,
					 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), sparrow->w - 1, sparrow->h - 1, color );
		spQuadTex3D( spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, sparrow->h - 1,
					 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
					 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), sparrow->w - 1, 0,
					 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), sparrow->w - 1, sparrow->h - 1, color );
		//Left / Right
		spQuadTex3D( -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, sparrow->h - 1,
					 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
					 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), sparrow->w - 1, 0,
					 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), sparrow->w - 1, sparrow->h - 1, color );
		spQuadTex3D( spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, sparrow->h - 1,
					 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
					 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), sparrow->w - 1, 0,
					 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), sparrow->w - 1, sparrow->h - 1, color );
		//Up / Down
		spQuadTex3D( spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, sparrow->h - 1,
					 spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
					 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), sparrow->w - 1, 0,
					 -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), sparrow->w - 1, sparrow->h - 1, color );
		spQuadTex3D( -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), 0, sparrow->h - 1,
					 -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), 0, 0,
					 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), sparrow->w - 1, 0,
					 spFloatToFixed( 1.5f ), -spFloatToFixed( 1.5f ), spFloatToFixed( 1.5f ), sparrow->w - 1, sparrow->h - 1, color );
		memcpy( spGetMatrix(), matrix, 16 * sizeof( Sint32 ) ); //glPop()
	}
}

int calc_test(Uint32 steps)
{
	rotation += steps * 32;
	if (spGetInput()->button[SP_BUTTON_START])
		return 1;
	return 0;
}

int main(int argc, char **argv)
{
	spInitCore();
	init_stereo();
	show_glasses();
	sparrow = spLoadSurface( "./sparrow.png" );
	spSetLight(1);
	spSetLightColor(0,SP_ONE*5,SP_ONE*5,SP_ONE*5);
	stereo_loop(draw_test,calc_test);
	spDeleteSurface(sparrow);
	spQuitCore();
	return 0;
}
