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

// The plugin-wide instance of the Plugin class
Plugin *plugin;

void init(rack::Plugin *p)
{
	plugin = p;
	// This is the unique identifier for your plugin
	p->website = "https://github.com/The-XOR/VCV-Sequencers";
	p->manual = "https://github.com/The-XOR/VCV-Sequencers/blob/master/README.md";

	// For each module, specify the ModuleWidget subclass, manufacturer slug (for saving in patches), manufacturer human-readable name, module slug, and module name
	p->addModel(Model::create<Klee, KleeWidget>("Klee"));
	p->addModel(Model::create<M581, M581Widget>("M581"));
	p->addModel(Model::create<Z8K, Z8KWidget>("Z8K"));
	p->addModel(Model::create<Renato, RenatoWidget>("Renato"));
	p->addModel(Model::create<Spiralone, SpiraloneWidget>("Spiralone"));
	p->addModel(Model::create<Burst, BurstWidget>("Burst"));
	p->addModel(Model::create<Uncertain, UncertainWidget>("Uncertain"));
	p->addModel(Model::create<PwmClock, PwmClockWidget>("PWMClock"));
	p->addModel(Model::create<Quantizer, QuantizerWidget>("Quantizer"));
	p->addModel(Model::create<Attenuator, AttenuatorWidget>("Attenuator"));
	p->addModel(Model::create<Boole, BooleWidget>("Boole"));
	p->addModel(Model::create<Switch, SwitchWidget>("Switch"));
	p->addModel(Model::create<Mplex, MplexWidget>("Mplex"));

#ifdef LPTEST_MODULE
	p->addModel(Model::create<LaunchpadTest, LaunchpadTestWidget>("TheXOR", "LaunchpadTest", "Launchpad Test", DIGITAL_TAG));
#endif

#ifdef OSCTEST_MODULE
	p->addModel(Model::create<OscTest, OscTestWidget>("TheXOR", "OSCTest", "OSC Test", DIGITAL_TAG));
#endif

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
