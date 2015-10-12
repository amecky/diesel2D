#include "SpriteTemplates.h"
#include "..\compiler\Converter.h"
#include "..\ui\IMGUI.h"

namespace ds {

	SpriteTemplates::SpriteTemplates() : _num(0) , _state(1) {
		for (int i = 0; i < 256; ++i) {
			_IDs[i] = -1;
		}
	}


	SpriteTemplates::~SpriteTemplates() {
	}

	const bool SpriteTemplates::contains(int id) const {
		return _IDs[id] != -1;
	}

	bool SpriteTemplates::get(const char* name, Sprite* sprite) {
		IdString hash = string::murmur_hash(name);
		if (_map.find(hash) != _map.end()) {
			Sprite& sp = _map[hash];
			sprite->id = 0;
			sprite->position = sp.position;
			sprite->rotation = sp.rotation;
			sprite->scale = sp.scale;
			sprite->texture = sp.texture;
			sprite->type = sp.type;
			sprite->color = sp.color;
			return true;
		}
		return false;

	}

	/*
	const Sprite& SpriteTemplates::get(int id) const {
		const int index = m_IDs[id];
		return m_Sprites[index];
	}
	*/
	void SpriteTemplates::load(BinaryLoader* loader) {
		while (loader->openChunk() == 0) {
			if (loader->getChunkID() == CHNK_SPRITE ) {
				IdString name;
				loader->read(&name);
				//int id = -1;
				//loader->read(&id);
				if (name != 0) {
					Sprite sp;// = m_Sprites[m_Num++];
					int tid = 0;
					loader->read(&tid);
					loader->read(&sp.position);
					Rect r;
					loader->read(&r);
					sp.texture = ds::math::buildTexture(r);
					sp.texture.textureID = tid;
					loader->read(&sp.scale);
					loader->read(&sp.rotation);
					loader->read(&sp.color);
					loader->read(&sp.type);
					//m_IDs[id] = m_Num - 1;
					_map[name] = sp;
					//LOGC("SpriteTemplates") << "added sprite: " << id << " at " << m_IDs[id];
				}
			}
			loader->closeChunk();
		}
	}

	/*
	"sprite" {
	"name" : "background" ,
	"texture_id" : "0" ,
	"position" : "512,384" ,
	"rect" : "0,512,512,384" ,
	"scale" : "2,2" ,
	"rotation" : "0" ,
	"color" : "32,32,32,255" ,
	"type" : "-1"
	}
	*/
	void SpriteTemplates::showDialog(v2* pos,const char* name) {
		gui::start(pos);
		if (gui::begin("Sprite Templates", &_state)) {		
			IdString hash = string::murmur_hash(name);
			if (_map.find(hash) != _map.end()) {
				Sprite& sprite = _map[hash];
				ds::Rect r = sprite.texture.rect;
				gui::InputInt(2, "Texture_id", &sprite.texture.textureID);
				gui::InputVec2(3, "Position", &sprite.position);
				gui::InputRect(4, "TextureRect", &r);
				gui::InputVec2(5, "Scale", &sprite.scale);
				gui::InputFloat(6, "Rotation", &sprite.rotation);
				gui::InputColor(7, "Color", &sprite.color);
				gui::InputInt(8, "Type", &sprite.type);
				sprite.texture = math::buildTexture(r);
				gui::beginGroup();
				if (gui::Button(9, "OK")) {
					LOG << "OK pressed";
				}
				if (gui::Button(10, "Cancel")) {
					LOG << "Cancel clicked";
				}
				gui::endGroup();
			}
		}
		gui::end();
	}
}