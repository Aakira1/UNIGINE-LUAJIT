#include "CommonIncludes.h"
#include "AppSystemLogic.h"
#include <UnigineComponentSystem.h>

#include <filesystem>
#include "LuaSystem.h"
#include <UnigineGame.h>




using namespace Unigine;
using namespace Unigine::Math;


// System logic, it exists during the application life cycle.
// These methods are called right after corresponding system script's (UnigineScript) methods.

AppSystemLogic::AppSystemLogic()
{

}

AppSystemLogic::~AppSystemLogic()
{
}



int AppSystemLogic::init()
{
    Unigine::LuaSystem::get()->initialize();
    // load world 
    Unigine::ComponentSystem::get()->initialize();
    Unigine::LuaSystem::get()->postInitialize();
	return 1;
}


////////////////////////////////////////////////////////////////////////////////
// start of the main loop
////////////////////////////////////////////////////////////////////////////////

int AppSystemLogic::update()
{
    LuaSystem::get()->update(Game::getIFps());

    return 1;
}

int AppSystemLogic::postUpdate()
{
	// Write here code to be called after updating each render frame
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// end of the main loop
////////////////////////////////////////////////////////////////////////////////

int AppSystemLogic::shutdown(){
	return 1;
}
