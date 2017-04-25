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

#include "matrix.h"
#include <math.h>
#include <string.h>

void mulSparrowMatrix(float* matrix)
{
	float result[16];
	int x,y;
	for (x = 0; x < 4; x++)
		for (y = 0; y < 4; y++)
			result[y+x*4] = matrix[y+0*4]*spFixedToFloat(spGetMatrix()[0+x*4])
			              + matrix[y+1*4]*spFixedToFloat(spGetMatrix()[1+x*4])
			              + matrix[y+2*4]*spFixedToFloat(spGetMatrix()[2+x*4])
			              + matrix[y+3*4]*spFixedToFloat(spGetMatrix()[3+x*4]);
	memcpy( matrix, result, sizeof( float ) * 16 );
}

void rotateX( float* dest, float rad )
{
	//Rotation matrix:
	float s = sin( rad );
	float c = cos( rad );

	float rotate[16];
	rotate[ 0] = 1.0f;
	rotate[ 4] = 0;
	rotate[ 8] = 0;
	rotate[12] = 0;
	rotate[ 1] = 0;
	rotate[ 5] = c;
	rotate[ 9] = -s;
	rotate[13] = 0;
	rotate[ 2] = 0;
	rotate[ 6] = s;
	rotate[10] = c;
	rotate[14] = 0;
	rotate[ 3] = 0;
	rotate[ 7] = 0;
	rotate[11] = 0;
	rotate[15] = 1.0f;

	float result[8];
	//+4
	result[ 0] = dest[ 4] * rotate[ 5] + dest[ 8] * rotate[ 6];
	result[ 1] = dest[ 5] * rotate[ 5] + dest[ 9] * rotate[ 6];
	result[ 2] = dest[ 6] * rotate[ 5] + dest[10] * rotate[ 6];
	result[ 3] = dest[ 7] * rotate[ 5] + dest[11] * rotate[ 6];

	result[ 4] = dest[ 4] * rotate[ 9] + dest[ 8] * rotate[10];
	result[ 5] = dest[ 5] * rotate[ 9] + dest[ 9] * rotate[10];
	result[ 6] = dest[ 6] * rotate[ 9] + dest[10] * rotate[10];
	result[ 7] = dest[ 7] * rotate[ 9] + dest[11] * rotate[10];

	memcpy( &( dest[4] ), result, sizeof( float ) * 8 );
}

void rotateY( float* dest, float rad )
{
	//Rotation matrix:
	float s = sin( rad );
	float c = cos( rad );

	float rotate[16];
	rotate[ 0] = c;
	rotate[ 4] = 0;
	rotate[ 8] = s;
	rotate[12] = 0;
	rotate[ 1] = 0;
	rotate[ 5] = 1.0f;
	rotate[ 9] = 0;
	rotate[13] = 0;
	rotate[ 2] = -s;
	rotate[ 6] = 0;
	rotate[10] = c;
	rotate[14] = 0;
	rotate[ 3] = 0;
	rotate[ 7] = 0;
	rotate[11] = 0;
	rotate[15] = 1.0f;

	float result[4];
	float result_8[4];
	result[ 0] = dest[ 0] * rotate[ 0] + dest[ 8] * rotate[ 2];
	result[ 1] = dest[ 1] * rotate[ 0] + dest[ 9] * rotate[ 2];
	result[ 2] = dest[ 2] * rotate[ 0] + dest[10] * rotate[ 2];
	result[ 3] = dest[ 3] * rotate[ 0] + dest[11] * rotate[ 2];
	//+8!
	result_8[ 0] = dest[ 0] * rotate[ 8] + dest[ 8] * rotate[10];
	result_8[ 1] = dest[ 1] * rotate[ 8] + dest[ 9] * rotate[10];
	result_8[ 2] = dest[ 2] * rotate[ 8] + dest[10] * rotate[10];
	result_8[ 3] = dest[ 3] * rotate[ 8] + dest[11] * rotate[10];
	memcpy( dest, result, sizeof( float ) * 4 );
	memcpy( &( dest[8] ), result_8, sizeof( float ) * 4 );
}


void rotateZ( float* dest, float rad )
{
	//Rotation matrix:
	float s = sin( rad );
	float c = cos( rad );

	float rotate[16];
	rotate[ 0] = c;
	rotate[ 4] = -s;
	rotate[ 8] = 0;
	rotate[12] = 0;
	rotate[ 1] = s;
	rotate[ 5] = c;
	rotate[ 9] = 0;
	rotate[13] = 0;
	rotate[ 2] = 0;
	rotate[ 6] = 0;
	rotate[10] = 1.0f;
	rotate[14] = 0;
	rotate[ 3] = 0;
	rotate[ 7] = 0;
	rotate[11] = 0;
	rotate[15] = 1.0f;

	float result[8];
	result[ 0] = dest[ 0] * rotate[ 0] + dest[ 4] * rotate[ 1];
	result[ 1] = dest[ 1] * rotate[ 0] + dest[ 5] * rotate[ 1];
	result[ 2] = dest[ 2] * rotate[ 0] + dest[ 6] * rotate[ 1];
	result[ 3] = dest[ 3] * rotate[ 0] + dest[ 7] * rotate[ 1];

	result[ 4] = dest[ 0] * rotate[ 4] + dest[ 4] * rotate[ 5];
	result[ 5] = dest[ 1] * rotate[ 4] + dest[ 5] * rotate[ 5];
	result[ 6] = dest[ 2] * rotate[ 4] + dest[ 6] * rotate[ 5];
	result[ 7] = dest[ 3] * rotate[ 4] + dest[ 7] * rotate[ 5];

	memcpy( dest, result, sizeof( float ) * 8 );
}
