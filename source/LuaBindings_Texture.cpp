#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UnigineTextures.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerTextureTypes(sol::state& lua)
{
    

    // Texture
    lua.new_usertype<TexturePtr>("Texture",
        sol::no_constructor,

        "create", []() { return Texture::create(); },

        "load", [](TexturePtr& tex, const char* path, int flags) { return tex->load(path, flags); },

        "isValid", [](const TexturePtr& tex) { return tex->isValid(); },
        "clear", [](TexturePtr& tex) { tex->clear(); },
        "destroy", [](TexturePtr& tex) { tex->destroy(); },

        "getWidth", [](const TexturePtr& tex) { return tex->getWidth(); },
        "getHeight", [](const TexturePtr& tex) { return tex->getHeight(); },
        "getDepth", [](const TexturePtr& tex) { return tex->getDepth(); },

        "getFormat", [](const TexturePtr& tex) { return tex->getFormat(); },
        "getFormatName", [](const TexturePtr& tex) { return tex->getFormatName(); },

        "getNumMipmaps", [](const TexturePtr& tex) { return tex->getNumMipmaps(); },
        "createMipmaps", [](TexturePtr& tex) { return tex->createMipmaps(); },

        sol::meta_function::to_string, [](const TexturePtr& tex) {
            if (!tex || !tex->isValid()) return std::string("Texture(null)");
            return std::string("Texture(") + std::to_string(tex->getWidth()) + "x" + std::to_string(tex->getHeight()) + ")";
        }
    );

    
    // TextureRamp Type
    lua.new_usertype<TextureRampPtr>("TextureRamp",
        sol::no_constructor,

        "create", sol::overload(
            []() { return TextureRamp::create(); },
            [](int channels, int resolution, int flags) { return TextureRamp::create(channels, resolution, flags); }
        ),

        "getTexture", [](const TextureRampPtr& ramp) { return ramp->getTexture(); },
        "getNumChannels", [](const TextureRampPtr& ramp) { return ramp->getNumChannels(); },
        "getResolution", [](const TextureRampPtr& ramp) { return ramp->getResolution(); },

        sol::meta_function::to_string, [](const TextureRampPtr& ramp) {
            if (!ramp) return std::string("TextureRamp(null)");
            return std::string("TextureRamp(channels: ") + std::to_string(ramp->getNumChannels()) + ")";
        }
    );

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Texture types registered\n");
}
