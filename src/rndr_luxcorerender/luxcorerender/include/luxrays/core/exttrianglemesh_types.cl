#line 2 "exttrianglemesh_types.cl"

/***************************************************************************
 * Copyright 1998-2020 by authors (see AUTHORS.txt)                        *
 *                                                                         *
 *   This file is part of LuxCoreRender.                                   *
 *                                                                         *
 * Licensed under the Apache License, Version 2.0 (the "License");         *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 *                                                                         *
 *     http://www.apache.org/licenses/LICENSE-2.0                          *
 *                                                                         *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/

#define EXTMESH_MAX_DATA_COUNT 8u

typedef struct {
	Transform appliedTrans;
	int appliedTransSwapsHandedness;
} TriangleMeshParam;

typedef struct {
	Transform trans;
	int transSwapsHandedness;
} TriangleInstanceMeshParam;

typedef struct {
	MotionSystem motionSystem;
} TriangleMotionMeshParam;

typedef struct {
	MeshType type;

	// Vertex information
	unsigned int vertsOffset;
	unsigned int normalsOffset;
	unsigned int triNormalsOffset;
	unsigned int uvsOffset[EXTMESH_MAX_DATA_COUNT];
	unsigned int colsOffset[EXTMESH_MAX_DATA_COUNT];
	unsigned int alphasOffset[EXTMESH_MAX_DATA_COUNT];

	 // Vertex and Triangle AOV
	unsigned int vertexAOVOffset[EXTMESH_MAX_DATA_COUNT];
	unsigned int triAOVOffset[EXTMESH_MAX_DATA_COUNT];

	// Triangle information
	unsigned int trisOffset;

	// Object space transformation
	union {
		TriangleMeshParam triangle;
		TriangleInstanceMeshParam instance;
		TriangleMotionMeshParam motion;
	};
} ExtMesh;

#if defined(SLG_OPENCL_KERNEL)

#define EXTMESH_PARAM_DECL , \
		__global const ExtMesh* restrict meshDescs, \
		__global const Point* restrict vertices, \
		__global const Normal* restrict vertNormals, \
		__global const Normal* restrict triNormals, \
		__global const UV* restrict vertUVs, \
		__global const Spectrum* restrict vertCols, \
		__global const float* restrict vertAlphas, \
		__global const float* restrict vertexAOVs, \
		__global const float* restrict triAOVs, \
		__global const Triangle* restrict triangles, \
		__global const InterpolatedTransform* restrict interpolatedTransforms
#define EXTMESH_PARAM , \
		meshDescs, \
		vertices, \
		vertNormals, \
		triNormals, \
		vertUVs, \
		vertCols, \
		vertAlphas, \
		vertexAOVs, \
		triAOVs, \
		triangles, \
		interpolatedTransforms

#endif
