#include "common.hpp"
#include "Klee.hpp"
#include "M581.hpp"
#include "Z8K.hpp"
#include "Renato.hpp"
#include "Spiralone.hpp"
#include "pwmClock.hpp"
#include "quantizer.hpp"
#include "burst.hpp"
#include "uncert.hpp"
#include "attenuator.hpp"
#include "boole.hpp"
#include "mplex.hpp"
#include "switch.hpp"

#ifdef LPTEST_MODULE
#include "lpTestModule.hpp"
#endif 

#ifdef OSCTEST_MODULE
#include "oscTestModule.hpp"
#endif 

// The pluginInstance-wide instance of the Plugin class
Plugin *pluginInstance;

void init(rack::Plugin *p)
{
	pluginInstance = p;
	// This is the unique identifier for your pluginInstance
	p->website = "https://github.com/The-XOR/VCV-Sequencers";
	p->manual = "https://github.com/The-XOR/VCV-Sequencers/blob/master/README.md";

	// For each module, specify the ModuleWidget subclass, manufacturer slug (for saving in patches), manufacturer human-readable name, module slug, and module name
	p->addModel(createModel<Klee, KleeWidget>("Klee"));
	p->addModel(createModel<M581, M581Widget>("M581"));
	p->addModel(createModel<Z8K, Z8KWidget>("Z8K"));
	p->addModel(createModel<Renato, RenatoWidget>("Renato"));
	p->addModel(createModel<Spiralone, SpiraloneWidget>("Spiralone"));
	p->addModel(createModel<Burst, BurstWidget>("Burst"));
	p->addModel(createModel<Uncertain, UncertainWidget>("Uncertain"));
	p->addModel(createModel<PwmClock, PwmClockWidget>("PWMClock"));
	p->addModel(createModel<Quantizer, QuantizerWidget>("Quantizer"));
	p->addModel(createModel<Attenuator, AttenuatorWidget>("Attenuator"));
	p->addModel(createModel<Boole, BooleWidget>("Boole"));
	p->addModel(createModel<Switch, SwitchWidget>("Switch"));
	p->addModel(createModel<Mplex, MplexWidget>("Mplex"));

#ifdef LPTEST_MODULE
	p->addModel(createModel<LaunchpadTest, LaunchpadTestWidget>("TheXOR", "LaunchpadTest", "Launchpad Test", DIGITAL_TAG));
#endif

#ifdef OSCTEST_MODULE
	p->addModel(createModel<OscTest, OscTestWidget>("TheXOR", "OSCTest", "OSC Test", DIGITAL_TAG));
#endif

	// Any other pluginInstance initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
