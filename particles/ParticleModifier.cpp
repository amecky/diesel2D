#include "ParticleModifier.h"



namespace ds {

	namespace modifier {

		ParticleModifier* create_by_name(const char* name) {
			if (strcmp(name, "Position") == 0) {
				ParticlePositionModifier* modifier = new ParticlePositionModifier();
				return modifier;
			}
			if (strcmp(name, "LinearColor") == 0) {
				LinearColorModifier* modifier = new LinearColorModifier();
				modifier->init(Color::WHITE, Color::BLACK);
				return modifier;
			}
			//_available_modifiers.push_back("LinearSize");
			//_available_modifiers.push_back("PerpendicularMove");
			//_available_modifiers.push_back("ColorPath");
			//_available_modifiers.push_back("AlphaPath");
			else if (strcmp(name, "DampingVelocity") == 0) {
				DampingVelocityModifier* modifier = new DampingVelocityModifier();
				modifier->init(0.0f);
				return modifier;
			}
			//_available_modifiers.push_back("SizePath");
			else if (strcmp(name, "VelocityRotation") == 0) {
				VelocityRotationModifier* modifier = new VelocityRotationModifier();
				return modifier;
			}
			else if (strcmp(name, "Time") == 0) {
				ParticleTimeModifier* modifier = new ParticleTimeModifier();
				return modifier;
			}
			else if (strcmp(name, "LinearAlpha") == 0) {
				LinearAlphaModifier* modifier = new LinearAlphaModifier();
				modifier->init(1.0f, 0.0f);
				return modifier;
			}
			return 0;
		}

	}
}