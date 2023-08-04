
#ifndef Y_OBJECT3D_H
#define Y_OBJECT3D_H

#include <yafray_config.h>

//#include "primitive.h"
#include "matrix4.h"

#include <vector>
#include <map>

__BEGIN_YAFRAY

class light_t;
class primitive_t;
class surfacePoint_t;

class YAFRAYCORE_EXPORT object3d_t
{
	public:
        object3d_t(): light(0), visible(true), is_base_mesh(false) {}
		/*! the number of primitives the object holds. Primitive is an element
			that by definition can perform ray-triangle intersection */
		virtual int numPrimitives() const = 0;
		/*! write the primitive pointers to the given array
			\return number of written primitives */
		virtual int getPrimitives(const primitive_t **prims) const { return 0; }
		/*! set a light source to be associated with this object */
		virtual void setLight(const light_t *l){ light=l; }
		/*! query whether object surface can be sampled right now */
		virtual bool canSample() { return false; }
		/*! try to enable sampling (may require additional memory and preprocessing time, if supported) */
		virtual bool enableSampling() { return false; }
		/*! sample object surface */
		virtual void sample(float s1, float s2, point3d_t &p, vector3d_t &n) const {};
		/*! Sets the object visibility to the renderer (is added or not to the kdtree) */
		void setVisibility(bool v) { visible = v; }
		/*! Indicates that this object should be used as base object for instances */
		void useAsBaseObject(bool v) { is_base_mesh = v; }
		/*! Returns if this object should be used for rendering. */
		bool isVisible() const { return visible; }
		/*! Returns if this object is used as base object for instances. */
		bool isBaseObject() const { return is_base_mesh; }
		virtual ~object3d_t(){};


	protected:
		const light_t *light;
		bool visible; //!< toggle whether geometry is visible or only guidance for other stuff
        bool is_base_mesh;
};



/*! simple "container" to handle primitives as objects, for objects that
	consist of just one primitive like spheres etc. */
class primObject_t : public object3d_t
{
	public:
		primObject_t(primitive_t *p): prim(p) { };
		virtual int numPrimitives() const { return 1; }
		virtual int getPrimitives(const primitive_t **prims) const{ *prims = prim; return 1; }
	private:
		primitive_t *prim;
};

__END_YAFRAY

#endif // Y_OBJECT3D_H
