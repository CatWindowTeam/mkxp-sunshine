/*
** vertex.h
**
** This file is part of mkxp.
**
** Copyright (C) 2014 Jonas Kulla <Nyocurio@gmail.com>
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
#include "etc-internal.h"
#include "render/irender.h"

/* Simple Vertex */
struct SVertex{
	Vec2 pos;
	Vec2 texPos;
};

/* Color Vertex */
struct CVertex{
	Vec2 pos;
	Vec4 color;
	CVertex();
};

struct Vertex{
	Vec2 pos;
	Vec2 texPos;
	Vec4 color;
	Vertex();
};

template<class VertType>
struct VertexTraits;

template<>
struct VertexTraits<SVertex>{
	static const VertexLayout layout = VertexLayout::Simple;
};

template<>
struct VertexTraits<CVertex>{
	static const VertexLayout layout = VertexLayout::Color;
};

template<>
struct VertexTraits<Vertex>{
	static const VertexLayout layout = VertexLayout::Full;
};
