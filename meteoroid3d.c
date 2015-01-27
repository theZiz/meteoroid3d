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

spModelPointer ship_mesh;

void draw_ship(Uint16 color)
{
	ship_mesh->color = color;
	spMesh3D( ship_mesh, 0);
}

void draw_test(int eye,Uint16 color,spFontPointer font)
{
	spIdentity();
	spResetZBuffer();
	spClearTarget( 0 );
	spSetZSet(1);
	spSetZTest(1);
	spSetAlphaTest(1);
	spSetLight(1);

	spTranslate(0,0,spFloatToFixed(-1.5f));

	draw_ship(color);
}

int calc_test(Uint32 steps)
{
	if (spGetInput()->button[SP_BUTTON_START])
		return 1;
	return 0;
}

int main(int argc, char **argv)
{
	spInitCore();
	init_stereo();
	show_glasses();
	ship_mesh = spMeshLoadObjSize("./data/ship.obj", spLoadSurface( "./data/ship_texture.png" ), 65535, SP_ONE/6);
	spSetLight(1);
	spSetLightColor(0,SP_ONE*5,SP_ONE*5,SP_ONE*5);
	stereo_loop(draw_test,calc_test);
	spMeshDelete(ship_mesh);
	spQuitCore();
	return 0;
}
