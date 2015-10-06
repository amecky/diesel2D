#include "font.h"
#include "..\utils\Log.h"
#include "PlainTextReader.h"

namespace ds {

	namespace font {

		// -------------------------------------------------------
		// Calculate size of text
		// -------------------------------------------------------
		Vector2f calculateSize(const BitmapFont& bitmapFont,const std::string& text,int padding,float scaleX,float scaleY) {
			float cPadding = 0.0f;
			v2 ret(0.0f,0.0f);
			ret.y = bitmapFont.getCharHeight() * scaleY;
			for ( size_t cnt = 0; cnt < text.length(); ++cnt ) {
				char c = text[cnt];
				if (bitmapFont.contains(c)) {
					CharDef cd = bitmapFont.getCharDef(c);
					cPadding = (cd.width + padding)  * scaleX;
					float dimX = cd.width * scaleX;
					ret.x += dimX + padding;
				}
			}
			return ret;
		}

		// -------------------------------------------------------
		// Calculate size of text
		// -------------------------------------------------------
		Vector2f calculateLimitedSize(const BitmapFont& bitmapFont, const std::string& text, int chars, int padding, float scaleX, float scaleY) {
			float cPadding = 0.0f;
			v2 ret(0.0f, 0.0f);
			ret.y = bitmapFont.getCharHeight() * scaleY;
			int total = chars;
			if (total >= text.length()) {
				total = text.length();
			}
			for (size_t cnt = 0; cnt < total; ++cnt) {
				char c = text[cnt];
				if (bitmapFont.contains(c)) {
					CharDef cd = bitmapFont.getCharDef(c);
					cPadding = (cd.width + padding)  * scaleX;
					float dimX = cd.width * scaleX;
					ret.x += dimX + padding;
				}
			}
			return ret;
		}
		
		// -------------------------------------------------------
		// Create text
		// -------------------------------------------------------
		void createText(const BitmapFont& bitmapFont,const Vector2f& pos,const std::string& text,const Color& color,std::vector<Sprite>& sprites,float scaleX,float scaleY) {
			float x = pos.x;
			float y = pos.y;
			float padding = 0.0f;
			float z = 0.0f;			
			for ( size_t cnt = 0; cnt < text.length(); ++cnt ) {
				char c = text[cnt];
				if (bitmapFont.contains(c)) {
					CharDef cd = bitmapFont.getCharDef(c);
					padding = (cd.width + 2)  * scaleX;
					float dimX = cd.width * scaleX;
					float dimY = bitmapFont.getCharHeight() * scaleY;
					// quad buffer is centered by default so adjust position!
					Sprite sp;
					sp.position = (Vector2f(x + dimX * 0.5f, y + dimY * 0.5f));
					sp.texture = math::buildTexture(cd.texureRect);
					sp.color = color;
					sp.scale = Vector2f(scaleX, scaleY);
					sprites.push_back(sp);
					x += dimX + 4;
				}
			}
		}

		void createCharacter(const BitmapFont& bitmapFont, const Vector2f& pos, char c, const Color& color, std::vector<Sprite>& sprites, float scaleX, float scaleY) {
			float x = pos.x;
			float y = pos.y;
			float padding = 0.0f;
			float z = 0.0f;
			CharDef cd = bitmapFont.getCharDef(c);
			padding = (cd.width + 2)  * scaleX;
			float dimX = cd.width * scaleX;
			float dimY = bitmapFont.getCharHeight() * scaleY;
			// quad buffer is centered by default so adjust position!
			Sprite sp;
			sp.position = (Vector2f(x + dimX * 0.5f, y + dimY * 0.5f));
			sp.texture = math::buildTexture(cd.texureRect);
			sp.color = color;
			sp.scale = Vector2f(scaleX, scaleY);
			sprites.push_back(sp);
				
		}

	}
}