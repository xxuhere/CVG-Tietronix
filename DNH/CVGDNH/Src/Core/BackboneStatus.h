#pragma once

namespace CVG
{
	/// <summary>
	/// The current state of a DNH backbone server.
	/// </summary>
	enum class BackboneStatus
	{
		/// <summary>
		/// The server is disconnected (and not because of an error).
		/// </summary>
		Disconnected,

		/// <summary>
		/// The server is disconnected. It was forced to disconnect
		/// as the result of an error.
		/// </summary>
		DiedFromError,

		/// <summary>
		/// The server is initiating.
		/// </summary>
		Starting,

		/// <summary>
		/// The server is running.
		/// </summary>
		Running,

		/// <summary>
		/// The server is shutting down.
		/// </summary>
		ShuttingDown,

		/// <summary>
		/// An unknown error happened.
		/// 
		/// Certain status values are expected when the server is in
		/// different states (i.e., connection, disconnected, ect), especially
		/// while transitioning. If we find the status is an unexpected value,
		/// the status will be forced to UnknownError to represent an
		/// unexpected status was found (the server state may possibly be corrupted).
		/// </summary>
		UnknownError
	};
}