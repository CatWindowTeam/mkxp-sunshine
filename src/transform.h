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

class Transform{
public:
	Transform()
	    : scale(1, 1),
	      rotation(0),
		  perspectiveZ(0),
		  perspectiveRotation(0, 0),
	      dirty(true)
	{
		SDL_memset(matrix, 0, sizeof(matrix));

		matrix[10] = 1;
		matrix[15] = 1;
	}

	Vec2 &getPosition()            { return position;            }
	Vec2 &getOrigin()              { return origin;              }
	Vec2 &getScale()               { return scale;               }
	Vec2 &getGlobalScale()         { return globalScale;         }
	Vec2 &getPerspectiveRotation() { return perspectiveRotation; }
	float getRotation()            { return rotation;            }
	float getGlobalRotation()      { return globalRotation;      }
	float getPerspectiveZ()        { return perspectiveZ;        }
	bool getPerspectiveMode()      { return perspectiveMode;     }

	Vec2i getPositionI() const{
		return Vec2i(position.x, position.y);
	}

	Vec2i getOriginI() const{
		return Vec2i(origin.x, origin.y);
	}

	void setPerspectiveMode(bool mode) {
		perspectiveMode = mode;
		dirty = true;
	}

	void setPosition(const Vec2 &value){
		position = value;
		dirty = true;
	}

	void setPerspectiveZ(float value) {
		perspectiveZ = value;
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

	void setPerspectiveRotation(const Vec2 &value){
		perspectiveRotation = value;
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
		if (!perspectiveMode) {
			
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
		}
		else {
    		if (perspectiveRotation.x >= 360 || perspectiveRotation.x < -360)
    		    perspectiveRotation.x = (float)SDL_fmod(perspectiveRotation.x, 360);
				
    		if (perspectiveRotation.y >= 360 || perspectiveRotation.y < -360)
    		    perspectiveRotation.y = (float)SDL_fmod(perspectiveRotation.y, 360);

			float rx = perspectiveRotation.x * PI / 180.0f;
			float ry = perspectiveRotation.y * PI / 180.0f;
			float rz = rotation * PI / 180.0f;
			
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

    		m00 *= scale.x;
    		m10 *= scale.x;
    		m20 *= scale.x;
			
    		m01 *= scale.y;
    		m11 *= scale.y;
    		m21 *= scale.y;
			
			float globalAngle = globalRotation * PI / 180.0f;
    		float gc = SDL_cos(globalAngle);
    		float gs = SDL_sin(globalAngle);

    		// Rotate X/Y basis around Z
			
    		float r00 = m00 * gc - m01 * gs;
    		float r01 = m00 * gs + m01 * gc;

    		float r10 = m10 * gc - m11 * gs;
    		float r11 = m10 * gs + m11 * gc;

    		float r20 = m20 * gc - m21 * gs;
    		float r21 = m20 * gs + m21 * gc;

    		m00 = r00;
    		m01 = r01;

    		m10 = r10;
    		m11 = r11;

    		m20 = r20;
    		m21 = r21;

			m00 *= globalScale.x;
    		m10 *= globalScale.x;
    		m20 *= globalScale.x;

			m01 *= globalScale.y;
			m11 *= globalScale.y;
			m21 *= globalScale.y;

		    float tx =    position.x - (m00 * origin.x + m01 * origin.y) + offset.x;
		    float ty =    position.y - (m10 * origin.x + m11 * origin.y) + offset.y;
		    float tz = -perspectiveZ - (m20 * origin.x + m21 * origin.y);

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

	Vec2 position;
	Vec2 origin;
	Vec2 scale;
	float rotation;
	float globalRotation;

	// 3D moments lol
	float perspectiveZ;
	// X and Y, Z is just rotation
	Vec2 perspectiveRotation;
	bool perspectiveMode = false;

	/* Silently added to position */
	Vec2i offset;
	/* Silently added to scale */
	Vec2 globalScale;

	float matrix[16];

	bool dirty;
};

#endif // TRANSFORM_H
