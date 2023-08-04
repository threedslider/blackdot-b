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

#ifndef _SLG_LIGHTSTRATEGY_DLSCBVH_H
#define	_SLG_LIGHTSTRATEGY_DLSCBVH_H

#include <vector>

#include "slg/core/indexbvh.h"

namespace slg {

class DLSCacheEntry;

class DLSCBvh : public IndexBvh<DLSCacheEntry> {
public:
	DLSCBvh(const std::vector<DLSCacheEntry> *entries,
			const float radius, const float normalAngle);
	virtual ~DLSCBvh();

	const DLSCacheEntry *GetNearestEntry(const luxrays::Point &p,
			const luxrays::Normal &n, const bool isVolume) const;
	void GetAllNearEntries(std::vector<u_int> &allNearEntryIndices,
			const luxrays::Point &p, const luxrays::Normal &n,
			const bool isVolume) const;

	// Used for OpenCL data translation
	const std::vector<DLSCacheEntry> *GetAllEntries() const { return allEntries; }

	friend class boost::serialization::access;

private:
	// Used by serialization
	DLSCBvh() { }

	template<class Archive> void serialize(Archive &ar, const u_int version) {
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(IndexBvh);
		ar & normalCosAngle;
	}

	float normalCosAngle;
};

}

#endif	/* _SLG_LIGHTSTRATEGY_DLSCBVH_H */
