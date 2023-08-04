#line 2 "texture_clamp_funcs.cl"

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

//------------------------------------------------------------------------------
// Clamp texture
//------------------------------------------------------------------------------

OPENCL_FORCE_INLINE float ClampTexture_ConstEvaluateFloat(const float v,
		const float minVal, const float maxVal) {
	return clamp(v, minVal, maxVal);
}

OPENCL_FORCE_INLINE float3 ClampTexture_ConstEvaluateSpectrum(const float3 v,
		const float minVal, const float maxVal) {
	return clamp(v, minVal, maxVal);
}

OPENCL_FORCE_NOT_INLINE void ClampTexture_EvalOp(
		__global const Texture* restrict texture,
		const TextureEvalOpType evalType,
		__global float *evalStack,
		uint *evalStackOffset,
		__global const HitPoint *hitPoint,
		const float sampleDistance
		TEXTURES_PARAM_DECL) {
	switch (evalType) {
		case EVAL_FLOAT: {
			float tex1;
			EvalStack_PopFloat(tex1);

			const float eval = ClampTexture_ConstEvaluateFloat(tex1,
					texture->clampTex.minVal, texture->clampTex.maxVal);
			EvalStack_PushFloat(eval);
			break;
		}
		case EVAL_SPECTRUM: {
			float3 tex1;
			EvalStack_PopFloat3(tex1);

			const float3 eval = ClampTexture_ConstEvaluateSpectrum(tex1,
					texture->clampTex.minVal, texture->clampTex.maxVal);
			EvalStack_PushFloat3(eval);
			break;
		}
		case EVAL_BUMP_GENERIC_OFFSET_U:
			Texture_EvalOpGenericBumpOffsetU(evalStack, evalStackOffset,
					hitPoint, sampleDistance);
			break;
		case EVAL_BUMP_GENERIC_OFFSET_V:
			Texture_EvalOpGenericBumpOffsetV(evalStack, evalStackOffset,
					hitPoint, sampleDistance);
			break;
		case EVAL_BUMP:
			Texture_EvalOpGenericBump(evalStack, evalStackOffset,
					hitPoint, sampleDistance);
			break;
		default:
			// Something wrong here
			break;
	}
}
