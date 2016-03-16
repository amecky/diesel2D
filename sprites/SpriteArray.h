#pragma once
#include <Vector.h>
#include "..\utils\Color.h"
#include "..\renderer\render_types.h"
#include "Sprite.h"

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
		v2* positions;
		v2* scales;
		float* rotations;
		Texture* textures;
		Color* colors;
		float* timers;
		int* types;
		int* layers;
		// physical
		v2* previous;
		v2* extents;
		SpriteShapeType* shapeTypes;

		char* buffer;

		unsigned short free_enqueue;
		unsigned short free_dequeue;

		SpriteArray() : num(0) , total(0) , buffer(0) {								
		}

		void clear() {
			if (buffer != 0) {
				for (unsigned short i = 0; i < total; ++i) {
					indices[i].id = i;
					indices[i].next = i + 1;
				}
				num = 0;
				free_dequeue = 0;
				free_enqueue = total - 1;
			}
		}

		bool verifySID(SID sid);

		void assertSID(SID sid) const;

		const int getType(SID sid) const {
			SpriteArrayIndex &in = indices[sid];
			assert(in.index != USHRT_MAX);
			return types[in.index];
		}

		void setType(SID sid,int type) {
			SpriteArrayIndex &in = indices[sid];
			assertSID(sid);
			types[in.index] = type;
		}

		const bool contains(SID sid) const {
			SpriteArrayIndex& in = indices[sid];
			return in.index != USHRT_MAX;
		}

		const int getIndex(SID sid) const {
			SpriteArrayIndex &in = indices[sid];
			assertSID(sid);
			return in.index;
		}

		const v2& getPosition(SID sid) const {
			SpriteArrayIndex &in = indices[sid];
			assertSID(sid);
			return positions[in.index];
		}

		const v2& getScale(SID sid) const {
			SpriteArrayIndex &in = indices[sid];
			assertSID(sid);
			return scales[in.index];
		}

		const float getRotation(SID sid) const {
			SpriteArrayIndex &in = indices[sid];
			assertSID(sid);
			return rotations[in.index];
		}

		const Texture& getTexture(SID sid) const {
			SpriteArrayIndex &in = indices[sid];
			assertSID(sid);
			return textures[in.index];
		}

		SID create(const v2& pos, const Texture& r, float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, const Color& color = Color::WHITE, int type = -1, int layer = 0);

		void attachCollider(SID sid, const v2& extent, SpriteShapeType shape = SST_CIRCLE) {
			SpriteArrayIndex &in = indices[sid];
			assertSID(sid);
			shapeTypes[in.index] = shape;
			extents[in.index] = extent;
		}

		void attachCollider(SID sid, SpriteShapeType shape = SST_CIRCLE) {
			const Texture& t = getTexture(sid);
			attachCollider(sid, t.dim, shape);
		}
	};

	namespace sar {

		void clear(SpriteArray& array);

		void setPosition(SpriteArray& array,SID sid,const v2& pos);

		const v2& getPosition(const SpriteArray& array,SID sid);

		void setScale(SpriteArray& array,SID sid,float sx,float sy);

		void scale(SpriteArray& array,SID sid,const v2& scale);

		void setColor(SpriteArray& array,SID sid,const Color& clr);

		void setAlpha(SpriteArray& array,SID sid,float alpha);

		void rotate(SpriteArray& array,SID sid,float angle);

		float getRotation(SpriteArray& array,SID sid);

		Sprite get(SpriteArray& array,SID sid);

		void set(SpriteArray& array,SID sid,const Sprite& sprite);

		//SID create(SpriteArray& array,const v2& pos,const Texture& r,int type = 0,int layer = 0);

		void remove(SpriteArray& array,SID id);

		void debug(SpriteArray& array);

		void debug(SpriteArray& array,SID sid);

		bool allocate(SpriteArray& array, int size);
		
	}

}
