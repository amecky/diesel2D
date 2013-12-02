#include "font.h"
#include "..\utils\Log.h"
#include "PlainTextReader.h"
#include "..\pipeline\PAKWriter.h"
#include "..\pipeline\PAKReader.h"

namespace ds {

	namespace font {

		// -------------------------------------------------------
		// Calculate size of text
		// -------------------------------------------------------
		Vec2 calculateSize(const BitmapFont& bitmapFont,const std::string& text,float scaleX,float scaleY) {
			float padding = 0.0f;
			Vec2 ret(0.0f,0.0f);
			ret.y = bitmapFont.charHeight * scaleY;
			for ( size_t cnt = 0; cnt < text.length(); ++cnt ) {
				char c = text[cnt];
				CharDef cd = bitmapFont.getCharDef(c);
				padding = (cd.width+2)  * scaleX;		
				float dimX = cd.width * scaleX;
				ret.x += dimX + 4;				
			}
			return ret;
		}

		// -------------------------------------------------------
		// Create text
		// -------------------------------------------------------
		void createText(const BitmapFont& bitmapFont,const Vec2& pos,const std::string& text,const Color& color,std::vector<Sprite>& sprites,float scaleX,float scaleY) {
			float x = pos.x;
			float y = pos.y;
			float padding = 0.0f;
			float z = 0.0f;			
			for ( size_t cnt = 0; cnt < text.length(); ++cnt ) {
				char c = text[cnt];
				CharDef cd = bitmapFont.getCharDef(c);
				padding = (cd.width+2)  * scaleX;		
				float dimX = cd.width * scaleX;
				float dimY = bitmapFont.charHeight * scaleY;
				// quad buffer is centered by default so adjust position!
				Sprite sp;
				sp.position = ds::Vec2(x + dimX * 0.5f ,y + dimY * 0.5f);
				//sp.textureRect = 
				sp.textureRect = cd.texureRect;
				sp.color = color;
				sp.scaleX = scaleX;
				sp.scaleY = scaleY;
				sprites.push_back(sp);		
				x += dimX +4;
			}
		}

		bool load(const char* name,Renderer* renderer,int textureId,BitmapFont& font,const Color& fillColor) {
			char fileName[256];
			sprintf(fileName,"content\\resources\\%s.settings",name);
			NewSettingsReader reader;
			if ( reader.parse(fileName)) {
				reader.get<uint32>("start_char",&font.startChar);
				reader.get<uint32>("end_char",&font.endChar);
				reader.get<uint32>("char_height",&font.charHeight);
				reader.get<uint32>("grid_height",&font.gridHeight);
				reader.get<uint32>("start_x",&font.startX);
				reader.get<uint32>("start_y",&font.startY);
				reader.get<uint32>("width",&font.width);
				reader.get<uint32>("height",&font.height);
				reader.get<uint32>("padding",&font.padding);
				reader.get<uint32>("texture_size",&font.textureSize);				
				assert(font.textureSize != 0 );
				renderer->initializeBitmapFont(font,textureId,fillColor);
				return true;
			}
			return false;
		}

		// -------------------------------------------------------
		// Load BitmapFont from json file
		// -------------------------------------------------------
		bool load2(const char* name,Renderer* renderer,int textureId,BitmapFont& font,const Color& fillColor) {
			char fileName[256];
			sprintf(fileName,"content\\resources\\%s.json",name);
			LOGC(logINFO,"font") << "trying to load bitmap font " << fileName;
			JSONReader reader;
			if ( reader.parse(fileName)) {
				Category* cat = reader.getCategory("bitmap_font");
				if ( cat != 0 ) {
					font.startChar = cat->read<int>("start_char",0);
					font.endChar = cat->read<int>("end_char",0);
					font.charHeight = cat->read<int>("char_height",0);
					font.gridHeight = cat->read<int>("grid_height",0);
					font.startX = cat->read<int>("start_x",0);
					font.startY = cat->read<int>("start_y",0);
					font.width = cat->read<int>("width",0);
					font.height = cat->read<int>("height",0);
					font.padding = cat->read<int>("padding",0);
					font.textureSize = cat->read<int>("texture_size",0);
					assert(font.textureSize != 0 );
					renderer->initializeBitmapFont(font,textureId,fillColor);					
					// save to binary
					LOGC(logINFO,"font") << "Saving binary format";
					char binFile[256];
					sprintf(binFile,"content\\data\\%s.dsb",name);
					PAKWriter writer;
					if ( writer.open(binFile) ) {
						writer.writeInt(font.startChar);
						writer.writeInt(font.endChar);
						writer.writeInt(font.charHeight);
						writer.writeInt(font.gridHeight);
						writer.writeInt(font.startX);
						writer.writeInt(font.startY);
						writer.writeInt(font.width);
						writer.writeInt(font.height);
						writer.writeInt(font.padding);
						writer.writeInt(font.textureSize);
						writer.close();
					}
					else {
						LOGC(logERROR,"font") << "Error while exporting binary data";
					}
					return true;
				}
				else {
					LOGC(logERROR,"font") << "No category 'bitmap_font' found";
					return false;
				}
			}
			else {
				LOGC(logERROR,"font") << "Failed to load bitmap font " << fileName;
				return false;
			}
		}

		// -------------------------------------------------------
		// Load BitmapFont from dsb file
		// -------------------------------------------------------
		bool loadBinary(const char* name,Renderer* renderer,int textureId,BitmapFont& font,const Color& fillColor) {
			char fileName[256];
			sprintf(fileName,"content\\data\\%s.dsb",name);
			LOGC(logINFO,"font") << "trying to load bitmap font " << fileName;
			PAKReader reader;
			if ( reader.open(fileName)) {
				font.startChar = reader.readInt();
				font.endChar = reader.readInt();
				font.charHeight = reader.readInt();
				font.gridHeight = reader.readInt();
				font.startX = reader.readInt();
				font.startY = reader.readInt();
				font.width = reader.readInt();
				font.height = reader.readInt();
				font.padding = reader.readInt();
				font.textureSize = reader.readInt();
				assert(font.textureSize != 0 );
				renderer->initializeBitmapFont(font,textureId,fillColor);				
				return true;
			}
			else {
				LOGC(logERROR,"font") << "Failed to load bitmap font " << fileName;
				return false;
			}
		}
	}
}