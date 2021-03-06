#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

using namespace rack;

namespace bogaudio {

struct MatrixBaseModule : BGModule {
	enum Clipping {
		SOFT_CLIPPING,
		HARD_CLIPPING
	};

	Clipping _clippingMode = SOFT_CLIPPING;
	float _inputGainDb = 0.0f;
	float _inputGainLevel = 1.0f;
	bool _sum = true;

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
	void modulate() override;
};

struct MatrixBaseModuleWidget : BGModuleWidget {
	void contextMenu(Menu* menu) override;
};

struct MatrixModule : MatrixBaseModule {
	static constexpr int maxN = 16;
	int _ins;
	int _outs;
	int _firstParamID;
	int _firstInputID;
	int _firstOutputID;

	float* _paramValues = NULL;
	bogaudio::dsp::SlewLimiter* _sls = NULL;
	Saturator* _saturators = NULL;
	bool* _inActive = NULL;
	float _invActive = 0.0f;

	MatrixModule(int ins, int outs, int firstParamID, int firstInputID, int firstOutputID)
	: _ins(ins)
	, _outs(outs)
	, _firstParamID(firstParamID)
	, _firstInputID(firstInputID)
	, _firstOutputID(firstOutputID)
	{
		assert(_ins <= maxN);
		assert(_outs <= maxN);
		_paramValues = new float[_ins * _outs] {};
		_sls = new bogaudio::dsp::SlewLimiter[_ins * _outs];
		_saturators = new Saturator[_outs * maxChannels];
		_inActive = new bool[_ins] {};
	}
	virtual ~MatrixModule() {
		delete[] _paramValues;
		delete[] _sls;
		delete[] _saturators;
		delete[] _inActive;
	}

	void sampleRateChange() override;
	int channels() override;
	void modulate() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

struct KnobMatrixModule : MatrixModule {
	bool _indicatorKnobs = true;

	KnobMatrixModule(int ins, int outs, int firstParamID, int firstInputID, int firstOutputID)
	: MatrixModule(ins, outs, firstParamID, firstInputID, firstOutputID)
	{}

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
};

struct KnobMatrixModuleWidget : MatrixBaseModuleWidget {
	std::vector<IndicatorKnob19*> _knobs;

	void createKnob(math::Vec& position, KnobMatrixModule* module, int id);
	void redrawKnobs();
	void contextMenu(Menu* menu) override;
};

struct SwitchMatrixModule : MatrixModule {
	enum Inverting {
		CLICK_INVERTING,
		PARAM_INVERTING,
		NO_INVERTING
	};

	Inverting _inverting = PARAM_INVERTING;
	bool _rowExclusive = false;
	bool _columnExclusive = false;
	std::vector<ParamQuantity*> _switchParamQuantities;

	SwitchMatrixModule(int ins, int outs, int firstParamID, int firstInputID, int firstOutputID)
	: MatrixModule(ins, outs, firstParamID, firstInputID, firstOutputID)
	{}

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
	void setInverting(Inverting inverting);
	void configSwitchParam(int id, const char* label);
	void switchChanged(int id, float value);
	void setRowExclusive(bool e);
	void setColumnExclusive(bool e);
};

struct SwitchMatrixModuleWidget : MatrixBaseModuleWidget {
	template<class W> void createSwitch(math::Vec& position, SwitchMatrixModule* module, int id) {
		auto s = dynamic_cast<W*>(createParam<W>(position, module, id));
		if (module) {
			s->setClickToInvertCallback([module]() { return module->_inverting == SwitchMatrixModule::CLICK_INVERTING; });
			s->setOnChangeCallback([module](int id, float value) { module->switchChanged(id, value); });
		}
		addParam(s);
	}

	void contextMenu(Menu* menu) override;
};

} // namespace bogaudio
