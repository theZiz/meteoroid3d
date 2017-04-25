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
#include "space.h"
#include "stereo.h"

spNetC4AProfilePointer profile;

int first = 1;

void draw(int eye,Uint16 color,spFontPointer font)
{
	spIdentity();
	spResetZBuffer();
	spClearTarget( 0 );

	spSetAlphaTest(0);	
	//more HUD
	draw_map(color);
	
	//Space
	draw_space(color);
	
	//HUD
	spSetAlphaTest(1);
	spSetZSet(0);
	spSetZTest(0);
	int px,py,pz,w;
	char buffer[512];
	if (first == 1)
	{
		spProjectPoint3D(spFloatToFixed(-0.0f), spFloatToFixed(0.0f), spFloatToFixed(-Z0+3.5f),&px,&py,&pz,&w,1);
		if (get_flip_direction())
			sprintf(buffer,"Welcome to Meteroid3D (with C4A support)\n\n*Destroy meteroids for beating the highscore\n*Don't get hit (you have 3 lives)\n\nControls:\n"SP_PAD_NAME" Direction ([v] is down, change with [c])\n[l][r]barrel roll\n[w]Boost   [s]Brake   [a]or[d]Shoot\n[S]Pause, Exit and volume\n\n[o]Let's go!");
		else
			sprintf(buffer,"Welcome to Meteroid3D (with C4A support)\n\n*Destroy meteroids for beating the highscore\n*Don't get hit (you have 3 lives)\n\nControls:\n"SP_PAD_NAME" Direction ([^] is down, change with [c])\n[l][r]barrel roll\n[w]Boost   [s]Brake   [a]or[d]Shoot\n[S]Pause, Exit and volume\n\n[o]Let's go!");
		spFontDrawMiddle(px,py-font->maxheight*6,pz,buffer,font);	
	}
	else
	if (first == -1)
	{
		spProjectPoint3D(spFloatToFixed(-0.0f), spFloatToFixed(0.0f), spFloatToFixed(-Z0+3.5f),&px,&py,&pz,&w,1);
		if (get_score() <= get_alltime())
			sprintf(buffer,"\nYou lost,\nbut your score was\n%i !\n\n[d]Try again beating\nthe highscore\n%i\n\n[S]Exit",get_score(),get_alltime());
		else
			sprintf(buffer,"You lost,\nbut your score was\n%i !\n\nFurthermore you beat\nthe highscore:\n%i!\nCongratulations!\n\n[d]Try again\n[S]Exit",get_score(),get_alltime());
		spFontDrawMiddle(px,py-font->maxheight*6,pz,buffer,font);
	}
	else
	if (first == -2)
	{
		spProjectPoint3D(spFloatToFixed(-0.0f), spFloatToFixed(0.0f), spFloatToFixed(-Z0+3.5f),&px,&py,&pz,&w,1);
		sprintf(buffer,"\n\n\nFinishing...\n\nCommitting C4A results\n\n[d]Cancel");
		spFontDrawMiddle(px,py-font->maxheight*6,pz,buffer,font);
	}
	else
	if (first == -3)
	{
		spProjectPoint3D(spFloatToFixed(-0.0f), spFloatToFixed(0.0f), spFloatToFixed(-Z0+3.5f),&px,&py,&pz,&w,1);
		sprintf(buffer,"\nPaused\n\n[l][r]Volume\n\n[S] continue\n\n[c]Exit");
		spFontDrawMiddle(px,py-font->maxheight*6,pz,buffer,font);
	}
	else
	{
		spProjectPoint3D(spFloatToFixed(-0.7f), spFloatToFixed(0.4f), spFloatToFixed(-Z0+3.2f),&px,&py,&pz,&w,1);
		sprintf(buffer,"Power: %i%%",spFixedToInt(getShip()->power));
		spFontDraw(px,py-font->maxheight/2,pz,buffer,font);	
		spProjectPoint3D(spFloatToFixed(0.7f), spFloatToFixed(0.4f), spFloatToFixed(-Z0+3.2f),&px,&py,&pz,&w,1);
		sprintf(buffer,"Alltime: %i\nRecent: %i",get_alltime(),get_score());
		spFontDrawRight(px,py-font->maxheight/2,pz,buffer,font);	
		spProjectPoint3D(spFloatToFixed(0.7f), spFloatToFixed(-0.4f), spFloatToFixed(-Z0+3.2f),&px,&py,&pz,&w,1);
		if (get_lives() == 1)
			sprintf(buffer,"Lives: Last");
		else
			sprintf(buffer,"Lives: %i",get_lives());
		spFontDrawRight(px,py-font->maxheight/2,pz,buffer,font);
	}
}

int last_volume = 0;

int calc(Uint32 steps)
{
	if (first == -3) //Pause
	{
		if (spGetInput()->button[SP_BUTTON_L])
			set_volume(get_volume()-steps);
		if (spGetInput()->button[SP_BUTTON_R])
			set_volume(get_volume()+steps);
		else
		if (last_volume != get_volume())
		{
			last_volume = get_volume();
			save_stereo();
		}
		if (spGetInput()->button[SP_PRACTICE_CANCEL])
		{
			spGetInput()->button[SP_PRACTICE_CANCEL] = 0;
			first = -2;
			set_alltime(get_score());
			save_stereo();
			if (profile)
			{
				if (get_score())
					spNetC4ACommitScore(profile,"meteoroid3d",get_score(),NULL,0);
				if (spNetC4ACommitScore(profile,"",0,NULL,20000))
					return 2;
			}
			else
				return 2;
		}
		if (spGetInput()->button[SP_BUTTON_START])
		{
			spGetInput()->button[SP_BUTTON_START] = 0;
			first = 0;
		}		
	}
	else
	if (first == 1) //Introducing screen
	{
		if (spGetInput()->button[SP_PRACTICE_OK])
		{
			spGetInput()->button[SP_PRACTICE_OK] = 0;
			first = 0;
		}
		if (spGetInput()->button[SP_PRACTICE_CANCEL])
		{
			spGetInput()->button[SP_PRACTICE_CANCEL] = 0;
			set_flip_direction(1-get_flip_direction());
			save_stereo();
		}
		if (spGetInput()->button[SP_BUTTON_START])
		{
			spGetInput()->button[SP_BUTTON_START] = 0;
			first = -3;
		}
	}
	else
	if (first == -1) //Finish
	{
		if (spGetInput()->button[SP_BUTTON_RIGHT])
		{
			spGetInput()->button[SP_BUTTON_RIGHT] = 0;
			first = 0;
			set_alltime(get_score());
			save_stereo();
			return 1;
		}
		if (spGetInput()->button[SP_BUTTON_START])
		{
			spGetInput()->button[SP_BUTTON_START] = 0;
			first = -2;
			set_alltime(get_score());
			save_stereo();
			if (profile)
			{
				if (get_score())
					spNetC4ACommitScore(profile,"meteoroid3d",get_score(),NULL,0);
				if (spNetC4ACommitScore(profile,"",0,NULL,20000))
					return 2;
			}
			else
				return 2;
		}
	}
	else
	if (first == -2) //C4A commit
	{
		if (spGetInput()->button[SP_BUTTON_RIGHT])
			spNetC4ACancelTask();
		if (spNetC4AGetStatus() != SP_C4A_PROGRESS)
			return 2;
	}
	else
	{
		handle_ship_input(steps);
		update_ship(steps);
		if (get_lives() <= 0)
		{
			spGetInput()->button[SP_BUTTON_LEFT] = 0;
			first = -1;
			if (get_score())
				spNetC4ACommitScore(profile,"meteoroid3d",get_score(),NULL,0);
		}
		if (spGetInput()->button[SP_BUTTON_START])
		{
			spGetInput()->button[SP_BUTTON_START] = 0;
			first = -3;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	spSetDefaultWindowSize( 320, 240 );
	//spSetDefaultWindowSize( 800, 480 );
	spInitCore();
	spInitNet();
	spSoundInit();
	profile = spNetC4AGetProfile();
	if (profile == NULL)
		printf("C4A-Manager or Mini-C4A not installed.");
	else
		spNetC4ASetCaching(2);
	init_stereo();
	last_volume = get_volume();
	show_glasses();
	spSoundSetMusic("./data/space_explorers.ogg");
	spSoundPlayMusic(0,-1);
	init_ship();
	int res = 1;
	while (res == 1)
	{
		init_game();
		res = stereo_loop(draw,calc);
		finish_game();
	}
	spSoundStopMusic(0);
	spNetC4AFreeProfile(profile);
	finish_ship();
	spQuitCore();
	spQuitNet();
	spSoundQuit();
	return 0;
}
