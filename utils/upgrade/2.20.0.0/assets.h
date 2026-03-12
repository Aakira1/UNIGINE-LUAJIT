#ifndef __MIGRATION_ASSETS_H__
#define __MIGRATION_ASSETS_H__

namespace Assets
{
	string GUID_PREFIX = "guid://";
	int asset_guid_to_filepath[];

	string getAssetFilepath(string guid_string)
	{
		if (!asset_guid_to_filepath.check(guid_string))
		{
			return "";
		}
		return asset_guid_to_filepath[guid_string];
	}

	string getAssetGuidByFilepath(string filepath)
	{
		Xml xml = new Xml();
		if (!xml.load(filepath))
		{
			delete xml;
			return "";
		}

		if (xml.getName() != "asset")
		{
			delete xml;
			return "";
		}

		Xml guid_xml = xml.getChild("guid");
		if (guid_xml == NULL)
		{
			delete xml;
			return "";
		}

		string ret = guid_xml.getData();
		delete xml;
		return ret;
	}

	string getPrimaryRuntimeGuid(string asset_filepath)
	{
		string meta_asset_filepath = asset_filepath + ".meta";
		Xml xml = new Xml();
		if (!xml.load(meta_asset_filepath)) {
			delete xml;
			return "";
		}

		if (xml.getName() != "asset")
		{
			delete xml;
			return "";
		}

		Xml runtimes_xml = xml.getChild("runtimes");
		if (runtimes_xml == NULL)
		{
			delete xml;
			return "";
		}

		int num_runtimes = runtimes_xml.getNumChildren();
		for (int i = 0; i < num_runtimes; i++)
		{
			Xml runtime_xml = runtimes_xml.getChild(i);
			if (!runtime_xml.isArg("link")) continue;
			if (runtime_xml.getArg("link") != "1") continue; //primary
			if (!runtime_xml.isArg("id")) continue;
			string ret = runtime_xml.getArg("id");
			delete xml;
			return ret;
		}

		delete xml;
		return "";
	}

	void init()
	{
		asset_guid_to_filepath.clear();
	}

	int collectAssetGuids(string data_path, string path)
	{
		Dir dir = new Dir();
		if (!dir.open(path))
		{
			delete dir;
			return false;
		}

		int num_subdirs = dir.getNumDirs();
		for (int i = 0; i < num_subdirs; i++)
		{
			if (skipFile(data_path, dir.getDirName(i)))
				continue;
			if (startsWith(relname(data_path, dir.getDirName(i)), ".runtimes"))
				continue;
			collectAssetGuids(data_path, dir.getDirName(i));
		}

		int num_files = dir.getNumFiles();
		for (int i = 0; i < num_files; i++)
		{
			string filepath = dir.getFileName(i);
			if (extension(filepath) == "meta")
			{
				string asset_guid = getAssetGuidByFilepath(filepath);
				if (asset_guid != "")
				{
					asset_guid_to_filepath.append(asset_guid, extension(filepath, ""));
				}
			}
		}

		delete dir;
		return true;
	}

}

#endif /* __MIGRATION_ASSETS_H__ */
