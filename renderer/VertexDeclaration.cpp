#include "VertexDeclaration.h"
#include "Renderer.h"

namespace ds {

VertexDeclaration::VertexDeclaration(const char* handle) {
	 m_Decl = NULL;
	 m_Size = 0;
}

VertexDeclaration::VertexDeclaration() {
	m_Decl = NULL;
	m_Size = 0;
}

VertexDeclaration::~VertexDeclaration() {
	SAFE_RELEASE(m_Decl);
}

void VertexDeclaration::addElement(const VDTYPE& type,const VDUSAGE& usage) {
	VDElement e;
	e.usage = usage;
	e.type = type;
	m_DecElements.push_back(e);
}


void VertexDeclaration::create() {
	if ( m_Decl == NULL ) {
		D3DVERTEXELEMENT9* vdElements = new D3DVERTEXELEMENT9[m_DecElements.size()+1];
		int offset = 0;
		for ( size_t i = 0; i < m_DecElements.size(); ++i ) {			
			vdElements[i].Stream = 0;
			vdElements[i].Offset = offset;			
			switch ( m_DecElements[i].type ) {
				case VT_FLOAT2 : 
					vdElements[i].Type = D3DDECLTYPE_FLOAT2;
					offset += 8;
					m_Size += 2;
					break;
				case VT_FLOAT3 : 
					vdElements[i].Type = D3DDECLTYPE_FLOAT3;
					offset += 12;
					m_Size += 3;
					break;
				case VT_FLOAT4 : 
					vdElements[i].Type = D3DDECLTYPE_FLOAT4;
					offset += 16;
					m_Size += 4;
					break;
				case VT_COLOR : 
					vdElements[i].Type = D3DDECLTYPE_D3DCOLOR;
					offset += 4;
					m_Size += 2;
					break;
			}
			
			vdElements[i].Method = D3DDECLMETHOD_DEFAULT;

			switch ( m_DecElements[i].usage ) {
				case VDU_POSITION : 
					vdElements[i].Usage = D3DDECLUSAGE_POSITION;
					break;
				case VDU_POSITIONT : 
					vdElements[i].Usage = D3DDECLUSAGE_POSITIONT;
					break;
				case VDU_COLOR : 
					vdElements[i].Usage = D3DDECLUSAGE_COLOR;
					break;
				case VDU_TANGENT : 
					vdElements[i].Usage = D3DDECLUSAGE_TANGENT;
					break;
				case VDU_BINORMAL : 
					vdElements[i].Usage = D3DDECLUSAGE_BINORMAL;
					break;
				case VDU_NORMAL : 
					vdElements[i].Usage = D3DDECLUSAGE_NORMAL;
					break;
				case VDU_TEXCOORD : 
					vdElements[i].Usage = D3DDECLUSAGE_TEXCOORD;
					break;
			}
			vdElements[i].UsageIndex = 0;			
		}	
		// closing element
		vdElements[m_DecElements.size()].Stream = 0xff;
		vdElements[m_DecElements.size()].Offset = 0;
		vdElements[m_DecElements.size()].Type = D3DDECLTYPE_UNUSED;
		vdElements[m_DecElements.size()].Method = 0;
		vdElements[m_DecElements.size()].Usage = 0;
		vdElements[m_DecElements.size()].UsageIndex = 0;

		renderContext.device->CreateVertexDeclaration(vdElements, &m_Decl);
		
		delete[] vdElements;
	}
}

void VertexDeclaration::clear() {
	SAFE_RELEASE(m_Decl);
}

};