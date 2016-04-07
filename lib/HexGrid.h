#pragma once
#include "..\math\hex.h"

namespace ds {

	template<class T>
	class HexGrid {

		struct HexGridNode {
			Hex hex;
			v2 position;
			bool filled;
			T data;
		};

	public:
		HexGrid() : _qMax(0), _rMax(0), _items(0), _layout(layout_pointy, v2(24.0f, 24.0f), v2(100, 130)) {}

		~HexGrid() {
			if (_items != 0) {
				delete[] _items;
			}
		}

		void resize(int qMax, int rMax, const T& fillValue) {
			if (_items != 0) {
				delete[] _items;
			}
			_qMax = qMax;
			_rMax = rMax;
			_items = new HexGridNode[qMax * rMax];
			fill(fillValue);
		}

		void fill(const T& fillValue) {
			assert(_items != 0);
			for (int r = 0; r < _rMax; r++) {
				int q_offset = r >> 1;
				for (int q = -q_offset; q < _qMax - q_offset; q++) {
					HexGridNode item;
					item.hex = Hex(q, r);
					item.position = hex_math::hex_to_pixel(_layout, item.hex);
					item.data = fillValue;
					int idx = (q + q_offset) + r * _qMax;
					_items[idx] = item;
				}
			}
		}

		const v2& position(const Hex& hex) const {
			int q_offset = hex.r >> 1;
			int idx = (hex.q + q_offset) + hex.r * _qMax;
			return _items[idx].position;
		}
		
		const v2& position(int index) const {
			const Hex& h = _items[index].hex;
			return hex_math::hex_to_pixel(_layout, h);
		}
		
		const T& get(int index) const {
			return _items[index].data;
		}
		
		T& get(int index) {
			return _items[index].data;
		}
		
		const T& get(const Hex& hex) const;
		
		T& get(const Hex& hex);

		void set(const Hex& hex,const T& t) {
			int q_offset = hex.r >> 1;
			int idx = (hex.q + q_offset) + hex.r * _qMax;
			_items[idx].data = t;
		}

		void set(int index, const T& t) {
			_items[index].data = t;
		}

		const int size() const {
			return _rMax * _qMax;
		}

		bool isValid(int q, int r) const {
			int q_offset = r >> 1;
			if (r < 0 || r >= _rMax) {
				return false;
			}
			if (q_offset + q < 0 || q_offset + q >= _qMax) {
				return false;
			}
			return true;
		}

		bool isValid(const Hex& hex) const {
			return isValid(hex.q, hex.r);
		}

		int neighbors(const Hex& hex, Hex* ret);
		
		Hex convertFromMousePos();

		Hex convert(const v2& position) {
			return hex_math::hex_round(hex_math::pixel_to_hex(_layout, position));
		}

		void setOrigin(const v2& origin) {
			_layout.origin = origin;
		}

		int getIndex(const Hex& hex) const {
			int q_offset = hex.r >> 1;
			return (hex.q + q_offset) + hex.r * _qMax;
		}
	private:
		int _qMax;
		int _rMax;
		HexGridNode* _items;
		Layout _layout;
		int _hover;
	};

}