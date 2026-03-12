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

#include "UniginePtr.h"
#include "UnigineVector.h"
#include "UnigineEllipsoid.h"

namespace Unigine
{


class UNIGINE_API MeshAnimation : public APIInterface
{
public:
	typedef const MeshAnimation ConstMeshAnimation;
	static constexpr bool HAS_OWNERSHIP = true;

	static Ptr<MeshAnimation> create();
	static Ptr<MeshAnimation> create(const char *path);
	static Ptr<MeshAnimation> create(const Ptr<MeshAnimation> &src);
	void assignFrom(const Ptr<MeshAnimation> &src);
	void clear();
	void flipYZ();
	bool info(const char *path) const;
	bool load(const char *path);
	bool save(const char *path) const;
	int getNumBones() const;
	int findBone(const char *name) const;
	int addBone(const char *name = 0, int parent = -1);
	void setBoneName(int bone, const char *name);
	const char *getBoneName(int bone) const;
	void setBoneParent(int bone, int parent);
	int getBoneParent(int bone) const;
	void setBoneTransform(int bone, const Math::mat4 &transform);
	Math::mat4 getBoneTransform(int bone) const;
	int setBoneTransforms(const Vector<Math::mat4> &transforms);
	void getBoneTransforms(Vector<Math::mat4> &transforms) const;
	int getNumAnimationBones() const;
	short getAnimationBoneID(int num) const;
	void setAnimationBones(const Vector<short> &bone_indices);
	void getAnimationBones(Vector<short> &bone_indices) const;
	int setAnimationBoneTransforms(const Vector<Math::mat4> &transforms, int frame);
	int getAnimationBoneTransforms(Vector<Math::mat4> &transforms, int frame) const;
	void setNumFrames(int num);
	int getNumFrames() const;
	void setFrame(int frame, const Vector<Math::mat4> &bones);
	void getFrame(int frame, Vector<Math::mat4> &bones) const;
	void setFrame(int frame, const Vector<Math::vec3> &xyz, const Vector<Math::quat> &rot, const Vector<Math::vec3> &scale);
	void getFrame(int frame, Vector<Math::vec3> &xyz, Vector<Math::quat> &rot, Vector<Math::vec3> &scale) const;
};
typedef Ptr<MeshAnimation> MeshAnimationPtr;
typedef Ptr<const MeshAnimation> ConstMeshAnimationPtr;
typedef const MeshAnimation ConstMeshAnimation;


class UNIGINE_API Mesh : public APIInterface
{
public:
	typedef const Mesh ConstMesh;
	static constexpr bool HAS_OWNERSHIP = true;


	enum LIGHTMAP_RESOLUTION
	{
		LIGHTMAP_RESOLUTION_MODE_32,
		LIGHTMAP_RESOLUTION_MODE_64,
		LIGHTMAP_RESOLUTION_MODE_128,
		LIGHTMAP_RESOLUTION_MODE_256,
		LIGHTMAP_RESOLUTION_MODE_512,
		LIGHTMAP_RESOLUTION_MODE_1024,
		LIGHTMAP_RESOLUTION_MODE_2048,
		LIGHTMAP_RESOLUTION_MODE_4096,
	};

	enum
	{
		VERTEX_CACHE = 1 << 0,	// vertex cache optimization
		BACK_TO_FRONT = 1 << 1,	// back to front order
		VERTEX_CACHE_LEGACY = 1 << 2,
		BACK_TO_FRONT_LEGACY = 1 << 3,
		NUM_WEIGHTS = 4,	// vertex cache optimization
	};

	enum
	{
		COLLISION_DATA_BOUNDS = 1 << 0,
		COLLISION_DATA_EDGES = 1 << 1,
		COLLISION_DATA_SPATIAL_TREE = 1 << 3,
		COLLISION_DATA_ALL = COLLISION_DATA_BOUNDS | COLLISION_DATA_EDGES | COLLISION_DATA_SPATIAL_TREE,
	};
	static Ptr<Mesh> create();
	static Ptr<Mesh> create(const char *path);
	static Ptr<Mesh> create(const Ptr<ConstMesh> &constmesh);
	void assignFrom(const Ptr<ConstMesh> &mesh);
	void swap(const Ptr<Mesh> &mesh);
	int info(const char *path) const;
	int load(const char *path);
	int save(const char *path) const;
	void clear();
	bool flipYZ(int surface = -1);
	bool flipTangent(int surface = -1);
	bool hasSpatialTree(int surface = -1) const;
	void clearSpatialTree(int surface = -1);
	void createSpatialTree(int surface = -1);
	void setSpatialTreeTriangles(int triangles);
	int getSpatialTreeTriangles() const;
	bool hasEdges(int surface = -1) const;
	void clearEdges(int surface = -1);
	void createEdges(int surface = -1);
	void createBounds(int surface = -1);
	bool hasCollisionData(int surface = -1, int flags = Mesh::COLLISION_DATA_ALL) const;
	void clearCollisionData(int surface = -1, int flags = Mesh::COLLISION_DATA_ALL);
	void createCollisionData(int surface = -1, int flags = Mesh::COLLISION_DATA_ALL);
	bool removeIndices(int surface = -1);
	bool createIndices(int surface = -1);
	bool optimizeIndices(int flags, int surface = -1);
	bool createNormals(int surface = -1, int target = -1);
	bool createNormals(float angle, int surface = -1, int target = -1);
	bool createTangents(int surface = -1, int target = -1);
	bool createTangents(float angle, const Vector<int> &surfaces);
	bool getRandomPoint(Math::vec3 &ret_point, Math::vec3 &ret_normal, Math::vec3 &ret_velocity, int surface) const;
	bool getIntersection(const Math::vec3 &p0, const Math::vec3 &p1, Math::vec3 *ret_point, Math::vec3 *ret_normal, int *ret_index, int surface, int target = 0) const;
	bool getIntersection(const Math::dvec3 &p0, const Math::dvec3 &p1, Math::dvec3 *ret_point, Math::vec3 *ret_normal, int *ret_index, int surface, int target = 0) const;
	int getNumBones() const;
	int findBone(const char *name) const;
	int addBone(const char *name = 0, int parent = -1);
	void setBoneName(int bone, const char *name);
	const char *getBoneName(int bone) const;
	void setBoneParent(int bone, int parent);
	int getBoneParent(int bone) const;
	void setBoneTransform(int bone, const Math::mat4 &transform);
	Math::mat4 getBoneTransform(int bone) const;
	int setBoneTransforms(const Vector<Math::mat4> &transforms);
	void getBoneTransforms(Vector<Math::mat4> &transforms) const;
	int getNumSurfaces() const;
	int findSurface(const char *name) const;
	void sortSurfaces();
	void clearSurface(int surface = -1, int target = -1);
	int addSurface(const char *name = 0);
	void setSurfaceName(int surface, const char *name);
	const char *getSurfaceName(int surface) const;
	void setSurfaceLightmapUVChannel(int surface, char uv_channel);
	char getSurfaceLightmapUVChannel(int surface) const;
	void setSurfaceLightmapResolution(int surface, Mesh::LIGHTMAP_RESOLUTION resolution);
	Mesh::LIGHTMAP_RESOLUTION getSurfaceLightmapResolution(int surface) const;
	void setNumSurfaceTargets(int surface, int num);
	int getNumSurfaceTargets(int surface) const;
	void setSurfaceTargetName(int surface, int target, const char *name);
	const char *getSurfaceTargetName(int surface, int target) const;
	int findSurfaceTarget(int surface, const char *name) const;
	bool getSurfaceCollision(const Math::BoundBox &bb, Vector<int> &surfaces) const;
	bool getSurfaceCollision(Math::BoundFrustum &bf, Vector<int> &surfaces) const;
	bool getSurfaceCollision(const Math::vec3 &p0, const Math::vec3 &p1, Vector<int> &surfaces) const;
	bool getTriangleCollision(const Math::BoundBox &bb, Vector<int> &triangles, int surface) const;
	bool getTriangleCollision(const Math::BoundFrustum &bf, Vector<int> &triangles, int surface) const;
	bool getTriangleCollision(const Math::vec3 &p0, const Math::vec3 &p1, Vector<int> &triangles, int surface) const;
	typedef bool (*CollisionFilter)(const Math::BoundBox &bb);
	typedef void (*CollisionResult)(int);
	void getSurfaceCollision(CollisionFilter filter, CollisionResult surface_index_rusult) const;
	void getTriangleCollision(CollisionFilter filter, CollisionResult triangle_index_rusult, int surface) const;
	bool setSurfaceTransform(const Math::mat4 &transform, int surface = -1, int target = -1);
	int addMeshSurface(const char *v, const Ptr<ConstMesh> &mesh, int surface, int target = -1, int colision_data_flags = Mesh::COLLISION_DATA_ALL);
	int addMeshSurface(int v, const Ptr<ConstMesh> &mesh, int surface, int target = -1, int colision_data_flags = Mesh::COLLISION_DATA_ALL);
	int addEmptySurface(const char *name, int num_vertex, int num_indices);
	int addSurfaceTarget(int surface, const char *name = 0);
	int addBoxSurface(const char *name, const Math::vec3 &size, int colision_data_flags = Mesh::COLLISION_DATA_ALL);
	int addPlaneSurface(const char *name, float width, float height, float step, int colision_data_flags = Mesh::COLLISION_DATA_ALL);
	int addSphereSurface(const char *name, float radius, int stacks, int slices, int colision_data_flags = Mesh::COLLISION_DATA_ALL);
	int addCapsuleSurface(const char *name, float radius, float height, int stacks, int slices, int colision_data_flags = Mesh::COLLISION_DATA_ALL);
	int addCylinderSurface(const char *name, float radius, float height, int stacks, int slices, int colision_data_flags = Mesh::COLLISION_DATA_ALL);
	int addPrismSurface(const char *name, float size_0, float size_1, float height, int sides, int colision_data_flags = Mesh::COLLISION_DATA_ALL);
	int addIcosahedronSurface(const char *name, float radius, int colision_data_flags = Mesh::COLLISION_DATA_ALL);
	int addDodecahedronSurface(const char *name, float radius, int colision_data_flags = Mesh::COLLISION_DATA_ALL);
	int getNumCVertex(int surface = 0) const;
	int getNumTVertex(int surface = 0) const;
	void remapCVertex(int surface = 0);
	void setNumWeights(int size, int surface = 0);
	int getNumWeights(int surface = 0) const;
	void setWeightCount(int num, int count, int surface = 0);
	int getWeightCount(int num, int surface = 0) const;
	void setWeightBones(int num, const Math::ivec4 &bones, int surface = 0);
	Math::ivec4 getWeightBones(int num, int surface = 0) const;
	void setWeightWeights(int num, const Math::vec4 &weights, int surface = 0);
	Math::vec4 getWeightWeights(int num, int surface = 0) const;
	void setNumVertex(int size, int surface = 0, int target = 0);
	int getNumVertex(int surface, int target = 0) const;
	void addVertex(const Vector<Math::vec3> &vertices, int surface = 0, int target = 0);
	void addVertex(const Math::vec3 &vertex, int surface = 0, int target = 0);
	void setVertex(int num, const Math::vec3 &vertex, int surface = 0, int target = 0);
	Math::vec3 getVertex(int num, int surface = 0, int target = 0) const;
	void setNumTexCoords0(int size, int surface = 0);
	int getNumTexCoords0(int surface = 0) const;
	void addTexCoords0(const Vector<Math::vec2> &texcoords, int surface = 0);
	void addTexCoord0(const Math::vec2 &texcoord, int surface = 0);
	void setTexCoord0(int num, const Math::vec2 &texcoord, int surface = 0);
	Math::vec2 getTexCoord0(int num, int surface = 0) const;
	void setNumTexCoords1(int size, int surface = 0);
	int getNumTexCoords1(int surface = 0) const;
	void addTexCoords1(const Vector<Math::vec2> &texcoords, int surface = 0);
	void addTexCoord1(const Math::vec2 &texcoord, int surface = 0);
	void setTexCoord1(int num, const Math::vec2 &texcoord, int surface = 0);
	Math::vec2 getTexCoord1(int num, int surface = 0) const;
	void setNumNormals(int size, int surface = 0, int target = 0);
	int getNumNormals(int surface = 0, int target = 0) const;
	void addNormals(const Vector<Math::vec3> &normals, int surface = 0, int target = 0);
	void addNormal(const Math::vec3 &normal, int surface = 0, int target = 0);
	void setNormal(int num, const Math::vec3 &normal, int surface = 0, int target = 0);
	Math::vec3 getNormal(int num, int surface = 0, int target = 0) const;
	void setNumTangents(int size, int surface = 0, int target = 0);
	int getNumTangents(int surface, int target = 0) const;
	void addTangents(const Vector<Math::quat> &tangents, int surface = 0, int target = 0);
	void addTangent(const Math::quat &tangent, int surface = 0, int target = 0);
	void setTangent(int num, const Math::quat &tangent, int surface = 0, int target = 0);
	Math::quat getTangent(int num, int surface = 0, int target = 0) const;
	void setNumColors(int size, int surface = 0);
	int getNumColors(int surface = 0) const;
	void addColors(const Vector<Math::vec4> &colors, int surface = 0);
	void addColor(const Math::vec4 &color, int surface = 0);
	void setColor(int num, const Math::vec4 &color, int surface = 0);
	Math::vec4 getColor(int num, int surface = 0) const;
	void setNumCIndices(int size, int surface = 0);
	int getNumCIndices(int surface = 0) const;
	void addCIndices(const Vector<int> &indices, int surface = 0);
	void addCIndex(int index, int surface = 0);
	void setCIndex(int num, int index, int surface = 0);
	int getCIndex(int num, int surface = 0) const;
	void setNumTIndices(int size, int surface = 0);
	int getNumTIndices(int surface = 0) const;
	void addTIndices(const Vector<int> &indices, int surface = 0);
	void addTIndex(int index, int surface = 0);
	void setTIndex(int num, int index, int surface = 0);
	int getTIndex(int num, int surface = 0) const;
	void setNumIndices(int size, int surface = 0);
	int getNumIndices(int surface = 0) const;
	void addIndices(const Vector<int> &indices, int surface = 0);
	void addIndex(int index, int surface = 0);
	void setIndex(int num, int index, int surface = 0);
	int getIndex(int num, int surface = 0) const;
	Vector<Math::vec3> &getVertices(int surface = 0, int target = 0);
	Vector<Math::vec3> &getNormals(int surface = 0, int target = 0);
	Vector<Math::quat> &getTangents(int surface = 0, int target = 0);
	Vector<Math::vec2> &getTexCoords0(int surface = 0);
	Vector<Math::vec2> &getTexCoords1(int surface = 0);
	Vector<Math::bvec4> &getColors(int surface = 0);
	Vector<int> &getCIndices(int surface = 0);
	Vector<int> &getTIndices(int surface = 0);
	const Vector<Math::vec3> &getVertices(int surface = 0, int target = 0) const;
	const Vector<Math::vec3> &getNormals(int surface = 0, int target = 0) const;
	const Vector<Math::quat> &getTangents(int surface = 0, int target = 0) const;
	const Vector<Math::vec2> &getTexCoords0(int surface = 0) const;
	const Vector<Math::vec2> &getTexCoords1(int surface = 0) const;
	const Vector<Math::bvec4> &getColors(int surface = 0) const;
	const Vector<int> &getCIndices(int surface = 0) const;
	const Vector<int> &getTIndices(int surface = 0) const;
	Math::BoundBox getBoundBox() const;
	Math::BoundBox getBoundBox(int surface) const;
	Math::BoundSphere getBoundSphere() const;
	Math::BoundSphere getBoundSphere(int surface) const;
	void setBoundBox(const Math::BoundBox &bb, int surface);
	void setBoundBox(const Math::BoundBox &bb);
	void setBoundSphere(const Math::BoundSphere &bs, int surface);
	void setBoundSphere(const Math::BoundSphere &bs);
	size_t getMemoryUsage() const;
};
typedef Ptr<Mesh> MeshPtr;
typedef Ptr<const Mesh> ConstMeshPtr;
typedef const Mesh ConstMesh;

} // namespace Unigine
