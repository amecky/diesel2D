#include "BehaviorDefinitions.h"
#include "actions\ColorFlashAction.h"
#include "actions\AbstractAction.h"

namespace ds {

	// -----------------------------------------------------
	// save data
	// -----------------------------------------------------
	bool BehaviorDefinitions::saveData(JSONWriter& writer) {
		return true;
	}

	// -----------------------------------------------------
	// load data
	// -----------------------------------------------------
	bool BehaviorDefinitions::loadData(const JSONReader& loader) {
		int cats[256];
		int num = loader.get_categories(cats, 256);
		int index = 0;
		for (int i = 0; i < num; ++i) {
			// create Behavior
			Behavior b;
			b.num = 0;
			b.hash = string::murmur_hash(loader.get_category_name(cats[i]));
			int sub_cats[64];
			int sub_num = loader.get_categories(sub_cats, 64, cats[i]);
			for (int j = 0; j < sub_num; ++j) {
				// find action definition
				AbstractActionDefinition* def = createDefinition(loader.get_category_name(sub_cats[j]),&index);
				if (def != 0) {
					def->read(loader, sub_cats[j]);
					b.definitions[b.num++] = index;
				}
			}
			_behaviors.push_back(b);
		}
		return true;
	}

	// -----------------------------------------------------
	// create definition
	// -----------------------------------------------------
	AbstractActionDefinition* BehaviorDefinitions::createDefinition(const char* name,int* index) {
		if (strcmp(name, "flash_color") == 0) {
			*index = _definitions.size();
			ColorFlashDefinition* def = new ColorFlashDefinition();
			_definitions.push_back(def);
			return def;
		}
		return 0;
	}

}