
#include <string>
#include "LUrlParser.h"

#include "interwebs.h"

namespace RBX
{
	class ContentId
	{
	private:
		std::string url;
		int id;
		LUrlParser::ParseURL parsed;
	public:

		/* Gets content from contentid */
		std::string resolve();

		/* resolves either on crapblox or roblox */
		std::string resolveById();

		/* Checks url for rbxassetid:// */
		bool isRbxAssetId();

		/* Finds id from rbxasset */
		int findId();

		/* Checks url for rbxasset:// */
		bool isRbxLocalAsset();

		/* Finds asset path from localasset */
		std::string findLocalPath();

		/* finds content folder path */
		std::string findContentPath();

		/* Finds asset in content folder */
		std::string findContentByPath(std::string path);

		ContentId(std::string _u)
		{
			url = _u;
			parsed = LUrlParser::ParseURL::parseURL(url);
		}
	};

	static std::string resolveUrl(std::string url)
	{
		RBX::ContentId* id = new RBX::ContentId(url);
		return id->resolve();
	}
}