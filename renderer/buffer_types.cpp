#include "buffer_types.h"
#include "vertex_types.h"
#include "..\math\math_types.h"
#include "..\utils\Log.h"

namespace ds {
	
	void GeometryBuffer::initialize() {
		size = 0;
		maxSize = 0;
		used = 0;
		userCounter = 0;
		vertexBuffer = 0;
	}

}
