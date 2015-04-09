#pragma once
#include <Vector.h>
#include "..\utils\Color.h"
#include "..\renderer\render_types.h"
#include "Sprite.h"
#include "..\lib\DataContainer.h"

namespace ds {

	struct SpriteArrayIndex {
		SID id;
		unsigned short index;
		unsigned short next;
	};

	struct SpriteArray {

		int num;
		int total;
		SpriteArrayIndex* indices;
		SID* ids;
		Vector2f* positions;
		Vector2f* scales;
		float* rotations;
		Texture* textures;
		Color* colors;
		float* timers;
		int* types;
		char* buffer;

		unsigned short free_enqueue;
		unsigned short free_dequeue;

		SpriteArray() : num(0) , total(0) , buffer(0) {								
		}

		const int getType(SID sid) const {
			SpriteArrayIndex &in = indices[sid];
			assert(in.index != USHRT_MAX);
			return types[in.index];
		}

		const bool contains(SID sid) const {
			SpriteArrayIndex& in = indices[sid];
			return in.index != USHRT_MAX;
		}

		const int getIndex(SID sid) const {
			SpriteArrayIndex &in = indices[sid];
			assert(in.index != USHRT_MAX);
			return in.index;
		}
	};

	namespace sar {

		void clear(SpriteArray& array);

		void setPosition(SpriteArray& array,SID sid,const Vector2f& pos);

		const Vector2f& getPosition(const SpriteArray& array,SID sid);

		void setScale(SpriteArray& array,SID sid,float sx,float sy);

		void scale(SpriteArray& array,SID sid,const Vector2f& scale);

		void setColor(SpriteArray& array,SID sid,const Color& clr);

		void setAlpha(SpriteArray& array,SID sid,float alpha);

		void rotate(SpriteArray& array,SID sid,float angle);

		float getRotation(SpriteArray& array,SID sid);

		Sprite get(SpriteArray& array,SID sid);

		void set(SpriteArray& array,SID sid,const Sprite& sprite);

		SID create(SpriteArray& array,const Vector2f& pos,const Texture& r,int type = 0);

		void remove(SpriteArray& array,SID id);

		void debug(SpriteArray& array);

		void debug(SpriteArray& array,SID sid);
		
	}

}
