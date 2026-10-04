/*
** quadarray.h
**
** This file is part of mkxp.
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

#pragma once
#include "vertex.h"
#include "render/irender.h"
#include <vector>
#include <stdint.h>

template<class VertexType>
struct QuadArray{
	std::vector<VertexType> vertices;

	GeometryHandle geom;

	size_t quadCount;
	ptrdiff_t vboSize;

	QuadArray() : quadCount(0), vboSize(-1){
		geom = activeRender().createGeometry(VertexTraits<VertexType>::layout);
	}

	~QuadArray(){
		activeRender().destroyGeometry(geom);
	}

	void resize(size_t size){
		vertices.resize(size * 4);
		quadCount = size;
	}

	void clear(){
		vertices.clear();
		quadCount = 0;
	}

	/* This needs to be called after the final 'append()' call
	 * and previous to the first 'draw()' call. */
	void commit(){
		IRender &render = activeRender();
		ptrdiff_t size = vertices.size() * sizeof(VertexType);
		if (size > vboSize){
			/* New data exceeds already allocated size.
			 * Reallocate the vertex buffer. */
			render.uploadGeometry(geom, size, dataPtr(vertices), GeometryUsage::Dynamic);
			vboSize = size;
			render.ensureQuadIndices(quadCount);
		}else{
			/* New data fits in allocated size */
			render.uploadGeometryRange(geom, 0, size, dataPtr(vertices));
		}
	}

	void draw(size_t offset, size_t count){
		activeRender().drawQuads(geom, offset, count);
	}

	void draw(){
		draw(0, quadCount);
	}

	size_t count() const {
		return quadCount;
	}
};

typedef QuadArray<Vertex> ColorQuadArray;
typedef QuadArray<SVertex> SimpleQuadArray;
