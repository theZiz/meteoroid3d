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

spNetC4AProfilePointer profile;

int first = 1;

void draw(int eye,Uint16 color,spFontPointer font)
{
	spSetZSet(1);
	spSetZTest(1);
	spIdentity();
	spResetZBuffer();
	spClearTarget( 0 );

	//HUD
	int px,py,pz,w;
	char buffer[512];
	if (first == 1)
	{
		spProjectPoint3D(spFloatToFixed(-0.0f), spFloatToFixed(0.33f), spFloatToFixed(-Z0+3.5f),&px,&py,&pz,&w,1);
		sprintf(buffer,"Welcome to Meteroid3D (with C4A support)\n\n*Destroy meteroids for beating the highscore\n*Don't get hit (you have 3 lives)\n\nControls:\n[<][>][v][^] Direction\n[w]Boost   [s]Brake   [a]Shoot\n[l][r]sound volume\n[S]Exit\n\n[o]Let's go!");
		spFontDrawMiddle(px,py-font->maxheight/2,pz,buffer,font);	
	}
	else
	if (first == -1)
	{
		spProjectPoint3D(spFloatToFixed(-0.0f), spFloatToFixed(0.3f), spFloatToFixed(-Z0+3.5f),&px,&py,&pz,&w,1);
		if (get_score() <= get_alltime())
			sprintf(buffer,"\nYou lost,\nbut your score was\n%i !\n\n[d]Try again beating\nthe highscore\n%i\n\n[S]Exit",get_score(),get_alltime());
		else
			sprintf(buffer,"You lost,\nbut your score was\n%i !\n\nFurthermore you beat\nthe highscore:\n%i!\nCongratulations!\n\n[d]Try again\n[S]Exit",get_score(),get_alltime());
		spFontDrawMiddle(px,py-font->maxheight/2,pz,buffer,font);
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
		spSetAlphaTest(0);
		
		//more HUD
		draw_map(color);
	}
	
	//Space
	draw_space(color);
	
	spSetAlphaTest(1);
}

int last_volume = 0;

int calc(Uint32 steps)
{
	if (spGetInput()->button[SP_BUTTON_START])
		return 2;
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
	if (first == 1)
	{
		if (spGetInput()->button[SP_PRACTICE_OK])
		{
			spGetInput()->button[SP_PRACTICE_OK] = 0;
			first = 0;
		}
	}
	else
	if (first == -1)
	{
		if (spGetInput()->button[SP_BUTTON_RIGHT])
		{
			spGetInput()->button[SP_BUTTON_RIGHT] = 0;
			first = 0;
			set_alltime(get_score());
			save_stereo();
			return 1;
		}
	}
	else
	{
		handle_ship_input(steps);
		update_ship(steps);
		if (get_lives() <= 0)
		{
			spGetInput()->button[SP_BUTTON_LEFT] = 0;
			first = -1;
			spNetC4ACommitScore(profile,"meteoroid3d",get_score(),NULL,0);
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
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
	finish_ship();
	if (profile)
	{
		if (spNetC4ACommitScore(profile,"",0,NULL,10000) == 0)
			while (spNetC4AGetStatus() == SP_C4A_PROGRESS)
				spSleep(10000);
		spNetC4AFreeProfile(profile);
	}
	spQuitCore();
	spQuitNet();
	spSoundQuit();
	return 0;
}
