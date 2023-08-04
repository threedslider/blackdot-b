
#include <textures/basictex.h>
#include <textures/imagetex.h>

__BEGIN_YAFRAY

noiseGenerator_t* newNoise(const std::string &ntype)
{
	if (ntype=="blender")
		return new blenderNoise_t();
	else if (ntype=="stdperlin")
		return new stdPerlin_t();
	else if (int(ntype.find("voronoi"))!=-1) {
		voronoi_t::voronoiType vt = voronoi_t::V_F1;	// default
		if (ntype=="voronoi_f1")
			vt = voronoi_t::V_F1;
		else if (ntype=="voronoi_f2")
			vt = voronoi_t::V_F2;
		else if (ntype=="voronoi_f3")
			vt = voronoi_t::V_F3;
		else if (ntype=="voronoi_f4")
			vt = voronoi_t::V_F4;
		else if (ntype=="voronoi_f2f1")
			vt = voronoi_t::V_F2F1;
		else if (ntype=="voronoi_crackle")
			vt = voronoi_t::V_CRACKLE;
		return new voronoi_t(vt);
	}
	else if (ntype=="cellnoise")
		return new cellNoise_t();
	// default
	return new newPerlin_t();
}

//-----------------------------------------------------------------------------------------
// Clouds Texture
//-----------------------------------------------------------------------------------------

textureClouds_t::textureClouds_t(int dep, PFLOAT sz, bool hd,
		const color_t &c1, const color_t &c2,
		const std::string &ntype, const std::string &btype)
		:depth(dep), size(sz), hard(hd), color1(c1), color2(c2)
{
	bias = 0;	// default, no bias
	if (btype=="positive") bias=1;
	else if (btype=="negative") bias=2;
	nGen = newNoise(ntype);
}

textureClouds_t::~textureClouds_t()
{
	if (nGen) delete nGen;
	nGen = NULL;
}

CFLOAT textureClouds_t::getFloat(const point3d_t &p) const
{
	CFLOAT v = turbulence(nGen, p, depth, size, hard);
	if (bias) {
		v *= v;
		if (bias==1) return -v;	// !!!
	}
	return v;
}

colorA_t textureClouds_t::getColor(const point3d_t &p) const
{
	return color1 + getFloat(p)*(color2 - color1);
}

texture_t *textureClouds_t::factory(paraMap_t &params,
		renderEnvironment_t &render)
{
	color_t color1(0.0), color2(1.0);
	int depth = 2;
	std::string _ntype, _btype;
	const std::string *ntype = &_ntype, *btype=&_btype;
	PFLOAT size = 1;
	bool hard = false;
	params.getParam("noise_type", ntype);
	params.getParam("color1", color1);
	params.getParam("color2", color2);
	params.getParam("depth", depth);
	params.getParam("size", size);
	params.getParam("hard", hard);
	params.getParam("bias", btype);
	return new textureClouds_t(depth, size, hard, color1, color2, *ntype, *btype);
}

//-----------------------------------------------------------------------------------------
// Simple Marble Texture
//-----------------------------------------------------------------------------------------

textureMarble_t::textureMarble_t(int oct, PFLOAT sz, const color_t &c1, const color_t &c2,
			PFLOAT _turb, PFLOAT shp, bool hrd, const std::string &ntype, const std::string &shape)
	:octaves(oct), color1(c1), color2(c2), turb(_turb), size(sz), hard(hrd)
{
	sharpness = 1.0;
	if (shp>1) sharpness = 1.0/shp;
	nGen = newNoise(ntype);
	wshape = SIN;
	if (shape=="saw") wshape = SAW;
	else if (shape=="tri") wshape = TRI;
}

CFLOAT textureMarble_t::getFloat(const point3d_t &p) const
{
	PFLOAT w = (p.x + p.y + p.z)*5.0
					+ ((turb==0.0) ? 0.0 : turb*turbulence(nGen, p, octaves, size, hard));
	switch (wshape) {
		case SAW:
			w *= (PFLOAT)(0.5*M_1_PI);
			w -= floor(w);
			break;
		case TRI:
			w *= (PFLOAT)(0.5*M_1_PI);
			w = std::fabs((PFLOAT)2.0*(w-floor(w))-(PFLOAT)1.0);
			break;
		default:
		case SIN:
			w = (PFLOAT)0.5 + (PFLOAT)0.5*fSin(w);
	}
	return fPow(w, sharpness);
}

colorA_t textureMarble_t::getColor(const point3d_t &p) const
{
	return color1 + getFloat(p)*(color2 - color1);
}

texture_t *textureMarble_t::factory(paraMap_t &params,
		renderEnvironment_t &render)
{
	color_t col1(0.0), col2(1.0);
	int oct = 2;
	PFLOAT turb=1.0, shp=1.0, sz=1.0;
	bool hrd = false;
	std::string _ntype, _shape;
	const std::string *ntype=&_ntype, *shape=&_shape;
	params.getParam("noise_type", ntype);
	params.getParam("color1", col1);
	params.getParam("color2", col2);
	params.getParam("depth", oct);
	params.getParam("turbulence", turb);
	params.getParam("sharpness", shp);
	params.getParam("size", sz);
	params.getParam("hard", hrd);
	params.getParam("shape", shape);
	return new textureMarble_t(oct, sz, col1, col2, turb, shp, hrd, *ntype, *shape);
}


//-----------------------------------------------------------------------------------------
// Simple Wood Texture
//-----------------------------------------------------------------------------------------

textureWood_t::textureWood_t(int oct, PFLOAT sz, const color_t &c1, const color_t &c2, PFLOAT _turb,
		bool hrd, const std::string &ntype, const std::string &wtype, const std::string &shape)
	:octaves(oct), color1(c1), color2(c2), turb(_turb), size(sz), hard(hrd)
{
	rings = (wtype=="rings");
	nGen = newNoise(ntype);
	wshape = SIN;
	if (shape=="saw") wshape = SAW;
	else if (shape=="tri") wshape = TRI;
}

CFLOAT textureWood_t::getFloat(const point3d_t &p) const
{
	PFLOAT w;
	if (rings)
		w = fSqrt(p.x*p.x + p.y*p.y + p.z*p.z)*20.0;
	else
		w = (p.x + p.y + p.z)*10.0;
	w += (turb==0.0) ? 0.0 : turb*turbulence(nGen, p, octaves, size, hard);
	switch (wshape) {
		case SAW:
			w *= (PFLOAT)(0.5*M_1_PI);
			w -= floor(w);
			break;
		case TRI:
			w *= (PFLOAT)(0.5*M_1_PI);
			w = std::fabs((PFLOAT)2.0*(w-floor(w))-(PFLOAT)1.0);
			break;
		default:
		case SIN:
			w = (PFLOAT)0.5 + (PFLOAT)0.5*fSin(w);
	}
	return w;
}

colorA_t textureWood_t::getColor(const point3d_t &p) const
{
	return color1 + getFloat(p)*(color2 - color1);
}

texture_t *textureWood_t::factory(paraMap_t &params,
		renderEnvironment_t &render)
{
	color_t col1(0.0), col2(1.0);
	int oct = 2;
	PFLOAT turb=1.0, sz=1.0, old_rxy;
	bool hrd = false;
	std::string _ntype, _wtype, _shape;
	const std::string *ntype=&_ntype, *wtype=&_wtype, *shape=&_shape;
	params.getParam("noise_type", ntype);
	params.getParam("color1", col1);
	params.getParam("color2", col2);
	params.getParam("depth", oct);
	params.getParam("turbulence", turb);
	params.getParam("size", sz);
	params.getParam("hard", hrd);
	params.getParam("wood_type", wtype);
	params.getParam("shape", shape);
	
	if (params.getParam("ringscale_x", old_rxy) || params.getParam("ringscale_y", old_rxy))
		Y_WARNING << "TextureWood: 'ringscale_x' and 'ringscale_y' are obsolete, use 'size' instead" << yendl;
		
	return new textureWood_t(oct, sz, col1, col2, turb, hrd, *ntype, *wtype, *shape);
}

//-----------------------------------------------------------------------------------------
/* even simpler RGB cube, goes r in x, g in y and b in z inside the unit cube.  */
//-----------------------------------------------------------------------------------------

colorA_t rgbCube_t::getColor(const point3d_t &p) const
{
	colorA_t col = colorA_t(p.x, p.y, p.z);
	col.clampRGB01();
	return col;
}
	
CFLOAT rgbCube_t::getFloat(const point3d_t &p) const
{
	color_t col = color_t(p.x, p.y, p.z);
	col.clampRGB01();
	return col.energy();
}

texture_t* rgbCube_t::factory(paraMap_t &params,renderEnvironment_t &render)
{
	// has no params (yet)...
	return new rgbCube_t();
}

//-----------------------------------------------------------------------------------------
// voronoi block
//-----------------------------------------------------------------------------------------

textureVoronoi_t::textureVoronoi_t(const color_t &c1, const color_t &c2,
		int ct,
		CFLOAT _w1, CFLOAT _w2, CFLOAT _w3, CFLOAT _w4,
		PFLOAT mex, PFLOAT sz,
		CFLOAT isc, const std::string &dname)
		:w1(_w1), w2(_w2), w3(_w3), w4(_w4), size(sz), coltype(ct)
{
	voronoi_t::dMetricType dm = voronoi_t::DIST_REAL;
	if (dname=="squared")
		dm = voronoi_t::DIST_SQUARED;
	else if (dname=="manhattan")
		dm = voronoi_t::DIST_MANHATTAN;
	else if (dname=="chebychev")
		dm = voronoi_t::DIST_CHEBYCHEV;
	else if (dname=="minkovsky_half")
		dm = voronoi_t::DIST_MINKOVSKY_HALF;
	else if (dname=="minkovsky_four")
		dm = voronoi_t::DIST_MINKOVSKY_FOUR;
	else if (dname=="minkovsky")
		dm = voronoi_t::DIST_MINKOVSKY;
	vGen.setDistM(dm);
	vGen.setMinkovskyExponent(mex);
	aw1 = std::fabs(_w1);
	aw2 = std::fabs(_w2);
	aw3 = std::fabs(_w3);
	aw4 = std::fabs(_w4);
	iscale = aw1 + aw2 + aw3 + aw4;
	if (iscale!=0) iscale = isc/iscale;
}

CFLOAT textureVoronoi_t::getFloat(const point3d_t &p) const
{
	PFLOAT da[4];
	point3d_t pa[4];
	vGen.getFeatures(p*size, da, pa);
	return iscale * std::fabs(w1*vGen.getDistance(0, da) + w2*vGen.getDistance(1, da)
			+ w3*vGen.getDistance(2, da) + w4*vGen.getDistance(3, da));
}

colorA_t textureVoronoi_t::getColor(const point3d_t &p) const
{
	PFLOAT da[4];
	point3d_t pa[4];
	vGen.getFeatures(p*size, da, pa);
	CFLOAT inte = iscale * std::fabs(w1*vGen.getDistance(0, da) + w2*vGen.getDistance(1, da)
			+ w3*vGen.getDistance(2, da) + w4*vGen.getDistance(3, da));
	colorA_t col(0.0);
	if (coltype) {
		col += aw1 * cellNoiseColor(vGen.getPoint(0, pa));
		col += aw2 * cellNoiseColor(vGen.getPoint(1, pa));
		col += aw3 * cellNoiseColor(vGen.getPoint(2, pa));
		col += aw4 * cellNoiseColor(vGen.getPoint(3, pa));
		if (coltype>=2) {
			CFLOAT t1 = (vGen.getDistance(1, da) - vGen.getDistance(0, da))*10.0;
			if (t1>1) t1=1;
			if (coltype==3) t1*=inte; else t1*=iscale;
			col *= t1;
		}
		else col *= iscale;
	}
	else col.set(inte, inte, inte, inte);
	return col;
}

texture_t *textureVoronoi_t::factory(paraMap_t &params, renderEnvironment_t &render)
{
	color_t col1(0.0), col2(1.0);
	std::string _cltype, _dname;
	const std::string *cltype=&_cltype, *dname=&_dname;
	CFLOAT fw1=1, fw2=0, fw3=0, fw4=0;
	PFLOAT mex=2.5;	// minkovsky exponent
	CFLOAT isc=1;	// intensity scale
	PFLOAT sz=1;	// size
	int ct=0;	// default "int" color type (intensity)
	
	params.getParam("color1", col1);
	params.getParam("color2", col2);
	
	params.getParam("color_type", cltype);
	if (*cltype=="col1") ct=1;
	else if (*cltype=="col2") ct=2;
	else if (*cltype=="col3") ct=3;
	
	params.getParam("weight1", fw1);
	params.getParam("weight2", fw2);
	params.getParam("weight3", fw3);
	params.getParam("weight4", fw4);
	params.getParam("mk_exponent", mex);
	
	params.getParam("intensity", isc);
	params.getParam("size", sz);
	
	params.getParam("distance_metric", dname);
	
	return new textureVoronoi_t(col1, col2, ct, fw1, fw2, fw3, fw4, mex, sz, isc, *dname);
}

//-----------------------------------------------------------------------------------------
// Musgrave block
//-----------------------------------------------------------------------------------------

textureMusgrave_t::textureMusgrave_t(const color_t &c1, const color_t &c2,
				PFLOAT H, PFLOAT lacu, PFLOAT octs, PFLOAT offs, PFLOAT gain,
				PFLOAT _size, CFLOAT _iscale,
				const std::string &ntype, const std::string &mtype)
				:color1(c1), color2(c2), size(_size), iscale(_iscale)
{
	nGen = newNoise(ntype);
	if (mtype=="multifractal")
		mGen = new mFractal_t(H, lacu, octs, nGen);
	else if (mtype=="heteroterrain")
		mGen = new heteroTerrain_t(H, lacu, octs, offs, nGen);
	else if (mtype=="hybridmf")
		mGen = new hybridMFractal_t(H, lacu, octs, offs, gain, nGen);
	else if (mtype=="ridgedmf")
		mGen = new ridgedMFractal_t(H, lacu, octs, offs, gain, nGen);
	else	// 'fBm' default
		mGen = new fBm_t(H, lacu, octs, nGen);
}

textureMusgrave_t::~textureMusgrave_t()
{
	if (nGen) {
		delete nGen;
		nGen = NULL;
	}
	if (mGen) {
		delete mGen;
		mGen = NULL;
	}
}

CFLOAT textureMusgrave_t::getFloat(const point3d_t &p) const
{
	return iscale * (*mGen)(p*size);
}

colorA_t textureMusgrave_t::getColor(const point3d_t &p) const
{
	return color1 + getFloat(p)*(color2 - color1);
}

texture_t *textureMusgrave_t::factory(paraMap_t &params, renderEnvironment_t &render)
{
	color_t col1(0.0), col2(1.0);
	std::string _ntype, _mtype;
	const std::string *ntype=&_ntype, *mtype=&_mtype;
	PFLOAT H=1, lacu=2, octs=2, offs=1, gain=1, size=1, iscale=1;
	
	params.getParam("color1", col1);
	params.getParam("color2", col2);
	
	params.getParam("musgrave_type", mtype);
	params.getParam("noise_type", ntype);
	
	params.getParam("H", H);
	params.getParam("lacunarity", lacu);
	params.getParam("octaves", octs);
	params.getParam("offset", offs);
	params.getParam("gain", gain);
	params.getParam("size", size);
	params.getParam("intensity", iscale);

	return new textureMusgrave_t(col1, col2, H, lacu, octs, offs, gain, size, iscale, *ntype, *mtype);
}

//-----------------------------------------------------------------------------------------
// Distored Noise block
//-----------------------------------------------------------------------------------------

textureDistortedNoise_t::textureDistortedNoise_t(const color_t &c1, const color_t &c2,
			PFLOAT _distort, PFLOAT _size,
			const std::string &noiseb1, const std::string noiseb2)
			:color1(c1), color2(c2), distort(_distort), size(_size)
{
	nGen1 = newNoise(noiseb1);
	nGen2 = newNoise(noiseb2);
}

textureDistortedNoise_t::~textureDistortedNoise_t()
{
	if (nGen1) {
		delete nGen1;
		nGen1 = NULL;
	}
	if (nGen2) {
		delete nGen2;
		nGen2 = NULL;
	}
}

CFLOAT textureDistortedNoise_t::getFloat(const point3d_t &p) const
{
	// get a random vector and scale the randomization
	const point3d_t ofs(13.5, 13.5, 13.5);
	point3d_t tp(p*size);
	point3d_t rv(getSignedNoise(nGen1, tp+ofs), getSignedNoise(nGen1, tp), getSignedNoise(nGen1, tp-ofs));
	return getSignedNoise(nGen2, tp+rv*distort);	// distorted-domain noise
}

colorA_t textureDistortedNoise_t::getColor(const point3d_t &p) const
{
	return color1 + getFloat(p)*(color2 - color1);
}

texture_t *textureDistortedNoise_t::factory(paraMap_t &params, renderEnvironment_t &render)
{
	color_t col1(0.0), col2(1.0);
	std::string _ntype1, _ntype2;
	const std::string *ntype1=&_ntype1, *ntype2=&_ntype2;
	PFLOAT dist=1, size=1;
	
	params.getParam("color1", col1);
	params.getParam("color2", col2);
	
	params.getParam("noise_type1", ntype1);
	params.getParam("noise_type2", ntype2);
	
	params.getParam("distort", dist);
	params.getParam("size", size);
	
	return new textureDistortedNoise_t(col1, col2, dist, size, *ntype1, *ntype2);
}

//-----------------------------------------------------------------------------------------
// Blend Texture
//-----------------------------------------------------------------------------------------

textureBlend_t::textureBlend_t(const std::string &stype)
{
	// TODO: Blend Types
}

textureBlend_t::~textureBlend_t()
{
}

CFLOAT textureBlend_t::getFloat(const point3d_t &p) const
{
	float blend;
	// Transform -1..1 to 0..1
	blend = 0.5 * ( p.x + 1. );
	// Clipping to 0..1
	blend = std::max(0.f, std::min(blend, 1.f));
	
	return blend;
}

colorA_t textureBlend_t::getColor(const point3d_t &p) const
{
	// TODO: colorband
        return colorA_t(1.0,1.0,1.0,1.0);
}

texture_t *textureBlend_t::factory(paraMap_t &params, renderEnvironment_t &render)
{
	std::string _stype;
	const std::string *stype=&_stype;
	//bool invertXY = false;
	
	params.getParam("stype", _stype);
	//params.getParam("invertXY", invertXY);
	
	return new textureBlend_t(*stype);
}

extern "C"
{
	YAFRAYPLUGIN_EXPORT void registerPlugin(renderEnvironment_t &render)
	{
		render.registerFactory("blend",		textureBlend_t::factory);
		render.registerFactory("clouds", 	textureClouds_t::factory);
		render.registerFactory("marble", 	textureMarble_t::factory);
		render.registerFactory("wood", 		textureWood_t::factory);
		render.registerFactory("voronoi", 	textureVoronoi_t::factory);
		render.registerFactory("musgrave", 	textureMusgrave_t::factory);
		render.registerFactory("distorted_noise", textureDistortedNoise_t::factory);
		render.registerFactory("rgb_cube",	rgbCube_t::factory);
		render.registerFactory("image",		textureImage_t::factory);
	}
}

__END_YAFRAY
