
#include "AMRM.hpp"

#define POLY_INPUT "poly_input"

json_t* AMRM::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, POLY_INPUT, json_integer(_polyInputID));
	return root;
}

void AMRM::dataFromJson(json_t* root) {
	json_t* p = json_object_get(root, POLY_INPUT);
	if (p) {
		_polyInputID = json_integer_value(p);
	}
}

bool AMRM::active() {
	return outputs[OUT_OUTPUT].isConnected() || outputs[RECTIFY_OUTPUT].isConnected();
}

int AMRM::channels() {
	return _polyInputID == MODULATOR_INPUT ? inputs[MODULATOR_INPUT].getChannels() : inputs[CARRIER_INPUT].getChannels();
}

void AMRM::processChannel(const ProcessArgs& args, int c) {
	float rectify = params[RECTIFY_PARAM].getValue();
	if (inputs[RECTIFY_INPUT].isConnected()) {
		rectify = clamp(rectify + inputs[RECTIFY_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	rectify = 1.0f - rectify;

	float depth = params[DRYWET_PARAM].getValue();
	if (inputs[DRYWET_INPUT].isConnected()) {
		depth = clamp(depth + inputs[DRYWET_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}

	float modulator = inputs[MODULATOR_INPUT].getPolyVoltage(c);
	if (rectify < 1.0f) {
		rectify *= -5.0f;
		if (modulator < rectify) {
			modulator = rectify - (modulator - rectify);
		}
	}
	outputs[RECTIFY_OUTPUT].setChannels(_channels);
	outputs[RECTIFY_OUTPUT].setVoltage(modulator, c);

	modulator *= depth;
	modulator += (1.0f - depth) * 5.0f;
	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[OUT_OUTPUT].setVoltage(_saturator.next(modulator * inputs[CARRIER_INPUT].getPolyVoltage(c) * 0.2f), c);
}

struct AMRMWidget : ModuleWidget {
	static constexpr int hp = 6;

	AMRMWidget(AMRM* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/AMRM.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto rectifyParamPosition = Vec(22.5, 66.5);
		auto drywetParamPosition = Vec(22.5, 165.5);

		auto modulatorInputPosition = Vec(16.0, 244.0);
		auto carrierInputPosition = Vec(50.0, 244.0);
		auto rectifyInputPosition = Vec(16.0, 280.0);
		auto drywetInputPosition = Vec(50.0, 280.0);

		auto rectifyOutputPosition = Vec(16.0, 320.0);
		auto outOutputPosition = Vec(50.0, 320.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob45>(rectifyParamPosition, module, AMRM::RECTIFY_PARAM));
		addParam(createParam<Knob45>(drywetParamPosition, module, AMRM::DRYWET_PARAM));

		addInput(createInput<Port24>(modulatorInputPosition, module, AMRM::MODULATOR_INPUT));
		addInput(createInput<Port24>(carrierInputPosition, module, AMRM::CARRIER_INPUT));
		addInput(createInput<Port24>(rectifyInputPosition, module, AMRM::RECTIFY_INPUT));
		addInput(createInput<Port24>(drywetInputPosition, module, AMRM::DRYWET_INPUT));

		addOutput(createOutput<Port24>(rectifyOutputPosition, module, AMRM::RECTIFY_OUTPUT));
		addOutput(createOutput<Port24>(outOutputPosition, module, AMRM::OUT_OUTPUT));
	}

	void appendContextMenu(Menu* menu) override {
		auto m = dynamic_cast<AMRM*>(module);
		assert(m);
		menu->addChild(new MenuLabel());
		OptionsMenuItem* p = new OptionsMenuItem("Polyphony channels from");
		p->addItem(OptionMenuItem("CAR input", [m]() { return m->_polyInputID == AMRM::CARRIER_INPUT; }, [m]() { m->_polyInputID = AMRM::CARRIER_INPUT; }));
		p->addItem(OptionMenuItem("MOD input", [m]() { return m->_polyInputID == AMRM::MODULATOR_INPUT; }, [m]() { m->_polyInputID = AMRM::MODULATOR_INPUT; }));
		OptionsMenuItem::addToMenu(p, menu);
	}
};

Model* modelAMRM = bogaudio::createModel<AMRM, AMRMWidget>("Bogaudio-AMRM", "AM/RM", "Variable ring/amplitude modulator and rectifier", "Ring modulator", "Effect", "Polyphonic");
