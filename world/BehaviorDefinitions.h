#include "..\io\DataFile.h"


namespace ds {

	struct AbstractActionDefinition;

	struct Behavior {
		IdString hash;
		int definitions[32];
		int num;
	};

	class BehaviorDefinitions : public DataFile {

		struct BehaviorDefinition {

			IdString hash;
			AbstractActionDefinition* definition;

		};

	public:
		BehaviorDefinitions() {}
		~BehaviorDefinitions() {}
		bool saveData(JSONWriter& writer);
		bool loadData(const JSONReader& loader);
		Behavior* get(const char* name);
		const char* getFileName() const {
			return "resources\\behaviors.json";
		}
		void* getDefinition(int index);
	private:
		AbstractActionDefinition* createDefinition(const char* name, int* index);
		Array<Behavior> _behaviors;
		Array<AbstractActionDefinition*> _definitions;
	};

}