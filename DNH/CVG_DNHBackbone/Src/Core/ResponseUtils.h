#include "json.hpp"
#include <string>

using json = nlohmann::json;

namespace CVG{ namespace ResponseUtils
{
	/// <summary>
	/// Extract the postage from a message.
	/// </summary>
	/// <param name="js">The postage found.</param>
	/// <returns>The postage found, or an empty string if none
	/// was found.</returns>
	inline std::string ExtractPostage(const json& js)
	{
		if (!js.contains("postage") || !js["postage"].is_string())
			return std::string();

		return js["postage"];
	}

	/// <summary>
	/// Apply a postage to a JSON object.
	/// </summary>
	/// <param name="jsdst">The JSON response object to apply the postage to.</param>
	/// <param name="postage">The postage to add. An empty still will do nothing.</param>
	/// <returns>
	/// True if the postage was applied. 
	/// False if the postage was empty, in which case the operation is ignored.
	/// </returns> 
	inline bool ApplyPostage(json& jsdst, const std::string& postage)
	{
		if (postage.size() == 0)
			return false;

		jsdst["postage"] = postage;
		return true;
	}

	/// <summary>
	/// Copy the postage from a source JSON object to a destination JSON object.
	/// 
	/// The operation will only occur if the source JSON contains a postage member.
	/// </summary>
	/// <param name="jsdst">The JSON request value to find the postage in.</param>
	/// <param name="jsscr">The JSON response value to apply the postage to.</param>
	/// <returns>True if a transfer occured. Else, false.</returns>
	inline bool ApplyPostage(json& jsdst, const json& jsscr)
	{
		std::string postage = ExtractPostage(jsscr);
		return ApplyPostage(jsdst, (std::string)jsscr["postage"]);
	}

	/// <summary>
	/// Add a request member to a response JSON.
	/// 
	/// When an message is sent as a response, it will include the value of the
	/// request's apity in the "request" member.
	/// 
	/// The member will only be added if the request value is not an empty string.
	/// </summary>
	/// <param name="js">"The response JSON.</param>
	/// <param name="request">The request value.</param>
	/// <returns>True if the request was applied. Else, false.</returns>
	inline bool ApplyRequest(json& js, const std::string& request)
	{
		if (request.size() == 0)
			return false;

		js["request"] = request;
		return true;
	}

}}