// Disable noisy warnings
#pragma warning(disable: 5321)  // UTF-8 encoding in sol2
#pragma warning(disable: 4244)  // Double to float conversions (common in game math)
#pragma warning(disable: 4189)  // Unreferenced local variables

#include "CommonIncludes.h"
#include <UnigineInit.h>
#include <UnigineEngine.h>

#include "AppEditorLogic.h"
#include "AppSystemLogic.h"
#include "AppWorldLogic.h"
#include "LuaSystem.h"

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	// init engine
	Unigine::EnginePtr engine(argc, argv);
	
	// UnigineLogic
	AppSystemLogic system_logic;
	AppWorldLogic world_logic;
	AppEditorLogic editor_logic;

	// enter main loop
	engine->main(&system_logic, &world_logic, &editor_logic);


	return 0;
}
