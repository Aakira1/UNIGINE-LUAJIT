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

#ifndef __MIGRATION_PROCESS_H__
#define __MIGRATION_PROCESS_H__

#include <utils.h>

namespace Process
{
	/******************************************************************************\
	*
	* Path
	*
	\******************************************************************************/

	void processPath(string data_path)
	{
	}

	/******************************************************************************\
	*
	* Materials
	*
	\******************************************************************************/

	void material(Material m)
	{
		foreach(Texture t ; m.textures)
		{
			string texture_filepath = t.getValue();
			if (strncmp(texture_filepath, Assets::GUID_PREFIX, strlen(Assets::GUID_PREFIX)) == 0)
			{
				string texture_guid = substr(texture_filepath, strlen(Assets::GUID_PREFIX));
				string texture_filepath = Assets::getAssetFilepath(texture_guid);
				if (texture_filepath != "")
				{
					string primary_guid = Assets::getPrimaryRuntimeGuid(texture_filepath);
					if (primary_guid != "" && primary_guid != texture_guid)
					{
						Log::info(format("Replace guid from %s to %s for asset %s in %s material.\n", texture_guid, primary_guid, texture_filepath, m.getPath()));
						t.setValue(Assets::GUID_PREFIX + primary_guid);
					}
				}
			}
		}
	}

	/******************************************************************************\
	*
	* MaterialGraphs
	*
	\******************************************************************************/

	void materialGraph(MaterialGraph graph)
	{
		foreach(MaterialGraphNode node; graph.nodes)
		{
			materialNode(node);
		}
	}

	void materialNode(MaterialGraphNode node)
	{
	}

	/******************************************************************************\
	*
	* Property
	*
	\******************************************************************************/

	// local changes of the property file
	void propertyFile(Xml xml)
	{
	}

	// global changes that related to another properties
	void property(Properties::Property p)
	{
	}

	/******************************************************************************\
	*
	* Nodes
	*
	\******************************************************************************/

	void node(Xml xml)
	{
		string type = xml.getArg("type","unknown");

		// En-11445
		if (type == "LightVoxelProbe")
		{
			xmlRenameChild(xml, "ambient_bias", "diffuse_normal_bias");
			xmlRenameChild(xml, "ambient_cubic_filtering", "diffuse_cubic_filtering");

			xmlRenameChild(xml, "reflection_enabled", "specular_enabled");
			xmlRenameChild(xml, "reflection_bias", "specular_reflection_bias");
			xmlRenameChild(xml, "reflection_cubic_filtering", "specular_cubic_filtering");
			xmlRenameChild(xml, "reflection_visibility_roughness_min", "specular_visibility_roughness_min");
			xmlRenameChild(xml, "reflection_visibility_roughness_max", "specular_visibility_roughness_max");

			float specular_normal_bias = xml.getChildData("diffuse_normal_bias", 0.0f);
			xmlAddChild(xml, "specular_normal_bias").setData(specular_normal_bias);
		}

		// En-11496
		if (type == "LightEnvironmentProbe")
		{
			if (xml.findChild("grab_bake_visibility_haze") == -1)
				xml.addChild("grab_bake_visibility_haze").setBoolData(true);
			if (xml.findChild("grab_bake_visibility_clouds") == -1)
				xml.addChild("grab_bake_visibility_clouds").setBoolData(true);
		}
	}

	void surfaceProperty(Xml xml_surface, Xml xml_prop, Properties::Property prop)
	{
	}

	/******************************************************************************\
	*
	* Bodies
	*
	\******************************************************************************/
	void body(Xml xml)
	{
	}

	/******************************************************************************\
	*
	* Shapes
	*
	\******************************************************************************/
	void shape(Xml xml)
	{
	}

	/******************************************************************************\
	*
	* Joints
	*
	\******************************************************************************/
	void joint(Xml xml)
	{
	}

	/******************************************************************************\
	*
	* Render
	*
	\******************************************************************************/
	void render(Xml xml)
	{
		xmlDeleteChild(xml, "render_ssrtgi_noise_step");

		int id = xml.findChild("render_sharpen_intensity");
		if (id != -1)
		{
			Xml child = xml.getChild(id);
			child.setFloatData(2.0f);
		}

		Xml render_vr_emulation = xmlGetChild(xml, "render_vr_emulation");
		if (render_vr_emulation != NULL)
		{
			int value = render_vr_emulation.getIntData();
			if (value == 3)
				render_vr_emulation.setIntData(7);
		}

		// WARNING: update render.xml and uncomment this before migration running!
		Xml xml_new = new Xml();
		string path_render = pathname(replace(__FILE__, "\\", "/")) + "render.xml";
		if(xml_new.load(path_render))
		{
			for(int i = 0; i < xml.getNumChildren(); i++)
			{
				Xml x = xml.getChild(i);

				if (xml_new.findChild(x.getName()) != -1)
					continue;
				delete xml.removeChild(x);
				i--;
			}

			xmlMergeChildren(xml_new, xml);
		}
		delete xml_new;
	}

	/******************************************************************************\
	*
	* Sound
	*
	\******************************************************************************/
	void sound(Xml xml)
	{
	}

	/******************************************************************************\
	*
	* Physics
	*
	\******************************************************************************/
	void physics(Xml xml)
	{
	}

	/******************************************************************************\
	*
	* World
	*
	\******************************************************************************/
	void world(Xml xml)
	{
	}

	/******************************************************************************\
	*
	* Track
	*
	\******************************************************************************/

	void track(Xml xml)
	{
	}

	/******************************************************************************\
	*
	* Meta
	*
	\******************************************************************************/

	void meta(Xml xml, string file_name)
	{
		Xml asset_type_xml = xmlGetChild(xml, "type");
		if (asset_type_xml != NULL && asset_type_xml.getData() == "fbx")
		{
			Xml parameters_xml = xmlGetChild(xml, "parameters");
			if (parameters_xml != NULL)
			{
				Xml uv_unwrap_version_xml = parameters_xml.addChild("parameter");
				uv_unwrap_version_xml.setArg("type", "string");
				uv_unwrap_version_xml.setArg("name", "uv_unwrap_version");
				uv_unwrap_version_xml.setData("xatlas_v0");
			}
		}
	}

	/******************************************************************************\
	*
	* AppProjection
	*
	\******************************************************************************/
	void projectionBinary(string file_name)
	{
	}

	void projection(Xml xml)
	{
	}

	/******************************************************************************\
	*
	* Syncker
	*
	\******************************************************************************/
	void synckerBinary(string file_name)
	{
	}

	void syncker(Xml xml)
	{
	}

	/******************************************************************************\
	*
	* Configs
	*
	\******************************************************************************/
	void configBoot(Xml xml)
	{
		int console_id = xml.findChild("console");
		if (console_id != -1)
		{
			Xml console_xml = xml.getChild(console_id);

			xmlDeleteChild(console_xml, "d3d12_small_pool_chunk_size");
			xmlDeleteChild(console_xml, "d3d12_small_pool_size");

			// En-10276
			xmlRenameChild(console_xml, "streamline_application_id", "dlss_application_id");
			xmlRenameChild(console_xml, "streamline_project_id", "dlss_project_id");
			xmlRenameChild(console_xml, "streamline_log_mode", "dlss_log_mode");

			xmlDeleteChild(console_xml, "fast_shutdown");
		}
	}

	void configControls(Xml xml)
	{
	}

	void configUser(Xml xml)
	{
	}

	/******************************************************************************\
	*
	* Editor's hotkey preset
	*
	\******************************************************************************/
	void editorHotkeyPreset(Json json)
	{
	}

	/******************************************************************************\
	*
	* Umount
	*
	\******************************************************************************/
	int is_value_exist(Json json, string value)
	{
		for(int i = 0; i < json.getNumChildren(); i++)
		{
			Json child = json.getChild(i);
			if (child.isString() && (child.getString() == value))
				return 1;
		}
		return 0;
	}

	void umount(Json json)
	{
		Json json_filters = json.find("guidsdb_ignore_filters");
		if (json_filters == NULL)
		{
			json_filters = json.addChild("guidsdb_ignore_filters");
			json_filters.setArray();
		}
		else if (!json_filters.isArray())
		{
			Log::warning("A wrong type for 'guidsdb_ignore_filters' element. It has been changed to Array");
			json_filters.setArray();
		}

		if (!is_value_exist(json_filters, "*.gpu_cache"))
			json_filters.addChild("").setString("*.gpu_cache");
		if (!is_value_exist(json_filters, "*.meta"))
			json_filters.addChild("").setString("*.meta");
	}

}

#endif /* __MIGRATION_PROCESS_H__ */
