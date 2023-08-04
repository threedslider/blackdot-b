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

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "slg/core/sdl.h"
#include "slg/imagemap/imagemapcache.h"
#include "slg/textures/imagemaptex.h"

using namespace std;
using namespace luxrays;
using namespace slg;

//------------------------------------------------------------------------------
// ImageMapCache
//------------------------------------------------------------------------------

ImageMapCache::ImageMapCache() {
	resizePolicy = new ImageMapResizeNonePolicy();
}

ImageMapCache::~ImageMapCache() {
	BOOST_FOREACH(ImageMap *m, maps) {
		// I avoid to free the static global ImageMapTexture::randomImageMap
		if (m != ImageMapTexture::randomImageMap.get())
			delete m;
	}

	delete resizePolicy;
}

void ImageMapCache::SetImageResizePolicy(ImageMapResizePolicy *policy) {
	delete resizePolicy;
	resizePolicy = policy;
}

string ImageMapCache::GetCacheKey(const string &fileName, const ImageMapConfig &imgCfg) const {
	string key = fileName + "_#_";

	switch (imgCfg.colorSpaceCfg.colorSpaceType) {
		case ColorSpaceConfig::NOP_COLORSPACE:
			key += "CS_NOP_#_";
			break;
		case ColorSpaceConfig::LUXCORE_COLORSPACE:
			key += "CS_LUXCORE_#_" + ToString(imgCfg.colorSpaceCfg.luxcore.gamma) + "_#_";
			break;
		case ColorSpaceConfig::OPENCOLORIO_COLORSPACE:
			key += "CS_OPENCOLORIO_#_" +
					ToString(imgCfg.colorSpaceCfg.ocio.configName) + "_#_" +
					ToString(imgCfg.colorSpaceCfg.ocio.colorSpaceName) + "_#_";
			break;
		default:
			throw runtime_error("Unknown color space type in ImageMapCache::GetCacheKey(): " + 
					ToString(imgCfg.colorSpaceCfg.colorSpaceType));
	}
	
	key += ToString(imgCfg.storageType) + "_#_" +
			ToString(imgCfg.wrapType) + "_#_" +
			ToString(imgCfg.selectionType);

	return key;
}

string ImageMapCache::GetCacheKey(const string &fileName) const {
	return fileName;
}

ImageMap *ImageMapCache::GetImageMap(const string &fileName, const ImageMapConfig &imgCfg,
		const bool applyResizePolicy) {
	// Compose the cache key
	string key = GetCacheKey(fileName);

	// Check if the image map has been already defined
	boost::unordered_map<std::string, ImageMap *>::const_iterator it = mapByKey.find(key);

	if (it != mapByKey.end()) {
		//SDL_LOG("Cached defined image map: " << fileName);
		ImageMap *im = (it->second);
		return im;
	}

	// Check if it is a reference to a file
	key = GetCacheKey(fileName, imgCfg);
	it = mapByKey.find(key);

	if (it != mapByKey.end()) {
		//SDL_LOG("Cached file image map: " << fileName);
		ImageMap *im = (it->second);
		return im;
	}

	// I haven't yet loaded the file

	ImageMap *im;
	if (applyResizePolicy) {
		// Scale the image if required
		bool toApply;
		im = resizePolicy->ApplyResizePolicy(fileName, imgCfg, toApply);
		
		resizePolicyToApply.push_back(toApply);
	} else {
		im = new ImageMap(fileName, imgCfg);

		resizePolicyToApply.push_back(false);
	}
	
	mapByKey.insert(make_pair(key, im));
	mapNames.push_back(fileName);
	maps.push_back(im);

	return im;
}

void ImageMapCache::DefineImageMap(ImageMap *im) {
	const string &name = im->GetName();

	SDL_LOG("Define ImageMap: " << name);

	// Compose the cache key
	const string key = GetCacheKey(name);

	boost::unordered_map<std::string, ImageMap *>::const_iterator it = mapByKey.find(key);
	if (it == mapByKey.end()) {
		// Add the new image definition
		mapByKey.insert(make_pair(key, im));
		mapNames.push_back(name);
		maps.push_back(im);
		
		resizePolicyToApply.push_back(false);
	} else {
		// Overwrite the existing image definition
		const u_int index = GetImageMapIndex(it->second);
		delete maps[index];
		maps[index] = im;

		resizePolicyToApply[index] = false;

		// I have to modify mapByName for last or it iterator would be modified
		// otherwise (it->second would point to the new ImageMap and not to the old one)
		mapByKey.erase(key);
		mapByKey.insert(make_pair(key, im));
	}
}

void ImageMapCache::DeleteImageMap(const ImageMap *im) {
	for (boost::unordered_map<std::string, ImageMap *>::iterator it = mapByKey.begin(); it != mapByKey.end(); ++it) {
		if (it->second == im) {
			delete it->second;
			mapByKey.erase(it);

			for (u_int i = 0; i < maps.size(); ++i) {
				if (maps[i] == im) {
					mapNames.erase(mapNames.begin() + i);
					maps.erase(maps.begin() + i);
					resizePolicyToApply.erase(resizePolicyToApply.begin() + i);
					break;
				}
			}

			return;
		}
	}
}

string ImageMapCache::GetSequenceFileName(const ImageMap *im) const {
	return ("imagemap-" + ((boost::format("%05d") % GetImageMapIndex(im)).str()) +
			"." + im->GetFileExtension());
}

u_int ImageMapCache::GetImageMapIndex(const ImageMap *im) const {
	for (u_int i = 0; i < maps.size(); ++i) {
		if (maps[i] == im)
			return i;
	}

	throw runtime_error("Unknown image map: " + ToString(im));
}

void ImageMapCache::GetImageMaps(vector<const ImageMap *> &ims) {
	ims.reserve(maps.size());

	BOOST_FOREACH(ImageMap *im, maps)
		ims.push_back(im);
}

void ImageMapCache::Preprocess(const Scene *scene, const bool useRTMode) {
	resizePolicy->Preprocess(*this, scene, useRTMode);
}
