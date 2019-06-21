#include "common.hpp"
#include "switch.hpp"

void Switch::process(const ProcessArgs &args)
{
	for(int k = 0; k < NUM_SWITCHES; k++)
	{
		if(outputs[OUT_1 + k].isConnected() && inputs[IN_1 + k].isConnected())
		{
			if(getSwitch(k))
			{
				lights[LED_1 + k].value = 5;;
				outputs[OUT_1 + k].setVoltage(inputs[IN_1 + k].getVoltage());
			} else
			{
				lights[LED_1 + k].value = outputs[OUT_1 + k].setVoltage(0);
			}
		} else
		{
			lights[LED_1 + k].value = outputs[OUT_1 + k].setVoltage(0);
		}
	}
}

SwitchWidget::SwitchWidget(Switch *module)
{
	setModule(module);
	box.size = Vec(10 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/Switch.svg")));		
		addChild(panel);
	}

	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	float in_x = mm2px(2.500);
	float mod_x = mm2px(17.306);
	float sw_x = mm2px(25.027);
	float led_x = mm2px(28.173);
	float out_x = mm2px(40.045);
	float y = 101.567;
	float y1 = 98.387;
	float yled = 114.949;
	float ysw = 105.667;
	float delta_y = 79.394 - 101.567;
	
	for(int k = 0; k < NUM_SWITCHES; k++)
	{
		addInput(createInput<PJ301GRPort>(Vec(in_x, yncscape(y, 8.255)), module, Switch::IN_1 + k));
		addInput(createInput<PJ301BPort>(Vec(mod_x, yncscape(y1, 8.255)), module, Switch::MOD_1 + k));
		if(module)
			module->configParam(Switch::SW_1+k, 0.0, 1.0, 0.0);
		addParam(createParam<NKK2>(Vec(sw_x, yncscape(ysw, 7.336)), module, Switch::SW_1+k));
		addChild(createLight<SmallLight<RedLight>>(Vec(led_x, yncscape(yled, 2.176)), module, Switch::LED_1 + k ));
		addOutput(createOutput<PJ301GPort>(Vec(out_x, yncscape(y, 8.255)), module, Switch::OUT_1+k));
		y += delta_y;
		y1 += delta_y;
		ysw += delta_y;
		yled += delta_y;
	}
}

