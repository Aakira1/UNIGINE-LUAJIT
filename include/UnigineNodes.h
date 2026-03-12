/* Copyright (C) 2005-2025, UNIGINE. All rights reserved.
*
* This file is a part of the UNIGINE 2 SDK.
*
* Your use and / or redistribution of this software in source and / or
* binary form, with or without modification, is subject to: (i) your
* ongoing acceptance of and compliance with the terms and conditions of
* the UNIGINE License Agreement; and (ii) your inclusion of this notice
* in any version of this software that you use or redistribute.
* A copy of the UNIGINE License Agreement is available by contacting
* UNIGINE. at http://unigine.com/
*/
// DO NOT EDIT DIRECTLY. This is an auto-generated file. Your changes will be lost.

#pragma once

#include "UnigineNode.h"
#include "UnigineAnimation.h"

namespace Unigine
{

class AnimationPlayback;
//////////////////////////////////////////////////////////////////////////

class UNIGINE_API NodeAnimationPlayback : public Node
{
public:
	typedef const NodeAnimationPlayback ConstNodeAnimationPlayback;

	static Node::TYPE type() { return Node::NODE_ANIMATION_PLAYBACK; }
	static bool convertible(Node *node) { return (node && node->getType() == type()); }

	static Ptr<NodeAnimationPlayback> create();
	void play();
	void pause();
	void stop();
	bool isPlaying() const;
	void setTime(float time, float from = -1.0f, float to = -1.0f);
	float getTime() const;
	void setTimeTo(float to);
	float getTimeTo() const;
	void setTimeFrom(float from);
	float getTimeFrom() const;
	void setLoop(bool loop);
	bool isLoop() const;
	void setSpeed(float speed);
	float getSpeed() const;
	void setTrackFileGUID(const UGUID &trackfileguid);
	UGUID getTrackFileGUID() const;
	void setTrackPath(const char *path);
	const char *getTrackPath() const;
	void setPlayOnEnable(bool enable);
	bool isPlayOnEnable() const;
	void setRestartOnEnable(bool enable);
	bool isRestartOnEnable() const;
};
typedef Ptr<NodeAnimationPlayback> NodeAnimationPlaybackPtr;
typedef Ptr<const NodeAnimationPlayback> ConstNodeAnimationPlaybackPtr;
typedef const NodeAnimationPlayback ConstNodeAnimationPlayback;

//////////////////////////////////////////////////////////////////////////

class UNIGINE_API NodeDummy : public Node
{
public:
	typedef const NodeDummy ConstNodeDummy;

	static Node::TYPE type() { return Node::NODE_DUMMY; }
	static bool convertible(Node *node) { return (node && node->getType() == type()); }

	static Ptr<NodeDummy> create();
};
typedef Ptr<NodeDummy> NodeDummyPtr;
typedef Ptr<const NodeDummy> ConstNodeDummyPtr;
typedef const NodeDummy ConstNodeDummy;

//////////////////////////////////////////////////////////////////////////

class UNIGINE_API NodeLayer : public Node
{
public:
	typedef const NodeLayer ConstNodeLayer;

	static Node::TYPE type() { return Node::NODE_LAYER; }
	static bool convertible(Node *node) { return (node && node->getType() == type()); }

	static Ptr<NodeLayer> create(const char *path);
	void setNodePath(const char *path);
	const char *getNodePath() const;
};
typedef Ptr<NodeLayer> NodeLayerPtr;
typedef Ptr<const NodeLayer> ConstNodeLayerPtr;
typedef const NodeLayer ConstNodeLayer;

//////////////////////////////////////////////////////////////////////////

class UNIGINE_API NodeReference : public Node
{
public:
	typedef const NodeReference ConstNodeReference;

	static Node::TYPE type() { return Node::NODE_REFERENCE; }
	static bool convertible(Node *node) { return (node && node->getType() == type()); }

	static Ptr<NodeReference> create(const char *path);
	Ptr<Node> getReference() const;
	Ptr<Node> detachReference() const;
	void setNodePath(const char *path);
	const char *getNodePath() const;
	static bool canBeReference(const char *path, const Ptr<Node> &node);
};
typedef Ptr<NodeReference> NodeReferencePtr;
typedef Ptr<const NodeReference> ConstNodeReferencePtr;
typedef const NodeReference ConstNodeReference;

class NodeTrigger;
template class UNIGINE_API EventHolder<EventInterfaceInvoker<const Ptr<NodeTrigger> &>>;
template class UNIGINE_API EventInterfaceConnection<EventInterfaceInvoker<const Ptr<NodeTrigger> &>>;
//////////////////////////////////////////////////////////////////////////

class UNIGINE_API NodeTrigger : public Node
{
public:
	typedef const NodeTrigger ConstNodeTrigger;

	static Node::TYPE type() { return Node::NODE_TRIGGER; }
	static bool convertible(Node *node) { return (node && node->getType() == type()); }

	static Ptr<NodeTrigger> create();
	const char *getEnabledCallbackName() const;
	void setEnabledCallbackName(const char *name);
	const char *getPositionCallbackName() const;
	void setPositionCallbackName(const char *name);
	Event<const Ptr<NodeTrigger> &> &getEventEnabled();
	Event<const Ptr<NodeTrigger> &> &getEventPosition();

private:

	EventHolder<EventInterfaceInvoker<const Ptr<NodeTrigger> &>> event_enabled;
	EventInterfaceConnection<EventInterfaceInvoker<const Ptr<NodeTrigger> &>> event_enabled_connection;
	EventHolder<EventInterfaceInvoker<const Ptr<NodeTrigger> &>> event_position;
	EventInterfaceConnection<EventInterfaceInvoker<const Ptr<NodeTrigger> &>> event_position_connection;
};
typedef Ptr<NodeTrigger> NodeTriggerPtr;
typedef Ptr<const NodeTrigger> ConstNodeTriggerPtr;
typedef const NodeTrigger ConstNodeTrigger;

class NodeExternBase;
//////////////////////////////////////////////////////////////////////////

class UNIGINE_API NodeExtern : public Node
{
public:
	typedef const NodeExtern ConstNodeExtern;

	static Node::TYPE type() { return Node::NODE_EXTERN; }
	static bool convertible(Node *node) { return (node && node->getType() == type()); }

	static Ptr<NodeExtern> create(int class_id);
	int getClassID();
	NodeExternBase *getNodeExtern() const;
};
typedef Ptr<NodeExtern> NodeExternPtr;
typedef Ptr<const NodeExtern> ConstNodeExternPtr;
typedef const NodeExtern ConstNodeExtern;


class UNIGINE_API  NodeExternBase : public virtual Base
{
public:
typedef const NodeExternBase ConstNodeExternBase;
	NodeExternBase();
	NodeExternBase(void * object);
	virtual ~NodeExternBase();
	virtual int getClassID() = 0;
	Ptr<Node> getNode() const;
	Ptr<NodeExtern> getNodeExtern() const;
	typedef NodeExternBase *(*ConstructorFunc)(void *object);
	static void addClassID(int class_id, ConstructorFunc func);
	template <class Type> static void addClassID(int class_id) { addClassID(class_id, constructor<Type>); }
	virtual void updateEnabled();
	virtual void updateTransform();
	virtual const Math::BoundBox &getBoundBox();
	virtual const Math::BoundSphere &getBoundSphere();
	virtual void renderHandler();
	virtual void renderVisualizer();
	virtual void copy(NodeExternBase *node, int is_root);
	virtual void swap(NodeExternBase *node);
	virtual int loadWorld(const Ptr<Xml> &xml);
	virtual int saveWorld(const Ptr<Xml> &xml);
	virtual int saveState(const Ptr<Stream> &stream);
	virtual int restoreState(const Ptr<Stream> &stream);
	static void savePointer(const StreamPtr &stream, NodeExternBase *node);
	static NodeExternBase *restorePointer(const StreamPtr &stream);
private:
	template <class Type> static NodeExternBase *constructor(void *object) { return new Type(object); }
	struct NodeExternData;
	NodeExternData *data;
};
typedef Ptr<NodeExternBase> NodeExternBasePtr;
typedef Ptr<const NodeExternBase> ConstNodeExternBasePtr;
typedef const NodeExternBase ConstNodeExternBase;

} // namespace Unigine
