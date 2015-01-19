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

#define DISTANCE 0.06f
#define Z0 5.0f
#define MIN_Z (-SP_ONE*3/2)
#define MAX_Z (-SP_ONE*10)

spFontPointer font = NULL;
Sint32 left_projection[16];
Sint32 right_projection[16];
SDL_Surface* right_screen;
SDL_Surface* screen = NULL;
SDL_Surface *sparrow;
Sint32 rotation;
Uint16 leftColor;
Uint16 rightColor;
int crossedEyes = 0;

void resize( Uint16 w, Uint16 h )
{
	spSelectRenderTarget(spGetWindowSurface());
	if (right_screen)
		spDeleteSurface(right_screen);
	right_screen = spCreateSurface(screen->w,screen->h);

	spStereoCreateProjectionMatrixes( left_projection, right_projection, 45.0, ( float )screen->w / ( float )screen->h, 1.0, 100.0f, Z0, DISTANCE , crossedEyes);

	//Font Loading
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./Play-Bold.ttf", spFixedToInt(12 * spGetSizeFactor()) );
	spFontAdd( font, SP_FONT_GROUP_ASCII,              rightColor | leftColor); //whole ASCII
	spFontAddButton( font, 'R', SP_BUTTON_START_NAME,  rightColor | leftColor, SP_ALPHA_COLOR ); //Return == START
	spFontAddButton( font, 'B', SP_BUTTON_SELECT_NAME, rightColor | leftColor, SP_ALPHA_COLOR ); //Backspace == SELECT
	spFontAddButton( font, 'q', SP_BUTTON_L_NAME,      rightColor | leftColor, SP_ALPHA_COLOR ); // q == L
	spFontAddButton( font, 'e', SP_BUTTON_R_NAME,      rightColor | leftColor, SP_ALPHA_COLOR ); // e == R
	spFontAddButton( font, 'a', SP_BUTTON_LEFT_NAME,   rightColor | leftColor, SP_ALPHA_COLOR ); //a == left button
	spFontAddButton( font, 'd', SP_BUTTON_RIGHT_NAME,  rightColor | leftColor, SP_ALPHA_COLOR ); // d == right button
	spFontAddButton( font, 'w', SP_BUTTON_UP_NAME,     rightColor | leftColor, SP_ALPHA_COLOR ); // w == up button
	spFontAddButton( font, 's', SP_BUTTON_DOWN_NAME,   rightColor | leftColor, SP_ALPHA_COLOR ); // s == down button
}

void draw_test(void)
{
	Sint32* modellViewMatrix=spGetMatrix();
	int eye;
	for (eye = 0; eye < 2; eye++)
	{
		Uint16 color;
		spIdentity();
		switch (eye)
		{
			case 0:
				//LEFT
				spSelectRenderTarget(screen);
				memcpy(spGetProjectionMatrix(), left_projection,sizeof(Sint32)*16);
				color = leftColor;
				break;
			case 1:
				//RIGHT
				spSelectRenderTarget(right_screen);
				memcpy(spGetProjectionMatrix(),right_projection,sizeof(Sint32)*16);
				color = rightColor;
				break;
		}	
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

	spStereoMergeSurfaces(screen,right_screen,crossedEyes);
	spSelectRenderTarget(screen);
	char buffer[256];
	sprintf(buffer,"FPS: %i",spGetFPS());
	spFontDrawRight(screen->w-2,screen->h-2-font->maxheight,0, buffer, font );
	spFlip();
}

int calc_test(Uint32 steps)
{
	rotation += steps * 32;
	PspInput engineInput = spGetInput();
	if (engineInput->button[SP_BUTTON_START])
		return 1;
	return 0;
}

int main(int argc, char **argv)
{
	if (crossedEyes)
	{
		leftColor  = spGetFastRGB(255,255,255);
		rightColor = spGetFastRGB(255,255,255);
	}
	else
	{
		leftColor  = spGetFastRGB(196,  0,  0);
		rightColor = spGetFastRGB(  0,196,  0);
	}
	spInitCore();
	//Setup
	screen = spCreateDefaultWindow();
	resize( screen->w, screen->h );
	sparrow = spLoadSurface( "./sparrow.png" );
	spSetLight(1);
	spSetLightColor(0,SP_ONE*5,SP_ONE*5,SP_ONE*5);
	spLoop(draw_test,calc_test,10,resize,NULL);
	spDeleteSurface(sparrow);
	spQuitCore();
	return 0;
}
