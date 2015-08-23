#include "VertexDeclaration.h"
#include "..\renderer\graphics.h"

namespace ds {

VertexDeclaration::VertexDeclaration(const char* handle) {
	 m_Decl = 0;
	 m_Size = 0;
}

VertexDeclaration::VertexDeclaration() {
	m_Decl = 0;
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
	if ( m_Decl == 0 ) {
		int counter[10];
		for (int i = 0; i < 10; ++i) {
			counter[i] = 0;
		}
		D3DVERTEXELEMENT9* vdElements = new D3DVERTEXELEMENT9[m_DecElements.size()+1];
		int offset = 0;
		for ( size_t i = 0; i < m_DecElements.size(); ++i ) {			
			vdElements[i].Stream = 0;
			vdElements[i].Offset = offset;			
			switch ( m_DecElements[i].type ) {
				case VT_FLOAT2 : 
					vdElements[i].Type = D3DDECLTYPE_FLOAT2;
					offset += 8;
					m_Size += 8;
					break;
				case VT_FLOAT3 : 
					vdElements[i].Type = D3DDECLTYPE_FLOAT3;
					offset += 12;
					m_Size += 12;
					break;
				case VT_FLOAT4 : 
					vdElements[i].Type = D3DDECLTYPE_FLOAT4;
					offset += 16;
					m_Size += 16;
					break;
				case VT_COLOR : 
					vdElements[i].Type = D3DDECLTYPE_D3DCOLOR;
					offset += 4;
					m_Size += 16;
					break;
			}
			
			vdElements[i].Method = D3DDECLMETHOD_DEFAULT;

			switch ( m_DecElements[i].usage ) {
				case VDU_POSITION : 
					vdElements[i].Usage = D3DDECLUSAGE_POSITION;
					vdElements[i].UsageIndex = counter[0];
					++counter[0];
					break;
				case VDU_POSITIONT : 
					vdElements[i].Usage = D3DDECLUSAGE_POSITIONT;
					vdElements[i].UsageIndex = counter[1];
					++counter[1];
					break;
				case VDU_COLOR : 
					vdElements[i].Usage = D3DDECLUSAGE_COLOR;
					vdElements[i].UsageIndex = counter[2];
					++counter[2];
					break;
				case VDU_TANGENT : 
					vdElements[i].Usage = D3DDECLUSAGE_TANGENT;
					vdElements[i].UsageIndex = counter[3];
					++counter[3];
					break;
				case VDU_BINORMAL : 
					vdElements[i].Usage = D3DDECLUSAGE_BINORMAL;
					vdElements[i].UsageIndex = counter[4];
					++counter[4];
					break;
				case VDU_NORMAL : 
					vdElements[i].Usage = D3DDECLUSAGE_NORMAL;
					vdElements[i].UsageIndex = counter[5];
					++counter[5];
					break;
				case VDU_TEXCOORD : 
					vdElements[i].Usage = D3DDECLUSAGE_TEXCOORD;
					vdElements[i].UsageIndex = counter[6];
					++counter[6];
					break;
			}
				
		}	
		// closing element
		vdElements[m_DecElements.size()].Stream = 0xff;
		vdElements[m_DecElements.size()].Offset = 0;
		vdElements[m_DecElements.size()].Type = D3DDECLTYPE_UNUSED;
		vdElements[m_DecElements.size()].Method = 0;
		vdElements[m_DecElements.size()].Usage = 0;
		vdElements[m_DecElements.size()].UsageIndex = 0;

		HR(renderer::getDevice()->CreateVertexDeclaration(vdElements, &m_Decl));
		assert(m_Decl != 0);
		delete[] vdElements;
	}
}

};