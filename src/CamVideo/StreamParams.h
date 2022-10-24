#pragma once


/// <summary>
/// Parameter ID values for ALL items that can be used for 
/// GetParam() or SetParam(), for ALL IManagedCam subclasses.
/// 
/// Note that some of these params are handled by delegating to
/// contained ICamImpl instances.
/// </summary>
enum StreamParams
{
	// Alpha blending amount
	Alpha,

	/// <summary>
	/// For simple thresholding image processing. The pixel value to threshold.
	/// </summary>
	StaticThreshold,

	/// <summary>
	/// For composite video feeds, the width of the video to save.
	/// </summary>
	CompositeVideoWidth,

	/// <summary>
	/// For composite video feeds, the height of the video to save.
	/// </summary>
	CompositeVideoHeight,

	/// <summary>
	/// The exposure of the camera, in microseconds.
	/// 
	/// NOTE: There are two ways a camera's threshold are set, from the AppOptions on
	/// the camera directly, and from using this StreamParams. This is used to override
	/// the per-cam option, dynamically at runtime.
	/// </summary>
	ExposureMicroseconds
};