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

  BDMatrix3x3.hpp

  Matrix 3x3 Class
  
*/

#ifndef __BDMatrix3x3_HPP_
#define __BDMatrix3x3_HPP_

#include <vector>

namespace BD3D
{
	
	class BDMatrix3x3
	{
		std::vector<std::vector<float>> a = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
		
		public:
		
		BDMatrix3x3() = default;
		
		BDMatrix3x3(float a00, float a01, float a02,
					float a10, float a11, float a12,
					float a20, float a21, float a22)
		{
			a.assign({{a00, a01, a02}, {a10, a11, a12}, {a20, a21, a22}}); 
		}
		
		float getij(int i, int j)
		{
			return a[i][j];
		}
		
		void setij(int i, int j, float num)
		{
			a[i][j] = num;
		}
		
		bool operator== (const BDMatrix3x3& m) const
		{
			return a[0][0] == m.a[0][0] && a[0][1] == m.a[0][1] && a[0][2] == m.a[0][2] &&
				   a[1][0] == m.a[1][0] && a[1][1] == m.a[1][1] && a[1][2] == m.a[1][2] &&
				   a[2][0] == m.a[2][0] && a[2][1] == m.a[2][1] && a[2][2] == m.a[2][2];
		}
		
		bool operator!= (const BDMatrix3x3& m) const
		{
			return ! operator== (m);
		}
		
		BDMatrix3x3& transpose()
		{
			for(int i=0; i<a.size(); i++)
			{
				for(int j=0; j<i; j++)
				{
					std::swap(a[i][j], a[j][i]);
				}
					
			}
			return *this;
		}
		
		BDMatrix3x3& operator- ()
		{
			a[0][0] = -a[0][0]; a[0][1] = -a[0][1]; a[0][2] = -a[0][2];
			a[1][0] = -a[1][0]; a[1][1] = -a[1][1]; a[1][2] = -a[1][2];
			a[2][0] = -a[2][0]; a[2][1] = -a[2][1]; a[2][2] = -a[2][2];
			
			return *this;
		}
		
		
	};
	
}


#endif