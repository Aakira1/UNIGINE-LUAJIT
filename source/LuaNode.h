#pragma once
#include <UnigineNode.h>
#include <UnigineLog.h>
#include <UnigineMathLib.h>
#include <Unigine.h>


struct LuaNode {
	Unigine::NodePtr node;

	bool isValid() const { return node && !node.isDeleted(); }

	const char* getName() const {
		return isValid() ? node->getName() : "";
	}

	// return position (world) as 3 numbers
	std::tuple<float, float, float> getWorldPosition() const {
		if (!isValid()) return { 0.f,0.f,0.f };
		Unigine::Math::Vec3 p = node->getWorldPosition();
		return { p.x, p.y, p.z };
	}

	// set position (world) from 3 numbers
	void setWorldPosition(float x, float y, float z) {
		if (!isValid()) return;
		node->setWorldPosition(Unigine::Math::Vec3(x, y, z));
	}

	Unigine::Math::Vec3 getPosition() const {
		return isValid() ? node->getWorldPosition() : Unigine::Math::Vec3(0, 0, 0);
	}

	void setPosition(const Unigine::Math::Vec3& p) {
		if (isValid()) node->setWorldPosition(p);
	}
};