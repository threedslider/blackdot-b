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

#ifndef _PYLUXCORE_UTILS_H
#define	_PYLUXCORE_UTILS_H

#include <memory>

#include <boost/python.hpp>

#include <Python.h>

namespace luxcore {

// From https://stackoverflow.com/questions/35957073/boost-python-returning-tuple-containing-custom-types

template <typename T> boost::python::object TransferToPython(T *t) {
	// Transfer ownership to a smart pointer, allowing for proper cleanup
	// incase Boost.Python throws.
	std::unique_ptr<T> ptr(t);

	// Use the manage_new_object generator to transfer ownership to Python.
	typename boost::python::manage_new_object::apply<T *>::type converter;

	// Transfer ownership to the Python handler and release ownership
	// from C++.
	boost::python::handle<> handle(converter(*ptr));
	ptr.release();

	return boost::python::object(handle);
}

template<typename T> void GetArray(const boost::python::object &obj, std::vector<T> &a,
		const u_int width = 1, const u_int stride = 0) {
	a.clear();
	
	if (!obj.is_none()) {
		//----------------------------------------------------------------------
		// Try if it is a list
		//----------------------------------------------------------------------
		boost::python::extract<boost::python::list> arrayListExtract(obj);
		if (arrayListExtract.check()) {
			const boost::python::list &arrayList = arrayListExtract();

			const boost::python::ssize_t size = len(arrayList);
			if (size % (width + stride) != 0)
				throw std::runtime_error("Wrong data size in GetArray() method: " + size);

			if (stride == 0) {
				a.reserve(size);

				// A fast path for stride = 0
				for (u_int i = 0; i < size; ++i)
					a.push_back(boost::python::extract<T>(arrayList[i]));
			} else {
				a.reserve((size / (width + stride))* width);

				for (u_int i = 0; i < size; i += width + stride) {
					for (u_int j = 0; j < width; ++j)
						a.push_back(boost::python::extract<T>(arrayList[i + j]));
				}
			}
		} else
		//----------------------------------------------------------------------
		// Try if it is a buffer
		//----------------------------------------------------------------------
		if (PyObject_CheckBuffer(obj.ptr())) {
			Py_buffer view;
			if (!PyObject_GetBuffer(obj.ptr(), &view, PyBUF_SIMPLE)) {
				size_t size = view.len / sizeof(T);
				if (size % (width + stride) != 0)
					throw std::runtime_error("Wrong data size in GetArray() method: " + size);

				T *buffer = (T *)view.buf;
				
				if (stride == 0) {
					a.resize(size);

					// A fast path for stride = 0
					copy(buffer, buffer + size, a.begin());
				} else {
					a.reserve((size / (width + stride))* width);

					for (u_int i = 0; i < size; i += width + stride) {
						for (u_int j = 0; j < width; ++j)
							a.push_back(buffer[i + j]);
					}
				}

				PyBuffer_Release(&view);
			} else {
				const std::string objType = boost::python::extract<std::string>((obj.attr("__class__")).attr("__name__"));
				throw std::runtime_error("Unable to get a data view in GetArray() method: " + objType);
			}
		}
		//----------------------------------------------------------------------
		// Unsupported type
		//----------------------------------------------------------------------
		else {
			const std::string objType = boost::python::extract<std::string>((obj.attr("__class__")).attr("__name__"));
			throw std::runtime_error("Wrong data type for the list of values of method GetArray(): " + objType);
		}
	} else
		throw std::runtime_error("None object in GetArray()");
}

static void GetMatrix4x4(const boost::python::object &obj, float mat[16]) {
	if (obj.is_none())
		throw std::runtime_error("None transformation in GetMatrix4x4()");
		
	boost::python::extract<boost::python::list> matListExtract(obj);
	if (matListExtract.check()) {
		const boost::python::list &matList = matListExtract();
		const boost::python::ssize_t size = len(matList);
		if (size != 16) {
			const std::string objType = boost::python::extract<std::string>((obj.attr("__class__")).attr("__name__"));
			throw std::runtime_error("Wrong number of elements for the list of values of method GetMatrix4x4(): " + objType);
		}

		for (u_int i = 0; i < 16; ++i)
			mat[i] = boost::python::extract<float>(matList[i]);

	} else {
		const std::string objType = boost::python::extract<std::string>((obj.attr("__class__")).attr("__name__"));
		throw std::runtime_error("Wrong data type for the list of values of method GetMatrix4x4(): " + objType);
	}
}

} // namespace luxcore

#endif
