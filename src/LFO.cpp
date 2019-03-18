
#include "LFO.hpp"

void LFO::onReset() {
	_resetTrigger.reset();
	_modulationStep = modulationSteps;
	_sampleStep = _phasor._sampleRate;
}

void LFO::onSampleRateChange() {
	_phasor.setSampleRate(engineGetSampleRate());
	_modulationStep = modulationSteps;
	_sampleStep = _phasor._sampleRate;
}

void LFO::step() {
	lights[SLOW_LIGHT].value = _slowMode = params[SLOW_PARAM].value > 0.5f;
	if (!(
		outputs[SINE_OUTPUT].active ||
		outputs[TRIANGLE_OUTPUT].active ||
		outputs[RAMP_UP_OUTPUT].active ||
		outputs[RAMP_DOWN_OUTPUT].active ||
		outputs[SQUARE_OUTPUT].active
	)) {
		return;
	}

	++_modulationStep;
	if (_modulationStep >= modulationSteps) {
		_modulationStep = 0;

		setFrequency(_slowMode, params[FREQUENCY_PARAM], inputs[PITCH_INPUT], _phasor);

		float pw = params[PW_PARAM].value;
		if (inputs[PW_INPUT].active) {
			pw *= clamp(inputs[PW_INPUT].value / 5.0f, -1.0f, 1.0f);
		}
		pw *= 1.0f - 2.0f * _square.minPulseWidth;
		pw *= 0.5f;
		pw += 0.5f;
		_square.setPulseWidth(pw);

		float sample = params[SAMPLE_PARAM].value;
		if (inputs[SAMPLE_INPUT].active) {
			sample *= clamp(inputs[SAMPLE_INPUT].value / 10.0f, 0.0f, 1.0f);
		}
		float maxSampleSteps = (_phasor._sampleRate / _phasor._frequency) / 4.0f;
		_sampleSteps = clamp((int)(sample * maxSampleSteps), 1, (int)maxSampleSteps);

		_offset = params[OFFSET_PARAM].value;
		if (inputs[OFFSET_INPUT].active) {
			_offset *= clamp(inputs[OFFSET_INPUT].value / 5.0f, -1.0f, 1.0f);
		}
		_offset *= 5.0f;

		_scale = params[SCALE_PARAM].value;
		if (inputs[SCALE_INPUT].active) {
			_scale *= clamp(inputs[SCALE_INPUT].value / 10.0f, 0.0f, 1.0f);
		}
	}

	if (_resetTrigger.next(inputs[RESET_INPUT].value)) {
		_phasor.resetPhase();
	}

	_phasor.advancePhase();
	bool useSample = false;
	if (_sampleSteps > 1) {
		++_sampleStep;
		if (_sampleStep >= _sampleSteps) {
			_sampleStep = 0;
		}
		else {
			useSample = true;
		}
	}
	updateOutput(_sine, useSample, false, outputs[SINE_OUTPUT], _sineSample, _sineActive);
	updateOutput(_triangle, useSample, false, outputs[TRIANGLE_OUTPUT], _triangleSample, _triangleActive);
	updateOutput(_ramp, useSample, false, outputs[RAMP_UP_OUTPUT], _rampUpSample, _rampUpActive);
	updateOutput(_ramp, useSample, true, outputs[RAMP_DOWN_OUTPUT], _rampDownSample, _rampDownActive);
	updateOutput(_square, false, false, outputs[SQUARE_OUTPUT], _squareSample, _squareActive);
}

void LFO::updateOutput(Phasor& wave, bool useSample, bool invert, Output& output, float& sample, bool& active) {
	if (output.active) {
		if (useSample && active) {
			output.value = sample;
		}
		else {
			sample = wave.nextFromPhasor(_phasor) * amplitude * _scale;
			if (invert) {
				sample = -sample;
			}
			sample += _offset;
			output.value = sample;
		}
		active = true;
	}
	else {
		active = false;
	}
}

struct LFOWidget : LFOBaseWidget {
	static constexpr int hp = 10;

	LFOWidget(LFO* module)
	: LFOBaseWidget(
		module,
		new SVGPanel(),
		SVG::load(assetPlugin(plugin, "res/LFO-classic.svg")),
		SVG::load(assetPlugin(plugin, "res/LFO.svg"))
	) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		_panel->box.size = box.size;
		addChild(_panel);

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(0, 365)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(41.0, 45.0);
		auto slowParamPosition = Vec(120.0, 249.0);
		auto sampleParamPosition = Vec(37.0, 150.0);
		auto pwParamPosition = Vec(102.0, 150.0);
		auto offsetParamPosition = Vec(42.0, 196.0);
		auto scaleParamPosition = Vec(107.0, 196.0);

		auto sampleInputPosition = Vec(15.0, 230.0);
		auto pwInputPosition = Vec(47.0, 230.0);
		auto offsetInputPosition = Vec(15.0, 274.0);
		auto scaleInputPosition = Vec(47.0, 274.0);
		auto pitchInputPosition = Vec(15.0, 318.0);
		auto resetInputPosition = Vec(47.0, 318.0);

		auto rampDownOutputPosition = Vec(79.0, 230.0);
		auto rampUpOutputPosition = Vec(79.0, 274.0);
		auto squareOutputPosition = Vec(111.0, 274.0);
		auto triangleOutputPosition = Vec(79.0, 318.0);
		auto sineOutputPosition = Vec(111.0, 318.0);

		auto slowLightPosition = Vec(111.0, 240.0);
		// end generated by svg_widgets.rb

		_frequencyKnob = ParamWidget::create<Knob68>(frequencyParamPosition, module, LFO::FREQUENCY_PARAM, -8.0, 5.0, 0.0);
		addParam(_frequencyKnob);
		addParam(ParamWidget::create<StatefulButton9>(slowParamPosition, module, LFO::SLOW_PARAM, 0.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob26>(sampleParamPosition, module, LFO::SAMPLE_PARAM, 0.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob26>(pwParamPosition, module, LFO::PW_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob16>(offsetParamPosition, module, LFO::OFFSET_PARAM, -1.0, 1.0, 0.0));
		addParam(ParamWidget::create<Knob16>(scaleParamPosition, module, LFO::SCALE_PARAM, 0.0, 1.0, 1.0));

		addInput(Port::create<Port24>(sampleInputPosition, Port::INPUT, module, LFO::SAMPLE_INPUT));
		addInput(Port::create<Port24>(pwInputPosition, Port::INPUT, module, LFO::PW_INPUT));
		addInput(Port::create<Port24>(offsetInputPosition, Port::INPUT, module, LFO::OFFSET_INPUT));
		addInput(Port::create<Port24>(scaleInputPosition, Port::INPUT, module, LFO::SCALE_INPUT));
		addInput(Port::create<Port24>(pitchInputPosition, Port::INPUT, module, LFO::PITCH_INPUT));
		addInput(Port::create<Port24>(resetInputPosition, Port::INPUT, module, LFO::RESET_INPUT));

		addOutput(Port::create<Port24>(rampUpOutputPosition, Port::OUTPUT, module, LFO::RAMP_UP_OUTPUT));
		addOutput(Port::create<Port24>(rampDownOutputPosition, Port::OUTPUT, module, LFO::RAMP_DOWN_OUTPUT));
		addOutput(Port::create<Port24>(squareOutputPosition, Port::OUTPUT, module, LFO::SQUARE_OUTPUT));
		addOutput(Port::create<Port24>(triangleOutputPosition, Port::OUTPUT, module, LFO::TRIANGLE_OUTPUT));
		addOutput(Port::create<Port24>(sineOutputPosition, Port::OUTPUT, module, LFO::SINE_OUTPUT));

		addChild(ModuleLightWidget::create<SmallLight<GreenLight>>(slowLightPosition, module, LFO::SLOW_LIGHT));
	}
};

Model* modelLFO = createModel<LFO, LFOWidget>("Bogaudio-LFO", "LFO",  "low-frequency oscillator", LFO_TAG);
