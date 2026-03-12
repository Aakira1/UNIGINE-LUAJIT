#include "CommonIncludes.h"
#include "LuaAnimationSystem.h"
#include "LuaSystem.h"
#include <UnigineObjects.h>
#include <UnigineGame.h>

using namespace Unigine;

// Static members for blend operations
std::vector<LuaAnimationSystem::BlendOperation> LuaAnimationSystem::active_blends;
float LuaAnimationSystem::current_time = 0.0f;

bool LuaAnimationSystem::playAnimation(Unigine::ObjectPtr obj,
                                      const Unigine::String& animation_name,
                                      float speed,
                                      bool loop)
{
    if (!obj)
    {
        Log::error("[AnimationSystem] Invalid object\n");
        return false;
    }
    
    // Cast to ObjectMeshSkinned
    if (!obj || obj->getType() != Node::OBJECT_MESH_SKINNED)
    {
        Log::error("[AnimationSystem] Object is not a ObjectMeshSkinned\n");
        return false;
    }
    ObjectMeshSkinnedPtr skinned = static_ptr_cast<ObjectMeshSkinned>(obj);
    
    // Find or create a layer for this animation
    int layer = -1;
    int num_layers = skinned->getNumLayers();
    
    // Try to find existing layer with this animation
    for (int i = 0; i < num_layers; ++i)
    {
        String layer_path = skinned->getLayerAnimationFilePath(i);
        if (layer_path == animation_name)
        {
            layer = i;
            break;
        }
    }
    
    // If not found, create a new layer
    if (layer == -1)
    {
        layer = skinned->addLayer();
        if (layer == -1)
        {
            Log::error("[AnimationSystem] Failed to create animation layer\n");
            return false;
        }
        skinned->setLayerAnimationFilePath(layer, animation_name.get());
    }
    
    // Configure the layer
    skinned->setLayerEnabled(layer, true);
    skinned->setLayerWeight(layer, 1.0f);
    skinned->setSpeed(speed);
    skinned->setLoop(loop);
    skinned->play();
    
    return true;
}

bool LuaAnimationSystem::stopAnimation(Unigine::ObjectPtr obj, const Unigine::String& animation_name)
{
    if (!obj || obj->getType() != Node::OBJECT_MESH_SKINNED)
        return false;
    
    ObjectMeshSkinnedPtr skinned = static_ptr_cast<ObjectMeshSkinned>(obj);
    
    // Find the layer with this animation
    int num_layers = skinned->getNumLayers();
    for (int i = 0; i < num_layers; ++i)
    {
        String layer_path = skinned->getLayerAnimationFilePath(i);
        if (layer_path == animation_name)
        {
            skinned->setLayerEnabled(i, false);
            skinned->stop();
            return true;
        }
    }
    
    return false;
}

bool LuaAnimationSystem::pauseAnimation(Unigine::ObjectPtr obj, const Unigine::String& animation_name)
{
    if (!obj || obj->getType() != Node::OBJECT_MESH_SKINNED)
        return false;
    
    ObjectMeshSkinnedPtr skinned = static_ptr_cast<ObjectMeshSkinned>(obj);
    
    // Find the layer with this animation
    int num_layers = skinned->getNumLayers();
    for (int i = 0; i < num_layers; ++i)
    {
        String layer_path = skinned->getLayerAnimationFilePath(i);
        if (layer_path == animation_name)
        {
            if (skinned->isPlaying())
            {
                skinned->stop();
                return true;
            }
            return false;
        }
    }
    
    return false;
}

LuaAnimationSystem::AnimationState LuaAnimationSystem::getAnimationState(Unigine::ObjectPtr obj,
                                                                         const Unigine::String& animation_name)
{
    AnimationState state;
    
    if (!obj || obj->getType() != Node::OBJECT_MESH_SKINNED)
        return state;
    
    ObjectMeshSkinnedPtr skinned = static_ptr_cast<ObjectMeshSkinned>(obj);
    
    // Find the layer with this animation
    int num_layers = skinned->getNumLayers();
    for (int i = 0; i < num_layers; ++i)
    {
        String layer_path = skinned->getLayerAnimationFilePath(i);
        if (layer_path == animation_name)
        {
            state.is_playing = skinned->isPlaying() && skinned->isLayerEnabled(i);
            state.current_time = skinned->getTime();
            state.speed = skinned->getSpeed();
            state.loop = skinned->isLoop();
            // Duration would need to be queried from the animation file, which is more complex
            // For now, we'll leave it at 0
            state.duration = 0.0f;
            return state;
        }
    }
    
    return state;
}

bool LuaAnimationSystem::setAnimationTime(Unigine::ObjectPtr obj,
                                         const Unigine::String& animation_name,
                                         float time)
{
    if (!obj || obj->getType() != Node::OBJECT_MESH_SKINNED)
        return false;
    
    ObjectMeshSkinnedPtr skinned = static_ptr_cast<ObjectMeshSkinned>(obj);
    
    // Find the layer with this animation
    int num_layers = skinned->getNumLayers();
    for (int i = 0; i < num_layers; ++i)
    {
        String layer_path = skinned->getLayerAnimationFilePath(i);
        if (layer_path == animation_name)
        {
            skinned->setTime(time);
            skinned->setLayerFrame(i, time);
            return true;
        }
    }
    
    return false;
}

bool LuaAnimationSystem::blendAnimations(Unigine::ObjectPtr obj,
                                        const Unigine::String& from_anim,
                                        const Unigine::String& to_anim,
                                        float blend_time)
{
    if (!obj || obj->getType() != Node::OBJECT_MESH_SKINNED)
        return false;
    
    ObjectMeshSkinnedPtr skinned = static_ptr_cast<ObjectMeshSkinned>(obj);
    
    // Find layers for both animations
    int from_layer = -1;
    int to_layer = -1;
    int num_layers = skinned->getNumLayers();
    
    for (int i = 0; i < num_layers; ++i)
    {
        String layer_path = skinned->getLayerAnimationFilePath(i);
        if (layer_path == from_anim)
            from_layer = i;
        if (layer_path == to_anim)
            to_layer = i;
    }
    
    // If 'to' animation doesn't exist, create it
    if (to_layer == -1)
    {
        to_layer = skinned->addLayer();
        if (to_layer == -1)
            return false;
        skinned->setLayerAnimationFilePath(to_layer, to_anim.get());
    }
    
    // Enable both layers
    if (from_layer >= 0)
        skinned->setLayerEnabled(from_layer, true);
    skinned->setLayerEnabled(to_layer, true);
    
    // Get current weights
    float from_weight = (from_layer >= 0) ? skinned->getLayerWeight(from_layer) : 0.0f;
    float to_weight = skinned->getLayerWeight(to_layer);
    
    // If blend_time is 0 or very small, snap immediately
    if (blend_time <= 0.001f)
    {
        if (from_layer >= 0)
            skinned->setLayerWeight(from_layer, 0.0f);
        skinned->setLayerWeight(to_layer, 1.0f);
        skinned->play();
        return true;
    }
    
    // Create blend operation
    BlendOperation blend;
    blend.obj = obj;
    blend.from_layer = from_layer;
    blend.to_layer = to_layer;
    blend.start_time = current_time;
    blend.blend_time = blend_time;
    blend.from_start_weight = from_weight;
    blend.to_start_weight = to_weight;
    
    // Set initial weights
    if (from_layer >= 0)
        skinned->setLayerWeight(from_layer, from_weight);
    skinned->setLayerWeight(to_layer, to_weight);
    
    skinned->play();
    
    // Add to active blends
    active_blends.push_back(blend);
    
    return true;
}

void LuaAnimationSystem::updateBlends(float dt)
{
    current_time += dt;
    
    // Update all active blends
    for (int i = static_cast<int>(active_blends.size()) - 1; i >= 0; --i)
    {
        BlendOperation& blend = active_blends[i];
        
        if (!blend.obj || blend.obj->getType() != Node::OBJECT_MESH_SKINNED)
        {
            active_blends.erase(active_blends.begin() + i);
            continue;
        }
        
        ObjectMeshSkinnedPtr skinned = static_ptr_cast<ObjectMeshSkinned>(blend.obj);
        if (!skinned)
        {
            active_blends.erase(active_blends.begin() + i);
            continue;
        }
        
        float elapsed = current_time - blend.start_time;
        float t = elapsed / blend.blend_time;
        
        if (t >= 1.0f)
        {
            // Blend complete - set final weights
            if (blend.from_layer >= 0)
                skinned->setLayerWeight(blend.from_layer, 0.0f);
            skinned->setLayerWeight(blend.to_layer, 1.0f);
            active_blends.erase(active_blends.begin() + i);
        }
        else
        {
            // Lerp weights
            float from_weight = blend.from_start_weight * (1.0f - t);
            float to_weight = blend.to_start_weight + (1.0f - blend.to_start_weight) * t;
            
            if (blend.from_layer >= 0)
                skinned->setLayerWeight(blend.from_layer, from_weight);
            skinned->setLayerWeight(blend.to_layer, to_weight);
        }
    }
}

void LuaAnimationSystem::registerToLua(sol::state& lua)
{
    lua["Animation"] = lua.create_table();
    
    lua["Animation"]["play"] = sol::overload(
        [](Unigine::ObjectPtr obj, const std::string& name) {
            return playAnimation(obj, name.c_str(), 1.0f, false);
        },
        [](Unigine::ObjectPtr obj, const std::string& name, float speed) {
            return playAnimation(obj, name.c_str(), speed, false);
        },
        [](Unigine::ObjectPtr obj, const std::string& name, float speed, bool loop) {
            return playAnimation(obj, name.c_str(), speed, loop);
        }
    );
    
    lua["Animation"]["stop"] = [](Unigine::ObjectPtr obj, const std::string& name) {
        return stopAnimation(obj, name.c_str());
    };
    
    lua["Animation"]["pause"] = [](Unigine::ObjectPtr obj, const std::string& name) {
        return pauseAnimation(obj, name.c_str());
    };
    
    lua["Animation"]["setTime"] = [](Unigine::ObjectPtr obj, const std::string& name, float time) {
        return setAnimationTime(obj, name.c_str(), time);
    };
    
    lua["Animation"]["blend"] = [](Unigine::ObjectPtr obj,
                                  const std::string& from,
                                  const std::string& to,
                                  float blend_time) {
        return blendAnimations(obj, from.c_str(), to.c_str(), blend_time);
    };
    
    lua["Animation"]["getState"] = [&lua](Unigine::ObjectPtr obj, const std::string& name) -> sol::table {
        AnimationState state = getAnimationState(obj, name.c_str());
        sol::table result = lua.create_table();
        result["is_playing"] = state.is_playing;
        result["current_time"] = state.current_time;
        result["duration"] = state.duration;
        result["speed"] = state.speed;
        result["loop"] = state.loop;
        return result;
    };
    
    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[AnimationSystem] Animation system registered\n");
}
