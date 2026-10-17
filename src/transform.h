/*
** tilequad.h
**
** This file is part of mkxp. It is based on parts of "SFML 2.0" (license further below)
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2012 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

// added global rotation and global scale, modified updateMatrix(), using SDL funcs instead of math.h

#pragma once
#include "etc-internal.h"
#include <SDL3/SDL_stdinc.h>

class Transform{
public:
	Transform()
	    : position(0, 0, 0),
	      rotation(0, 0, 0),
		  scale(1, 1),
	      dirty(true)
	{
		SDL_memset(matrix, 0, sizeof(matrix));

		matrix[10] = 1;
		matrix[15] = 1;
	}

	Vec3 &getPosition()            { return position;            }
	Vec3 &getRotation()            { return rotation;            }
	Vec3 &getOrigin()              { return origin;              }
	Vec2 &getScale()               { return scale;               }
	Vec2 &getGlobalScale()         { return globalScale;         }
	Vec3 &getGlobalRotation()      { return globalRotation;      }
	bool getPerspectiveMode()      { return perspectiveMode;     }

	Vec3i getPositionI() const{
		return Vec3i(position.x, position.y, position.z);
	}

	Vec3i getOriginI() const{
		return Vec3i(origin.x, origin.y, origin.z);
	}

	void setPerspectiveMode(bool mode) {
		perspectiveMode = mode;
		dirty = true;
	}

	void setPosition(const Vec3 &value){
		position = value;
		dirty = true;
	}

	void setOrigin(const Vec3 &value){
		origin = value;
		dirty = true;
	}

	void setScale(const Vec2 &value){
		scale = value;
		dirty = true;
	}

	void setRotation(const Vec3 &value){
		rotation = value;
		dirty = true;
	}

	void setGlobalRotation(const Vec3 &value){
		globalRotation = value;
		dirty = true;
	}

	void setGlobalOffset(const Vec3i &value){
		offset = value;
		dirty = true;
	}

	void setGlobalScale(const Vec2 &value){
		globalScale = value;
		dirty = true;
	}

	const float *getMatrix(){
		if (dirty){
			updateMatrix();
			dirty = false;
		}

		return matrix;
	}

private:
	void updateMatrix(){
    	if (rotation.z >= 360 || rotation.z < -360)
    	    rotation.z = (float)SDL_fmod(rotation.z, 360);
		
    	if (globalRotation.z >= 360 || globalRotation.z < -360)
    	    globalRotation.z = (float)SDL_fmod(globalRotation.z, 360);
		if (!perspectiveMode) {
			
    		// object rotation cos and sin
    		float angle = rotation.z * PI / 180.0f;
    		float cosine = SDL_cos(angle);
    		float sine   = SDL_sin(angle);
			
    		float sxc = scale.x * cosine;
    		float syc = scale.y * cosine;
    		float sxs = scale.x * sine;
    		float sys = scale.y * sine;
			
    		// object matrix
    		float a =  sxc * globalScale.x;
    		float b = -sxs * globalScale.y;
    		float c =  sys * globalScale.x;
    		float d =  syc * globalScale.y;
			
    		float tx = (-origin.x * sxc - origin.y * sys + position.x) * globalScale.x;
    		float ty = ( origin.x * sxs - origin.y * syc + position.y) * globalScale.y;
			
    		// viewport rotation cos and sin
    		float globalAngle = globalRotation.z * PI / 180.0f;
    		float gc = SDL_cos(globalAngle);
    		float gs = SDL_sin(globalAngle);
			
    		// basis rotation
    		float na = a * gc - b * gs;
    		float nb = a * gs + b * gc;
    		float nc = c * gc - d * gs;
    		float nd = c * gs + d * gc;
			
    		float dx = tx;
    		float dy = ty;
			
			// appling global rotation
    		tx = dx * gc - dy * gs + offset.x;
    		ty = dx * gs + dy * gc + offset.y;
			
    		matrix[0]  = na;
    		matrix[1]  = nb;
			matrix[2]  = 0;
			matrix[3]  = 0;
    		matrix[4]  = nc;
    		matrix[5]  = nd;
			matrix[6]  = 0;
			matrix[7]  = 0;
			matrix[8]  = 0;
			matrix[9]  = 0;
			matrix[10] = 1;
			matrix[11] = 0;
    		matrix[12] = tx;
    		matrix[13] = ty;
			matrix[14] = -1;
			matrix[15] = 1;
		}else{
    		if (rotation.x >= 360 || rotation.x < -360)
    		    rotation.x = (float)SDL_fmod(rotation.x, 360);
				
    		if (rotation.y >= 360 || rotation.y < -360)
    		    rotation.y = (float)SDL_fmod(rotation.y, 360);

    		if (globalRotation.x >= 360 || globalRotation.x < -360)
    		    globalRotation.x = (float)SDL_fmod(globalRotation.x, 360);
				
    		if (globalRotation.y >= 360 || globalRotation.y < -360)
    		    globalRotation.y = (float)SDL_fmod(globalRotation.y, 360);

			// local rotation

			float rx = rotation.x * PI / 180.0f;
			float ry = rotation.y * PI / 180.0f;
			float rz = rotation.z * PI / 180.0f;
			
			float cx = SDL_cos(rx);
			float sx = SDL_sin(rx);
			
			float cy = SDL_cos(ry);
			float sy = SDL_sin(ry);
			
			float cz = SDL_cos(rz);
			float sz = SDL_sin(rz);
			
    		float m00 = cy * cz;
    		float m01 = cz * sx * sy - cx * sz;
    		float m02 = sx * sz + cx * cz * sy;

    		float m10 = cy * sz;
    		float m11 = cx * cz + sx * sy * sz;
    		float m12 = cx * sy * sz - cz * sx;

    		float m20 = -sy;
    		float m21 = cy * sx;
    		float m22 = cx * cy;

			// local scale

    		m00 *= scale.x;
    		m10 *= scale.x;
    		m20 *= scale.x;
			
    		m01 *= scale.y;
    		m11 *= scale.y;
    		m21 *= scale.y;

			// global rotation

			float grx = globalRotation.x * PI / 180.0f;
			float gry = globalRotation.y * PI / 180.0f;
			float grz = globalRotation.z * PI / 180.0f;
			
			float gcx = SDL_cos(grx);
			float gsx = SDL_sin(grx);
			
			float gcy = SDL_cos(gry);
			float gsy = SDL_sin(gry);
			
			float gcz = SDL_cos(grz);
			float gsz = SDL_sin(grz);
			
			float g00 = gcy * gcz;
			float g01 = gcz * gsx * gsy - gcx * gsz;
			float g02 = gsx * gsz + gcx * gcz * gsy;

			float g10 = gcy * gsz;
			float g11 = gcx * gcz + gsx * gsy * gsz;
			float g12 = gcx * gsy * gsz - gcz * gsx;

			float g20 = -gsy;
			float g21 = gcy * gsx;
			float g22 = gcx * gcy;

			float a00 = m00, a01 = m01, a02 = m02;
			float a10 = m10, a11 = m11, a12 = m12;
			float a20 = m20, a21 = m21, a22 = m22;

			m00 = a00 * g00 + a01 * g01 + a02 * g02;
			m01 = a00 * g10 + a01 * g11 + a02 * g12;
			m02 = a00 * g20 + a01 * g21 + a02 * g22;
			
			m10 = a10 * g00 + a11 * g01 + a12 * g02;
			m11 = a10 * g10 + a11 * g11 + a12 * g12;
			m12 = a10 * g20 + a11 * g21 + a12 * g22;

			m20 = a20 * g00 + a21 * g01 + a22 * g02;
			m21 = a20 * g10 + a21 * g11 + a22 * g12;
			m22 = a20 * g20 + a21 * g21 + a22 * g22;

			// global scale

			m00 *= globalScale.x;
    		m10 *= globalScale.x;
    		m20 *= globalScale.x;

			m01 *= globalScale.y;
			m11 *= globalScale.y;
			m21 *= globalScale.y;

			// offsets

		    float tx =    position.x - (m00 * origin.x + m01 * origin.y + m02 * origin.z) + offset.x;
		    float ty =    position.y - (m10 * origin.x + m11 * origin.y + m12 * origin.z) + offset.y;
		    float tz = -perspectiveZ - (m20 * origin.x + m21 * origin.y + m22 * origin.z) + offset.z;

			matrix[0] = m00;
			matrix[1] = m10;
			matrix[2] = m20;
			matrix[3] = 0;

			matrix[4] = m01;
			matrix[5] = m11;
			matrix[6] = m21;
			matrix[7] = 0;

			matrix[8]  = m02;
			matrix[9]  = m12;
			matrix[10] = m22;
			matrix[11] = 0;

			matrix[12] = tx;
			matrix[13] = ty;
			matrix[14] = tz;
			matrix[15] = 1;
		}
	}

	Vec3 position;
	Vec3 rotation;
	Vec3 origin;
	Vec2 scale;
	Vec3 globalRotation;

	// 3D moments lol
	float perspectiveZ;
	bool perspectiveMode = false;

	/* Silently added to position */
	Vec3i offset;
	/* Silently added to scale */
	Vec2 globalScale;

	float matrix[16];
	bool dirty;
};
