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
		if (num + 1 > capacity) {
			allocate(capacity * 2 + 8);
		}
		ID id = 0;
		if (freeList.empty()) {
			id = current++;
		}
		else {
			id = freeList.back();
			freeList.pop_back();
		}
		SpriteArrayIndex &in = indices[id];
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

	void SpriteArray::remove(SID id) {
		SpriteArrayIndex& in = indices[id];
		assert(in.index != UINT16_MAX);
		assert(id < capacity);
		if (num > 1) {
			int last = num - 1;
			ID lastID = ids[last];
			SpriteArrayIndex& lastIn = indices[lastID];
			if (last != in.index) {
				ids[in.index] = ids[last];
				positions[in.index] = positions[last];
				scales[in.index] = scales[last];
				rotations[in.index] = rotations[last];
				textures[in.index] = textures[last];
				colors[in.index] = colors[last];
				timers[in.index] = timers[last];
				types[in.index] = types[last];
				layers[in.index] = layers[last];
				previous[in.index] = previous[last];
				extents[in.index] = extents[last];
				shapeTypes[in.index] = shapeTypes[last];
			}
			lastIn.index = in.index;
		}
		in.index = UINT16_MAX;
		freeList.push_back(id);
		--num;
	}

	void SpriteArray::allocate(uint16_t size) {
		if (size > capacity) {
			int sz = size * (sizeof(SpriteArrayIndex) + sizeof(SID) + sizeof(v2) + sizeof(v2) + sizeof(float) + sizeof(Texture) + sizeof(Color) + sizeof(float) + sizeof(uint16_t) + sizeof(uint16_t) + 2 * sizeof(v2) + sizeof(SpriteShapeType));
			char* b = (char*)ALLOC(sz);
			capacity = size;
			indices = (SpriteArrayIndex*)b;
			ids = (SID*)(indices + size);
			positions = (v2*)(ids + size);
			scales = (v2*)(positions + size);
			rotations = (float*)(scales + size);
			textures = (ds::Texture*)(rotations + size);
			colors = (ds::Color*)(textures + size);
			timers = (float*)(colors + size);
			types = (uint16_t*)(timers + size);
			layers = (uint16_t*)(types + size);
			previous = (v2*)(layers + size);
			extents = (v2*)(previous + size);
			shapeTypes = (SpriteShapeType*)(extents + size);
			//sad.clear();
			if (buffer != 0) {
				memcpy(indices, buffer, num * sizeof(SpriteArrayIndex));
				int index = num * sizeof(SpriteArrayIndex);
				memcpy(ids, buffer + index, num * sizeof(SID));
				index += num * sizeof(SID);
				memcpy(positions, buffer + index, num * sizeof(v2));
				index += num * sizeof(v2);
				memcpy(scales, buffer + index, num * sizeof(v2));
				index += num * sizeof(v2);
				memcpy(rotations, buffer + index, num * sizeof(float));
				index += num * sizeof(float);
				memcpy(textures, buffer + index, num * sizeof(Texture));
				index += num * sizeof(Texture);
				memcpy(colors, buffer + index, num * sizeof(Color));
				index += num * sizeof(Color);
				memcpy(timers, buffer + index, num * sizeof(float));
				index += num * sizeof(float);
				memcpy(types, buffer + index, num * sizeof(uint16_t));
				index += num * sizeof(uint16_t);
				memcpy(layers, buffer + index, num * sizeof(uint16_t));
				index += num * sizeof(uint16_t);
				memcpy(previous, buffer + index, num * sizeof(v2));
				index += num * sizeof(v2);
				memcpy(extents, buffer + index, num * sizeof(v2));
				index += num * sizeof(v2);
				memcpy(shapeTypes, buffer + index, num * sizeof(SpriteShapeType));
				for (int i = num; i < capacity; ++i) {
					indices[i].id = i;
					indices[i].index = UINT16_MAX;
				}
				DEALLOC(buffer);
			}
			buffer = b;
		}
	}

	void SpriteArray::debug() {
		for (int i = 0; i < num; ++i) {
			LOG << i << " : id: " << ids[i] << " type: " << types[i] << " pos: " << DBG_V2(positions[i]);
		}
	}

	void SpriteArray::debug(SID sid) {
		SpriteArrayIndex &in = indices[sid];
		assertSID(sid);
		LOG << "id      : " << sid;
		LOG << "index   : " << in.index;
		LOG << "position: " << DBG_V2(positions[in.index]);
		LOG << "scale   : " << DBG_V2(scales[in.index]);
		LOG << "rotation: " << RADTODEG(rotations[in.index]);
		LOG << "color   : " << DBG_CLR(colors[in.index]);
		LOG << "type    : " << types[in.index];
		LOG << "layer   : " << layers[in.index];
		LOG << "previous: " << DBG_V2(previous[in.index]);
		LOG << "extents : " << DBG_V2(extents[in.index]);
		LOG << "shape   : " << shapeTypes[in.index];
	}

	namespace sar {
		/*
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
		*/
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
		

		

		

		

	}
}