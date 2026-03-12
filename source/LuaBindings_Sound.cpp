#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UnigineSounds.h>
#include <UnigineNodes.h>
#include <cmath>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerSoundAPI(sol::state& lua)
{
    // AmbientSource
    lua.new_usertype<AmbientSourcePtr>("AmbientSource",
        sol::no_constructor,
        "create", [](const char* name, int stream) {return AmbientSource::create(name, stream);},
        "setGain", [](AmbientSourcePtr& a, float gain) {a->setGain(gain);},
        "getGain", [](const AmbientSourcePtr& a) {return a->getGain();},
        "getLength", [](const AmbientSourcePtr& a) {return a->getLength();},
        "setLoop", [](AmbientSourcePtr& a, int loop) {a->setLoop(loop);},
        "getLoop", [](const AmbientSourcePtr& a) {return a->getLoop();},
        "setPitch", [](AmbientSourcePtr& a, float pitch) {a->setPitch(pitch);},
        "getPitch", [](const AmbientSourcePtr& a) {return a->getPitch();},
        "setSampleName", [](AmbientSourcePtr& a, const char* name) {a->setSampleName(name);},
        "getSampleName", [](const AmbientSourcePtr& a) {return a->getSampleName();},
        "setSourceMask", [](AmbientSourcePtr& a, int mask) {a->setSourceMask(mask);},
        "getSourceMask", [](const AmbientSourcePtr& a) {return a->getSourceMask();},
        "setTime", [](AmbientSourcePtr& a, float time) {a->setTime(time);},
        "getTime", [](const AmbientSourcePtr& a) {return a->getTime();},
        "isPlaying", [](const AmbientSourcePtr& a) {return a->isPlaying();},
        "isStopped", [](const AmbientSourcePtr& a) {return a->isStopped();},
        "play", [](AmbientSourcePtr& a) {a->play();},
        "stop", [](AmbientSourcePtr& a) {a->stop();}
    );


    // SoundSource
    lua.new_usertype<SoundSourcePtr>("SoundSource",
        sol::no_constructor,
        "type", []() {return SoundSource::type();},
        "convertible", [](Node* node) {return SoundSource::convertible(node);},
        "create", [](const char* name, int stream) {
            if (!name || name[0] == '\0') {
                Log::warning("SoundSource::create: empty path ignored\n");
                return SoundSourcePtr();
            }
            return SoundSource::create(name, stream);
        },
        "setGain", [](SoundSourcePtr& s, float gain) noexcept {
            if (!std::isfinite(gain) || gain < 0.0f) gain = 0.0f;
            s->setGain(gain);
        },
        "getGain", [](const SoundSourcePtr& s) {return s->getGain();},
        "getLength", [](const SoundSourcePtr& s) {return s->getLength();},
        "setLoop", [](SoundSourcePtr& s, int loop) {s->setLoop(loop);},
        "getLoop", [](const SoundSourcePtr& s) {return s->getLoop();},
        "setPlayOnEnable", [](SoundSourcePtr& s, bool playOnEnable) {s->setPlayOnEnable(playOnEnable);},
        "isPlayOnEnable", [](const SoundSourcePtr& s) {return s->isPlayOnEnable();},
        "setRestartOnEnable", [](SoundSourcePtr& s, bool restartOnEnable) {s->setRestartOnEnable(restartOnEnable);},
        "isRestartOnEnable", [](const SoundSourcePtr& s) {return s->isRestartOnEnable();},
        "setStream", [](SoundSourcePtr& s, bool stream) {s->setStream(stream);},
        "isStream", [](const SoundSourcePtr& s) {return s->isStream();},
        "setPitch", [](SoundSourcePtr& s, float pitch) noexcept {
            // OpenAL AL_PITCH valid range is 0.5 to 2.0; outside causes AL_INVALID_VALUE
            if (!std::isfinite(pitch) || pitch <= 0.0f) pitch = 0.5f;
            else if (pitch < 0.5f) pitch = 0.5f;
            else if (pitch > 2.0f) pitch = 2.0f;
            s->setPitch(pitch);
        },
        "getPitch", [](const SoundSourcePtr& s) {return s->getPitch();},
        "setSampleName", [](SoundSourcePtr& s, const char* name) {
            if (name && name[0] != '\0') s->setSampleName(name);
        },
        "getSampleName", [](const SoundSourcePtr& s) {return s->getSampleName();},
        "setSourceMask", [](SoundSourcePtr& s, int mask) {s->setSourceMask(mask);},
        "getSourceMask", [](const SoundSourcePtr& s) {return s->getSourceMask();},
        "setReverbMask", [](SoundSourcePtr& s, int mask) {s->setReverbMask(mask);},
        "getReverbMask", [](const SoundSourcePtr& s) {return s->getReverbMask();},
        "setTime", [](SoundSourcePtr& s, float time) {s->setTime(time);},
        "getTime", [](const SoundSourcePtr& s) {return s->getTime();},
        "setAirAbsorption", [](SoundSourcePtr& s, float airAbsorption) {s->setAirAbsorption(airAbsorption);},
        "getAirAbsorption", [](const SoundSourcePtr& s) {return s->getAirAbsorption();},
        "setConeInnerAngle", [](SoundSourcePtr& s, float coneInnerAngle) {s->setConeInnerAngle(coneInnerAngle);},
        "getConeInnerAngle", [](const SoundSourcePtr& s) {return s->getConeInnerAngle();},
        "setConeOuterAngle", [](SoundSourcePtr& s, float coneOuterAngle) {s->setConeOuterAngle(coneOuterAngle);},
        "getConeOuterAngle", [](const SoundSourcePtr& s) {return s->getConeOuterAngle();},
        "setConeOuterGainHF", [](SoundSourcePtr& s, float coneOuterGainHF) {s->setConeOuterGainHF(coneOuterGainHF);},
        "getConeOuterGainHF", [](const SoundSourcePtr& s) {return s->getConeOuterGainHF();},
        "setConeOuterGain", [](SoundSourcePtr& s, float coneOuterGain) {s->setConeOuterGain(coneOuterGain);},
        "getConeOuterGain", [](const SoundSourcePtr& s) {return s->getConeOuterGain();},
        "setMaxDistance", [](SoundSourcePtr& s, float maxDistance) noexcept {
            if (std::isfinite(maxDistance) && maxDistance > 0.0f) s->setMaxDistance(maxDistance);
        },
        "getMaxDistance", [](const SoundSourcePtr& s) {return s->getMaxDistance();},
        "setMinDistance", [](SoundSourcePtr& s, float minDistance) noexcept {
            if (std::isfinite(minDistance) && minDistance > 0.0f) s->setMinDistance(minDistance);
        },
        "getMinDistance", [](const SoundSourcePtr& s) {return s->getMinDistance();},
        "setOcclusionMask", [](SoundSourcePtr& s, int occlusionMask) {s->setOcclusionMask(occlusionMask);},
        "getOcclusionMask", [](const SoundSourcePtr& s) {return s->getOcclusionMask();},
        "setOcclusion", [](SoundSourcePtr& s, int occlusion) {s->setOcclusion(occlusion);},
        "getOcclusion", [](const SoundSourcePtr& s) {return s->getOcclusion();},
        "setAdaptation", [](SoundSourcePtr& s, float adaptation) {s->setAdaptation(adaptation);},
        "getAdaptation", [](const SoundSourcePtr& s) {return s->getAdaptation();},
        "setRoomRolloff", [](SoundSourcePtr& s, float roomRolloff) {s->setRoomRolloff(roomRolloff);},
        "getRoomRolloff", [](const SoundSourcePtr& s) {return s->getRoomRolloff();},
        "isPlaying", [](const SoundSourcePtr& s) {return s->isPlaying();},
        "isStopped", [](const SoundSourcePtr& s) {return s->isStopped();},
        "play", [](SoundSourcePtr& s) noexcept {
            // Validate sound source before playing
            if (!s) {
                Log::error("SoundSource::play: Sound source is null\n");
                return;
            }
            if (!s->isEnabled()) {
                s->setEnabled(true);
            }
            // Check if sample is set (getSampleName returns const char*, check for null/empty)
            const char* sample_name = s->getSampleName();
            if (!sample_name || sample_name[0] == '\0') {
                Log::warning("SoundSource::play: No sample set, cannot play\n");
                return;
            }
            s->play();
        },
        "stop", [](SoundSourcePtr& s) {s->stop();},

		// Node base class methods (manually exposed since SoundSource inherits from Node)
        "setPosition", [](SoundSourcePtr& s, const Vec3& pos) { s->setPosition(pos); },
        "getPosition", [](const SoundSourcePtr& s) { return s->getPosition(); },
        "setWorldPosition", [](SoundSourcePtr& s, const Vec3& pos) { s->setWorldPosition(pos); },
        "getWorldPosition", [](const SoundSourcePtr& s) { return s->getWorldPosition(); },
        "addChild", [](SoundSourcePtr& s, const NodePtr& child) { s->addChild(child); },
        "removeChild", [](SoundSourcePtr& s, const NodePtr& child) { s->removeChild(child); },
        "setEnabled", [](SoundSourcePtr& s, bool enabled) { s->setEnabled(enabled); },
        "isEnabled", [](const SoundSourcePtr& s) { return s->isEnabled(); }
    );


    // SoundReverb
    lua.new_usertype<SoundReverbPtr>("SoundReverb",
        sol::no_constructor,
        "type", []() {return SoundReverb::type();},
        "convertible", [](Node* n) {return SoundReverb::convertible(n);},
        "create", [](const vec3& s) {return SoundReverb::create(s);},
        "setReverbMask", [](SoundReverbPtr& r, int mask) {r->setReverbMask(mask);},
        "getReverbMask", [](const SoundReverbPtr& r) {return r->getReverbMask();},
        "setRoomRolloff", [](SoundReverbPtr& r, float rolloff) {r->setRoomRolloff(rolloff);},
        "getRoomRolloff", [](const SoundReverbPtr& r) {return r->getRoomRolloff();},
        "setAirAbsorption", [](SoundReverbPtr& r, float absorb) {r->setAirAbsorption(absorb);},
        "getAirAbsorption", [](const SoundReverbPtr& r) {return r->getAirAbsorption();},
        "setDecayHFRatio", [](SoundReverbPtr& r, float ratio) {r->setDecayHFRatio(ratio);},
        "getDecayHFRatio", [](const SoundReverbPtr& r) {return r->getDecayHFRatio();},
        "setDecayLFRatio", [](SoundReverbPtr& r, float decayLFRatio) {r->setDecayLFRatio(decayLFRatio);},
        "getDecayLFRatio", [](const SoundReverbPtr& r) {return r->getDecayLFRatio();},
        "setDecayTime", [](SoundReverbPtr& r, float v) {r->setDecayTime(v);},
        "getDecayTime", [](const SoundReverbPtr& r) {return r->getDecayTime();},
        "setDensity", [](SoundReverbPtr& r, float density) {r->setDensity(density);},
        "getDensity", [](const SoundReverbPtr& r) {return r->getDensity();},
        "setDiffusion", [](SoundReverbPtr& r, float diffusion) {r->setDiffusion(diffusion);},
        "getDiffusion", [](const SoundReverbPtr& r) {return r->getDiffusion();},
        "setEchoDepth", [](SoundReverbPtr& r, float depth) {r->setEchoDepth(depth);},
        "getEchoDepth", [](const SoundReverbPtr& r) {return r->getEchoDepth();},
        "setEchoTime", [](SoundReverbPtr& r, float time) {r->setEchoTime(time);},
        "getEchoTime", [](const SoundReverbPtr& r) {return r->getEchoTime();},
        "setGain", [](SoundReverbPtr& r, float gain) {r->setGain(gain);},
        "getGain", [](const SoundReverbPtr& r) {return r->getGain();},
        "setGainHF", [](SoundReverbPtr& r, float gainHF) {r->setGainHF(gainHF);},
        "getGainHF", [](const SoundReverbPtr& r) {return r->getGainHF();},
        "setGainLF", [](SoundReverbPtr& r, float gainLF) {r->setGainLF(gainLF);},
        "getGainLF", [](const SoundReverbPtr& r) {return r->getGainLF();},
        "setLateReverbDelay", [](SoundReverbPtr& r, float v) {r->setLateReverbDelay(v);},
        "getLateReverbDelay", [](const SoundReverbPtr& r) {return r->getLateReverbDelay();},
        "setLateReverbGain", [](SoundReverbPtr& r, float v) {r->setLateReverbGain(v);},
        "getLateReverbGain", [](const SoundReverbPtr& r) {return r->getLateReverbGain();},
        "setModulationDepth", [](SoundReverbPtr& r, float v) {r->setModulationDepth(v);},
        "getModulationDepth", [](const SoundReverbPtr& r) {return r->getModulationDepth();},
        "setModulationTime", [](SoundReverbPtr& r, float modulationTime) {r->setModulationTime(modulationTime);},
        "getModulationTime", [](SoundReverbPtr& r) {return r->getModulationTime();},
        "setReflectionDelay", [](SoundReverbPtr& r, float reflectionDelay) {r->setReflectionDelay(reflectionDelay);},
        "getReflectionDelay", [](const SoundReverbPtr& r) {return r->getReflectionDelay();},
        "setReflectionGain", [](SoundReverbPtr& r, float reflectionGain) {r->setReflectionGain(reflectionGain);},
        "getReflectionGain", [](const SoundReverbPtr& r) {return r->getReflectionGain();},
        "setSize", [](SoundReverbPtr& r, const vec3& size) {r->setSize(size);},
        "getSize", [](const SoundReverbPtr& r) {return r->getSize();},
        "setThreshold", [](SoundReverbPtr& r, const vec3& threshold) {r->setThreshold(threshold);},
        "getThreshold", [](const SoundReverbPtr& r) {return r->getThreshold();}
    );


    // Create Sound table
    lua["Sound"] = lua.create_table();
    lua["Sound"]["isInitialized"] = []() { return Sounds::isInitialized(); };
    lua["Sound"]["isEnabled"] = []() { return Sound::isEnabled(); };
    lua["Sound"]["setEnabled"] = [](bool enable) { Sound::setEnabled(enable); };
    lua["Sound"]["setData"] = [](const char* data) { Sound::setData(data); };
    lua["Sound"]["getData"] = []() { return Sound::getData(); };
    lua["Sound"]["getScale"] = []() { return Sound::getScale(); };
    lua["Sound"]["setScale"] = [](float scale) { Sound::setScale(scale); };
    lua["Sound"]["setSourceLimit"] = [](int source, int limit) { Sound::setSourceLimit(source, limit); };
    lua["Sound"]["setDoppler"] = [](bool enable) { Sound::setDoppler(enable); };
    lua["Sound"]["getDoppler"] = []() { return Sound::getDoppler(); };
    lua["Sound"]["getDoppler"] = []() { return Sound::getDoppler(); };
    lua["Sound"]["setAdaptation"] = [](float adaptation) { Sound::setAdaptation(adaptation); };
    lua["Sound"]["getAdaptation"] = []() { return Sound::getAdaptation(); };
    lua["Sound"]["setVolume"] = [](float volume) { Sound::setVolume(volume); };
    lua["Sound"]["getVolume"] = []() { return Sound::getVolume(); };
    lua["Sound"]["setVelocity"] = [](float velocity) { Sound::setVelocity(velocity); };
    lua["Sound"]["getVelocity"] = []() { return Sound::getVelocity(); };
    lua["Sound"]["getTotalTime"] = []() { return Sound::getTotalTime(); };
    lua["Sound"]["isDeviceConnected"] = []() { return Sounds::isDeviceConnected(); };
    lua["Sound"]["getNumDevices"] = []() { return Sounds::getNumDevices(); };
    lua["Sound"]["getDeviceName"] = [](int index) { return Sounds::getDeviceName(index); };
    lua["Sound"]["getDefaultDeviceName"] = []() { return Sounds::getDefaultDeviceName(); };
    lua["Sound"]["setCurrentDeviceName"] = [](const char* name) { Sounds::setCurrentDeviceName(name); };
    lua["Sound"]["getCurrentDeviceName"] = []() { return Sounds::getCurrentDeviceName(); };
    lua["Sound"]["updateDeviceList"] = []() { Sounds::updateDeviceList(); };
    lua["Sound"]["loadSettings"] = [](const char* name, bool clear) { return Sound::loadSettings(name, clear); };
    lua["Sound"]["saveSettings"] = [](const char* name, int force) { return Sound::saveSettings(name, force); };
    lua["Sound"]["renderWorld"] = [](int force) { Sound::renderWorld(force); };
    lua["Sound"]["loadWorld"] = [](const Ptr<Xml>& xml) { return Sound::loadWorld(xml); };
    lua["Sound"]["saveWorld"] = [](const Ptr<Xml>& xml, int force) { return Sound::saveWorld(xml, force); };
    lua["Sound"]["restoreState"] = [](const Ptr<Stream>& stream) { return Sound::restoreState(stream); };
    lua["Sound"]["saveState"] = [](const Ptr<Stream>& stream) { return Sound::saveState(stream); };

	lua["Sounds"] = lua.create_table(); // Sounds table
	lua["Sounds"]["isInitialized"] = []() { return Sounds::isInitialized(); };
	lua["Sounds"]["isDeviceConnected"] = []() { return Sounds::isDeviceConnected(); };
	lua["Sounds"]["isDeviceEnumerationSupported"] = []() { return Sounds::isDeviceEnumerationSupported(); };
	lua["Sounds"]["getNumDevices"] = []() { return Sounds::getNumDevices(); };
	lua["Sounds"]["getDeviceName"] = [](int num) { return Sounds::getDeviceName(num); };
	lua["Sounds"]["getDefaultDeviceName"] = []() { return Sounds::getDefaultDeviceName(); };
	lua["Sounds"]["setCurrentDeviceName"] = [](const char* name) { Sounds::setCurrentDeviceName(name); };
	lua["Sounds"]["getCurrentDeviceName"] = []() { return Sounds::getCurrentDeviceName(); };
	lua["Sounds"]["updateDeviceList"] = []() { Sounds::updateDeviceList(); };

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Sound sources registered\n");
}
