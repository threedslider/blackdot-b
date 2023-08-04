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

/*!
 * \file
 *
 * \brief LuxCore is the LuxCoreRender C++/Python API.
 * \author Bucciarelli David et al.
 * \version 2.0
 * \date October 2013
 *
 */

/*!
 * \mainpage
 * \section intro Introduction
 * LuxCore is the LuxCoreRender C++/Python API. It can be used to create and
 * render scenes. It includes the support for advanced new features like editing
 * materials, lights, geometry, interactive rendering and more.
 */

#ifndef _LUXCORE_H
#define	_LUXCORE_H

#include <cstddef>
#include <stdexcept>
#include <string>
#include <array>

#include <luxcore/cfg.h>
#include <luxrays/utils/exportdefs.h>
#include <luxrays/utils/properties.h>
#include <luxrays/utils/cyhair/cyHairFile.h>

/*! \mainpage LuxCore
 *
 * \section intro_sec Introduction
 *
 * LuxCore is the LuxCoreRender C++ API.
 */

/*!
 * \namespace luxcore
 *
 * \brief The LuxCore classes are defined within this namespace.
 */
namespace luxcore {

#define LC_MESH_MAX_DATA_COUNT 8

// This is mostly for compatibility with the past
#define LC_LOG(a) { std::stringstream _LUXCORE_LOG_LOCAL_SS; _LUXCORE_LOG_LOCAL_SS << a; luxcore::PrintLogMsg(_LUXCORE_LOG_LOCAL_SS.str()); }

/*!
 * \brief Initializes LuxCore API. This function is thread safe.
 *
 * \param LogHandler is a pointer to a function receiving all LuxCore log
 * messages. If it is NULL, the default handler will be used.
 */
CPP_EXPORT CPP_API void Init(void (*LogHandler)(const char *) = NULL);

/*!
 * \brief This function can be called if you want to update the log handler.
 *
 * \param LogHandler is a pointer to a function receiving all LuxCore log
 * messages. If it is NULL, there will no printed information.
 */
CPP_EXPORT CPP_API void SetLogHandler(void (*LogHandler)(const char *) = NULL);

/*!
 * \brief This function can be called if you want to enable file logging. By
 * default, file logging is disabled.
 *
 * \param fileName is the file name of the log file. If it is set to "", it will
 * disable file logging.
 * \param size is the maximum size in bytes of the log files. If it is set to
 * 0, it will disable file logging.
 * \param count is the maximum number of rotating log files to keep. If it is set to
 * 0, it will disable file logging.
 */
CPP_EXPORT CPP_API void SetFileLog(const std::string &fileName, const unsigned int size, const unsigned int count);

/*!
 * \brief This function can be called to print a log message
 *
 * \param msg is the message to print in all enabled log outputs.
 */
CPP_EXPORT CPP_API void PrintLogMsg(const std::string &msg);

typedef enum {
	LOG_LUXRAYS,
	LOG_SDL,
	LOG_SLG,
	LOG_API
} LogSubSystemType;

/*!
 * \brief This function can be called to enable/disable log messages of a single
 * subsystem.
 *
 * \param type is the type of subsystem to enable/disable.
 * \param v sets if to enable/disable the messages.
 */
CPP_EXPORT CPP_API void SetEnableLogSubSystem(const LogSubSystemType type, const bool v);

/*!
 * \brief Parses a scene described using LuxRender SDL (Scene Description Language).
 *
 * \param fileName is the name of the file to parse.
 * \param renderConfig is where the rendering configuration properties are returned.
 * \param scene is where the scene properties are returned.
 */
CPP_EXPORT CPP_API void ParseLXS(const std::string &fileName, luxrays::Properties &renderConfig, luxrays::Properties &scene);

/*!
 * \brief File the OpenCL kernel cache with entries
 *
 * \param config defines how to fill the cache. The supported properties are:
 * kernelcachefill.renderengine.types, kernelcachefill.sampler.types,
 * kernelcachefill.camera.types, kernelcachefill.geometry.types, kernelcachefill.light.types,
 * kernelcachefill.material.types, kernelcachefill.texture.types.
 * However, since v2.4, LuxCoreRender uses only one kernel for each render engine
 * so kernelcachefill.renderengine.types is now the only one worth using.
 * They can be used to define the list of types to use, for instance with a
 * Property("kernelcachefill.renderengine.types")("PATHOCL", "TILEPATHOCL", "RTPATHOCL").
 */
CPP_EXPORT CPP_API void KernelCacheFill(const luxrays::Properties &config, void (*ProgressHandler)(const size_t, const size_t) = NULL);

/*!
 * \brief Return a list of properties describing the features available. The
 * properties currently supported are:
 * - version.number\n
 *		The LuxCore version. As string with "major.minor" version format.
 * - compile.LUXRAYS_ENABLE_OPENCL and compile.LUXRAYS_DISABLE_OPENCL\n
 *		true or false if the sources has been compiled with LUXRAYS_ENABLE_OPENCL and
 *		OpenCL rendering engines are available or not.
 * - compile.LUXRAYS_ENABLE_CUDA and compile.LUXRAYS_DISABLE_CUDA\n
 *		true or false if the sources has been compiled with LUXRAYS_ENABLE_CUDA and
 *		CUDA support is available or not.
 * - compile.LUXRAYS_ENABLE_OPTIX is true if Optix is available, false otherwise.
 * - compile.LUXCORE_DISABLE_EMBREE_BVH_BUILDER\n
 *		true or false if the sources has been compiled with LUXCORE_DISABLE_EMBREE_BVH_BUILDER and
 *		Embree BVH builder is used for OpenCL or not. This is now always false.
 */
CPP_EXPORT CPP_API luxrays::Properties GetPlatformDesc();

/*!
 * \brief Return the list of OpenCL devices available. For instance:
 * - opencl.device.0.name = "GeForce GTX 980"
 * - opencl.device.0.type = "OPENCL_GPU"
 * - opencl.device.0.units = 16
 * - opencl.device.0.nativevectorwidthfloat = 1
 * - opencl.device.0.maxmemory = 4294770688
 * - opencl.device.0.maxmemoryallocsize = 1073692672
 * - opencl.device.1.name = "Hawaii"
 * - opencl.device.1.type = "OPENCL_GPU"
 * - opencl.device.1.units = 44
 * - opencl.device.1.nativevectorwidthfloat = 1
 * - opencl.device.1.maxmemory = 3888119808
 * - opencl.device.1.maxmemoryallocsize = 2847670272
 * - opencl.device.2.name = "Intel(R) Core(TM) i7-3930K CPU @ 3.20GHz"
 * - opencl.device.2.type = "OPENCL_CPU"
 * - opencl.device.2.units = 12
 * - opencl.device.2.nativevectorwidthfloat = 8
 * - opencl.device.2.maxmemory = 16765145088
 * - opencl.device.2.maxmemoryallocsize = 4191286272
 */
CPP_EXPORT CPP_API luxrays::Properties GetOpenCLDeviceDescs();

/*!
 * \brief Convert an image file to TX format
 *
 * \param srcFileName defines the source image file name
 * \param srcFileName defines the destination image file name
 */
CPP_EXPORT CPP_API void MakeTx(const std::string &srcFileName, const std::string &dstFileName);

/*!
 * \brief Clear the list of places where to look for files.
 */
CPP_EXPORT CPP_API void ClearFileNameResolverPaths();

/*!
 * \brief Add a path to the list of places where to look for files.
 */
CPP_EXPORT CPP_API void AddFileNameResolverPath(const std::string &path);

/*!
 * \brief Return the list of places where to look for files.
 */
CPP_EXPORT CPP_API std::vector<std::string> GetFileNameResolverPaths();

class RenderSession;
class RenderState;

/*!
 * \brief Film stores all the outputs of a rendering. It can be obtained
 * from a RenderSession or as stand alone object loaded from a file.
 */
CPP_EXPORT class CPP_API Film {
public:
	/*!
	* \brief Types of Film channel available.
	*/
	typedef enum {
		// This list must be aligned with slg::FilmOutputs::FilmOutputType
		OUTPUT_RGB,
		OUTPUT_RGBA,
		OUTPUT_RGB_IMAGEPIPELINE,
		OUTPUT_RGBA_IMAGEPIPELINE,
		OUTPUT_ALPHA,
		OUTPUT_DEPTH,
		OUTPUT_POSITION,
		OUTPUT_GEOMETRY_NORMAL,
		OUTPUT_SHADING_NORMAL,
		OUTPUT_MATERIAL_ID,
		OUTPUT_DIRECT_DIFFUSE,
		OUTPUT_DIRECT_DIFFUSE_REFLECT,
		OUTPUT_DIRECT_DIFFUSE_TRANSMIT,
		OUTPUT_DIRECT_GLOSSY,
		OUTPUT_DIRECT_GLOSSY_REFLECT,
		OUTPUT_DIRECT_GLOSSY_TRANSMIT,
		OUTPUT_EMISSION,
		OUTPUT_INDIRECT_DIFFUSE,
		OUTPUT_INDIRECT_DIFFUSE_REFLECT,
		OUTPUT_INDIRECT_DIFFUSE_TRANSMIT,
		OUTPUT_INDIRECT_GLOSSY,
		OUTPUT_INDIRECT_GLOSSY_REFLECT,
		OUTPUT_INDIRECT_GLOSSY_TRANSMIT,
		OUTPUT_INDIRECT_SPECULAR,
		OUTPUT_INDIRECT_SPECULAR_REFLECT,
		OUTPUT_INDIRECT_SPECULAR_TRANSMIT,
		OUTPUT_MATERIAL_ID_MASK,
		OUTPUT_DIRECT_SHADOW_MASK,
		OUTPUT_INDIRECT_SHADOW_MASK,
		OUTPUT_RADIANCE_GROUP,
		OUTPUT_UV,
		OUTPUT_RAYCOUNT,
		OUTPUT_BY_MATERIAL_ID,
		OUTPUT_IRRADIANCE,
		OUTPUT_OBJECT_ID,
		OUTPUT_OBJECT_ID_MASK,
		OUTPUT_BY_OBJECT_ID,
		OUTPUT_SAMPLECOUNT,
		OUTPUT_CONVERGENCE,
		OUTPUT_SERIALIZED_FILM,
		OUTPUT_MATERIAL_ID_COLOR,
		OUTPUT_ALBEDO,
		OUTPUT_AVG_SHADING_NORMAL,
		OUTPUT_NOISE,
		OUTPUT_USER_IMPORTANCE,
		OUTPUT_CAUSTIC
	} FilmOutputType;

	/*!
	 * \brief Types of Film channel available.
	 */
	typedef enum {
		// This list must be aligned with slg::Film::FilmChannelType
		CHANNEL_RADIANCE_PER_PIXEL_NORMALIZED,
		CHANNEL_RADIANCE_PER_SCREEN_NORMALIZED,
		CHANNEL_ALPHA,
		CHANNEL_IMAGEPIPELINE,
		CHANNEL_DEPTH,
		CHANNEL_POSITION,
		CHANNEL_GEOMETRY_NORMAL,
		CHANNEL_SHADING_NORMAL,
		CHANNEL_MATERIAL_ID,
		CHANNEL_DIRECT_DIFFUSE,
		CHANNEL_DIRECT_DIFFUSE_REFLECT,
		CHANNEL_DIRECT_DIFFUSE_TRANSMIT,
		CHANNEL_DIRECT_GLOSSY,
		CHANNEL_DIRECT_GLOSSY_REFLECT,
		CHANNEL_DIRECT_GLOSSY_TRANSMIT,
		CHANNEL_EMISSION,
		CHANNEL_INDIRECT_DIFFUSE,
		CHANNEL_INDIRECT_DIFFUSE_REFLECT,
		CHANNEL_INDIRECT_DIFFUSE_TRANSMIT,
		CHANNEL_INDIRECT_GLOSSY,
		CHANNEL_INDIRECT_GLOSSY_REFLECT,
		CHANNEL_INDIRECT_GLOSSY_TRANSMIT,
		CHANNEL_INDIRECT_SPECULAR,
		CHANNEL_INDIRECT_SPECULAR_REFLECT,
		CHANNEL_INDIRECT_SPECULAR_TRANSMIT,
		CHANNEL_MATERIAL_ID_MASK,
		CHANNEL_DIRECT_SHADOW_MASK,
		CHANNEL_INDIRECT_SHADOW_MASK,
		CHANNEL_UV,
		CHANNEL_RAYCOUNT,
		CHANNEL_BY_MATERIAL_ID,
		CHANNEL_IRRADIANCE,
		CHANNEL_OBJECT_ID,
		CHANNEL_OBJECT_ID_MASK,
		CHANNEL_BY_OBJECT_ID,
		CHANNEL_SAMPLECOUNT,
		CHANNEL_CONVERGENCE,
		CHANNEL_MATERIAL_ID_COLOR,
		CHANNEL_ALBEDO,
		CHANNEL_AVG_SHADING_NORMAL,
		CHANNEL_NOISE,
		CHANNEL_USER_IMPORTANCE
	} FilmChannelType;

	virtual ~Film();

	/*!
	 * \brief Loads a stand alone Film (i.e. not connected to a rendering session)
	 * from a file.
	 *
	 * \param fileName is the name of the file with the serialized film to read.
	 */
	static Film *Create(const std::string &fileName);
	/*!
	 * \brief Create a stand alone Film (i.e. not connected to a rendering session)
	 * from the properties.
	 *
	 * \param props defining the film.
	 * \param hasPixelNormalizedChannel if the film must have CHANNEL_RADIANCE_PER_PIXEL_NORMALIZED.
	 * Required by all render engines.
	 * \param hasScreenNormalizedChannel if the film must have CHANNEL_RADIANCE_PER_SCREEN_NORMALIZED.
	 * Required by BIDIRCPU and LIGHTCPU render engines.
	 *
	 */
	static Film *Create(const luxrays::Properties &props,
			const bool hasPixelNormalizedChannel,
			const bool hasScreenNormalizedChannel);

	/*!
	 * \brief Returns the Film width.
	 *
	 * \return the Film width.
	 */
	virtual unsigned int GetWidth() const = 0;
	/*!
	 * \brief Returns the Film height.
	 *
	 * \return the Film width.
	 */
	virtual unsigned int GetHeight() const = 0;
	/*!
	 * \brief Returns a list of statistics related to the film. Mostly useful
	 * for stand alone films.
	 *
	 * \return a Properties container with the statistics.
	 */
	virtual luxrays::Properties GetStats() const = 0;
	/*!
	 * \brief Returns the Film average luminance. It can be used to
	 * estimate a good value for variance clamping.
	 *
	 * \param imagePipelineIndex the index of the image pipeline radiance group scale to use.
	 *
	 * \return the Film average luminance.
	 */
	virtual float GetFilmY(const unsigned int imagePipelineIndex = 0) const = 0;
	/*!
	 * \brief Clear the film.
	 */
	virtual void Clear() = 0;
	/*!
	 * \brief Add a film.
	 *
	 * \param film the film to add.
	 *
	 */
	virtual void AddFilm(const Film &film) = 0;
	/*!
	 * \brief Add a film.
	 *
	 * \param film the film to add.
	 * \param srcOffsetX the X offset of the region of the film to add.
	 * \param srcOffsetY the y offset of the region of the film to add.
	 * \param srcWidth the width of the region of the film to add.
	 * \param srcHeight the height of the region of the film to add.
	 * \param dstOffsetX the X offset of the destination film.
	 * \param dstOffsetY the Y offset of the destination film.
	 *
	 */
	virtual void AddFilm(const Film &film,
		const unsigned int srcOffsetX, const unsigned int srcOffsetY,
		const unsigned int srcWidth, const unsigned int srcHeight,
		const unsigned int dstOffsetX, const unsigned int dstOffsetY) = 0;
	/*!
	 * \brief Saves all Film output channels defined in the current
	 * RenderSession. This method can not be used with a standalone film.
	 */
	virtual void SaveOutputs() const = 0;

	/*!
	 * \brief Saves the specified Film output channel.
	 *
	 * \param fileName is the name of the file where to save the output channel.
	 * \param type is the Film output channel to use. It must be one
	 * of the enabled channels.
	 * \param props can include some additional information defined by the
	 * following property:
	 * "id" for the ID of MATERIAL_ID_MASK,
	 * "id" for the index of RADIANCE_GROUP,
	 * "id" for the ID of BY_MATERIAL_ID.
	 * "id" for the ID of OBJECT_ID_MASK,
	 * "id" for the ID of BY_OBJECT_ID.
	 */
	virtual void SaveOutput(const std::string &fileName, const FilmOutputType type, const luxrays::Properties &props) const = 0;

	/*!
	 * \brief Serializes a Film in a file.
	 *
	 * \param fileName is the name of the file where to serialize the film.
	 */
	virtual void SaveFilm(const std::string &fileName) const = 0;

	/*!
	 * \brief Returns the total sample count.
	 *
	 * \return the total sample count.
	 */
	virtual double GetTotalSampleCount() const = 0;
	/*!
	 * \brief Returns the size (in float or unsigned int) of a Film output channel.
	 *
	 * \param type is the Film output channel to use.
	 *
	 * \return the size (in float or unsigned int) of a Film output channel.
	 */
	virtual size_t GetOutputSize(const FilmOutputType type) const = 0;
	/*!
	 * \brief Returns if a film channel output is available.
	 *
	 * \param type is the Film output channel to use.
	 *
	 * \return true if the output is available, false otherwise.
	 */
	virtual bool HasOutput(const FilmOutputType type) const = 0;
	/*!
	 * \brief Returns the number of output channels of the passed type.
	 *
	 * \param type is the Film output channel to use.
	 *
	 * \return the number of output channels. Returns 0 if the output channel is not available.
	 */
	virtual unsigned int GetOutputCount(const FilmOutputType type) const = 0;
	/*!
	 * \brief Returns the number of radiance groups.
	 *
	 * \return the number of radiance groups.
	 */
	virtual unsigned int GetRadianceGroupCount() const = 0;
	/*!
	 * \brief Fills the buffer with a Film output channel.
	 *
	 * \param type is the Film output channel to use. It must be one
	 * of the enabled channels in RenderConfig. The supported template types are
	 * float and unsigned int.
	 * \param buffer is the place where the data will be copied.
	 * \param index of the buffer to use. Usually 0, however, for instance,
	 * if more than one light group is used, select the group to return.
	 * \param executeImagePipeline is a flag to enable/disable automatic image
	 * pipeline execution before to perform the copy.
	 */
	template<class T> void GetOutput(const FilmOutputType type, T *buffer, const unsigned int index = 0,
			const bool executeImagePipeline = true) {
		throw std::runtime_error("Called Film::GetOutput() with wrong type");
	}
	/*!
	 * \brief Copy the buffer to a Film output channel. Currently, only USER_IMPORTANCE
	 * output channel can be updated.
	 *
	 * \param type is the Film output channel to use. It must be one
	 * of the enabled channels in RenderConfig. The supported template types are
	 * float and unsigned int.
	 * \param buffer is the place where the data will be copied from.
	 * \param index of the buffer to use. Usually 0, however, for instance,
	 * if more than one light group is used, select the group to return.
	 * \param executeImagePipeline is a flag to enable/disable automatic image
	 * pipeline execution before to perform the copy.
	 */
	template<class T> void UpdateOutput(const FilmOutputType type, const T *buffer, const unsigned int index = 0,
			const bool executeImagePipeline = false) {
		throw std::runtime_error("Called Film::UpdateOutput() with wrong type");
	}
	/*!
	 * \brief Returns if a film channel is available.
	 *
	 * \param type is the Film channel to use.
	 *
	 * \return true if the output is available, false otherwise.
	 */
	virtual bool HasChannel(const FilmChannelType type) const = 0;
	/*!
	 * \brief Returns the number of channels of the passed type.
	 *
	 * \param type is the Film channel to use.
	 *
	 * \return the number of channels. Returns 0 if the channel is not available.
	 */
	virtual unsigned int GetChannelCount(const FilmChannelType type) const = 0;
	/*!
	 * \brief Returns a pointer to the type of channel requested. The channel is
	 * not normalized (if it has a weight channel).
	 *
	 * \param type is the Film output channel to return. It must be one
	 * of the enabled channels in RenderConfig. The supported template types are
	 * float and unsigned int.
	 * \param index of the buffer to use. Usually 0, however, for instance,
	 * if more than one light group is used, it selects the group to return.
	 * \param executeImagePipeline is a flag to enable/disable automatic image
	 * pipeline execution before to perform the copy.
	 *
	 * \return a pointer to the requested raw buffer.
	 */
	template<class T> const T *GetChannel(const FilmChannelType type, const unsigned int index = 0,
			const bool executeImagePipeline = true) {
		throw std::runtime_error("Called Film::GetChannel() with wrong type");
	}
	/*!
	 * \brief Returns a pointer to the type of channel requested. The channel is
	 * not normalized (if it has a weight channel). Currently, only USER_IMPORTANCE
	 * channel can be updated.
	 *
	 * \param type is the Film output channel to return. It must be one
	 * of the enabled channels in RenderConfig. The supported template types are
	 * float and unsigned int.
	 * \param index of the buffer to use. Usually 0, however, for instance,
	 * if more than one light group is used, it selects the group to return.
	 * \param executeImagePipeline is a flag to enable/disable automatic image
	 * pipeline execution before to perform the copy.
	 *
	 * \return a pointer to the requested raw buffer.
	 */
	template<class T> T *UpdateChannel(const FilmChannelType type, const unsigned int index = 0,
			const bool executeImagePipeline = true) {
		throw std::runtime_error("Called Film::UpdateChannel() with wrong type");
	}
	/*!
	 * \brief Sets configuration Properties with new values. This method can be
	 * used only when the Film is not in use by a RenderSession. Image pipeline
	 * and radiance scale values can be redefined with this method.
	 *
	 * \param props are the Properties to set.
	 */
	virtual void Parse(const luxrays::Properties &props) = 0;
	/*!
	 * \brief Delete all image pipelines and goes the default image
	 * pipeline (AutoLinearToneMap + GammaCorrectionPlugin). This method can
	 * be used with a stand alone film or with a session film.
	 *
	 */
	virtual void DeleteAllImagePipelines() = 0;

	/*!
	 * \brief Execute the an image pipeline.
	 *
	 * \param index is the index of the image pipeline to run.
	 *
	 */
	virtual void ExecuteImagePipeline(const unsigned int index) = 0;
	/*!
	 * \brief Asynchronously execute an image pipeline. Only one image pipeline
	 * can be executed asynchronously at time.
	 *
	 * \param index is the index of the image pipeline to run.
	 *
	 */
	virtual void AsyncExecuteImagePipeline(const unsigned int index) = 0;
	/*!
	 * \brief wait for the end of the asynchronously execution of an image pipeline.
	 *
	 */
	virtual void WaitAsyncExecuteImagePipeline() = 0;
	/*!
	 * \brief wait for the end of the asynchronously execution of an image pipeline.
	 *
	 * \return if the execution of an asynchronously image pipeline has terminated.
	 */
	virtual bool HasDoneAsyncExecuteImagePipeline() = 0;

protected:
	virtual void GetOutputFloat(const FilmOutputType type, float *buffer, const unsigned int index,
			const bool executeImagePipeline = true) = 0;
	virtual void GetOutputUInt(const FilmOutputType type, unsigned int *buffer, const unsigned int index,
			const bool executeImagePipeline = true) = 0;

	virtual void UpdateOutputFloat(const FilmOutputType type, const float *buffer, const unsigned int index,
			const bool executeImagePipeline = false) = 0;
	virtual void UpdateOutputUInt(const FilmOutputType type, const unsigned int *buffer, const unsigned int index,
			const bool executeImagePipeline = false) = 0;
	
	virtual const float *GetChannelFloat(const FilmChannelType type, const unsigned int index,
			const bool executeImagePipeline = true) = 0;
	virtual const unsigned int *GetChannelUInt(const FilmChannelType type, const unsigned int index,
			const bool executeImagePipeline = true) = 0;

	virtual float *UpdateChannelFloat(const FilmChannelType type, const unsigned int index,
			const bool executeImagePipeline = true) = 0;
	virtual unsigned int *UpdateChannelUInt(const FilmChannelType type, const unsigned int index,
			const bool executeImagePipeline = true) = 0;
};

template<> CPP_API void Film::GetOutput<float>(const FilmOutputType type, float *buffer,
		const unsigned int index, const bool executeImagePipeline);
template<> CPP_API void Film::GetOutput<unsigned int>(const FilmOutputType type, unsigned int *buffer,
		const unsigned int index, const bool executeImagePipeline);
template<> CPP_API void Film::UpdateOutput<float>(const FilmOutputType type, const float *buffer,
		const unsigned int index, const bool executeImagePipeline);
template<> CPP_API void Film::UpdateOutput<unsigned int>(const FilmOutputType type, const unsigned int *buffer,
		const unsigned int index, const bool executeImagePipeline);
template<> CPP_API const float *Film::GetChannel<float>(const FilmChannelType type,
		const unsigned int index, const bool executeImagePipeline);
template<> CPP_API const unsigned int *Film::GetChannel<unsigned int>(const FilmChannelType type,
		const unsigned int index, const bool executeImagePipeline);

class Scene;

/*!
 * \brief Camera stores camera definitions.
 */
CPP_EXPORT class CPP_API Camera {
public:
	/*!
	* \brief Types of cameras.
	*/
	typedef enum {
		// This list must be aligned with slg::Camera::CameraType
		PERSPECTIVE,
		ORTHOGRAPHIC,
		STEREO,
		ENVIRONMENT
	} CameraType;

	virtual ~Camera();

	/*!
	 * \brief Returns the camera type.
	 *
	 * \return a camera type.
	 */
	virtual const CameraType GetType() const = 0;
	/*!
	 * \brief Translates by vector t. This method can be used only when
	 * the Scene is not in use by a RenderSession.
	 *
	 * \param t is the translation vector.
	 */
	virtual void Translate(const float x, const float y, const float z) const = 0;
	/*!
	 * \brief Translates left by t. This method can be used only when
	 * the Scene is not in use by a RenderSession.
	 *
	 * \param t is the translation distance.
	 */
	virtual void TranslateLeft(const float t) const = 0;
	/*!
	 * \brief Translates right by t. This method can be used only when
	 * the Scene is not in use by a RenderSession.
	 *
	 * \param t is the translation distance.
	 */
	virtual void TranslateRight(const float t) const = 0;
	/*!
	 * \brief Translates forward by t. This method can be used only when
	 * the Scene is not in use by a RenderSession.
	 *
	 * \param t is the translation distance.
	 */
	virtual void TranslateForward(const float t) const = 0;
	/*!
	 * \brief Translates backward by t. This method can be used only when
	 * the Scene is not in use by a RenderSession.
	 *
	 * \param t is the translation distance.
	 */
	virtual void TranslateBackward(const float t) const = 0;

	/*!
	 * \brief Rotates by angle around the axis. This method can be used only when
	 * the Scene is not in use by a RenderSession.
	 *
	 * \param angle is the rotation angle.
	 * \param axis is the rotation axis.
	 */
	virtual void Rotate(const float angle, const float x, const float y, const float z) const = 0;
	/*!
	* \brief Rotates left by angle. This method can be used only when
	 * the Scene is not in use by a RenderSession.
	 *
	 * \param angle is the rotation angle.
	 */
	virtual void RotateLeft(const float angle) const = 0;
	/*!
	 * \brief Rotates right by angle. This method can be used only when
	 * the Scene is not in use by a RenderSession.
	 *
	 * \param angle is the rotation angle.
	 */
	virtual void RotateRight(const float angle) const = 0;
	/*!
	 * \brief Rotates up by angle. This method can be used only when
	 * the Scene is not in use by a RenderSession.
	 *
	 * \param angle is the rotation angle.
	 */
	virtual void RotateUp(const float angle) const = 0;
	/*!
	 * \brief Rotates down by angle. This method can be used only when
	 * the Scene is not in use by a RenderSession.
	 *
	 * \param angle is the rotation angle.
	 */
	virtual void RotateDown(const float angle) const = 0;
};

/*!
 * \brief Scene stores textures, materials and objects definitions.
 */
CPP_EXPORT class CPP_API Scene {
public:
	/*!
	* \brief Types of image map channel selection.
	*/
	typedef enum {
		// This list must be aligned with slg::ImageMapStorage::ChannelSelectionType
		DEFAULT,
		RED,
		GREEN,
		BLUE,
		ALPHA,
		MEAN,
		WEIGHTED_MEAN,
		RGB
	} ChannelSelectionType;
	/*!
	* \brief Types of image map wrap mode.
	*/
	typedef enum {
		// This list must be aligned with slg::ImageMapStorage::WrapType
		REPEAT,
		BLACK,
		WHITE,
		CLAMP
	} WrapType;
	/*!
	* \brief Types of strands tessellation.
	*/
	typedef enum {
		// This list must be aligned with slg::StrendsShape::TessellationType
		TESSEL_RIBBON,
		TESSEL_RIBBON_ADAPTIVE,
		TESSEL_SOLID,
		TESSEL_SOLID_ADAPTIVE
	} StrandsTessellationType;

	/*!
	 * \brief Create a new empty Scene.
	 *
	 * \param resizePolicyProps defines texture image maps resize policy.
	 */
	static Scene *Create(const luxrays::Properties *resizePolicyProps = nullptr);
	/*!
	 * \brief Creates a new Scene as defined by props.
	 *
	 * \param props are the Properties used to build the new Scene.
	 * \param resizePolicyProps defines texture image maps resize policy.
	 */
	static Scene *Create(const luxrays::Properties &props, const luxrays::Properties *resizePolicyProps = nullptr);
	/*!
	 * \brief Creates a new Scene as defined in fileName file.
	 *
	 * \param fileName is the name of the file with the scene description to read. It
	 * can be a text SDL file or a serialized binary file. The extension for the
	 * binary format must be ".bsc".
	 * \param resizePolicyProps defines texture image maps resize policy.
	 * This parameter has no effect when loading binary serialized binary
	 * file.
	 */
	static Scene *Create(const std::string &fileName, const luxrays::Properties *resizePolicyProps = nullptr);

	virtual ~Scene();

	/*!
	 * \brief Returns the bounding box of the complete scene (as minimum and
	 * maximum point). It is available only during the rendering (i.e. after a
	 * RenderSession::Start()).
	 */
	virtual void GetBBox(float min[3], float max[3]) const = 0;
	/*!
	 * \brief Returns the Camera of the scene.
	 *
	 * \return a reference to the Camera of this Scene. It is available only
	 * during the rendering (i.e. after a RenderSession::Start()).
	 */
	virtual const Camera &GetCamera() const = 0;
	/*!
	 * \brief Defines an image map (to be later used in textures, infinite lights, etc.).
	 * The memory allocated for pixels array is NOT freed by the Scene class nor
	 * is used after the execution of this method. The types supported are
	 * "unsigned char", "unsigned short" (as a place holder for half type) and "float".
	 *
	 * \param imgMapName is the name of the defined image map.
	 * \param pixels is a pointer to an array of image map pixels.
	 * \param gamma is the gamma correction value of the image.
	 * \param channels is the number of data used for each pixel (1 or 3).
	 * \param width is the width of the image map.
	 * \param height is the height of the image map.
	 */
	template<class T> void DefineImageMap(const std::string &imgMapName,
			T *pixels, const float gamma, const unsigned int channels,
			const unsigned int width, const unsigned int height,
			ChannelSelectionType selectionType = DEFAULT,
			WrapType wrapType = REPEAT) {
		throw std::runtime_error("Called Scene::DefineImageMap() with wrong type");
	}
	/*!
	 * \brief Check if an image map with the given name has been defined.
	 *
	 * \param imgMapName is the name to check.
	 *
	 * \return true if the image map has been defined, false otherwise.
	 */
	virtual bool IsImageMapDefined(const std::string &imgMapName) const = 0;
	/*!
	 * \brief Sets if the Scene class destructor will delete the arrays
	 * pointed to by the defined meshes.
	 *
	 * \param v defines if the Scene class destructor will delete the mesh data.
	 */
	virtual void SetDeleteMeshData(const bool v) = 0;
	/*!
	 * \brief Sets the applied transformation matrix for a normal
	 * mesh (i.e. not instanced or motion blurred).
	 *
	 * \param meshName is the name of the mesh to use.
	 * \param appliedTransMat is the transformation 4x4 matrix to use.
	 */
	virtual void SetMeshAppliedTransformation(const std::string &meshName,
			const float *appliedTransMat) = 0;
	/*!
	 * \brief Defines a mesh (to be later used in one or more scene objects). The
	 * memory allocated for the ExtTriangleMesh is always freed by the Scene class,
	 * however freeing of memory for the vertices, triangle indices, etc. depends
	 * on the setting of SetDeleteMeshData().
	 * NOTE: vertices and triangles buffers MUST be allocated with
	 * Scene::AllocVerticesBuffer() and Scene::AllocTrianglesBuffer().
	 *
	 * \param meshName is the name of the defined mesh.
	 * \param plyNbVerts is the number of mesh vertices.
	 * \param plyNbTris is the number of mesh triangles.
	 * \param p is a pointer to an array of vertices. Embree accelerator has
	 * a very special requirement. The 4 bytes after the z-coordinate of the
	 * last vertex have to be readable memory, thus padding is required.
	 * \param vi is a pointer to an array of triangles.
	 * \param n is a pointer to an array of normals. It can be NULL.
	 * \param uv is a pointer to an array of UV coordinates. It can be NULL.
	 * \param cols is a pointer to an array of vertices colors. It can be NULL.
	 * \param alphas is a pointer to an array of vertices alphas. It can be NULL.
	 */
	virtual void DefineMesh(const std::string &meshName,
		const long plyNbVerts, const long plyNbTris,
		float *p, unsigned int *vi, float *n,
		float *uvs,	float *cols, float *alphas) = 0;
	/*!
	 * \brief This is a special version of Scene::DefineMesh() used to define
	 * meshes with multiple set of UVs, Colors and/or Alphas.
	 * NOTE: the array of UVs, Colors and Alphas pointers can be freed after the
	 * call however freeing of memory for the vertices, triangle indices, etc.
	 * depends on the setting of SetDeleteMeshData().
	 *
	 * \param meshName is the name of the defined mesh.
	 * \param plyNbVerts is the number of mesh vertices.
	 * \param plyNbTris is the number of mesh triangles.
	 * \param p is a pointer to an array of vertices. Embree accelerator has
	 * a very special requirement. The 4 bytes after the z-coordinate of the
	 * last vertex have to be readable memory, thus padding is required.
	 * \param vi is a pointer to an array of triangles.
	 * \param n is a pointer to an array of normals. It can be NULL.
	 * \param uv is a pointer to an array of pointers. It can be NULL. If not, each
	 * pointer can also be NULL or a pointer to an arrays of UV coordinates.
	 * \param cols is a pointer to an array of pointers. It can be NULL. If not, each
	 * pointer can also be NULL or a pointer to an arrays of vertices colors.
	 * \param alphas is a pointer to an array of pointers. It can be NULL. If not, each
	 * pointer can also be NULL or a pointer to an arrays of vertices alphas.
	 */
	virtual void DefineMeshExt(const std::string &meshName,
		const long plyNbVerts, const long plyNbTris,
		float *p, unsigned int *vi, float *n,
		std::array<float *, LC_MESH_MAX_DATA_COUNT> *uvs,
		std::array<float *, LC_MESH_MAX_DATA_COUNT> *cols,
		std::array<float *, LC_MESH_MAX_DATA_COUNT> *alphas) = 0;
	/*!
	 * \brief Set a mesh geometry vertex AOV (i.e. generic data associated to
	 * vertices and used by textures and more).
	 * NOTE: the array can be freed after the
	 * call however freeing of memory for the vertices, triangle indices, etc.
	 * depends on the setting of SetDeleteMeshData().
	 *
	 * \param meshName is the name of the defined mesh.
	 * \param index of AOV to set.
	 * \param data to use for the AOV.
	 */
	virtual void SetMeshVertexAOV(const std::string &meshName,
			const unsigned int index, float *data) = 0;
	/*!
	 * \brief Set a mesh geometry triangle AOV (i.e. generic data associated to
	 * triangles and used by textures and more).
	 * NOTE: the array can be freed after the
	 * call however freeing of memory for the vertices, triangle indices, etc.
	 * depends on the setting of SetDeleteMeshData().
	 *
	 * \param meshName is the name of the defined mesh.
	 * \param index of AOV to set.
	 * \param data to use for the AOV.
	 */
	virtual void SetMeshTriangleAOV(const std::string &meshName,
			const unsigned int index, float *data) = 0;
	/*!
	 * \brief Save a previously defined mesh to file system in PLY or BPY format.
	 *
	 * \param meshName is the name of the defined mesh to be saved.
	 * \param fileName is the name of the file where to save the mesh. If it has
	 * the extension ".ply", the text PLY format will be used. If it has
	 * the extension ".bpy", the text PLY format will be used.
	 */
	virtual void SaveMesh(const std::string &meshName, const std::string &fileName) = 0;
	/*!
	 * \brief Defines a mesh (to be later used in one or more scene objects) starting
	 * from the strands/hairs definition included in strandsFile.
	 *
	 * \param shapeName is the name of the defined shape.
	 * \param strandsFile includes all information about the strands .
	 * \param tesselType is the tessellation used to transform the strands in a triangle mesh.
	 * \param adaptiveMaxDepth is maximum number of subdivisions for adaptive tessellation.
	 * \param adaptiveError is the error threshold for adaptive tessellation.
	 * \param solidSideCount is the number of sides for solid tessellation.
	 * \param solidCapBottom is a flag to set if strands has to have a bottom cap.
	 * \param solidCapTop is a flag to set if strands has to have a top cap.
	 * \param useCameraPosition is a flag to set if ribbon tessellation has to
	 * be faced toward the camera.
	 */
	virtual void DefineStrands(const std::string &shapeName, const luxrays::cyHairFile &strandsFile,
		const StrandsTessellationType tesselType,
		const unsigned int adaptiveMaxDepth, const float adaptiveError,
		const unsigned int solidSideCount, const bool solidCapBottom, const bool solidCapTop,
		const bool useCameraPosition) = 0;
	/*!
	 * \brief Check if a mesh with the given name has been defined.
	 *
	 * \param meshName is the name to check.
	 *
	 * \return true if the mesh has been defined, false otherwise.
	 */
	virtual bool IsMeshDefined(const std::string &meshName) const = 0;
	/*!
	 * \brief Check if a texture with the given name has been defined.
	 *
	 * \param texName is the name to check.
	 *
	 * \return true if the texture has been defined, false otherwise.
	 */
	virtual bool IsTextureDefined(const std::string &texName) const = 0;
	/*!
	 * \brief Check if a material with the given name has been defined.
	 *
	 * \param matName is the name to check.
	 *
	 * \return true if the material has been defined, false otherwise.
	 */
	virtual bool IsMaterialDefined(const std::string &matName) const = 0;
	/*!
	 * \brief Returns the number of light sources in the Scene.
	 *
	 * \return the number of light sources in the Scene.
	 */
	virtual const unsigned int GetLightCount() const = 0;
	/*!
	 * \brief Returns the number of objects in the Scene.
	 *
	 * \return the number of objects in the Scene.
	 */
	virtual const unsigned int GetObjectCount() const = 0;

	/*!
	 * \brief Edits or creates camera, textures, materials and/or objects
	 * based on the Properties defined. If the scene is in use by a
	 * RenderSession, it must be called between a RenderSession::BeginSceneEdit()
	 * and RenderSession::EndSceneEdit().
	 *
	 * \param props are the Properties with the definition of camera, textures,
	 * materials and/or objects.
	 */
	virtual void Parse(const luxrays::Properties &props) = 0;

	/*!
	 * \brief Duplicate an object in an instance using the passed transformation.
	 *
	 * \param srcObjName is the name of the object to duplicate.
	 * \param dstObjName is the name of the object to create.
	 * \param transMat is the transformation 4x4 matrix to use.
	 * \param objectID is object ID that will be assigned to the duplicate.
	 * If the ID is 0xffffffff (the Null index), the object ID of
	 * the source object will be copied.
	 */
	virtual void DuplicateObject(const std::string &srcObjName, const std::string &dstObjName,
			const float *transMat, const unsigned int objectID = 0xffffffff) = 0;
	/*!
	 * \brief Duplicate an object multiple times in instances using the passed
	 * transformations. Mostly useful for fast creating many copies of the same
	 * object (for instance for particles).
	 *
	 * \param srcObjName is the name of the object to duplicate.
	 * \param dstObjNamePrefix is the prefix of the names of the object to create. The
	 * number of the copy will be appended.
	 * \param count is the number of the object to create.
	 * \param transMat is an array of transformation 4x4 matrices to use.
	 * \param objectIDs is an array of object IDs that will be assigned to the duplicates.
	 * If NULL is passed, the object ID of the source object will be copied to all duplicates.
	 */
	virtual void DuplicateObject(const std::string &srcObjName, const std::string &dstObjNamePrefix,
			const unsigned int count, const float *transMat, const unsigned int *objectIDs = NULL) = 0;
	/*!
	 * \brief Duplicate an object in a motion blur instance using the passed transformation.
	 *
	 * \param srcObjName is the name of the object to duplicate.
	 * \param dstObjName is the name of the object to create.
	 * \param steps is the number of motion blur steps.
	 * \param times is an array of times to use
	 * \param transMat is an array of the transformation 4x4 matrix to use.
	 * \param objectID is object ID that will be assigned to the duplicate.
	 * If the ID is 0xffffffff (the Null index), the object ID of
	 * the source object will be copied.
	 */
	virtual void DuplicateObject(const std::string &srcObjName, const std::string &dstObjName,
			const unsigned int steps, const float *times, const float *transMat,
			const unsigned int objectID = 0xffffffff) = 0;
	/*!
	 * \brief Duplicate an object multiple times in a motion blur instance using
	 * the passed transformations. Mostly useful for fast creating many copies of
	 * the same object (for instance for particles).
	 *
	 * \param srcObjName is the name of the object to duplicate.
	 * \param dstObjNamePrefix is the name of the object to create.
	 * \param dstObjNamePrefix is the prefix of the names of the object to create. The
	 * number of the copy will be appended.
	 * \param count is the number of the object to create.
	 * \param steps is the number of motion blur steps.
	 * \param times is an array of times to use
	 * \param transMat is an array of the transformation 4x4 matrix to use.
	 * \param objectIDs is an array of object IDs that will be assigned to the duplicates.
	 * If NULL is passed, the object ID of the source object will be copied to all duplicates.
	 */
	virtual void DuplicateObject(const std::string &srcObjName, const std::string &dstObjNamePrefix,
			const unsigned int count, const unsigned int steps, const float *times,
			const float *transMat, const unsigned int *objectIDs = NULL) = 0;
	/*!
	 * \brief Apply a transformation to an object
	 *
	 * \param objName is the name of the object to transform.
	 * \param transMat is the transformation 4x4 matrix to apply.
	 */
	virtual void UpdateObjectTransformation(const std::string &objName, const float *transMat) = 0;
	/*!
	 * \brief Apply a new material to an object
	 *
	 * \param objName is the name of the object to apply the material to.
	 * \param matName is the new material name.
	 */
	virtual void UpdateObjectMaterial(const std::string &objName, const std::string &matName) = 0;
	
	/*!
	 * \brief Deletes an object from the scene.
	 *
	 * \param objName is the name of the object to delete.
	 */
	virtual void DeleteObject(const std::string &objName) = 0;

	/*!
	 * \brief Deletes a light from the scene.
	 *
	 * \param lightName is the name of the object to delete. Note: to delete
	 * area lights, use DeleteObject().
	 */
	virtual void DeleteLight(const std::string &lightName) = 0;

	/*!
	 * \brief Removes all unused image maps.
	 */
	virtual void RemoveUnusedImageMaps() = 0;
	/*!
	 * \brief Removes all unused textures.
	 */
	virtual void RemoveUnusedTextures() = 0;
	/*!
	 * \brief Removes all unused materials.
	 */
	virtual void RemoveUnusedMaterials() = 0;
	/*!
	 * \brief Removes all unused meshes.
	 */
	virtual void RemoveUnusedMeshes() = 0;

	/*!
	 * \brief Returns all the Properties required to define this Scene.
	 *
	 * \return a reference to the Properties of this Scene.
	 */
	virtual const luxrays::Properties &ToProperties() const = 0;
	/*!
	 * \brief Serializes a Scene in a file.
	 *
	 * \param fileName is the name of the file where to serialize the scene.
	 */
	virtual void Save(const std::string &fileName) const = 0;

	/*!
	 * \brief This must be used to allocate Mesh vertices buffer.
	 */
	static float *AllocVerticesBuffer(const unsigned int meshVertCount);
	/*!
	 * \brief This must be used to allocate Mesh triangles buffer.
	 */
	static unsigned int *AllocTrianglesBuffer(const unsigned int meshTriCount);

protected:
	virtual void DefineImageMapUChar(const std::string &imgMapName,
			unsigned char *pixels, const float gamma, const unsigned int channels,
			const unsigned int width, const unsigned int height,
			ChannelSelectionType selectionType, WrapType wrapType) = 0;
	virtual void DefineImageMapHalf(const std::string &imgMapName,
			unsigned short *pixels, const float gamma, const unsigned int channels,
			const unsigned int width, const unsigned int height,
			ChannelSelectionType selectionType, WrapType wrapType) = 0;
	virtual void DefineImageMapFloat(const std::string &imgMapName,
			float *pixels, const float gamma, const unsigned int channels,
			const unsigned int width, const unsigned int height,
			ChannelSelectionType selectionType, WrapType wrapType) = 0;
};

template<> void CPP_API Scene::DefineImageMap<unsigned char>(const std::string &imgMapName,
	unsigned char *pixels, const float gamma, const unsigned int channels,
	const unsigned int width, const unsigned int height,
	Scene::ChannelSelectionType selectionType, Scene::WrapType wrapType);
template<> void CPP_API Scene::DefineImageMap<unsigned short>(const std::string &imgMapName,
	unsigned short *pixels, const float gamma, const unsigned int channels,
	const unsigned int width, const unsigned int height,
	Scene::ChannelSelectionType selectionType, Scene::WrapType wrapType);
template<> void CPP_API Scene::DefineImageMap<float>(const std::string &imgMapName,
	float *pixels, const float gamma, const unsigned int channels,
	const unsigned int width, const unsigned int height,
	Scene::ChannelSelectionType selectionType, Scene::WrapType wrapType);

/*!
 * \brief RenderConfig stores all the configuration settings used to render a
 * scene.
 */
CPP_EXPORT class CPP_API RenderConfig {
public:
	/*!
	 * \brief Create a new RenderConfig using the provided Properties and
	 * (optional) Scene.
	 *
	 * \param props are the Properties used to build the new RenderConfig.
	 * \param scene is the Scene used to build the new RenderConfig. If specified,
	 * the Scene will not be deleted by the destructor. If NULL, the Scene will be
	 * read from the file specified in the "scene.file" Property and deleted by
	 * the destructor.
	 */
	static RenderConfig *Create(const luxrays::Properties &props, Scene *scene = NULL);
	/*!
	 * \brief Create a new RenderConfig using the provided binary file.
	 *
	 * \param fileName is the binary file used to build the new
	 * RenderConfig. The extension for the binary format must be ".bcf".
	 */
	static RenderConfig *Create(const std::string &fileName);
	/*!
	 * \brief Create a new RenderConfig using the provided resume binary file.
	 *
	 * \param fileName is the binary file used to build the new
	 * RenderConfig. The extension for the binary format must be ".rsm".
	 * \param startState the pointer to the render state will be returned here.
	 * \param startFilm the pointer to the film will be returned here.
	 */
	static RenderConfig *Create(const std::string &fileName, RenderState **startState, Film **startFilm);

	virtual ~RenderConfig();

	/*!
	 * \brief Returns a reference to the Properties used to create the RenderConfig.
	 *
	 * \return the RenderConfig properties.
	 */
	virtual const luxrays::Properties &GetProperties() const = 0;
	/*!
	 * \brief Returns the Property with the given name or the default value if it
	 * has not been defined.
	 *
	 * \return the Property with the given name.
	 */
	virtual const luxrays::Property GetProperty(const std::string &name) const = 0;

	/*!
	 * \brief Returns a reference to all Properties (including default values)
	 * defining the RenderConfig.
	 *
	 * \return the RenderConfig properties.
	 */
	virtual const luxrays::Properties &ToProperties() const = 0;

	/*!
	 * \brief Returns a reference to the Scene used in the RenderConfig.
	 *
	 * \return the reference to the RenderConfig Scene.
	 */
	virtual Scene &GetScene() const = 0;

	/*!
	 * \brief Sets configuration Properties with new values. This method can be
	 * used only when the RenderConfig is not in use by a RenderSession.
	 *
	 * \param props are the Properties to set.
	 */
	virtual void Parse(const luxrays::Properties &props) = 0;
	/*!
	 * \brief Deletes any configuration Property starting with the given prefix.
	 * This method can be used only when the RenderConfig is not in use by a
	 * RenderSession.
	 *
	 * \param prefix is the prefix of the Properties to delete.
	 */
	virtual void Delete(const std::string &prefix) = 0;

	/*!
	 * \brief Return the configured Film width, height, sub-region width, height,
	 * and if sub-region is enabled.
	 *
	 * \param filmFullWidth is where the configured Film width is returned if the
	 * pointer is not NULL.
	 * \param filmFullHeight is where the configured Film height is returned if the
	 * pointer is not NULL.
	 * \param filmSubRegion is an array of 4 values with the horizontal
	 * (followed by the vertical) begin and end of the Film region to
	 * render (in pixels).
	 *
	 * \return true if there is a sub-region to render, false otherwise.
	 */
	virtual bool GetFilmSize(unsigned int *filmFullWidth, unsigned int *filmFullHeight,
		unsigned int *filmSubRegion) const = 0;

	/*!
	 * \brief Delete the scene passed to the constructor when the class
	 * destructor is invoked.
	 */
	virtual void DeleteSceneOnExit() = 0;

	/*!
	 * \brief Save all the scene related information (the LuxCore RenderConfig,
	 * and Scene) in a file.
	 * 
	 * \param fileName of file where to save.
	 */
	virtual void Save(const std::string &fileName) const = 0;
	/*!
	 * \brief Save all the scene related information (the LuxCore RenderConfig,
	 * and Scene) in a directory using text format for the SDL. This performs the
	 * same work of FILESAVER render engine.
	 * 
	 * \param dirName of directory where to save.
	 */
	virtual void Export(const std::string &dirName) const = 0;

	/*!
	 * \brief Save all the scene related information in glTF 2.0 format.
	 * 
	 * \param fileName of file where to save.
	 */
	virtual void ExportGLTF(const std::string &fileName) const = 0;	

	/*!
	 * \brief Returns false if a (long) kernel compilation time is required at the
	 * start of the rendering. True otherwise.
	 *
	 * \return if a (long) kernel compilation time is not required.
	 */
	virtual bool HasCachedKernels() const = 0;

	/*!
	 * \brief Returns a Properties container with all default values.
	 *
	 * \return the default Properties.
	 */
	static const luxrays::Properties &GetDefaultProperties();
};

/*!
 * \brief RenderState is used to resume a rendering from a previous saved point.
 */
CPP_EXPORT class CPP_API RenderState {
public:
	/*!
	 * \brief Creates a new RenderState from a file.
	 *
	 * \param fileName id the file name of the render state file to load.
	 */
	static RenderState *Create(const std::string &fileName);
	virtual ~RenderState();
	
	/*!
	 * \brief Serializes a RenderState in a file.
	 *
	 * \param fileName is the name of the file where to serialize the render state.
	 */
	virtual void Save(const std::string &fileName) const = 0;
};

/*!
 * \brief RenderSession executes a rendering based on the RenderConfig provided.
 */
CPP_EXPORT class CPP_API RenderSession {
public:
	/*!
	 * \brief Creates a new RenderSession using the provided RenderConfig.
	 *
	 * \param config is the RenderConfig used to create the rendering session. The
	 * RenderConfig is not deleted by the destructor.
	 * \param startState is the optional RenderState to use to resume rendering. The
	 * memory for RenderState is freed by RenderSession.
	 * \param startFilm is the optional Film to use to resume rendering. The
	 * memory for Film is freed by RenderSession.
	 */
	static RenderSession *Create(const RenderConfig *config, RenderState *startState = NULL, Film *startFilm = NULL);

	/*!
	 * \brief Creates a new RenderSession using the provided RenderConfig.
	 *
	 * \param config is the RenderConfig used to create the rendering session. The
	 * RenderConfig is not deleted by the destructor.
	 * \param startStateFileName is the file name of a RenderState to use to resume rendering.
	 * \param startFilmFileName is the file name of a Film to use to resume rendering.
	 */
	static RenderSession *Create(const RenderConfig *config, const std::string &startStateFileName, const std::string &startFilmFileName);

	virtual ~RenderSession();

	/*!
	 * \brief Returns a reference to the RenderingConfig used to create this
	 * RenderSession.
	 *
	 * \return a reference to the RenderingConfig.
	 */
	virtual const RenderConfig &GetRenderConfig() const = 0;

	/*!
	 * \brief Returns a pointer to the current RenderState. The session must be
	 * paused.
	 *
	 * \return a pointer to the RenderState.
	 */
	virtual RenderState *GetRenderState() = 0;

	/*!
	 * \brief Starts the rendering.
	 */
	virtual void Start() = 0;
	/*!
	 * \brief Stops the rendering.
	 */
	virtual void Stop() = 0;

	/*!
	 * \brief It can be used to check if the session has been started.
	 */
	virtual bool IsStarted() const = 0;

	/*!
	 * \brief Stops the rendering and allows to edit the Scene.
	 */
	virtual void BeginSceneEdit() = 0;
	/*!
	 * \brief Ends the Scene editing and start the rendering again.
	 */
	virtual void EndSceneEdit() = 0;

	/*!
	 * \brief It can be used to check if the session is in scene editing mode.
	 */
	virtual bool IsInSceneEdit() const = 0;

	/*!
	 * \brief Pause the rendering.
	 */
	virtual void Pause() = 0;

	/*!
	 * \brief Resume the rendering.
	 */
	virtual void Resume() = 0;

	/*!
	 * \brief It can be used to check if the session is in scene editing mode.
	 */
	virtual bool IsInPause() const = 0;

	/*!
	 * \brief It can be used to check if the rendering is over.
	 */
	virtual bool HasDone() const = 0;

	/*!
	 * \brief Used to wait for the end of the rendering.
	 */
	virtual void WaitForDone() const = 0;

	/*!
	 * \brief Used to wait for the next frame with real-time render engines like
	 * RTPATHOCL. It does nothing with other render engines.
	 */
	virtual void WaitNewFrame() = 0;

	/*!
	 * \brief Returns a reference to a Film with the output of the rendering.
	 *
	 * \return the reference to the Film.
	 */
	virtual Film &GetFilm() = 0;

	/*!
	 * \brief Updates the statistics.
	 * This function perform also all periodic checks and tasks (like saving
	 * the film outputs, etc.).
	 */
	virtual void UpdateStats() = 0;
	/*!
	 * \brief Returns a list of statistics related to the ongoing rendering. The
	 * returned Properties is granted to have content only after the first call
	 * to the UpdateStats method.
	 *
	 * \return a Properties container with the statistics.
	 */
	virtual const luxrays::Properties &GetStats() const = 0;

	/*!
	 * \brief Dynamic edit the definition of RenderConfig properties.
	 *
	 * \param props are the Properties with the definition of: film.imagepipeline(s).*
	 * (including radiance channel scales), film.outputs.*, film.width or film.height.
	 */
	virtual void Parse(const luxrays::Properties &props) = 0;

	/*!
	 * \brief Save all the rendering related information (the LuxCore RenderConfig,
	 * Scene, RenderState and Film) in a file for a later restart. The resume
	 * file extension must be ".rsm".
	 *
	 * \param fileName is the binary file used to save.
	 */
	virtual void SaveResumeFile(const std::string &fileName) = 0;
};

}

#endif	/* _LUXCORE_H */
