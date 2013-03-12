#include "UI.h"
#include "..\base\Engine.h"
#include "..\utils\Log.h"
#include "..\utils\StringUtils.h"
#include "UIDialog.h"
#include "UICheckbox.h"
#include "UITextbox.h"
#include "..\content\BitmapFont.h"

namespace ds {

namespace UI {

Vec3 getVec3(UIDialog* dialog,uint32 id1,uint32 id2,uint32 id3) {
	Vec3 ret;
	ret.x = getFloat(dialog,id1);
	ret.y = getFloat(dialog,id2);
	ret.z = getFloat(dialog,id3);	
	return ret;
}

bool getBool(UIDialog* dialog,uint32 id,bool defaultValue) {
	if ( dialog->getCheckbox(id) != 0 ) {
		return dialog->getCheckbox(id)->getValue();
	}	
	return defaultValue;
}

float getFloat(UIDialog* dialog,uint32 id,float defaultValue) {
	if ( dialog->getTextbox(id) != 0 ) {
		return string::from_string<float>(dialog->getTextbox(id)->getValue());
	}
	return defaultValue;
}

void setVec3(UIDialog* dialog,const Vec3& v,uint32 id1,uint32 id2,uint32 id3) {
	setFloat(dialog,v.x,id1);
	setFloat(dialog,v.y,id2);
	setFloat(dialog,v.z,id3);
}

void setFloat(UIDialog* dialog,float value,uint32 id) {	
	if ( dialog->getTextbox(id) != 0 ) {
		dialog->getTextbox(id)->setValue(string::to_string(value));
	}
}

Color getColor(UIDialog* dialog,uint32 id1,uint32 id2,uint32 id3,uint32 id4) {
	Color c;
	c.r = getFloat(dialog,id1);
	c.g = getFloat(dialog,id2);
	c.b = getFloat(dialog,id3);
	c.a = getFloat(dialog,id4);
	return c;
}

void setColor(UIDialog* dialog,const Color& v,uint32 id1,uint32 id2,uint32 id3,uint32 id4) {
	setFloat(dialog,v.r,id1);
	setFloat(dialog,v.g,id2);
	setFloat(dialog,v.b,id3);
	setFloat(dialog,v.a,id4);
}

int calculateTextSize(float scale,BitmapFont* font,const std::string& text) {
	int size = 0;
	float padding = 1.0f;
	float texSize = (float)font->getTextureSize();
	float z = 0;
	for ( size_t cnt = 0; cnt < text.length(); ++cnt ) {
		char c = text[cnt];
		CharDef cd = font->getCharDef(c);
		size += padding;
		//padding = 2.0f;
		size += cd.width * scale;		
	}
	return size;
}

void fillText(const Vec2& position,float scale,BitmapFont* font,const std::string& text,const Color& color,std::vector<TransformedTextureColorVertex>& vertices) {
	float padding = 0.0f;
	float x = position.x;
	float y = position.y;
	float texSize = (float)font->getTextureSize();
	float z = 0;
	for ( size_t cnt = 0; cnt < text.length(); ++cnt ) {
		char c = text[cnt];
		CharDef cd = font->getCharDef(c);
		x += padding;
		padding = (cd.width+2)  * scale;
		float dimX = cd.width * scale;
		float dimY = font->getCharHeight() * scale;
		vertices.push_back(TransformedTextureColorVertex(x ,y , z, cd.u1, cd.v1,color));
		vertices.push_back(TransformedTextureColorVertex(x + dimX , y , z, cd.u2, cd.v1,color));		
		vertices.push_back(TransformedTextureColorVertex(x + dimX , y + dimY , z, cd.u2, cd.v2,color));	
		vertices.push_back(TransformedTextureColorVertex(x , y + dimY , z, cd.u1, cd.v2,color));
	}
}

void fillQuad(float x,float y,float dimX,float dimY,const Rect& textureRect,std::vector<TransformedTextureColorVertex>& vertices) {
	dimX = textureRect.width() + 0.5f;
	dimY = textureRect.height() + 0.5f;
	float xp = (float)x - 0.5f;
	float yp = (float)y - 0.5f;
	int z = 0;
	float u1,v1,u2,v2;
	math::getTextureCoordinates(textureRect,512,&u1,&v1,&u2,&v2);	
	vertices.push_back(TransformedTextureColorVertex(xp ,yp , z, u1, v1,Color(1.0f,1.0f,1.0f,1.0f)));
	vertices.push_back(TransformedTextureColorVertex(xp + dimX , yp , z, u2, v1,Color(1.0f,1.0f,1.0f,1.0f)));	
	vertices.push_back(TransformedTextureColorVertex(xp + dimX , yp + dimY , z, u2, v2,Color(1.0f,1.0f,1.0f,1.0f)));	
	vertices.push_back(TransformedTextureColorVertex(xp , yp + dimY , z, u1, v2,Color(1.0f,1.0f,1.0f,1.0f)));
}

}

}
