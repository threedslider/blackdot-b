#line 2 "triangle_funcs.cl"

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

/*OPENCL_FORCE_INLINE void Triangle_UniformSample(const float u0, const float u1, float *b1, float *b2) {
	const float su1 = sqrt(u0);
	*b1 = 1.f - su1;
	*b2 = u1 * su1;
}*/

OPENCL_FORCE_INLINE void Triangle_LowDiscrepancySample(const float u1, float *u, float *v) {
	// New implementation from: https://pharr.org/matt/blog/2019/02/27/triangle-sampling-1.html
	// and: https://pharr.org/matt/blog/2019/03/13/triangle-sampling-1.5.html
	
	// Using 1ull crashes AMD, NVIDIA and Intel OpenCL compilers !
	//uint uf = u1 * (1ull << 32);  // Fixed point
	uint uf = u1 * 0xffffffffu;  // Fixed point
	
	float cx = 0.f, cy = 0.f;
	float w = .5f;

	for (uint i = 0; i < 16; i++) {
		uint uu = uf >> 30;
		bool flip = (uu & 3) == 0;

		cy += ((uu & 1) == 0) * w;
		cx += ((uu & 2) == 0) * w;

		w *= flip ? -.5f : .5f;
		uf <<= 2;
	}

	*u = cx + w / 3.f;
	*v = cy + w / 3.f;
}

OPENCL_FORCE_INLINE float3 Triangle_Sample(const float3 p0, const float3 p1, const float3 p2,
	const float u0, const float u1,
	float *b0, float *b1, float *b2) {
	// Old triangle uniform sampling
	//Triangle_UniformSample(u0, u1, b1, b2);

	// This new implementation samples from a one dimensional sample
	Triangle_LowDiscrepancySample(u0, b1, b2);
	
	*b0 = 1.f - (*b1) - (*b2);

	return (*b0) * p0 + (*b1) * p1 + (*b2) * p2;
}

OPENCL_FORCE_INLINE float3 Triangle_GetGeometryNormal(const float3 p0, const float3 p1, const float3 p2) {
	return normalize(cross(p1 - p0, p2 - p0));
}

OPENCL_FORCE_INLINE float3 Triangle_InterpolateNormal(const float3 n0, const float3 n1, const float3 n2,
		const float b0, const float b1, const float b2) {
	return normalize(b0 * n0 + b1 * n1 + b2 * n2);
}

OPENCL_FORCE_INLINE float2 Triangle_InterpolateUV(const float2 uv0, const float2 uv1, const float2 uv2,
		const float b0, const float b1, const float b2) {
	return b0 * uv0 + b1 * uv1 + b2 * uv2;
}

OPENCL_FORCE_INLINE float3 Triangle_InterpolateColor(const float3 rgb0, const float3 rgb1, const float3 rgb2,
		const float b0, const float b1, const float b2) {
	return b0 * rgb0 + b1 * rgb1 + b2 * rgb2;
}

OPENCL_FORCE_INLINE float Triangle_InterpolateAlpha(const float a0, const float a1, const float a2,
		const float b0, const float b1, const float b2) {
	return b0 * a0 + b1 * a1 + b2 * a2;
}

OPENCL_FORCE_INLINE float Triangle_InterpolateVertexAOV(const float v0, const float v1, const float v2,
		const float b0, const float b1, const float b2) {
	return b0 * v0 + b1 * v1 + b2 * v2;
}

OPENCL_FORCE_INLINE void Triangle_Intersect(
		const float3 rayOrig,
		const float3 rayDir,
		const float mint,
		float *maxt,
		uint *hitMeshIndex,
		uint *hitTriangleIndex,
		float *hitB1,
		float *hitB2,
		const uint currentMeshIndex,
		const uint currentTriangleIndex,
		const float3 v0,
		const float3 v1,
		const float3 v2) {
	// Calculate intersection
	const float3 e1 = v1 - v0;
	const float3 e2 = v2 - v0;
	const float3 s1 = cross(rayDir, e2);

	const float divisor = dot(s1, e1);
	if (divisor == 0.f)
		return;

	const float invDivisor = 1.f / divisor;

	// Compute first barycentric coordinate
	const float3 d = rayOrig - v0;
	const float b1 = dot(d, s1) * invDivisor;
	if (b1 < 0.f)
		return;

	// Compute second barycentric coordinate
	const float3 s2 = cross(d, e1);
	const float b2 = dot(rayDir, s2) * invDivisor;
	if (b2 < 0.f)
		return;

	const float b0 = 1.f - b1 - b2;
	if (b0 < 0.f)
		return;

	// Compute _t_ to intersection point
	const float t = dot(e2, s2) * invDivisor;
	if (t < mint || t > *maxt)
		return;

	*maxt = t;
	*hitB1 = b1;
	*hitB2 = b2;
	*hitMeshIndex = currentMeshIndex;
	*hitTriangleIndex = currentTriangleIndex;
}
