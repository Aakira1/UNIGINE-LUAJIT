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

#ifndef __MIGRATION_WORLDS_NODES_H__
#define __MIGRATION_WORLDS_NODES_H__

namespace WorldsNodes {

	string data_catalog;
	string current_file;
	string node_binary_data_dir;

	string get_save_name(string name) {
		if(is_file(data_catalog + name))
			return data_catalog + name;
		return findFile(data_catalog, name);
	}

	void rename_child(Xml xml, string from, string to) {

		int num = xml.findChild(from);

		if(num != -1) {
			xml.getChild(num).setName(to);
		}
	}

	void processPath(string path, string ext) {
		data_catalog = path;

		string files[0] = ();
		findFilesByExt(files, path, ext);
		foreach(string file; files)
			if(!skipFile(path, file))
				processFile(file);
	}

	void processFile(string name) {

		current_file = name;
		node_binary_data_dir = "";

		Xml xml = new Xml();

		if(!xml.load(name)) {
			delete xml;
			return;
		}

		Log::info("upgrading \"%s\"", name);
		xml.setArg("version", ENGINE_VERSION);

		if(pathExtensionCompare(name, ".node")) {

			for(int i = 0; i < xml.getNumChildren(); i++) {

				Xml x = xml.getChild(i);
				if(x.getName() == "node") {
					processNodes(x);
					if(x == NULL)
						i--;
				}
			}
		} else {
			if(pathExtensionCompare(name, ".world")) {
				Process::world(xml);

				Xml editor_element = xml.find("editor");
				if(editor_element != NULL) {
					for(int i = 0; i < editor_element.getNumChildren(); i++) {

						Xml x = editor_element.getChild(i);
						if(x.getName() == "node") {
							processNodes(x);
							if(x == NULL) i--;
						}
					}
				}

				if(xml.find("render") == NULL)
					xml.addChild("render");
			}

			Xml render_element = xml.find("render");
			if(render_element != NULL)
				Process::render(render_element);

			Xml sound_element = xml.find("sound");
			if(sound_element != NULL)
				Process::sound(sound_element);

			Xml physics_element = xml.find("physics");
			if(physics_element != NULL)
				Process::physics(physics_element);

			if(!pathExtensionCompare(name, ".world")) {

				for(int i = 0; i < xml.getNumChildren(); i++) {

					Xml x = xml.getChild(i);
					if(x.getName() == "render")
						continue;
					if(x.getName() == "sound")
						continue;
					if(x.getName() == "physics")
						continue;
					delete xml.removeChild(x);
					i--;
				}
			}
		}

		if (node_binary_data_dir != "" && (pathExtensionCompare(name, ".node") || pathExtensionCompare(name, ".nodes") || pathExtensionCompare(name, ".world")))
		{
			xml.setArg("binary_nodes_data", node_binary_data_dir);
		}

		xml.save(name);
		delete xml;
	}

	string find_guid(string path)
	{
		string meta_path = path + ".meta";

		// try meta file
		{
			Xml xml = new Xml();
			if (xml.load(meta_path))
			{
				int x_id = xml.findChild("guid");
				if (x_id != -1)
				{
					Xml x_guid = xml.getChild(x_id);
					string ret = x_guid.getData();
					return ret;
				}
			}
		}

		// try guids.db
		{
			Json json = new Json();
			if (json.load(data_catalog + "guids.db"))
			{
				string virtual_path = substr(path, strlen(data_catalog));
				forloop (int i = 0; json.getNumChildren())
				{
					Json child = json.getChild(i);
					if (child.getString() == virtual_path)
					{
						string ret = child.getName();
						return ret;
					}
				}
			}
		}

		// create temp
		log.message("Could not find `%s` guid, creating new guid\n", path);
		return stringToGUID(path);
	}

	class RenderTransform
	{
		vec4 row0;
		vec4 row1;
		vec4 row2;
	};

	void save_cluster_data(Xml &x, int node_id, RenderTransform &positions[])
	{
		string node_guid = find_guid(current_file);

		if (node_binary_data_dir == "")
		{
			node_binary_data_dir = ".binary_nodes_data/" + node_guid;
		}

		string cluster_dir = data_catalog + ".binary_nodes_data/" + node_guid + "/clusters";
		mkdir(cluster_dir, 1);

		string name = format("%i.cluster", node_id);
		string cluster_path = cluster_dir + "/" + name;

		x.addChild("cluster_file_id").setData(format("%i", node_id));

		log.message("Cluster path %s\n", cluster_path);

		File f = new File();
		if (!f.open(cluster_path, "wb"))
		{
			log.message("Couldn't save cluster at %s\n", cluster_path);
			return;
		}

		// magic & version
		f.writeInt(479);
		f.writeInt(1);

		// flags
		f.writeInt(0);

		f.writeInt(positions.size());
		forloop(int i = 0; positions.size())
		{
			RenderTransform t = positions[i];
			f.writeVec4(t.row0);
			f.writeVec4(t.row1);
			f.writeVec4(t.row2);
		}

		f.close();
	}

	void processNodes(Xml xml) {

		Process::node(xml);

		int type_arg_id = xml.findArg("type");
		if (type_arg_id != -1 && xml.getArgValue(type_arg_id) == "ObjectMeshCluster")
		{
			int node_id = xml.getIntArg("id");
			log.message("Upgrading cluster %s, node %i\n", current_file, node_id);

			RenderTransform render_transforms[0];

			int trasnforms_id = xml.findChild("transforms");
			if (trasnforms_id != -1)
			{
				Xml x = xml.getChild(trasnforms_id);

				float positions[0];
				x.getFloatArrayData(positions);

				int i = 0;
				while (i < positions.size())
				{
					RenderTransform t = new RenderTransform();

					t.row0.x = positions[i + 0];
					t.row0.y = positions[i + 1];
					t.row0.z = positions[i + 2];
					t.row0.w = positions[i + 3];

					t.row1.x = positions[i + 4];
					t.row1.y = positions[i + 5];
					t.row1.z = positions[i + 6];
					t.row1.w = positions[i + 7];

					t.row2.x = positions[i + 8];
					t.row2.y = positions[i + 9];
					t.row2.z = positions[i + 10];
					t.row2.w = positions[i + 11];

					render_transforms.append(t);

					i += 12;
				}

				xml.removeChild(x);
			}

			int positions_id = xml.findChild("positions");
			if (positions_id != -1)
			{
				Xml x = xml.getChild(positions_id);

				float positions[0];
				x.getFloatArrayData(positions);

				int i = 0;
				while (i < positions.size())
				{
					RenderTransform t = new RenderTransform();

					t.row0.x = positions[i + 0 + 0];
					t.row0.y = positions[i + 0 + 4];
					t.row0.z = positions[i + 0 + 8];
					t.row0.w = positions[i + 0 + 12];

					t.row1.x = positions[i + 1 + 0];
					t.row1.y = positions[i + 1 + 4];
					t.row1.z = positions[i + 1 + 8];
					t.row1.w = positions[i + 1 + 12];

					t.row2.x = positions[i + 2 + 0];
					t.row2.y = positions[i + 2 + 4];
					t.row2.z = positions[i + 2 + 8];
					t.row2.w = positions[i + 2 + 12];

					render_transforms.append(t);

					i += 16;
				}

				xml.removeChild(x);
			}

			int position_id = xml.findChild("position");
			if (position_id != -1)
			{
				int id = 0;
				while (id < xml.getNumChildren())
				{
					Xml x = xml.getChild(id);
					if (x.getName() == "position")
					{
						float positions[0];
						x.getFloatArrayData(positions);

						RenderTransform t = new RenderTransform();

						t.row0.x = positions[0 + 0];
						t.row0.y = positions[0 + 4];
						t.row0.z = positions[0 + 8];
						t.row0.w = positions[0 + 12];

						t.row1.x = positions[1 + 0];
						t.row1.y = positions[1 + 4];
						t.row1.z = positions[1 + 8];
						t.row1.w = positions[1 + 12];

						t.row2.x = positions[2 + 0];
						t.row2.y = positions[2 + 4];
						t.row2.z = positions[2 + 8];
						t.row2.w = positions[2 + 12];

						render_transforms.append(t);

						xml.removeChild(x);
					} else {
						id++;
					}
				}
			}

			if (render_transforms.size() != 0)
				save_cluster_data(xml, node_id, render_transforms);
		}

		// processing child nodes
		for(int i = 0; i < xml.getNumChildren(); i++) {

			Xml x = xml.getChild(i);
			string name = x.getName();

			if (name == "node")
			{
				processNodes(x);
				if(x == NULL) i--;
			}
			else if (name == "body")
			{
				processBody(x);
				if(x == NULL) i--;
			}
		}
	}

	void processBody(Xml xml)
	{

		Process::body(xml);

		// processing child nodes
		for(int i = 0; i < xml.getNumChildren(); i++) {

			Xml x = xml.getChild(i);
			string name = x.getName();

			if(name == "joint")
			{
				Process::joint(x);
				if(x == NULL) i--;
			}
			if(name == "shape")
			{
				Process::shape(x);
				if(x == NULL) i--;
			}
		}

	}

}

#endif /* __MIGRATION_WORLDS_NODES_H__ */
