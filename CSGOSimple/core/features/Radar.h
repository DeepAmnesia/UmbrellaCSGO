#pragma once
#include "../../valve_sdk/csgostructs.hpp"
#include "../../render.hpp"

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

namespace radar 
{
	struct map_info 
	{
		Vector pos = Vector(0, 0, 0);
		Vector size = Vector(0, 0, 0);
	};

	void calc_radar_point(C_BasePlayer* local_player, Vector origin, float& x, float& y);
	radar::map_info get_map();
	void draw_map(C_BasePlayer* local_player);
	void load_map(long map_overview);
	void draw_player(C_BasePlayer* local_player);
	void render(C_BasePlayer* local_player);
	inline IDirect3DTexture9* map_image;
	inline Vector map_origin;
	inline double map_scale;

	inline bool is_valid() 
	{
		return !(map_origin.x == 0.f && map_origin.y == 0.f && map_scale == 1.f);
	}

	inline bool is_grenade(C_BaseEntity* entity) 
	{
		const model_t* model = entity->GetModel();

		if (!model)
			return false;

		studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(model);

		if (!hdr)
			return false;

		std::string name = hdr->szName;

		if (name.find("dropped") != std::string::npos || name.find("thrown") != std::string::npos) {
			if (name.find("flashbang") != std::string::npos ||
				name.find("incendiarygrenade") != std::string::npos ||
				name.find("molotov") != std::string::npos ||
				name.find("fraggrenade") != std::string::npos ||
				name.find("smokegrenade") != std::string::npos ||
				name.find("decoy") != std::string::npos)
				return true;
		}

		return false;
	}

	inline bool OnMapLoad(const char* pszMapName, IDirect3DDevice9* pDevice)
	{
		//draw_plantA = false;
		//draw_plantB = false;

		char szPath[MAX_PATH];
		sprintf(szPath, "csgo\\resource\\overviews\\%s_radar.dds", pszMapName);

		std::ifstream file;
		file.open(szPath);

		if (!file.is_open())
			return false;

		file.close();

		if (FAILED(D3DXCreateTextureFromFileA(pDevice, szPath, &map_image)))
			return false;

		sprintf(szPath, ".\\csgo\\resource\\overviews\\%s.txt", pszMapName);
		std::ifstream ifs(szPath);

		if (ifs.bad())
			return false;

		std::string szInfo((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

		if (szInfo.empty())
			return false;

		//m_vMapOrigin.x = std::stoi(parseString(crypt_str("\"pos_x\""), szInfo));
		//m_vMapOrigin.y = std::stoi(parseString(crypt_str("\"pos_y\""), szInfo));
		//m_flMapScale = std::stod(parseString(crypt_str("\"scale\""), szInfo));

		//auto bombA_x = parseString(crypt_str("\"bombA_x\""), szInfo);

		//if (!bombA_x.empty())
		//{
		//	bomb_siteA.x = std::stod(parseString(crypt_str("\"bombA_x\""), szInfo));
		//	bomb_siteA.y = std::stod(parseString(crypt_str("\"bombA_y\""), szInfo));
		//	bomb_siteA.z = 0.0f;

		//	draw_plantA = true;
		//}

		//auto bombB_x = parseString(crypt_str("\"bombB_x\""), szInfo);

		//if (!bombB_x.empty())
		//{
		//	bomb_siteB.x = std::stod(parseString(crypt_str("\"bombB_x\""), szInfo));
		//	bomb_siteB.y = std::stod(parseString(crypt_str("\"bombB_y\""), szInfo));
		//	bomb_siteB.z = 0.0f;

		//	draw_plantB = true;
		//}

		g_cheat.update_radar = false;

		return true;
	}
}