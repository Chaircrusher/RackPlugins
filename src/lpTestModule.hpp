#include "common.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

#ifdef LPTEST_MODULE
////////////////////
// module widgets
////////////////////
using namespace rack;
extern Plugin *pluginInstance;

struct LaunchpadTest;
struct LaunchpadTestWidget : ModuleWidget
{
	LaunchpadTestWidget(LaunchpadTest * module);
};

struct PatternBtn : SVGSwitch, ToggleSwitch {
	PatternBtn() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Patternbtn_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Patternbtn_1.svg")));
	}
};

struct LaunchpadTest : Module
{
	enum ParamIds
	{
		BTN,
		SW,
		KNOB,
		NUM_PARAMS
	};
	enum InputIds
	{
		IN_V,
		NUM_INPUTS
	};
	enum OutputIds
	{
		KNOB_OUT,
		BTN_OUT,
		SW_OUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		LP_CONNECTED,
		NUM_LIGHTS
	};
	LaunchpadTest()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		v_in = 0;
		drv = new LaunchpadBindingDriver(this, Scene8, 1);
	}
	~LaunchpadTest()
	{
		delete drv;
	}
	void process(const ProcessArgs &args) override;

	LaunchpadBindingDriver *drv;
	float v_in;
};

#endif
