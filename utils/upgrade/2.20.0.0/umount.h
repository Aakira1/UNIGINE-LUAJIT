/* Copyright (C) 2005-2025, UNIGINE. All rights reserved.
*
* This file is a part of the UNIGINE 2 SDK.
*
* Your use and / or redistribution of this software in source and / or
* binary form, with or without modification, is subject to: (i) your
* ongoing acceptance of and compliance with the terms and conditions of
* the UNIGINE License Agreement; and (ii) your inclusion of this notice
* in any version of this software that you use or redistribute.
* A copy of the UNIGINE License Agreement is available by contacting
* UNIGINE. at http://unigine.com/
*/


#ifndef __MIGRATION_UMOUNT_H__
#define __MIGRATION_UMOUNT_H__

namespace Umount
{
	void processPath(string data_path)
	{
		string files[0] = ();
		findFilesByExt(files, data_path, ".umount");
		foreach(string file; files)
		{
			if(!skipFile(data_path, file))
				processFile(pathname(data_path), pathname(file) + basename(file));
		}
	}

	void processFile(string data_path, string file)
	{
		Json json = new Json();
		if(!json.load(file))
		{
			Log::error("failed to load \"%s\"", file);
			delete json;
			return;
		}

		Log::info("upgrading \"%s\"", file);

		Process::umount(json);

		json.save(file);
		delete json;
	}

} /* namespace Umount */

#endif /* __MIGRATION_UMOUNT_H__ */
