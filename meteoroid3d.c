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

void draw(int eye,Uint16 color,spFontPointer font)
{
	spSetZSet(1);
	spSetZTest(1);
	spIdentity();
	spResetZBuffer();
	spClearTarget( 0 );

	//HUD
	int px,py,pz,w;
	spProjectPoint3D(spFloatToFixed(-0.7f), spFloatToFixed(0.4f), spFloatToFixed(-Z0+3.2f),&px,&py,&pz,&w,1);
	char buffer[256];
	sprintf(buffer,"Power: %i%%\n",spFixedToInt(getShip()->power));
	spFontDraw(px,py-font->maxheight/2,pz,buffer,font);	
	
	spSetAlphaTest(0);
	
	//more HUD
	draw_map(color);
	
	//Space
	draw_space(color);
	
	spSetAlphaTest(1);
}

int calc(Uint32 steps)
{
	if (spGetInput()->button[SP_BUTTON_START])
		return 1;
	handle_ship_input(steps);
	update_ship(steps);
	return 0;
}

int main(int argc, char **argv)
{
	spInitCore();
	init_stereo();
	show_glasses();
	init_ship();
	stereo_loop(draw,calc);
	spMeshDelete(getShip()->mesh);
	spQuitCore();
	return 0;
}
