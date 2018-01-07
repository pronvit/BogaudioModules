
#include "BogaudioModules.hpp"

struct VCA : Module {
	enum ParamsIds {
		LEVEL_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		EXP_INPUT,
		LIN_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	VCA() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}

	virtual void step() override;
};

void VCA::step() {
	if (inputs[IN_INPUT].active && outputs[OUT_OUTPUT].active) {
		float level = clampf(params[LEVEL_PARAM].value, 0.0, 1.0);
		if (inputs[EXP_INPUT].active) {
			level *= powf(clampf(inputs[EXP_INPUT].value, 0.0, 10.0) / 10.0, 1.5);
		}
		else if (inputs[LIN_INPUT].active) {
			level *= clampf(inputs[LIN_INPUT].value, 0.0, 10.0) / 10.0;
		}
		outputs[OUT_OUTPUT].value = level * inputs[IN_INPUT].value;
	}
	else {
		outputs[OUT_OUTPUT].value = 0.0;
	}
}


VCAWidget::VCAWidget() {
	VCA *module = new VCA();
	setModule(module);
	box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/VCA.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(0, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 15, 365)));

	// generated by svg_widgets.rb
	auto levelParamPosition = Vec(8.0, 34.0);

	auto expInputPosition = Vec(10.5, 183.0);
	auto linInputPosition = Vec(10.5, 218.0);
	auto inInputPosition = Vec(10.5, 253.0);

	auto outOutputPosition = Vec(10.5, 291.0);
	// end generated by svg_widgets.rb

	addParam(createParam<Knob29>(levelParamPosition, module, VCA::LEVEL_PARAM, 0.0, 1.0, 0.5));

	addInput(createInput<Port24>(expInputPosition, module, VCA::EXP_INPUT));
	addInput(createInput<Port24>(linInputPosition, module, VCA::LIN_INPUT));
	addInput(createInput<Port24>(inInputPosition, module, VCA::IN_INPUT));

	addOutput(createOutput<Port24>(outOutputPosition, module, VCA::OUT_OUTPUT));
}
