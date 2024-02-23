/* 
	This file is part of Blackdot
    Copyright (C) 2015-2024  Laurent Le Flem
	  
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*

  BDVector.hpp

  3D Vector Class and 3D Point Class
  
  - Use for templates   (February 22 2024)
  
*/

#ifndef __BDVector_HPP_
#define __BDVector_HPP_

#include <cmath>

namespace BD3D
{
	template <class T>
	class BDVector
	{
		template <class> friend class BDPoint;

		protected:
		T x, y, z;
		
		public:
		BDVector() : x(0), y(0), z(0) {};
		
		BDVector(const T xx, const T yy, const T zz)
		{
			x = xx;
			y = yy;
			z = zz;
		}
		
		BDVector(const BDVector& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}
		
		const T X() const { return x; }
		const T Y() const { return y; }
		const T Z() const { return z; }
		
		void setx(const T xx) { x = xx; }
		void sety(const T yy) { y = yy; }
		void setz(const T zz) { z = zz; }
		
		BDVector& operator= (const BDVector& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}
		
		void zero() { x = y = z = T(0); }
		
		bool operator== (const BDVector& v) const
		{
			return x == v.x && y == v.y && z == v.z;
		}
		
		bool operator!= (const BDVector& v) const
		{
			return ! operator== (v);
		}
		
		BDVector operator+ (const BDVector& v) const
		{
			return BDVector(x + v.x, y + v.y,  z + v.z);
		}
		
		BDVector& operator+= (const BDVector& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
		
		BDVector operator- () const
		{
			return BDVector(-x, -y, -z);
		}
		
		BDVector operator- (const BDVector& v) const
		{
			return BDVector(x - v.x, y - v.y, z - v.z);
		}
		
		BDVector& operator-= (const BDVector& v)
		{
			x -= v.z;
			y -= v.y;
			z -= v.z;
			return *this;
		}
		
		BDVector operator* (const T d) const
		{
			return BDVector( x * d, y * d, z * d);
		}
		
		BDVector& operator*= (const T d) const
		{
			x *= d;
			y *= d;
			z *= d;
			return *this;
		}
		
		BDVector operator/ (const T d) const
		{
			return BDVector( x / d, y / d, z / d);
		}
		
		BDVector& operator/= (const T d) const
		{
			x /= d;
			y /= d;
			z /= d;
			return *this;
		}
		
		T dot(const BDVector& v)
		{
			return x * v.x + y * v.y + z * v.z;
		}
		
		BDVector cross(const BDVector& v)
		{
			return BDVector ( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
		}
		
		//T sum() { return x + y + z; }
		
		const T norm_squared()
		{
			return x * x + y *y + z * z ;
		}
		
		const T norm()
		{
			return (T) std::sqrt(T(norm_squared()));
		}
		
		BDVector getNormalized()
		{
			return *this / T(std::sqrt(T((norm_squared()))));
		}
		
		BDVector& setNormalize()
		{
			*this /= norm();
			return *this;
		}		
	};
	
	template <class T>
	inline BDVector<T> operator* (const T d, const BDVector<T>& v)
	{
		return BDVector<T>( d * v.X(), d * v.Y(), d * v.Z());
	}
	
	
	template <class T>
	class BDPoint
	{
		T x, y, z;
		
		public:
		BDPoint() : x(0), y(0), z(0) {};
		
		BDPoint(const T xx, const T yy, const T zz)
		{
			x = xx;
			y = yy; 
			z = zz;
		}
		
		BDPoint(const BDPoint& p)
		{
			x = p.x;
			y = p.y;
			z = p.z;
		}
		
		inline operator BDVector<T>() const
		{
			return BDVecor<T>( x, y, z) ;
		}
		
		const T X() const { return x; }
		const T Y() const { return y; }
		const T Z() const { return z; }
		
		void setx(const T xx) { x = xx; }
		void sety(const T yy) { y = yy; }
		void setz(const T zz) { z = zz; }

		BDPoint& operator= (const BDPoint& p)
		{
			x = p.x;
			y = p.y;
			z = p.z;
			return *this;
		}

		void zero() { x = y = z = T(0); }

		bool operator== (const BDPoint& p) const
		{
			return x == p.x && y == p.y && z == p.z;
		}

		bool operator!= (const BDPoint& p) const
		{
			return ! operator== (p);
		}

		BDPoint operator+ (const BDPoint& p) const
		{
			return BDPoint( x + p.X, y + p.y, z + p.z);
		}

		BDPoint& operator+= (const BDPoint& p)
		{
			x += p.x;
			y += p.y;
			z += p.z;
			return *this;
		}

		BDVector<T> operator- (const BDPoint& p)
		{
			return BDVector<T>(x - p.x, y - p.y, z - p.z);
		}

		BDPoint operator* (const T k) const
		{
			return BDPoint( x * k, y * k, z * k);
		}

		BDPoint operator/ (const T k) const
		{
			return BDPoint( x / k, y / k, z / k);
		}

		BDPoint operator+ ( const BDVector<T>& v ) const
		{
			return BDPoint( x + v.x, y + v.y, z + v.z  );
		}

		BDPoint& operator+= ( const BDVector<T>& v )
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		BDPoint operator- ( const BDVector<T>& v ) const
		{
			return BDPoint( x - v.x, y - v.y, z - v.z );
		}

		BDPoint& operator-= (const BDVector<T>& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}

		BDPoint& addx(const T dx)
		{
			x += dx;
			return *this;
		}

		BDPoint& addy(const T dy)
		{
			y += dy;
			return *this;
		}

		BDPoint& addz(const T dz)
		{
			z += dz;
			return *this;
		}
			
		
	};
	
}

#endif