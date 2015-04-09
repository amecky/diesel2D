#include "SpriteArray.h"
#include "..\utils\Log.h"

namespace ds {

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

		void setPosition(SpriteArray& array,SID sid,const Vector2f& pos) {
			SpriteArrayIndex &in = array.indices[sid];
			assert(in.index != USHRT_MAX);
			array.positions[in.index] = pos;
		}

		const Vector2f& getPosition(const SpriteArray& array,SID sid) {
			SpriteArrayIndex &in = array.indices[sid];
			assert(in.index != USHRT_MAX);
			return array.positions[in.index];
		}

		void setScale(SpriteArray& array,SID sid,float sx,float sy) {
			SpriteArrayIndex &in = array.indices[sid];
			assert(in.index != USHRT_MAX);
			array.scales[in.index] = Vector2f(sx,sy);
		}

		void scale(SpriteArray& array,SID sid,const Vector2f& scale) {
			SpriteArrayIndex &in = array.indices[sid];
			assert(in.index != USHRT_MAX);
			array.scales[in.index] = scale;
		}

		void setColor(SpriteArray& array,SID sid,const Color& clr) {
			SpriteArrayIndex &in = array.indices[sid];
			assert(in.index != USHRT_MAX);
			array.colors[in.index] = clr;
		}

		void setAlpha(SpriteArray& array,SID sid,float alpha) {
			SpriteArrayIndex &in = array.indices[sid];
			assert(in.index != USHRT_MAX);
			array.colors[in.index].a = alpha;
		}

		void rotate(SpriteArray& array,SID sid,float angle) {
			SpriteArrayIndex &in = array.indices[sid];
			assert(in.index != USHRT_MAX);
			if ( angle > TWO_PI ) {
				angle -= TWO_PI;
			}
			array.rotations[in.index] = angle;
		}

		float getRotation(SpriteArray& array,SID sid) {
			SpriteArrayIndex &in = array.indices[sid];
			assert(in.index != USHRT_MAX);
			return array.rotations[in.index];
		}

		Sprite get(SpriteArray& array,SID sid) {
			Sprite sprite;
			SpriteArrayIndex &in = array.indices[sid];
			assert(in.index != USHRT_MAX);
			sprite.id = sid;
			sprite.position = array.positions[in.index];
			sprite.scale = array.scales[in.index];
			sprite.rotation = array.rotations[in.index];
			sprite.texture = array.textures[in.index];
			sprite.color = array.colors[in.index];
			sprite.type = array.types[in.index];
			return sprite;
		}

		void set(SpriteArray& array,SID sid,const Sprite& sprite) {
			SpriteArrayIndex &in = array.indices[sid];
			assert(in.index != USHRT_MAX);
			array.ids[in.index] = sid;
			array.positions[in.index] = sprite.position;
			array.scales[in.index] = sprite.scale;
			array.rotations[in.index] = sprite.rotation;
			array.textures[in.index] = sprite.texture;
			array.colors[in.index] = sprite.color;
			array.types[in.index] = sprite.type;
		}

		SID create(SpriteArray& array,const Vector2f& pos,const Texture& r,int type) {
			SpriteArrayIndex &in = array.indices[array.free_dequeue];
			array.free_dequeue = in.next;
			in.index = array.num++;
			array.ids[in.index] = in.id;
			array.positions[in.index] = pos;
			array.scales[in.index] = Vector2f(1,1);
			array.rotations[in.index] = 0.0f;
			array.textures[in.index] = r;
			array.colors[in.index] = Color::WHITE;
			array.timers[in.index] = 0.0f;
			array.types[in.index] = type;
			return in.id;
		}

		void remove(SpriteArray& array,SID id) {
			SpriteArrayIndex &in = array.indices[id & INDEX_MASK];
			assert(in.index != USHRT_MAX);
			ID currentID = array.ids[array.num - 1];
			SpriteArrayIndex& next = array.indices[currentID & INDEX_MASK];
			array.ids[in.index] = array.ids[next.index];
			array.positions[in.index] = array.positions[next.index];
			array.scales[in.index] = array.scales[next.index];
			array.rotations[in.index] = array.rotations[next.index];
			array.textures[in.index] = array.textures[next.index];
			array.colors[in.index] = array.colors[next.index];
			array.timers[in.index] = array.timers[next.index];
			array.types[in.index] = array.types[next.index];
			--array.num;
			array.indices[currentID & INDEX_MASK].index = in.index;
			in.index = USHRT_MAX;
			array.indices[array.free_enqueue].next = id & INDEX_MASK;
			array.free_enqueue = id & INDEX_MASK;
		}

		void debug(SpriteArray& array) {
			for ( int i = 0; i < array.num; ++i ) {
				LOG << i << " : id: " << array.ids[i] << " pos: " << DBG_V2(array.positions[i]);
			}
		}

		void debug(SpriteArray& array,SID sid) {
			SpriteArrayIndex &in = array.indices[sid];
			assert(in.index != USHRT_MAX);
			LOG << "id      : " << sid;
			LOG << "index   : " << in.index;
			LOG << "position: " << DBG_V2(array.positions[in.index]);
			LOG << "scale   : " << DBG_V2(array.scales[in.index]);
			LOG << "rotation: " << RADTODEG(array.rotations[in.index]);
			LOG << "color   : " << DBG_CLR(array.colors[in.index]);
			LOG << "type    : " << array.types[in.index];
		}

	}
}