#pragma once
#include "common.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "M581Types.hpp"
////////////////////
// module widgets
////////////////////

struct M581;
struct M581Widget : SequencerWidget
{
private:
	enum MENUACTIONS
	{
		RANDOMIZE_PITCH,
		RANDOMIZE_COUNTER,
		RANDOMIZE_MODE,
		RANDOMIZE_ENABLE
	};
	Menu *addContextMenu(Menu *menu) override;

public:
	M581Widget(M581 *module);
	void onMenu(int action);
};


struct BefacoSlidePotFix : SVGSlider
{
	BefacoSlidePotFix()
	{
		Vec margin = Vec(4.5, 4.5);
	
		maxHandlePos = Vec(mm2px(-3.09541 / 2.0 + 2.27312 / 2.0), -mm2px(5.09852 / 2.0)).plus(margin);
		minHandlePos = Vec(mm2px(-3.09541 / 2.0 + 2.27312 / 2.0), mm2px(27.51667 - 5.09852 / 2.0)).plus(margin);
		setSVGs(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BefacoSlidePot.svg")), APP->window->loadSvg(asset::plugin(pluginInstance, "res/BefacoSlidePotHandle.svg")));
		background->box.pos = margin;
		box.size = background->box.size.plus(margin.mult(2));
	}
};


struct CounterSwitch : SVGFader
{
	CounterSwitch()
	{
		snap = true;
		maxHandlePos = Vec(-mm2px(2.3-2.3/2.0), 0);
		minHandlePos = Vec(-mm2px(2.3-2.3/2.0), mm2px(24-2.8));
		background->svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/counterSwitchPot.svg"));
		background->wrap();
		background->box.pos = Vec(0, 0);
		box.size = background->box.size;
		handle->svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/counterSwitchPotHandle.svg"));
		handle->wrap();
	}

	void randomize() override { setValue(roundf(random::uniform() * maxValue)); }
};

struct RunModeDisplay : TransparentWidget
{
	float *mode;
	std::shared_ptr<Font> font;

	RunModeDisplay()
	{
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
	}

	void draw(const DrawArgs &args) override
	{
		// Background
		NVGcolor backgroundColor = nvgRGB(0x20, 0x20, 0x20);
		NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
		nvgFillColor(args.vg, backgroundColor);
		nvgFill(args.vg);
		nvgStrokeWidth(args.vg, 1.0);
		nvgStrokeColor(args.vg, borderColor);
		nvgStroke(args.vg);
		// text
		nvgFontSize(args.vg, 18);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, 2.5);

		Vec textPos = Vec(2, 18);
		NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
		nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
		nvgText(args.vg, textPos.x, textPos.y, "~~", NULL);

		textColor = nvgRGB(0xda, 0xe9, 0x29);
		nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
		nvgText(args.vg, textPos.x, textPos.y, "\\\\", NULL);

		textColor = nvgRGB(0xf0, 0x00, 0x00);
		nvgFillColor(args.vg, textColor);
		nvgText(args.vg, textPos.x, textPos.y, run_modes[int(std::round(*mode))], NULL);
	}

private:
	const char *run_modes[5] = {
		"FWD",
		"BWD",
		"PNG",
		"BRN",
		"RND"
	};
};
struct M581 : Module
{
	enum ParamIds
	{
		GATE_SWITCH,
		COUNTER_SWITCH = GATE_SWITCH + 8,
		STEP_NOTES = COUNTER_SWITCH + 8,
		STEP_ENABLE = STEP_NOTES + 8,
		GATE_TIME = STEP_ENABLE + 8,
		SLIDE_TIME,
		NUM_STEPS,
		RUN_MODE,
		STEP_DIV,
		MAXVOLTS
		, NUM_PARAMS
	};

	enum InputIds
	{
		CLOCK,
		RESET,
		NUM_INPUTS
	};

	enum OutputIds
	{
		CV,
		GATE,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		LED_STEP,
		LED_SUBDIV = LED_STEP + 8,
		temp = LED_SUBDIV + 8,
		NUM_LIGHTS
	};

	M581()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		#ifdef LAUNCHPAD
		drv = new LaunchpadBindingDriver(this, Scene2, 3);
		drv->SetAutoPageKey(LaunchpadKey::SESSION, 0);
		drv->SetAutoPageKey(LaunchpadKey::NOTE, 1);
		drv->SetAutoPageKey(LaunchpadKey::DEVICE, 2);
		#endif
		#ifdef OSCTEST_MODULE
		oscDrv = new OSCDriver(this, 2);
		#endif

		on_loaded();
	}

	#ifdef DIGITAL_EXT
	~M581()
	{
		#if defined(LAUNCHPAD)
		delete drv;
		#endif
		#if defined(OSCTEST_MODULE)
		delete oscDrv;
		#endif
	}
	#endif

	void process(const ProcessArgs &args) override;
	void reset() override { load(); }
	void randomize() override { load(); }

	void dataFromJson(json_t *root) override { Module::dataFromJson(root); on_loaded(); }
	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();

		return rootJ;
	}

	float *getAddress(int var)
	{
		switch(var)
		{
			case 0: return &params[M581::RUN_MODE].getValue();
			case 1: return &params[M581::NUM_STEPS].getValue();
		}
		return NULL;
	}

	#ifdef DIGITAL_EXT
	float connected;
	#endif
	#ifdef LAUNCHPAD
	LaunchpadBindingDriver *drv;
	#endif
	#if defined(OSCTEST_MODULE)
	OSCDriver *oscDrv;
	#endif

private:
	CV_LINE cvControl;
	GATE_LINE gateControl;
	TIMER Timer;
	STEP_COUNTER stepCounter;
	ParamGetter getter;

	void _reset();
	void on_loaded();
	void load();
	void beginNewStep();
	void showCurStep(int cur_step, int sub_div);
	bool any();
	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
};
