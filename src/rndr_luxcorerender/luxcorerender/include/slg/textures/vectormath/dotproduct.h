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

#ifndef _SLG_DOTPRODUCTTEX_H
#define	_SLG_DOTPRODUCTTEX_H

#include "slg/textures/texture.h"

namespace slg {

//------------------------------------------------------------------------------
// Dot product texture
//------------------------------------------------------------------------------

class DotProductTexture : public Texture {
public:
	DotProductTexture(const Texture *t1, const Texture *t2) : tex1(t1), tex2(t2) { }
	virtual ~DotProductTexture() { }

	virtual TextureType GetType() const { return DOT_PRODUCT_TEX; }
	virtual float GetFloatValue(const HitPoint &hitPoint) const;
	virtual luxrays::Spectrum GetSpectrumValue(const HitPoint &hitPoint) const;
	virtual float Y() const { return 0.f; } // TODO
	virtual float Filter() const { return 0.f; } // TODO

	virtual void AddReferencedTextures(boost::unordered_set<const Texture *> &referencedTexs) const {
		Texture::AddReferencedTextures(referencedTexs);

		tex1->AddReferencedTextures(referencedTexs);
		tex2->AddReferencedTextures(referencedTexs);
	}
	virtual void AddReferencedImageMaps(boost::unordered_set<const ImageMap *> &referencedImgMaps) const {
		tex1->AddReferencedImageMaps(referencedImgMaps);
		tex2->AddReferencedImageMaps(referencedImgMaps);
	}

	virtual void UpdateTextureReferences(const Texture *oldTex, const Texture *newTex) {
		if (tex1 == oldTex)
			tex1 = newTex;
		if (tex2 == oldTex)
			tex2 = newTex;
	}

	const Texture *GetTexture1() const { return tex1; }
	const Texture *GetTexture2() const { return tex2; }

	virtual luxrays::Properties ToProperties(const ImageMapCache &imgMapCache, const bool useRealFileName) const;

private:
	const Texture *tex1;
	const Texture *tex2;

	inline float Dot(const luxrays::Spectrum &v1, const luxrays::Spectrum &v2) const {
		return v1.c[0] * v2.c[0] + v1.c[1] * v2.c[1] + v1.c[2] * v2.c[2];
	}
};

}

#endif	/* _SLG_DOTPRODUCTTEX_H */
