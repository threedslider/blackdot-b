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

#ifndef _SLG_SKY2LIGHT_H
#define	_SLG_SKY2LIGHT_H

#include "slg/lights/light.h"
#include "slg/lights/visibility/envlightvisibilitycache.h"

namespace slg {

//------------------------------------------------------------------------------
// Sky2 implementation
//------------------------------------------------------------------------------

class SkyLight2 : public EnvLightSource {
public:
	SkyLight2();
	virtual ~SkyLight2();

	virtual void Preprocess();
	void GetPreprocessedData(float *absoluteDirData, float *absoluteUpDirData,
		float *scaledGroundColor, int *isGroundBlackData,
		float *aTermData, float *bTermData, float *cTermData, float *dTermData,
		float *eTermData, float *fTermData, float *gTermData, float *hTermData,
		float *iTermData, float *radianceTermData,
		const luxrays::Distribution2D **skyDistributionData,
		const EnvLightVisibilityCache **visibilityMapCache) const;

	virtual void UpdateVisibilityMap(const Scene *scene, const bool useRTMode);

	virtual LightSourceType GetType() const { return TYPE_IL_SKY2; }
	virtual float GetPower(const Scene &scene) const;

	virtual luxrays::Spectrum Emit(const Scene &scene,
		const float time, const float u0, const float u1,
		const float u2, const float u3, const float passThroughEvent,
		luxrays::Ray &ray, float &emissionPdfW,
		float *directPdfA = NULL, float *cosThetaAtLight = NULL) const;

    virtual luxrays::Spectrum Illuminate(const Scene &scene, const BSDF &bsdf,
		const float time, const float u0, const float u1, const float passThroughEvent,
        luxrays::Ray &shadowRay, float &directPdfW,
		float *emissionPdfW = NULL, float *cosThetaAtLight = NULL) const;

	virtual luxrays::Spectrum GetRadiance(const Scene &scene, const BSDF *bsdf,
			const luxrays::Vector &dir,
			float *directPdfA = NULL, float *emissionPdfW = NULL) const;
	virtual luxrays::UV GetEnvUV(const luxrays::Vector &dir) const;

	virtual luxrays::Properties ToProperties(const ImageMapCache &imgMapCache, const bool useRealFileName) const;

	luxrays::Vector localSunDir;
	float turbidity;
	luxrays::Spectrum groundAlbedo;
	luxrays::Spectrum groundColor;
	bool hasGround, hasGroundAutoScale;

	u_int distributionWidth, distributionHeight;

	// Visibility map cache options
	ELVCParams visibilityMapCacheParams;
	bool useVisibilityMapCache;

private:
	luxrays::Vector SampleSkyDome(const float u0, const float u1) const;
	void SampleSkyDomePdf(const Scene &scene, float *directPdf, float *emissionPdf) const;
	luxrays::Spectrum ComputeSkyRadiance(const luxrays::Vector &w) const;
	luxrays::Spectrum ComputeRadiance(const luxrays::Vector &w) const;

	luxrays::Vector absoluteSunDir, absoluteUpDir;
	luxrays::Spectrum scaledGroundColor;

	luxrays::Spectrum model[10];
	luxrays::Spectrum aTerm, bTerm, cTerm, dTerm, eTerm, fTerm,
		gTerm, hTerm, iTerm, radianceTerm;

	bool isGroundBlack;

	luxrays::Distribution2D *skyDistribution;

	EnvLightVisibilityCache *visibilityMapCache;
};

}

#endif	/* _SLG_SKY2LIGHT_H */
