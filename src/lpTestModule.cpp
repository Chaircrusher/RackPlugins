#include "common.hpp"
#ifdef LPTEST_MODULE
#include "lpTestModule.hpp"
#include <string.h>


void LaunchpadTest::process(const ProcessArgs &args)
{
	v_in = inputs[IN_V].getVoltage();
	lights[LP_CONNECTED].value = params[LaunchpadTest::BTN].getValue() > 0 ? 1.0 : 0.0;

	outputs[KNOB_OUT].setVoltage(params[KNOB].getValue());
	outputs[BTN_OUT].setVoltage(inputs[BTN].getVoltage());
	outputs[SW_OUT].setVoltage(params[SW].getValue());
	drv->ProcessLaunchpad();
}

LaunchpadTestWidget::LaunchpadTestWidget(LaunchpadTest *module)
{
	setModule(module);
	box.size = Vec(13 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/test.svg")));
		addChild(panel);
	}

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	int y = 40;
	SigDisplayWidget *display = new SigDisplayWidget(5, 2);
	display->box.pos = Vec(60, y);
	display->box.size = Vec(50 + 53, 24);
	display->value = &module->v_in;
	addChild(display);

	addInput(createInput<PJ301RPort>(Vec(10, y), module, LaunchpadTest::IN_V));
	
	y += 60;

	if(module)
		module->configParam(0.0, 1.0, 0.0);
	ParamWidget *pctrl = createParam<PatternBtn>(Vec(10, y), module, LaunchpadTest::BTN);
	LaunchpadLed offColor;
	LaunchpadLed onColor;
	offColor.r_color = 20;
	onColor.r_color = 3;
	LaunchpadSwitch *sw1 = new LaunchpadSwitch(0, LaunchpadKey::R1C1, offColor, onColor);
	module->drv->Add(sw1, pctrl);
	addParam(pctrl);
	addOutput(createOutput<PJ301GPort>(Vec(150, y), module, LaunchpadTest::BTN_OUT));

	y += 60;

	LaunchpadKnob *pknob = new LaunchpadKnob(0, ILaunchpadPro::RC2Key(6, 6), LaunchpadLed::Rgb(20, 10, 10), LaunchpadLed::Rgb(60, 40, 40));
	if(module)
		module->configParam(0.0, 5.0, 0.25);
	ParamWidget *pEna = createParam<Davies1900hBlackKnob>(Vec(10, y), module, LaunchpadTest::KNOB);
	module->drv->Add(pknob, pEna);
	addParam(pEna);
	addOutput(createOutput<PJ301GPort>(Vec(150, y), module, LaunchpadTest::KNOB_OUT));

	y += 60;

	if(module)
		module->configParam(0.0, 2.0, 1.0);
	pEna = createParam<BefacoSwitch>(Vec(10, y), module, LaunchpadTest::SW);
	addParam(pEna);
	addOutput(createOutput<PJ301GPort>(Vec(150, y), module, LaunchpadTest::SW_OUT));

#ifdef DEBUG
	info("RDY");
#endif
		
}
#endif
