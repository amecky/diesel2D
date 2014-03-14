#include "SpriteBatchRenderComponent.h"
#include "..\..\base\Engine.h"

namespace ds {

SpriteBatchRenderComponent::SpriteBatchRenderComponent(void)
{
}


SpriteBatchRenderComponent::~SpriteBatchRenderComponent(void)
{
}

void SpriteBatchRenderComponent::prepare() {
}

void SpriteBatchRenderComponent::render() {
	assert(m_Material != 0);
	assert(m_Material->getTexture(0) != 0);	
	IDirect3DDevice9 * pDevice = gEngine->getDevice();
	Viewport *vp = gEngine->getViewport();
	Vector2f vpPos = vp->getPosition();
	pSprite->Begin( D3DXSPRITE_ALPHABLEND );
	Vector2f vCenter( 0.0f, 0.0f );
	size_t total = m_Sprites.size();
	for ( size_t i = 0; i < total ;i++) {		
		Sprite *sp = m_Sprites[i];			
		if ( sp->visible ) {
			Vector3f pos = sp->position;
			pos.x = pos.x - vpPos.x;
			pos.y = pos.y - vpPos.y;
			vCenter = Vector2f((sp->offset.right-sp->offset.left)/2,(sp->offset.bottom - sp->offset.top)/2);
			Vector2f trans = Vector2f(pos.x,pos.y);
			D3DXMATRIX mat;
			Vector2f scaling(sp->scale.x,sp->scale.y);
			D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,&vCenter,sp->rotation,&trans);
			pSprite->SetTransform(&mat);
			pSprite->Draw( m_Material->getTexture(0)->texture,&sp->offset,NULL,NULL,sp->color);
		}
	}
	pSprite->End();		
}

}
