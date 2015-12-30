#pragma once
#include <Vector.h>

typedef void(*ScriptFunction)(float, float, float);

struct ParticleChannel {

	char* name;
	v4* ptr;
	int index;
};

struct ParticleChannelBuffer {

	ParticleChannel* channels;
	v4* buffer;
	int count;
	int channel_size;
	int num_channels;
	int size;

};

struct ScriptOperation {
	int variable;
	ScriptFunction operation;
	int first_operand;
	int second_operand;
};

class ParticleVirtualMachine {

public:

private:
	ScriptFunction* _functions;
	int _num_functions;
};

class ScriptedParticles {

public:
	ScriptedParticles(int maxParticles);
	~ScriptedParticles();
	void create(const char** names, int count);
private:
	int _max_particles;
};

