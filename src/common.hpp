#pragma once
#include "rack.hpp"
#include <algorithm>

#define LVL_ON    (10.0)
#define LVL_OFF   (0.0)

using namespace rack;
extern Plugin *plugin;

#if defined(ARCH_WIN) && defined(USE_LAUNCHPAD)
#define LAUNCHPAD
#endif

#ifdef LAUNCHPAD
#include "../digitalExt/launchpad.hpp"
#include "../digitalExt/launchpadControls.hpp"
// #define TEST_MODULE
#endif

struct PJ301YPort : SVGPort {
	PJ301YPort() {
		background->svg = SVG::load(assetPlugin(plugin, "res/PJ301Y.svg"));
		background->wrap();
		box.size = background->box.size;
	}
};

struct PJ301GPort : SVGPort {
	PJ301GPort() {
		background->svg = SVG::load(assetPlugin(plugin, "res/PJ301G.svg"));
		background->wrap();
		box.size = background->box.size;
	}
};

struct PJ301RPort : SVGPort {
	PJ301RPort() {
		background->svg = SVG::load(assetPlugin(plugin, "res/PJ301R.svg"));
		background->wrap();
		box.size = background->box.size;
	}
};

struct PJ301WPort : SVGPort {
	PJ301WPort() {
		background->svg = SVG::load(assetPlugin(plugin, "res/PJ301W.svg"));
		background->wrap();
		box.size = background->box.size;
	}
};

struct SchmittTrigger2
{
	// UNKNOWN is used to represent a stable state when the previous state is not yet set
	enum { UNKNOWN, LOW, HIGH } state = UNKNOWN;
	float low = 0.0;
	float high = 1.0;
	void setThresholds(float low, float high)
	{
		this->low = low;
		this->high = high;
	}
	/** Returns true if triggered */
	int process(float in) {
		switch(state)
		{
		case LOW:
			if(in >= high)
			{
				state = HIGH;
				return 1;
			}
			break;
		case HIGH:
			if(in <= low)
			{
				state = LOW;
				return -1;
			}
			break;
		default:
			if(in >= high)
			{
				state = HIGH;
			} else if(in <= low)
			{
				state = LOW;
			}
			break;
		}
		return 0;
	}

	void reset() {
		state = UNKNOWN;
	}
};

struct NKK2 : NKK
{
	void randomize() override
	{
		if(randomf() >= 0.5)
			setValue(1.0);
		else
			setValue(0.0);
	}
};

struct BefacoSnappedSwitch : SVGSwitch, ToggleSwitch
{
	void randomize() override
	{
		if(randomf() >= 0.5)
			setValue(1.0);
		else
			setValue(0.0);
	}

	BefacoSnappedSwitch() {
		addFrame(SVG::load(assetGlobal("res/ComponentLibrary/BefacoSwitch_0.svg")));
		addFrame(SVG::load(assetGlobal("res/ComponentLibrary/BefacoSwitch_2.svg")));
	}
};

struct NKK3 : NKK
{
	void randomize() override
	{
		setValue(randomf() * maxValue);
	}
};


struct CKSS2 : CKSS
{
	void randomize() override
	{
		if(randomf() >= 0.5)
			setValue(1.0);
		else
			setValue(0.0);
	}
};

struct BefacoSnappedTinyKnob : BefacoTinyKnob
{
	BefacoSnappedTinyKnob() : BefacoTinyKnob()
	{
		snap = true;
	}
	void randomize() override {setValue(roundf(rescalef(randomf(), 0.0, 1.0, minValue, maxValue))); }
};

struct VerticalSwitch : SVGSlider
{
	VerticalSwitch()
	{
		snap = true;
		maxHandlePos = Vec(-4, 0);
		minHandlePos = Vec(-4, 37);
		background->svg = SVG::load(assetPlugin(plugin, "res/counterSwitchShort.svg"));
		background->wrap();
		background->box.pos = Vec(0, 0);
		box.size = background->box.size;
		handle->svg = SVG::load(assetPlugin(plugin, "res/counterSwitchPotHandle.svg"));
		handle->wrap();
	}

	void randomize() override { setValue(roundf(randomf() * maxValue)); }

};

template<class T> struct SeqMenuItem : MenuItem
{
public:
	SeqMenuItem(const char *title, T *pW, int act)
	{
		text = title;
		widget = pW;
		action = act;
	};

	void onAction(EventAction &e) override { widget->onMenu(action); };

private:
	T *widget;
	int action;
};

class SequencerWidget : public ModuleWidget
{
protected:
	int getParamIndex(int index)
	{
		auto it = std::find_if(params.begin(), params.end(), [&index](const ParamWidget *m) -> bool { return m->paramId == index; });
		if(it != params.end())
			return std::distance(params.begin(), it);

		return -1;
	}

	void std_randomize(int first_index, int last_index)
	{
		for(int k = first_index; k < last_index; k++)
		{
			int index = getParamIndex(k);
			if(index >= 0)
			{
				params[index]->randomize();
			}
		}
	}

	Menu *createContextMenu() override
	{
		Menu *menu = ModuleWidget::createContextMenu();
		MenuLabel *spacerLabel = new MenuLabel();
		menu->addChild(spacerLabel);
		return addContextMenu(menu);
	}

	virtual Menu *addContextMenu(Menu *menu) { return menu; }
};

#ifdef LAUNCHPAD
struct DigitalLed : SVGWidget
{
	float *value;
	std::vector<std::shared_ptr<SVG>> frames;

	DigitalLed(int x, int y, float *pVal)
	{
		frames.push_back(SVG::load(assetPlugin(plugin, "res/digitalLed_off.svg")));
		frames.push_back(SVG::load(assetPlugin(plugin, "res/digitalLed_on.svg")));
		setSVG(frames[0]);
		wrap();
		box.pos = Vec(x, y);
		value = pVal;
	}

	void draw(NVGcontext *vg) override
	{
		int index = (*value > 0) ? 1 : 0;
		setSVG(frames[index]);
		SVGWidget::draw(vg);
	}
};
#endif
