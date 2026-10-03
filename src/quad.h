/*
** quad.h
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

struct Quad{
	Vertex vert[4];
	GeometryHandle geom;
	bool vboDirty;

	template<typename V>
	static void setPosRect(V *vert, const FloatRect &r){
		int i = 0;
		vert[i++].pos = r.topLeft();
		vert[i++].pos = r.topRight();
		vert[i++].pos = r.bottomRight();
		vert[i++].pos = r.bottomLeft();
	}

	template<typename V>
	static void setTexRect(V *vert, const FloatRect &r){
		int i = 0;
		vert[i++].texPos = r.topLeft();
		vert[i++].texPos = r.topRight();
		vert[i++].texPos = r.bottomRight();
		vert[i++].texPos = r.bottomLeft();
	}

	template<typename V>
	static int setTexPosRect(V *vert, const FloatRect &tex, const FloatRect &pos){
		setPosRect(vert, pos);
		setTexRect(vert, tex);
		return 1;
	}

	template<typename V>
	static void setColor(V *vert, const Vec4 &c){
		for (int i = 0; i < 4; ++i)
			vert[i].color = c;
	}

	Quad() : vboDirty(true){
		IRender &render = activeRender();
		geom = render.createGeometry(VertexTraits<Vertex>::layout);
		render.allocGeometry(geom, sizeof(Vertex[4]), GeometryUsage::Dynamic);

		setColor(Vec4(1, 1, 1, 1));
	}

	~Quad(){
		activeRender().destroyGeometry(geom);
	}

	void updateBuffer(){
		activeRender().uploadGeometryRange(geom, 0, sizeof(Vertex[4]), vert);
	}

	void setPosRect(const FloatRect &r){
		setPosRect(vert, r);
		vboDirty = true;
	}

	void setTexRect(const FloatRect &r){
		setTexRect(vert, r);
		vboDirty = true;
	}

	void setTexPosRect(const FloatRect &tex, const FloatRect &pos){
		setTexPosRect(vert, tex, pos);
		vboDirty = true;
	}

	void setColor(const Vec4 &c){
		for (int i = 0; i < 4; ++i)
			vert[i].color = c;

		vboDirty = true;
	}

	void draw(){
		if (vboDirty){
			updateBuffer();
			vboDirty = false;
		}

		activeRender().drawQuads(geom, 0, 1);
	}
};
