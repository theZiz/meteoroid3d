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

#include "stereo.h"
#include "glasses.h"
#include <string.h>

spFontPointer font = NULL;
spFontPointer left_font = NULL;
spFontPointer right_font = NULL;
Sint32 left_projection[16];
Sint32 middle_projection[16];
Sint32 right_projection[16];
SDL_Surface* right_screen = NULL;
SDL_Surface* screen = NULL;
Uint16 leftColor;
Uint16 rightColor;
int crossedEyes = 0;
int alltime = 0;
int volume = 2048;
int flip_direction = 0;

int get_volume()
{
	return volume;
}

int get_flip_direction()
{
	return flip_direction;
}

void set_flip_direction(int v)
{
	flip_direction = (v != 0);
}

void set_volume(int v)
{
	if (v < 0)
		v = 0;
	if (v > 2048)
		v = 2048;
	volume = v;
	spSoundSetMusicVolume(volume>>4);
	spSoundSetVolume(volume>>4);
}

#if defined(DESKTOP) || defined(PANDORA)
	#define FONT_SIZE 18
	#define FONT_CORRECT 4
#else
	#define FONT_SIZE 16
	#define FONT_CORRECT 0
#endif


void reload_font()
{
	//Font Loading
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./data/Play-Bold.ttf", spFixedToInt(FONT_SIZE * spGetSizeFactor())-FONT_CORRECT );
	spFontAdd( font, SP_FONT_GROUP_ASCII,              rightColor | leftColor); //whole ASCII
	spFontAddButton( font, 'S', SP_BUTTON_START_NAME,  rightColor | leftColor, 0 ); //Return == START
	spFontAddButton( font, 'E', SP_BUTTON_SELECT_NAME, rightColor | leftColor, 0 ); //Backspace == SELECT
	spFontAddButton( font, 'l', SP_BUTTON_L_NAME,      rightColor | leftColor, 0 ); // q == L
	spFontAddButton( font, 'e', SP_BUTTON_R_NAME,      rightColor | leftColor, 0 ); // e == R
	spFontAddButton( font, 'o', SP_PRACTICE_OK_NAME,   rightColor | leftColor, 0 ); //a == left button
	spFontAddButton( font, 'c', SP_PRACTICE_CANCEL_NAME,  rightColor | leftColor, 0 ); // d == right button
	spFontAddButton( font, '3', SP_PRACTICE_3_NAME,     rightColor | leftColor, 0 ); // w == up button
	spFontAddButton( font, '4', SP_PRACTICE_4_NAME,   rightColor | leftColor, 0 ); // s == down button
	spFontAddButton( font, 'a', SP_BUTTON_LEFT_NAME,   rightColor | leftColor, 0 ); //a == left button
	spFontAddButton( font, 'w', SP_BUTTON_UP_NAME,  rightColor | leftColor, 0 ); // d == right button
	spFontAddButton( font, 'd', SP_BUTTON_RIGHT_NAME,     rightColor | leftColor, 0 ); // w == up button
	spFontAddButton( font, 's', SP_BUTTON_DOWN_NAME,   rightColor | leftColor, 0 ); // s == down button
	spFontAddArrowButton( font, '<', SP_BUTTON_ARROW_LEFT,  rightColor | leftColor, 0 );
	spFontAddArrowButton( font, '^', SP_BUTTON_ARROW_UP,    rightColor | leftColor, 0 );
	spFontAddArrowButton( font, '>', SP_BUTTON_ARROW_RIGHT, rightColor | leftColor, 0 );
	spFontAddArrowButton( font, 'v', SP_BUTTON_ARROW_DOWN,  rightColor | leftColor, 0 );

	//Font Loading
	Uint16 dark = spGetRGB(
		spGetRFromColor(leftColor)/2,
		spGetGFromColor(leftColor)/2,
		spGetBFromColor(leftColor)/2);
	if ( left_font )
		spFontDelete( left_font );
	left_font = spFontLoad( "./data/Play-Bold.ttf", spFixedToInt(FONT_SIZE * spGetSizeFactor())-FONT_CORRECT );
	spFontAdd( left_font, SP_FONT_GROUP_ASCII,              leftColor); //whole ASCII
	spFontAddButton( left_font, 'S', SP_BUTTON_START_NAME,  leftColor, dark ); //Return == START
	spFontAddButton( left_font, 'E', SP_BUTTON_SELECT_NAME, leftColor, dark ); //Backspace == SELECT
	spFontAddButton( left_font, 'l', SP_BUTTON_L_NAME,      leftColor, dark ); // q == L
	spFontAddButton( left_font, 'r', SP_BUTTON_R_NAME,      leftColor, dark ); // e == R
	spFontAddButton( left_font, 'o', SP_PRACTICE_OK_NAME,   leftColor, dark ); //a == left button
	spFontAddButton( left_font, 'c', SP_PRACTICE_CANCEL_NAME,  leftColor, dark ); // d == right button
	spFontAddButton( left_font, '3', SP_PRACTICE_3_NAME,     leftColor, dark ); // w == up button
	spFontAddButton( left_font, '4', SP_PRACTICE_4_NAME,   leftColor, dark ); // s == down button
	spFontAddButton( left_font, 'a', SP_BUTTON_LEFT_NAME,   leftColor, dark ); //a == left button
	spFontAddButton( left_font, 'w', SP_BUTTON_UP_NAME,  leftColor, dark ); // d == right button
	spFontAddButton( left_font, 'd', SP_BUTTON_RIGHT_NAME,     leftColor, dark ); // w == up button
	spFontAddButton( left_font, 's', SP_BUTTON_DOWN_NAME,   leftColor, dark ); // s == down button
	spFontAddArrowButton( left_font, '<', SP_BUTTON_ARROW_LEFT,  leftColor, dark );
	spFontAddArrowButton( left_font, '^', SP_BUTTON_ARROW_UP,    leftColor, dark );
	spFontAddArrowButton( left_font, '>', SP_BUTTON_ARROW_RIGHT, leftColor, dark );
	spFontAddArrowButton( left_font, 'v', SP_BUTTON_ARROW_DOWN,  leftColor, dark );

	//Font Loading
	dark = spGetRGB(
		spGetRFromColor(rightColor)/2,
		spGetGFromColor(rightColor)/2,
		spGetBFromColor(rightColor)/2);
	if ( right_font )
		spFontDelete( right_font );
	right_font = spFontLoad( "./data/Play-Bold.ttf", spFixedToInt(FONT_SIZE * spGetSizeFactor())-FONT_CORRECT );
	spFontAdd( right_font, SP_FONT_GROUP_ASCII,              rightColor); //whole ASCII
	spFontAddButton( right_font, 'S', SP_BUTTON_START_NAME,  rightColor, dark ); //Return == START
	spFontAddButton( right_font, 'E', SP_BUTTON_SELECT_NAME, rightColor, dark ); //Backspace == SELECT
	spFontAddButton( right_font, 'l', SP_BUTTON_L_NAME,      rightColor, dark ); // q == L
	spFontAddButton( right_font, 'r', SP_BUTTON_R_NAME,      rightColor, dark ); // e == R
	spFontAddButton( right_font, 'o', SP_PRACTICE_OK_NAME,   rightColor, dark ); //a == left button
	spFontAddButton( right_font, 'c', SP_PRACTICE_CANCEL_NAME,  rightColor, dark ); // d == right button
	spFontAddButton( right_font, '3', SP_PRACTICE_3_NAME,     rightColor, dark ); // w == up button
	spFontAddButton( right_font, '4', SP_PRACTICE_4_NAME,   rightColor, dark ); // s == down button
	spFontAddButton( right_font, 'a', SP_BUTTON_LEFT_NAME,   rightColor, dark ); //a == left button
	spFontAddButton( right_font, 'w', SP_BUTTON_UP_NAME,  rightColor, dark ); // d == right button
	spFontAddButton( right_font, 'd', SP_BUTTON_RIGHT_NAME,     rightColor, dark ); // w == up button
	spFontAddButton( right_font, 's', SP_BUTTON_DOWN_NAME,   rightColor, dark ); // s == down button
	spFontAddArrowButton( right_font, '<', SP_BUTTON_ARROW_LEFT,  rightColor, dark );
	spFontAddArrowButton( right_font, '^', SP_BUTTON_ARROW_UP,    rightColor, dark );
	spFontAddArrowButton( right_font, '>', SP_BUTTON_ARROW_RIGHT, rightColor, dark );
	spFontAddArrowButton( right_font, 'v', SP_BUTTON_ARROW_DOWN,  rightColor, dark );
}

void resize( Uint16 w, Uint16 h )
{
	spSelectRenderTarget(spGetWindowSurface());
	if (right_screen)
		spDeleteSurface(right_screen);
	right_screen = spCreateSurface(screen->w,screen->h);
	
	spSetPerspectiveStereoscopic( middle_projection, 45.0, ( float )screen->w / ( float )screen->h, 1.0, 100.0f, Z0, 0);
	spStereoCreateProjectionMatrixes( left_projection, right_projection, 45.0, ( float )screen->w / ( float )screen->h, 1.0, 100.0f, Z0, DISTANCE , crossedEyes);

	reload_font();
}

void ( *draw_stereo_callback )( int, Uint16, spFontPointer) = NULL;

void draw_stereo(void)
{
	Sint32* modellViewMatrix=spGetMatrix();
	int eye;	
	if (get_glasses() == 0)
	{
		spSelectRenderTarget(screen);
		memcpy(spGetProjectionMatrix(), middle_projection,sizeof(Sint32)*16);
		draw_stereo_callback(0,leftColor,left_font);
	}
	else
	for (eye = 0; eye < 2; eye++)
	{
		Uint16 color;
		spFontPointer font;
		switch (eye)
		{
			case 0:
				//LEFT
				spSelectRenderTarget(screen);
				memcpy(spGetProjectionMatrix(), left_projection,sizeof(Sint32)*16);
				color = leftColor;
				font = left_font;
				break;
			case 1:
				//RIGHT
				spSelectRenderTarget(right_screen);
				memcpy(spGetProjectionMatrix(),right_projection,sizeof(Sint32)*16);
				color = rightColor;
				font = right_font;
				break;
		}
		draw_stereo_callback(eye,color,font);	
	}
	if (get_glasses() != 0)
		spStereoMergeSurfaces(screen,right_screen,crossedEyes);
	#ifdef DEBUG
		spSelectRenderTarget(screen);
		char buffer[256];
		sprintf(buffer,"FPS: %i",spGetFPS());
		spFontDraw(2,screen->h-2-font->maxheight,0, buffer, font );
	#endif
	spFlip();
}

int stereo_loop ( void ( *draw )( int, Uint16, spFontPointer), int ( *calc )( Uint32 steps ))
{
	void ( *old_callback )( int, Uint16, spFontPointer) = draw_stereo_callback;
	draw_stereo_callback = draw;
	int res = spLoop(draw_stereo,calc,10,resize,NULL);	
	draw_stereo_callback = old_callback;
	return res;
}

int leftColorArray[3];
int rightColorArray[3];
int brightness;
int flipped = 0;

void set_flipped(int f)
{
	flipped = (f != 0);
}

int get_flipped()
{
	return flipped;
}

void set_brightness(int b)
{
	brightness = spMax(spMin(b,100),20);
}

int get_brightness()
{
	return brightness;
}

void set_some_color(int eye,int r,int g,int b)
{
	if (eye == 0)
	{
		leftColorArray[0] = r;
		leftColorArray[1] = g;
		leftColorArray[2] = b;
		leftColor = spGetRGB(r*brightness/100,g*brightness/100,b*brightness/100);
	}
	else
	{
		rightColorArray[0] = r;
		rightColorArray[1] = g;
		rightColorArray[2] = b;
		rightColor = spGetRGB(r*brightness/100,g*brightness/100,b*brightness/100);
	}
}

void set_color(int lr,int lg,int lb,int rr,int rg,int rb)
{
	set_some_color(  flipped,lr,lg,lb);
	set_some_color(1-flipped,rr,rg,rb);
	reload_font();
}

void set_alltime(int value)
{
	if (value > alltime)
		alltime = value;
}

int get_alltime()
{
	return alltime;
}

spConfigPointer stereo_config;

void save_stereo()
{
	spConfigSetInt(stereo_config,"brightness",brightness);
	spConfigSetInt(stereo_config,"stereo_mode",get_glasses());
	spConfigSetBool(stereo_config,"flipped",flipped);
	spConfigSetInt(stereo_config,"alltime",alltime);
	spConfigSetInt(stereo_config,"volume",volume);
	spConfigSetBool(stereo_config,"flip_control",flip_direction);
	spConfigWrite(stereo_config);
}

void init_stereo()
{
	stereo_config = spConfigRead("config.ini","meteoroid3d");
	set_brightness(spConfigGetInt(stereo_config,"brightness",80));
	set_glasses(spConfigGetInt(stereo_config,"stereo_mode",0));
	flipped = spConfigGetBool(stereo_config,"flipped",0);
	set_alltime(spConfigGetInt(stereo_config,"alltime",0));
	set_volume(spConfigGetInt(stereo_config,"volume",2048));
	flip_direction = spConfigGetBool(stereo_config,"flip_control",0);
	screen = spCreateDefaultWindow();
	set_color(
		get_color_value(0,0),
		get_color_value(0,1),
		get_color_value(0,2),
		get_color_value(1,0),
		get_color_value(1,1),
		get_color_value(1,2));
	resize( screen->w, screen->h );
}
