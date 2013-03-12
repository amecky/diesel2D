#include "TexGen.h"
#include "..\pipeline\PAKReader.h"
#include "..\utils\Log.h"
#include "..\pipeline\PAKWriter.h"


namespace ds {

	namespace texture {

		void saveRawTexture(Renderer* renderer,int id,const char* fileName) {
			PAKWriter writer;
			if ( writer.open(fileName)) {
				writer.writeInt(1895);

				writer.close();
			}
		}
		// -------------------------------------------------------
		// Load raw texture from file
		// -------------------------------------------------------
		int loadRawTexture(Renderer* renderer,const char* fileName) {
			PAKReader reader;
			if ( reader.open(fileName)) {
				int magic = readI(reader);
				LOG(logINFO) << "magic: " << magic;		

				int width = readI(reader);
				int height = readI(reader);
				int type = readI(reader);
				int jpeg = readI(reader);
				LOG(logINFO) << "dim: " << width << " : " << height;
				LOG(logINFO) << "type: " << type << " jpeg:" << jpeg;
				int id = renderer->createTexture(width,height);
				D3DLOCKED_RECT r = renderer->lockTexture(id);
				int length = width * height;
				uchar c[4];
				for ( int i = 0; i < length; ++i ) {
					for ( int j = 0 ; j < 4; ++j ) {
						c[j] = reader.readUChar();
					}
					uchar * pBits = (uchar *)r.pBits;
					int p = (i)*4;
					pBits[p]   = c[0];
					pBits[p+1] = c[1];
					pBits[p+2] = c[2];
					pBits[p+3] = c[3];
				}


				renderer->unlockTexture(id);
				reader.close();
				return id;
			}	
			return -1;
		}

		// -------------------------------------------------------
		// Converts java integer to c int
		// -------------------------------------------------------
		int readI(PAKReader& reader) {
			int c[4];
			for ( int i =0; i < 4; ++i ) {
				c[i] = (int)reader.readChar();		
			}
			return (c[0] * 256 + c[1] ) * 65536 + c[2] * 256 + c[3];
		}
	}
}