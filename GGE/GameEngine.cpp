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
	, { 800, 600, 32, "[Default Configuration] : GEE : Instance rendering"}
};
const GCL::Events::EventHandler<>::T_EventID GGE::GameEngine::TicksSystem::Event::FrameDrop = "FrameDrop";