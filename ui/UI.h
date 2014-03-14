#pragma once
#include "UIDialog.h"

namespace ds {

class UIControl;
class UIDialog;

namespace UI {


Vector3f getVector3f(UIDialog* dialog,uint32 id1,uint32 id2,uint32 id3);
void setVector3f(UIDialog* dialog,const Vector3f& v,uint32 id1,uint32 id2,uint32 id3);
float getFloat(UIDialog* dialog,uint32 id,float defaultValue = 0.0f);
bool getBool(UIDialog* dialog,uint32 id,bool defaultValue = false);
void setFloat(UIDialog* dialog,float value,uint32 id);
Color getColor(UIDialog* dialog,uint32 id1,uint32 id2,uint32 id3,uint32 id4);
void setColor(UIDialog* dialog,const Color& v,uint32 id1,uint32 id2,uint32 id3,uint32 id4);
void fillText(const Vector2f& position,float scale,BitmapFont* font,const std::string& text,const Color& color,std::vector<TransformedTextureColorVertex>& vertices);
int calculateTextSize(float scale,BitmapFont* font,const std::string& text);
void fillQuad(float x,float y,float dimX,float dimY,const Rect& textureRect,std::vector<TransformedTextureColorVertex>& vertices);
}

};