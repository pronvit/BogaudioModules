
#include "bogaudio.hpp"

struct TenHP : Module {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	TenHP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onReset();
	}

	virtual void onReset() override;
	virtual void step() override;
};

void TenHP::onReset() {
}

void TenHP::step() {
}


struct TenHPWidget : ModuleWidget {
	TenHPWidget(TenHP* module);
};

TenHPWidget::TenHPWidget(TenHP* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 10, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/TenHP.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(0, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));
}


Model* modelTenHP = Model::create<TenHP, TenHPWidget>("Bogaudio", "Bogaudio-TenHP", "10HP");
