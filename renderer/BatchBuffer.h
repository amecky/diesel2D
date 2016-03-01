#pragma once

namespace ds {

	struct BatchBufferDescriptor {
		int maxItems;
		int descriptorID;
		int vertexDeclaration;
	};

	template<class T>
	class BatchBuffer {

	public:
		BatchBuffer(const BatchBufferDescriptor& descriptor) {
			//_data = (T*)gDefaultMemory->allocate(sizeof(T) * descriptor.maxItems);
			_data = (T*)ALLOC(sizeof(T) * descriptor.maxItems);
			_max = descriptor.maxItems;
			_bufferIndex = renderer::createVertexBuffer(descriptor.vertexDeclaration, descriptor.maxItems, true);
			_descriptorID = descriptor.descriptorID;
			_index = 0;
		}
		~BatchBuffer() {
			//gDefaultMemory->deallocate(_data);
			DEALLOC(_data);
		}

		void begin() {
			_index = 0;
		}

		void append(const T& t) {
			if (_index + 1 >= _max) {
				flush();
			}
			_data[_index++] = t;
		}

		void end(bool count = true) {
			if (_index > 0) {
				ZoneTracker z("BatchBuffer:end");
				renderer::setWorldMatrix(matrix::m4identity());
				renderer::fillBuffer(_bufferIndex, _data, _index);
				renderer::draw(_descriptorID, _bufferIndex, _index, renderer::getQuadIndexBufferIndex());
				if (count) {
					renderer::drawCounter().sprites += _index;
				}
			}
		}

		void flush() {
			ZoneTracker z("BatchBuffer:flush");
			end(true);
			begin();
		}

		int size() const {
			return _index;
		}
		void setTextureID(int textureID) {
			renderer::getDescriptorData()->textures[_descriptorID] = textureID;
		}
	private:
		T* _data;
		int _max;
		int _index;
		int _bufferIndex;
		int _descriptorID;
	};

}