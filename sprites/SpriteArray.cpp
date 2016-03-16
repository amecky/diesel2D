#include "SpriteArray.h"
#include "..\utils\Log.h"
#include "..\memory\DefaultAllocator.h"

namespace ds {

	bool SpriteArray::verifySID(SID sid) {
		SpriteArrayIndex &in = indices[sid];
		if (in.index != USHRT_MAX) {
			return true;
		}
		LOG << "SID: " << sid << " is NOT valid - no valid index found";
		return false;
	}

	void SpriteArray::assertSID(SID sid) const {
		SpriteArrayIndex &in = indices[sid];
		if (in.index == USHRT_MAX) {
			LOG << "SID: " << sid << " is NOT valid - no valid index found";
		}
		assert(in.index != USHRT_MAX);
	}

	SID SpriteArray::create(const v2& pos, const Texture& r, float rotation, float scaleX, float scaleY, const Color& color, int type, int layer) {
		SpriteArrayIndex &in = indices[free_dequeue];
		free_dequeue = in.next;
		in.index = num++;
		LOG << "creating: " << in.id << " at: " << in.index;
		ids[in.index] = in.id;
		positions[in.index] = pos;
		scales[in.index] = v2(scaleX, scaleY);
		rotations[in.index] = rotation;
		textures[in.index] = r;
		colors[in.index] = color;
		timers[in.index] = 0.0f;
		types[in.index] = type;
		layers[in.index] = layer;
		previous[in.index] = pos;
		extents[in.index] = v2(0, 0);
		shapeTypes[in.index] = SST_NONE;
		return in.id;
	}

	namespace sar {

		void clear(SpriteArray& array) {
			if ( array.buffer != 0 ) {
				for ( unsigned short i = 0; i < array.total; ++i ) {
					array.indices[i].id = i;
					array.indices[i].next = i + 1;
				}
				array.num = 0;
				array.free_dequeue = 0;
				array.free_enqueue = array.total - 1;
			}
		}

		void setPosition(SpriteArray& array,SID sid,const v2& pos) {
			SpriteArrayIndex &in = array.indices[sid];
			array.assertSID(sid);
			array.positions[in.index] = pos;
		}

		const v2& getPosition(const SpriteArray& array,SID sid) {
			SpriteArrayIndex &in = array.indices[sid];
			array.assertSID(sid);
			return array.positions[in.index];
		}

		void setScale(SpriteArray& array,SID sid,float sx,float sy) {
			SpriteArrayIndex &in = array.indices[sid];
			array.assertSID(sid);
			array.scales[in.index] = v2(sx,sy);
		}

		void scale(SpriteArray& array,SID sid,const v2& scale) {
			SpriteArrayIndex &in = array.indices[sid];
			array.assertSID(sid);
			array.scales[in.index] = scale;
		}

		void setColor(SpriteArray& array,SID sid,const Color& clr) {
			SpriteArrayIndex &in = array.indices[sid];
			array.assertSID(sid);
			array.colors[in.index] = clr;
		}

		void setAlpha(SpriteArray& array,SID sid,float alpha) {
			SpriteArrayIndex &in = array.indices[sid];
			array.assertSID(sid);
			array.colors[in.index].a = alpha;
		}

		void rotate(SpriteArray& array,SID sid,float angle) {
			SpriteArrayIndex &in = array.indices[sid];
			array.assertSID(sid);
			if ( angle > TWO_PI ) {
				angle -= TWO_PI;
			}
			array.rotations[in.index] = angle;
		}

		float getRotation(SpriteArray& array,SID sid) {
			SpriteArrayIndex &in = array.indices[sid];
			array.assertSID(sid);
			return array.rotations[in.index];
		}

		Sprite get(SpriteArray& array,SID sid) {
			Sprite sprite;
			SpriteArrayIndex &in = array.indices[sid];
			array.assertSID(sid);
			sprite.id = sid;
			sprite.position = array.positions[in.index];
			sprite.scale = array.scales[in.index];
			sprite.rotation = array.rotations[in.index];
			sprite.texture = array.textures[in.index];
			sprite.color = array.colors[in.index];
			sprite.type = array.types[in.index];
			sprite.layer = array.layers[in.index];
			return sprite;
		}

		void set(SpriteArray& array,SID sid,const Sprite& sprite) {
			SpriteArrayIndex &in = array.indices[sid];
			array.assertSID(sid);
			array.ids[in.index] = sid;
			array.positions[in.index] = sprite.position;
			array.scales[in.index] = sprite.scale;
			array.rotations[in.index] = sprite.rotation;
			array.textures[in.index] = sprite.texture;
			array.colors[in.index] = sprite.color;
			array.types[in.index] = sprite.type;
			array.layers[in.index] = sprite.layer;
		}
		/*
		SID create(SpriteArray& array,const v2& pos,const Texture& r,int type,int layer) {
			SpriteArrayIndex &in = array.indices[array.free_dequeue];
			array.free_dequeue = in.next;
			in.index = array.num++;
			LOG << "creating: " << in.id;
			array.ids[in.index] = in.id;
			array.positions[in.index] = pos;
			array.scales[in.index] = v2(1,1);
			array.rotations[in.index] = 0.0f;
			array.textures[in.index] = r;
			array.colors[in.index] = Color::WHITE;
			array.timers[in.index] = 0.0f;
			array.types[in.index] = type;
			array.layers[in.index] = layer;
			array.previous[in.index] = pos;
			array.extents[in.index] = v2(0,0);
			array.shapeTypes[in.index] = SST_NONE;
			return in.id;
		}
		*/
		void remove(SpriteArray& array,SID id) {
			SpriteArrayIndex &in = array.indices[id & INDEX_MASK];
			array.assertSID(id);
			SID currentID = array.ids[array.num - 1];			
			SpriteArrayIndex& next = array.indices[currentID & INDEX_MASK];
			LOG << "removing: " << id << " at " << in.index << " and moving: " << currentID << " at: " << next.index;
			array.ids[in.index] = array.ids[next.index];
			array.positions[in.index] = array.positions[next.index];
			array.scales[in.index] = array.scales[next.index];
			array.rotations[in.index] = array.rotations[next.index];
			array.textures[in.index] = array.textures[next.index];
			array.colors[in.index] = array.colors[next.index];
			array.timers[in.index] = array.timers[next.index];
			array.types[in.index] = array.types[next.index];
			array.layers[in.index] = array.layers[next.index];
			array.previous[in.index] = array.previous[next.index];
			array.extents[in.index] = array.extents[next.index];
			array.shapeTypes[in.index] = array.shapeTypes[next.index];
			--array.num;
			array.indices[currentID & INDEX_MASK].index = in.index;
			in.index = USHRT_MAX;
			array.indices[array.free_enqueue].next = id & INDEX_MASK;
			array.free_enqueue = id & INDEX_MASK;
			LOG << "free_enqueue: " << id;
		}

		void debug(SpriteArray& array) {
			for ( int i = 0; i < array.num; ++i ) {
				LOG << i << " : id: " << array.ids[i] << " type: " << array.types[i] << " pos: " << DBG_V2(array.positions[i]);
			}
		}

		void debug(SpriteArray& array,SID sid) {
			SpriteArrayIndex &in = array.indices[sid];
			array.assertSID(sid);
			LOG << "id      : " << sid;
			LOG << "index   : " << in.index;
			LOG << "position: " << DBG_V2(array.positions[in.index]);
			LOG << "scale   : " << DBG_V2(array.scales[in.index]);
			LOG << "rotation: " << RADTODEG(array.rotations[in.index]);
			LOG << "color   : " << DBG_CLR(array.colors[in.index]);
			LOG << "type    : " << array.types[in.index];
			LOG << "layer   : " << array.layers[in.index];
			LOG << "previous: " << DBG_V2(array.previous[in.index]);
			LOG << "extents : " << DBG_V2(array.extents[in.index]);
			LOG << "shape   : " << array.shapeTypes[in.index];
		}

		bool allocate(SpriteArray& array,int size) {
			if (size > array.total) {
				SpriteArray sad;
				int sz = size * (sizeof(SpriteArrayIndex) + sizeof(SID) + sizeof(v2) + sizeof(v2) + sizeof(float) + sizeof(Texture) + sizeof(Color) + sizeof(float) + sizeof(int) + sizeof(int) + 2 * sizeof(v2) + sizeof(SpriteShapeType));
				sad.buffer = (char*)ALLOC(sz);
				sad.total = size;
				sad.num = 0;
				sad.indices = (SpriteArrayIndex*)(sad.buffer);
				sad.ids = (SID*)(sad.indices + size);
				sad.positions = (v2*)(sad.ids + size);
				sad.scales = (v2*)(sad.positions + size);
				sad.rotations = (float*)(sad.scales + size);
				sad.textures = (ds::Texture*)(sad.rotations + size);
				sad.colors = (ds::Color*)(sad.textures + size);
				sad.timers = (float*)(sad.colors + size);
				sad.types = (int*)(sad.timers + size);
				sad.layers = (int*)(sad.types + size);
				sad.previous = (v2*)(sad.layers + size);
				sad.extents = (v2*)(sad.previous + size);
				sad.shapeTypes = (SpriteShapeType*)(sad.extents + size);
				clear(sad);
				if (array.buffer != 0) {
					memcpy(sad.indices, array.indices, array.num * sizeof(SpriteArrayIndex));
					memcpy(sad.ids, array.ids, array.num * sizeof(SID));
					memcpy(sad.positions, array.positions, array.num * sizeof(v2));
					memcpy(sad.scales, array.scales, array.num * sizeof(v2));
					memcpy(sad.rotations, array.rotations, array.num * sizeof(float));
					memcpy(sad.textures, array.textures, array.num * sizeof(Texture));
					memcpy(sad.colors, array.colors, array.num * sizeof(Color));
					memcpy(sad.timers, array.timers, array.num * sizeof(float));
					memcpy(sad.types, array.types, array.num * sizeof(int));
					memcpy(sad.layers, array.layers, array.num * sizeof(int));
					memcpy(sad.previous, array.previous, array.num * sizeof(v2));
					memcpy(sad.extents, array.extents, array.num * sizeof(v2));
					memcpy(sad.shapeTypes, array.shapeTypes, array.num * sizeof(SpriteShapeType));
					sad.free_dequeue = array.free_dequeue;
					sad.free_enqueue = array.free_enqueue;
					// FIXME: fill indices like clear
					sad.num = array.num;
					DEALLOC(array.buffer);
				}
				//else {
					//clear(sad);
				//}
				array = sad;
				return true;
			}
			return false;
		}

	}
}