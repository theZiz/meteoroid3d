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

#include "glasses.h"
#include "stereo.h"
#include <sparrow3d.h>

SDL_Surface* glasses = NULL;
Sint32 g_rot = 0;

#define GLASSES_COUNT 6

char glasses_names[GLASSES_COUNT][32] = {
	"None",
	"Red & Cyan",
	"Red & Green",
	"Magenta & Green",
	"Yellow & Blue",
	"Red & Blue"
};

int glasses_color[GLASSES_COUNT][2][3] = {
	{{255,255,255},{255,255,255}},
	{{255,0,0},{0,255,255}},
	{{255,0,0},{0,255,0}},
	{{240,0,255},{0,255,0}},
	{{255,255,0},{0,0,255}},
	{{196,0,0},{0,0,255}}
};

int choosen_glasses = 0;

int set_glasses(int mode)
{
	choosen_glasses = abs(mode)%GLASSES_COUNT;
}

int get_glasses()
{
	return choosen_glasses;
}

int get_color_value(int eye,int c_pos)
{
	return glasses_color[choosen_glasses][eye][c_pos];
}

#if defined(DESKTOP) || defined(PANDORA)
	#define FONT_EXTRA ""
#else
	#define FONT_EXTRA "\nFont: Pixel Mania by HiBan (CC-BY-SA)"
#endif

void draw_glasses(int eye,Uint16 color,spFontPointer font)
{
	spIdentity();
	spResetZBuffer();
	spClearTarget( 0 );
	spSetZSet(1);
	spSetZTest(1);
	spSetAlphaTest(1);
	spTranslate(0,SP_ONE/7, spFloatToFixed(-Z0+3.2f));
	
	spBindTexture( glasses );
	int x = spFloatToFixed(  0.5f );
	int y = spFloatToFixed( 0.15f );
	int px,py,pz,w;
	spProjectPoint3D(0, SP_ONE/4,0,&px,&py,&pz,&w,1);
	spFontDrawMiddle(px,py-font->maxheight/2,pz,"Choose your 3d glasses ([<][>])",font);
	spProjectPoint3D(0,-SP_ONE/4,0,&px,&py,&pz,&w,1);
	spFontDrawMiddle(px,py-font->maxheight/2,pz,glasses_names[choosen_glasses],font);
	spRotateY(spSin(g_rot)/2);
	spQuadTex3D( -x,  y, 0 , get_flipped()?glasses->w-1:0, 0,
				 -x, -y, 0 , get_flipped()?glasses->w-1:0, glasses->h - 1,
				  x, -y, 0 , get_flipped()?0:glasses->w-1, glasses->h - 1,
				  x,  y, 0 , get_flipped()?0:glasses->w-1, 0, color );
	spRotateY(-spSin(g_rot)/2);
	spTranslate(0,-SP_ONE/3,0);
	spProjectPoint3D(0,0,0,&px,&py,&pz,&w,1);
	char buffer[128];
	if (get_flipped())
		sprintf(buffer,"Brightness: %i%% ([v][^])\n[3]Flip (Flipped)\n[o]Okay, let's play!"FONT_EXTRA,get_brightness());
	else
		sprintf(buffer,"Brightness: %i%% ([v][^])\n[3]Flip (Normal)\n[o]Okay, let's play!"FONT_EXTRA,get_brightness());
	spFontDrawMiddle(px,py,pz,buffer,font);
	
}

int next_glasses = 0;
int pressed = 0;
int old_pressed = 200;

int calc_glasses(Uint32 steps)
{
	g_rot += steps*128;
	if (spGetInput()->button[SP_PRACTICE_OK])
	{
		spGetInput()->button[SP_PRACTICE_OK] = 0;
		return 1;
	}
	int just_flipped = 0;
	if (spGetInput()->button[SP_PRACTICE_3])
	{
		set_flipped(1-get_flipped());
		just_flipped = 1;
		spGetInput()->button[SP_PRACTICE_3] = 0;
	}
	int i;
	for (i = 0; i < steps; i++)
	{
		if (spGetInput()->axis[1] > 0)
		{
			if (pressed == 0)
			{
				set_brightness(get_brightness()-1);
				pressed = old_pressed;
				old_pressed = old_pressed*9/10+1;
			}
			pressed--;
		}
		else
		if (spGetInput()->axis[1] < 0)
		{
			if (pressed == 0)
			{
				set_brightness(get_brightness()+1);
				pressed = old_pressed;
				old_pressed = old_pressed*9/10+1;
			}
			pressed--;
		}
		else
		{
			pressed = 0;
			old_pressed = 200;
		}
	}
	if (spGetInput()->axis[0] < 0)
	{
		spGetInput()->axis[0] = 0;
		next_glasses = (choosen_glasses+GLASSES_COUNT-1)%GLASSES_COUNT;
	}
	if (spGetInput()->axis[0] > 0)
	{
		spGetInput()->axis[0] = 0;
		next_glasses = (choosen_glasses+1)%GLASSES_COUNT;
	}
	if (next_glasses != choosen_glasses || pressed || just_flipped)
	{
		choosen_glasses = next_glasses;
		set_color(
			glasses_color[choosen_glasses][0][0],
			glasses_color[choosen_glasses][0][1],
			glasses_color[choosen_glasses][0][2],
			glasses_color[choosen_glasses][1][0],
			glasses_color[choosen_glasses][1][1],
			glasses_color[choosen_glasses][1][2]);
	}
	return 0;
}

int show_glasses()
{
	glasses = spLoadSurface("./data/glasses.png");
	spSelectRenderTarget(glasses);
	next_glasses = choosen_glasses;
	stereo_loop(draw_glasses,calc_glasses);
	save_stereo();
	spDeleteSurface( glasses );
}
