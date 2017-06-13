#include "GameEngine.h"

GGE::GameEngine::Configuration GGE::GameEngine::Configuration::_Default =
{ 
	{ 
		60.f
		, {
			[]() { std::cerr << "[Warning] : Frame drop detected !" << std::endl; }
			// , ...
		}
	}
	, { 800, 600, 32, "[Default Configuration] : GGE : Instance rendering"}
};
const gcl::old::events::EventHandler<>::T_EventID GGE::GameEngine::TicksSystem::Event::FrameDrop = "FrameDrop";
