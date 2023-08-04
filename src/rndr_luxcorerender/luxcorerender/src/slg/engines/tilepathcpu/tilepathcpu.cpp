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

#include <limits>

#include "slg/samplers/tilepathsampler.h"
#include "slg/engines/tilepathcpu/tilepathcpu.h"
#include "slg/engines/tilepathcpu/tilepathcpurenderstate.h"
#include "slg/engines/caches/photongi/photongicache.h"
#include "slg/samplers/sobol.h"

using namespace std;
using namespace luxrays;
using namespace slg;

//------------------------------------------------------------------------------
// TilePathCPURenderEngine
//------------------------------------------------------------------------------

TilePathCPURenderEngine::TilePathCPURenderEngine(const RenderConfig *rcfg) :
		CPUTileRenderEngine(rcfg), photonGICache(nullptr) {
}

TilePathCPURenderEngine::~TilePathCPURenderEngine() {
	delete photonGICache;
}

void TilePathCPURenderEngine::InitFilm() {
	film->AddChannel(Film::RADIANCE_PER_PIXEL_NORMALIZED);
	film->SetRadianceGroupCount(renderConfig->scene->lightDefs.GetLightGroupCount());
	film->Init();
}

RenderState *TilePathCPURenderEngine::GetRenderState() {
	return new TilePathCPURenderState(bootStrapSeed, tileRepository, photonGICache);
}

void TilePathCPURenderEngine::StartLockLess() {
	const Properties &cfg = renderConfig->cfg;

	//--------------------------------------------------------------------------
	// Check to have the right sampler settings
	//--------------------------------------------------------------------------

	// Sobol is the default sampler (but it can not work with TILEPATH)
	CheckSamplersForTile(RenderEngineType2String(GetType()), cfg);

	//--------------------------------------------------------------------------
	// Initialize rendering parameters
	//--------------------------------------------------------------------------

	aaSamples = Max(1, cfg.Get(GetDefaultProps().Get("tilepath.sampling.aa.size")).Get<int>());

	// pathTracer must be configured here because it is then used
	// to set tileRepository->varianceClamping, etc.
	pathTracer.ParseOptions(cfg, GetDefaultProps());

	//--------------------------------------------------------------------------
	// Restore render state if there is one
	//--------------------------------------------------------------------------

	if (startRenderState) {
		// Check if the render state is of the right type
		startRenderState->CheckEngineTag(GetObjectTag());

		TilePathCPURenderState *rs = (TilePathCPURenderState *)startRenderState;

		// Use a new seed to continue the rendering
		const u_int newSeed = rs->bootStrapSeed + 1;
		SLG_LOG("Continuing the rendering with new TILEPATHCPU seed: " + ToString(newSeed));
		SetSeed(newSeed);

		// Transfer the ownership of TileRepository pointer
		tileRepository = rs->tileRepository;
		rs->tileRepository = nullptr;

		// Transfer the ownership of PhotonGI cache pointer
		photonGICache = rs->photonGICache;
		rs->photonGICache = nullptr;
		
		delete startRenderState;
		startRenderState = NULL;
	} else {
		film->Reset();

		tileRepository = TileRepository::FromProperties(renderConfig->cfg);
		tileRepository->varianceClamping = VarianceClamping(pathTracer.sqrtVarianceClampMaxValue);
		tileRepository->InitTiles(*film);
	}

	//--------------------------------------------------------------------------
	// Allocate PhotonGICache if enabled
	//--------------------------------------------------------------------------

	// note: photonGICache could have been restored from the render state
	if ((GetType() != RTPATHCPU) && !photonGICache) {
		photonGICache = PhotonGICache::FromProperties(renderConfig->scene, cfg);

		// photonGICache will be nullptr if the cache is disabled
		if (photonGICache)
			photonGICache->Preprocess(renderThreads.size());
	}

	//--------------------------------------------------------------------------
	// Initialize the PathTracer class with rendering parameters
	//--------------------------------------------------------------------------

	pathTracer.InitPixelFilterDistribution(pixelFilter);
	pathTracer.SetPhotonGICache(photonGICache);

	//--------------------------------------------------------------------------

	CPURenderEngine::StartLockLess();
}

void TilePathCPURenderEngine::StopLockLess() {
	CPUTileRenderEngine::StopLockLess();

	pathTracer.DeletePixelFilterDistribution();
	
	delete photonGICache;
	photonGICache = nullptr;
}

//------------------------------------------------------------------------------
// Static methods used by RenderEngineRegistry
//------------------------------------------------------------------------------

Properties TilePathCPURenderEngine::ToProperties(const Properties &cfg) {
	Properties props;
	
	props <<
			CPUTileRenderEngine::ToProperties(cfg) <<
			cfg.Get(GetDefaultProps().Get("renderengine.type")) <<
			cfg.Get(GetDefaultProps().Get("tilepath.sampling.aa.size")) <<
			PathTracer::ToProperties(cfg) <<
			PhotonGICache::ToProperties(cfg);

	return props;
}

RenderEngine *TilePathCPURenderEngine::FromProperties(const RenderConfig *rcfg) {
	return new TilePathCPURenderEngine(rcfg);
}

const Properties &TilePathCPURenderEngine::GetDefaultProps() {
	static Properties props = Properties() <<
			CPUTileRenderEngine::GetDefaultProps() <<
			Property("renderengine.type")(GetObjectTag()) <<
			Property("tilepath.sampling.aa.size")(3) <<
			PathTracer::GetDefaultProps() <<
			PhotonGICache::GetDefaultProps();

	return props;
}
