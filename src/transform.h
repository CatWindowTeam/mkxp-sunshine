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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "etc-internal.h"

#include <SDL3/SDL_stdinc.h>

constexpr float PI = 3.14159265358979323846f;

class Transform{
public:
	Transform()
	    : scale(1, 1),
	      rotation(0),
	      dirty(true)
	{
		SDL_memset(matrix, 0, sizeof(matrix));

		matrix[10] = 1;
		matrix[15] = 1;
	}

	Vec2 &getPosition()       { return position;       }
	Vec2 &getOrigin()         { return origin;         }
	Vec2 &getScale()          { return scale;          }
	Vec2 &getGlobalScale()    { return globalScale;    }
	float getRotation()       { return rotation;       }
	float getGlobalRotation() { return globalRotation; }

	Vec2i getPositionI() const{
		return Vec2i(position.x, position.y);
	}

	Vec2i getOriginI() const{
		return Vec2i(origin.x, origin.y);
	}

	void setPosition(const Vec2 &value){
		position = value;
		dirty = true;
	}

	void setOrigin(const Vec2 &value){
		origin = value;
		dirty = true;
	}

	void setScale(const Vec2 &value){
		scale = value;
		dirty = true;
	}

	void setRotation(float value){
		rotation = value;
		dirty = true;
	}

	void setGlobalRotation(float value){
		globalRotation = value;
		dirty = true;
	}

	void setGlobalOffset(const Vec2i &value){
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
    	if (rotation >= 360 || rotation < -360)
    	    rotation = (float)SDL_fmod(rotation, 360);
		
    	if (globalRotation >= 360 || globalRotation < -360)
    	    globalRotation = (float)SDL_fmod(globalRotation, 360);
		
    	// object rotation cos and sin
    	float angle = rotation * PI / 180.0f;
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
		
    	float tx = (-origin.x * sxc - origin.y * sys + position.x)
    	         * globalScale.x;
		
    	float ty = ( origin.x * sxs - origin.y * syc + position.y)
    	         * globalScale.y;
		
    	// viewport rotation cos and sin
    	float globalAngle = globalRotation * PI / 180.0f;
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
    	matrix[4]  = nc;
    	matrix[5]  = nd;
    	matrix[12] = tx;
    	matrix[13] = ty;
	}

	Vec2 position;
	Vec2 origin;
	Vec2 scale;
	float rotation;
	float globalRotation;

	/* Silently added to position */
	Vec2i offset;
	/* Silently added to scale */
	Vec2 globalScale;

	float matrix[16];

	bool dirty;
};

#endif // TRANSFORM_H
