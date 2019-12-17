
#include "Mumix.hpp"
#include "mixer.hpp"

const float Mumix::maxDecibels = 0.0f;
const float Mumix::minDecibels = Amplifier::minDecibels;
const float Mumix::slewTimeMS = 5.0f;

void Mumix::sampleRateChange() {
	float sampleRate = APP->engine->getSampleRate();
	for (int i = 0; i < 8; ++i) {
		_slewLimiters[i].setParams(sampleRate, slewTimeMS, maxDecibels - minDecibels);
	}
}

bool Mumix::active() {
	return outputs[OUT_OUTPUT].isConnected();
}

int Mumix::channels() {
	return inputs[IN1_INPUT].getChannels();
}

void Mumix::modulate() {
	MatrixBaseModule::modulate();

	bool solo = false;
	for (int i = 0; i < 8; ++i) {
		solo = solo || params[MUTE1_PARAM + i].getValue() > 1.5f;
	}
	for (int i = 0; i < 8; ++i) {
		_muted[i] = solo ? params[MUTE1_PARAM + i].getValue() < 2.0f : params[MUTE1_PARAM + i].getValue() > 0.5f;
	}
}

void Mumix::processAll(const ProcessArgs& args) {
	for (int i = 0; i < 8; ++i) {
		_amplifiers[i].setLevel(_slewLimiters[i].next(_muted[i] ? minDecibels : maxDecibels));
	}
}

void Mumix::processChannel(const ProcessArgs& args, int c) {
	outputs[OUT_OUTPUT].setChannels(_channels);

	float out = 0.0f;
	for (int i = 0; i < 8; ++i) {
		out += _inputGainLevel * _amplifiers[i].next(inputs[IN1_INPUT + i].getPolyVoltage(c));
	}
	if (_clippingMode == HARD_CLIPPING) {
		outputs[OUT_OUTPUT].setVoltage(clamp(out, -12.0f, 12.0f), c);
	}
	else {
		outputs[OUT_OUTPUT].setVoltage(_saturator[c].next(out), c);
	}
}

struct MumixWidget : MatrixBaseModuleWidget {
	static constexpr int hp = 6;

	MumixWidget(Mumix* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Mumix.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto mute1ParamPosition = Vec(56.2, 32.7);
		auto mute2ParamPosition = Vec(56.2, 69.7);
		auto mute3ParamPosition = Vec(56.2, 106.7);
		auto mute4ParamPosition = Vec(56.2, 143.7);
		auto mute5ParamPosition = Vec(56.2, 180.7);
		auto mute6ParamPosition = Vec(56.2, 217.7);
		auto mute7ParamPosition = Vec(56.2, 254.7);
		auto mute8ParamPosition = Vec(56.2, 291.7);

		auto in1InputPosition = Vec(12.5, 30.0);
		auto in2InputPosition = Vec(12.5, 67.0);
		auto in3InputPosition = Vec(12.5, 104.0);
		auto in4InputPosition = Vec(12.5, 141.0);
		auto in5InputPosition = Vec(12.5, 178.0);
		auto in6InputPosition = Vec(12.5, 215.0);
		auto in7InputPosition = Vec(12.5, 252.0);
		auto in8InputPosition = Vec(12.5, 289.0);

		auto outOutputPosition = Vec(12.5, 321.0);
		// end generated by svg_widgets.rb

		addParam(createParam<SoloMuteButton>(mute1ParamPosition, module, Mumix::MUTE1_PARAM));
		addParam(createParam<SoloMuteButton>(mute2ParamPosition, module, Mumix::MUTE2_PARAM));
		addParam(createParam<SoloMuteButton>(mute3ParamPosition, module, Mumix::MUTE3_PARAM));
		addParam(createParam<SoloMuteButton>(mute4ParamPosition, module, Mumix::MUTE4_PARAM));
		addParam(createParam<SoloMuteButton>(mute5ParamPosition, module, Mumix::MUTE5_PARAM));
		addParam(createParam<SoloMuteButton>(mute6ParamPosition, module, Mumix::MUTE6_PARAM));
		addParam(createParam<SoloMuteButton>(mute7ParamPosition, module, Mumix::MUTE7_PARAM));
		addParam(createParam<SoloMuteButton>(mute8ParamPosition, module, Mumix::MUTE8_PARAM));

		addInput(createInput<Port24>(in1InputPosition, module, Mumix::IN1_INPUT));
		addInput(createInput<Port24>(in2InputPosition, module, Mumix::IN2_INPUT));
		addInput(createInput<Port24>(in3InputPosition, module, Mumix::IN3_INPUT));
		addInput(createInput<Port24>(in4InputPosition, module, Mumix::IN4_INPUT));
		addInput(createInput<Port24>(in5InputPosition, module, Mumix::IN5_INPUT));
		addInput(createInput<Port24>(in6InputPosition, module, Mumix::IN6_INPUT));
		addInput(createInput<Port24>(in7InputPosition, module, Mumix::IN7_INPUT));
		addInput(createInput<Port24>(in8InputPosition, module, Mumix::IN8_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, Mumix::OUT_OUTPUT));
	}
};

Model* modelMumix = createModel<Mumix, MumixWidget>("Bogaudio-Mumix", "MUMIX", "8-input unity mixer with mutes", "Mixer", "Polyphonic");
