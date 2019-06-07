#pragma once

#include "bogaudio.hpp"
#include "dsp/noise.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelWalk;

namespace bogaudio {

struct Walk : Module {
	enum ParamsIds {
		RATE_PARAM,
		OFFSET_PARAM,
		SCALE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		RATE_INPUT,
		OFFSET_INPUT,
		SCALE_INPUT,
		JUMP_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	const int modulationSteps = 100;
	int _modulationStep = 0;
	float _offset = 0.0f;
	float _scale = 0.0f;
	Trigger _jumpTrigger;
	RandomWalk _walk;
	bogaudio::dsp::SlewLimiter _slew;

	Walk() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RATE_PARAM, 0.0f, 1.0f, 0.1f, "Rate", "%", 0.0f, 100.0f);
		configParam(OFFSET_PARAM, -1.0f, 1.0f, 0.0f, "Offset", "V", 0.0f, 5.0f);
		configParam(SCALE_PARAM, 0.0f, 1.0f, 1.0f, "Scale", "%", 0.0f, 100.0f);

		onReset();
	}

	void onReset() override;
	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
