#include "weaponids.h"
#include "../../helpers/utils.hpp"
#include "citemsystem.h"

weaponids_t weaponids = {};

void weaponids_t::on_init() {
	const auto item_system = reinterpret_cast<uintptr_t*>(g_cheat.get_item_system());

	for (size_t i = 0; i < item_system[58]; i++) {
		auto def = *reinterpret_cast<CEconItemDefinition**>(item_system[53] + 4 * (3 * i) + 4);
		const auto type = get_hash(def->get_item_type_name());

		if (type != Utils::get_const("#CSGO_Type_Pistol") && type != Utils::get_const("#CSGO_Type_Rifle") && type != Utils::get_const("#CSGO_Type_SniperRifle") && type != Utils::get_const("#CSGO_Type_Machinegun") &&
			type != Utils::get_const("#CSGO_Type_SMG") && type != Utils::get_const("#CSGO_Type_Shotgun") && type != Utils::get_const("#CSGO_Type_Knife") && type != Utils::get_const("#CSGO_Type_Grenade")
			&& type != Utils::get_const("#CSGO_Type_Equipment") && type != Utils::get_const("#CSGO_Type_C4") && type != Utils::get_const("#Type_Hands") && type != Utils::get_const("#CSGO_Type_Collectible")
			&& type != Utils::get_const("#Type_CustomPlayer") && type != Utils::get_const("#CSGO_Tool_Sticker") && type != Utils::get_const("#CSGO_Type_WeaponCase") && type != Utils::get_const("#CSGO_Tool_WeaponCase_KeyTag")
			&& type != Utils::get_const("#CSGO_Type_MusicKit"))
			continue;

		m_list[def->get_definition_index()] = def;
		m_types[type].emplace_back(def);
	}
}