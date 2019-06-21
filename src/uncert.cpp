#include "common.hpp"
#include "uncert.hpp"

void Uncertain::on_loaded()
{
	load();
}

void Uncertain::load()
{
	fluctParams.reset();
}

void Uncertain::process(const ProcessArgs &args)
{
	if(inputs[CLOCK_FLUCT].isConnected())
		out_fluct(clock_fluct.process(inputs[CLOCK_FLUCT].getVoltage()));
	else
		fluctParams.reset();

	if(inputs[CLOCK_QUANTIZED].isConnected())
		out_quantized(clock_quantized.process(inputs[CLOCK_QUANTIZED].getVoltage()));

	if(inputs[CLOCK_STORED].isConnected())
		out_stored(clock_stored.process(inputs[CLOCK_STORED].getVoltage()));
}

void Uncertain::out_quantized(int clk)
{
	if(clk == 1)
	{
		int position = getInt(QUANTIZED_AMT, IN_QUANTIZED, 0.0, 5.0)+1;
		outputs[OUT_QUANTIZED_N1].setVoltage(roundf(rescale(random::uniform(), 0.0, 1.0, 0.0, position)));		// 1V
		float n2= roundf(rescale(random::uniform(), 0.0, 1.0, 1.0, 2 << position));
		outputs[OUT_QUANTIZED_2N].setVoltage(clamp(Uncertain::SEMITONE*n2, 0.0, 10.0));
	}
}

void Uncertain::out_stored(int clk)
{
	if(clk == 1)
	{
		outputs[OUT_STORED_RND].setVoltage(clamp(roundf(rescale(random::uniform(), 0.0, 1.0, 0.0, 1000.0)) * MIN_VOLTAGE, 0.0,10.0));
		outputs[OUT_STORED_BELL].setVoltage(rndGaussianVoltage());
	}
}

float Uncertain::rndGaussianVoltage()
{
	float mu = getFloat(STORED_AMT, IN_STORED, MIN_VOLTAGE, MAX_VOLTAGE);
	float sigma = getFloat(CURVEAMP_AMT, IN_CURVEAMP, 0.01, 2.0);
	float u1 = 1.0 - random::uniform();
	float u2 = 1.0 - random::uniform();
	float randStdNormal = sqrtf(-2.0 * logf(u1)) * sinf(2.0 * M_PI * u2); //random normal(0,1)
	return clamp(mu + sigma * randStdNormal, MIN_VOLTAGE, MAX_VOLTAGE);
}

float Uncertain::rndFluctVoltage() 
{ 
	float vmax = getFloat(FLUCT_AMT, IN_FLUCT, MIN_VOLTAGE, MAX_VOLTAGE);
	return clamp(rescale(random::uniform(), 0.0, 1.0, MIN_VOLTAGE, vmax), MIN_VOLTAGE, MAX_VOLTAGE);
}

void Uncertain::out_fluct(int clk)
{
	switch(clk)
	{
		case 1: //rise
		{
			if(fluctParams.duration == 0)	// 0 if first cycle
			{
				outputs[OUT_FLUCT].setVoltage(fluctParams.vA = fluctParams.vB = rndFluctVoltage());
			}

			fluctParams.tStart = clock();
		}
		break;

		case -1: //fall
		{
			fluctParams.vB = rndFluctVoltage();
			fluctParams.duration = clock() - fluctParams.tStart;
			fluctParams.tStart = 0;
			if(fluctParams.duration > 0)
			{
				fluctParams.vA = outputs[OUT_FLUCT].value;
				fluctParams.deltaV = (fluctParams.vB - fluctParams.vA) / fluctParams.duration;
			}
		}
		break;

		default: // in progress
		{
			if(fluctParams.duration != 0 && fluctParams.tStart != 0)	// 0 if first cycle
			{
				clock_t elapsed = clock() - fluctParams.tStart;
				float v = fluctParams.vA + fluctParams.deltaV * elapsed;
				outputs[OUT_FLUCT].setVoltage(clamp(v, MIN_VOLTAGE, MAX_VOLTAGE));
			}
		}
		break;
	}
}

float Uncertain::getFloat(ParamIds p_id, InputIds i_id, float minValue, float maxValue)
{
	float offs = inputs[i_id].isConnected() ? rescale(inputs[i_id].getVoltage(), 0.0, 5.0, minValue, maxValue) : 0.0;
	return clamp(offs + params[p_id].getValue(), minValue, maxValue);
}

UncertainWidget::UncertainWidget(Uncertain *module) : SequencerWidget(module)
{
	box.size = Vec(12 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/modules/uncert.svg")));		
		addChild(panel);
	}
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));

	float lft_x = mm2px(4.726);
	float rgt_x = mm2px(47.988);
	float center_x = mm2px(25.717);

	addInput(createInput<PJ301RPort>(Vec(lft_x, yncscape(107.334,8.255)), module, Uncertain::CLOCK_FLUCT));
	addInput(createInput<PJ301BPort>(Vec(lft_x, yncscape(89.809, 8.255)), module, Uncertain::IN_FLUCT));
	if(module)
		module->configParam(Uncertain::FLUCT_AMT, Uncertain::MIN_VOLTAGE, Uncertain::MAX_VOLTAGE, Uncertain::MIN_VOLTAGE);
	addParam(createParam<Davies1900hFixRedKnob>(Vec(center_x, yncscape(97.936, 9.525)), module, Uncertain::FLUCT_AMT));
	addOutput(createOutput<PJ301GPort>(Vec(rgt_x, yncscape(98.571, 8.255)), module, Uncertain::OUT_FLUCT));

	addInput(createInput<PJ301RPort>(Vec(lft_x, yncscape(68.885, 8.255)), module, Uncertain::CLOCK_QUANTIZED));
	addInput(createInput<PJ301BPort>(Vec(lft_x, yncscape(51.360, 8.255)), module, Uncertain::IN_QUANTIZED));
	if(module)
		module->configParam(Uncertain::QUANTIZED_AMT, 0.0, 5.0, 0.0);
	ParamWidget *pw = createParam<Davies1900hFixWhiteKnob>(Vec(center_x, yncscape(59.487, 9.525)), module, Uncertain::QUANTIZED_AMT);
	((Davies1900hKnob *)pw)->snap = true;
	addParam(pw);
	addOutput(createOutput<PJ301GPort>(Vec(rgt_x, yncscape(68.885, 8.255)), module, Uncertain::OUT_QUANTIZED_N1));
	addOutput(createOutput<PJ301GPort>(Vec(rgt_x, yncscape(51.360, 8.255)), module, Uncertain::OUT_QUANTIZED_2N));

	addInput(createInput<PJ301RPort>(Vec(lft_x, yncscape(28.407, 8.255)), module, Uncertain::CLOCK_STORED));
	addInput(createInput<PJ301BPort>(Vec(lft_x, yncscape(10.882, 8.255)), module, Uncertain::IN_STORED));
	addInput(createInput<PJ301BPort>(Vec(mm2px(23.591), yncscape(10.882, 8.255)), module, Uncertain::IN_CURVEAMP));
	if(module)
		module->configParam(Uncertain::STORED_AMT, Uncertain::MIN_VOLTAGE + 2.5, Uncertain::MAX_VOLTAGE - 2.5, 5.0);
	addParam(createParam<Davies1900hFixBlackKnob>(Vec(mm2px(16.285), yncscape(19.010, 9.525)), module, Uncertain::STORED_AMT));
	if(module)
		module->configParam(Uncertain::CURVEAMP_AMT, 0.0,2.0,1.0);
	addParam(createParam<Davies1900hFixBlackKnob>(Vec(mm2px(35.150), yncscape(19.010, 9.525)), module, Uncertain::CURVEAMP_AMT));
	addOutput(createOutput<PJ301GPort>(Vec(rgt_x, yncscape(28.407, 8.255)), module, Uncertain::OUT_STORED_RND));
	addOutput(createOutput<PJ301GPort>(Vec(rgt_x, yncscape(10.882, 8.255)), module, Uncertain::OUT_STORED_BELL));
}
