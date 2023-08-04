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

#ifndef _SLG_LIGHTSTRATEGY_DLSCACHE_H
#define	_SLG_LIGHTSTRATEGY_DLSCACHE_H

#include "slg/slg.h"
#include "slg/lights/strategies/lightstrategy.h"
#include "slg/lights/strategies/dlscacheimpl/dlscacheimpl.h"
#include "slg/lights/strategies/logpower.h"

namespace slg {

// OpenCL data types
namespace ocl {
#include "slg/lights/strategies/dlsc_types.cl"
}

//------------------------------------------------------------------------------
// LightStrategyDLSCache
//------------------------------------------------------------------------------

class LightStrategyDLSCache : public LightStrategy {
public:
	LightStrategyDLSCache(const DLSCParams &params);
	virtual ~LightStrategyDLSCache();

	virtual void Preprocess(const Scene *scene, const LightStrategyTask taskType,
			const bool useRTMode);
	
	// Used for direct light sampling
	virtual LightSource *SampleLights(const float u,
			const luxrays::Point &p, const luxrays::Normal &n,
			const bool isVolume,
			float *pdf) const;
	virtual float SampleLightPdf(const LightSource *light,
			const luxrays::Point &p, const luxrays::Normal &n,
			const bool isVolume) const;

	// Used for light emission
	virtual LightSource *SampleLights(const float u, float *pdf) const;

	virtual LightStrategyType GetType() const { return GetObjectType(); }
	virtual std::string GetTag() const { return GetObjectTag(); }

	virtual luxrays::Properties ToProperties() const;

	// Used for OpenCL data translation
	const luxrays::Distribution1D *GetLightsDistribution() const { return distributionStrategy.GetLightsDistribution(); }
	const DLSCBvh *GetBVH() const { return DLSCache.GetBVH(); }
	bool UseRTMode() const { return useRTMode; }
	float GetEntryRadius() const { return DLSCache.GetParams().visibility.lookUpRadius; }
	float GetEntryNormalAngle() const { return DLSCache.GetParams().visibility.lookUpNormalAngle; }

	//--------------------------------------------------------------------------
	// Static methods used by LightStrategyRegistry
	//--------------------------------------------------------------------------

	static LightStrategyType GetObjectType() { return TYPE_DLS_CACHE; }
	static std::string GetObjectTag() { return "DLS_CACHE"; }
	static luxrays::Properties ToProperties(const luxrays::Properties &cfg);
	static LightStrategy *FromProperties(const luxrays::Properties &cfg);

protected:
	static const luxrays::Properties &GetDefaultProps();

	LightStrategyTask taskType;
	LightStrategyLogPower distributionStrategy;
	DirectLightSamplingCache DLSCache;
	
	bool useRTMode;
};

}

#endif	/* _SLG_LIGHTSTRATEGY_DLSCACHE_H */
