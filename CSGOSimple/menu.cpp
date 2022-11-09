#pragma warning (disable: 4018)

#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>
#include <array>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"
#include "config.hpp"
#include "helpers/utils.hpp"
#include "core/features/legit_bot.h"
#include "core/features/model.h"
#include "core/features/game_chams.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"

const auto color_edit_flags = (ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar);

static char* sidebar_tabs[] = {
    "TRIGGER",
    "ESP",
    "AIM",
    "MISC",
    "CONFIG",
    "RADAR",
    "GREANDES",
    "WEAPON",
    "BUY BOT"
};

enum {
    TAB_TRIGGER,
    TAB_ESP,
    TAB_AIMBOT,
    TAB_MISC,
    TAB_CONFIG,
    TAB_RADAR,
    TAB_GRENADES,
    TAB_WEAPON,
    TAB_BUYBOT
};

namespace ImGuiEx
{
    inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
    {
        auto clr = ImVec4{
            v->r() / 255.0f,
            v->g() / 255.0f,
            v->b() / 255.0f,
            v->a() / 255.0f
        };

        if (ImGui::ColorEdit4(label, &clr.x, show_alpha)) {
            v->SetColor(clr.x, clr.y, clr.z, clr.w);
            return true;
        }
        return false;
    }
    inline bool ColorEdit3(const char* label, Color* v)
    {
        return ColorEdit4(label, v, false);
    }
}

template<size_t N>
void render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
    bool values[N] = { false };

    values[activetab] = true;

    for (auto i = 0; i < N; ++i) {
        if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h })) {
            activetab = i;
        }
        if (sameline && i < N - 1)
            ImGui::SameLine();
    }
}

ImVec2 get_sidebar_size()
{
    constexpr float padding = 10.0f;
    constexpr auto size_w = padding * 2.0f + 100;
    constexpr auto size_h = padding * 2.0f + (sizeof(sidebar_tabs) / sizeof(char*)) * 35;

    return ImVec2{ size_w, ImMax(375.0f, size_h) };
}

void RenderBuyBotTab()
{
    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::Checkbox("Buy Bot", &g_Options.buy_bot);

        ImGui::Combo("Primary", &g_Options.primary_buy_bot, "None\0SCAR20/G3SG1\0Scout\0AWP\0M4A1/AK47\0");
       
        ImGui::Combo("Secondary", &g_Options.secondary_buy_bot, "None\0FN57/TEC9\0Dual elites\0Deagle/Revolver\0P2000/Glock-18\0P250\0");

        const std::vector log_type
        {
            ("Fire grenade/Molotov"),
            ("Smoke grenade"),
            ("Flash grenade"),
            ("Explosive grenade"),
            ("Taser"),
            ("Heavy armor"),
            ("Helmet"),
            ("Defuser")
        };

        if (ImGui::BeginCombo("Equipment", "Select"))
        {
            for (int i = 0; i < log_type.size(); i++)
                ImGui::Checkbox(log_type[i], &g_Options.equipment_buy_bot[i]);

            ImGui::EndCombo();
        }
        ImGui::SliderInt("Round Start Delay (ms)", &g_Options.buy_bot_first_buy_delay, g_Options.safe_mode ? 100 : 0, 1000);
        ImGui::SliderInt("Delay Between Buy (ms)", &g_Options.buy_bot_active_buy_delay, g_Options.safe_mode ? 200 : 0, 1000);
    }
    ImGui::EndGroupBox();
}

void RenderTriggetBotTab()
{
    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::Combo("Weapon", &Utils::cur_weapon, legitbot::weapon_data, IM_ARRAYSIZE(legitbot::weapon_data));
        ImGui::Text("Bind (empty = work without bind)");
        ImGui::Keybind("trig bind", g_Options.trigget_key, g_Options.trigger_mode);
        ImGui::Spacing();

        ImGui::Checkbox("Enable", &g_Options.trigger_for_weapon[Utils::cur_weapon].enable);
        ImGui::Checkbox("Only In Scope", &g_Options.trigger_only_in_scope);
        ImGui::Spacing();
 
        ImGui::Checkbox("Team Attack", &g_Options.trigger_for_weapon[Utils::cur_weapon].legit_teamattack);
        if (!g_Options.safe_mode)
        {
            ImGui::Checkbox("Legit Checks", &g_Options.trigger_for_weapon[Utils::cur_weapon].legit_checks);
        }
        ImGui::Spacing();
       
        ImGui::SliderInt("Min. Damage", &g_Options.trigger_for_weapon[Utils::cur_weapon].minimal_damage, 0, 100);
        ImGui::SliderInt("Chance", &g_Options.trigger_for_weapon[Utils::cur_weapon].trigger_chance, 0, g_Options.safe_mode? 50 : 100);
        ImGui::Spacing();

        ImGui::Combo("Hitgroup", &g_Options.trigger_for_weapon[Utils::cur_weapon].hitgroup, "All\0Head\0Chest\0Stomach\0Left arm\0Right arm\0Left leg\0Right leg\0");
    }
    ImGui::EndGroupBox();
}

void RenderRadarTab()
{
    auto& style = ImGui::GetStyle();
    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
        ImGui::Checkbox("Radar", &g_Options.radar_enable);
 
        ImGui::ColorEdit4("Color CT Visible", g_Options.color_radar_ct_visible);
     
        ImGui::ColorEdit4("Color T Visible", g_Options.color_radar_t_visible);
        ImGui::Checkbox("Enemy Only", &g_Options.radar_enemy_only);
        ImGui::Checkbox("Invisible", &g_Options.radar_invisible);
       
        ImGui::ColorEdit4("Color CT Invisible", g_Options.color_radar_ct_invisible);
       
        ImGui::ColorEdit4("Color T Invisible", g_Options.color_radar_t_invisible);
        ImGui::Checkbox("Texture", &g_Options.radar_texture);
        ImGui::Checkbox("Grenades", &g_Options.radar_grenades);
       
        ImGui::ColorEdit4("Color Grenades", g_Options.color_radar_grenades);
        ImGui::SliderInt("Range", &g_Options.radar_range, 1, 5000);
        ImGui::SliderInt("Size", &g_Options.radar_size, 50, 400);
        ImGui::PopStyleVar();
    }
    ImGui::EndGroupBox();
}

void RenderGrenadesTab()
{
    ImGui::BeginGroupBox("##body_content");
    {  
        ImGui::Checkbox("Grenade Prediction", &g_Options.grenade_prediction);
        ImGui::Checkbox("Grenade Timers", &g_Options.grenade_timers);

        ImGui::Spacing();
        ImGui::Text("Projectiles:");
        ImGui::Checkbox("Prjoectile esp", &g_Options.grenade_projectiles);
        ImGui::Checkbox("Box", &g_Options.grenade_box);
        ImGui::Checkbox("Text", &g_Options.grenade_text);
        ImGui::Checkbox("Icon", &g_Options.grenade_icon);

        ImGui::Spacing();
        ImGui::Text("Text Size:");
        ImGui::SliderFloat("Text Size", &g_Options.grenade_text_size, 5.f, 30.f);
        ImGui::SliderFloat("Icon Size", &g_Options.grenade_icon_size, 5.f, 30.f);

        ImGui::Spacing();
        ImGui::Text("Colors:");
        ImGui::ColorEdit3("Color Trajectory", g_Options.color_grenade_trajectory);
        ImGui::ColorEdit3("Color Circle", g_Options.color_grenade_circle);
        ImGui::ColorEdit3("Color Box", g_Options.color_grenade_box);
        ImGui::ColorEdit3("Color Text", g_Options.color_grenade_text);
        ImGui::ColorEdit3("Color Icon", g_Options.color_grenade_icon);
    }
    ImGui::EndGroupBox();
}

void RenderWeaponTab()
{
    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::Checkbox("Dropped esp", &g_Options.dropped_weapon);

        ImGui::Spacing();
        ImGui::Text("Options:");
        ImGui::Checkbox("Box", &g_Options.dropped_weapon_box);
        ImGui::Checkbox("Name", &g_Options.dropped_weapon_text);
        ImGui::Checkbox("Icon", &g_Options.dropped_weapon_icon);
        ImGui::Checkbox("Ammo", &g_Options.dropped_weapon_ammo);

        ImGui::Spacing();
        ImGui::Text("Text Size:");
        ImGui::SliderFloat("Text Size", &g_Options.dropped_weapon_text_size, 5.f, 30.f);
        ImGui::SliderFloat("Icon Size", &g_Options.dropped_weapon_icon_size, 5.f, 30.f);
        ImGui::SliderFloat("Ammo Size", &g_Options.dropped_weapon_ammo_size, 5.f, 30.f);

        ImGui::Spacing();
        ImGui::Text("Colors:");
        ImGui::ColorEdit3("Color Box", g_Options.color_dropped_weapon_box);
        ImGui::ColorEdit3("Color Text", g_Options.color_dropped_weapon_text);
        ImGui::ColorEdit3("Color Icon", g_Options.color_dropped_weapon_icon);
        ImGui::ColorEdit3("Color Ammo", g_Options.color_dropped_weapon_ammo);
    }
    ImGui::EndGroupBox();
}

void RenderEspTab()
{
    static char* esp_tab_names[] = { "ESP", "GLOW", "CHAMS" };
    static int   active_esp_tab = 0;
    static int esp_target_num = 0;
    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    {
        render_tabs(esp_tab_names, active_esp_tab, group_w / _countof(esp_tab_names), 25.0f, true);
    }
    ImGui::PopStyleVar();
    ImGui::BeginGroupBox("##body_content");
    {
        if (active_esp_tab == 0) 
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
            ImGui::Combo("Esp Target##----", &esp_target_num, "Enemy\0\rTeam\0\rLocal\0\0");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("Enable", &g_Options.esp_settings[esp_target_num].enable);
            if (esp_target_num != 2)
            {
                ImGui::Checkbox("Only Visible", &g_Options.esp_settings[esp_target_num].only_visible);
            }
            ImGui::Checkbox("Dormant", &g_Options.esp_settings[esp_target_num].dormant);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::PushItemWidth(200);
            if (ImGui::BeginCombo("Esp", "Select"))
            {
                ImGui::Spacing();
                if (esp_target_num != 2)
                {
#pragma region  sound_arrows
                    ImGui::Checkbox("Sound Arrows", &g_Options.esp_settings[esp_target_num].sound_arrows);
                    ImGui::SameLine(120.f);
                    if (ImGui::Button("+##17", ImVec2(15, 15)))
                        ImGui::OpenPopup("##Sound Arrows_settings");

                    if (ImGui::BeginPopup("##Sound Arrows_settings")) {
                        ImGui::PushItemWidth(200.f);
                        ImGui::Spacing();
                        ImGui::Checkbox("Only Visible", &g_Options.esp_settings[esp_target_num].sound_arrows_only_visible);
                        ImGui::Text("Color");
                        ImGui::SameLine(180.f);
                        ImGui::ColorEdit3("Color Sound Arrows", g_Options.esp_settings[esp_target_num].sound_arrow_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Combo("Style##Sound Arrows_style", &g_Options.esp_settings[esp_target_num].sound_arrows_style, "Arc\0\rArrow\0\0");
                        ImGui::Spacing();
                        ImGui::PopItemWidth();
                        ImGui::EndPopup();
                    }
#pragma endregion
                }

#pragma region  esp_box
                ImGui::Checkbox("Box", &g_Options.esp_settings[esp_target_num].esp_box);
                ImGui::SameLine(120.f);
                if (ImGui::Button( "+##1", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_box_settings");

                ImGui::PushItemWidth(200.f);
                if (ImGui::BeginPopup("##esp_box_settings")) {
                    ImGui::PushItemWidth(180.f);

                    ImGui::Spacing();
                    ImGui::Text("Visible");
                    ImGui::SameLine(210.f);
                    ImGui::ColorEdit3("Color Box Visible", g_Options.esp_settings[esp_target_num].box_clr_vis, color_edit_flags);

                    if (esp_target_num != 2)
                    {
                        ImGui::Text("Invisible");
                        ImGui::SameLine(210.f);
                        ImGui::ColorEdit3("Color Box inVisible", g_Options.esp_settings[esp_target_num].box_clr_invis, color_edit_flags);
                    }

                    ImGui::Checkbox("Fill", &g_Options.esp_settings[esp_target_num].filled_box);
                    ImGui::Checkbox("Gradient##esp_box_filled_fade", &g_Options.esp_settings[esp_target_num].gradient_fade);

                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
                ImGui::PopItemWidth();
#pragma endregion
#pragma region esp_name
                  ImGui::Checkbox("Name", &g_Options.esp_settings[esp_target_num].esp_names);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##2", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_name_settings");

                if (ImGui::BeginPopup("##esp_name_settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::SliderFloat("Font size##name", &g_Options.esp_settings[esp_target_num].esp_names_size, 5.0f, 30.0f, "%.2f"); ImGui::SameLine(); ImGui::Text(" ");
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color Name", g_Options.esp_settings[esp_target_num].names_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion
#pragma region weapon_icon
                  ImGui::Checkbox("Weapon Icon", &g_Options.esp_settings[esp_target_num].esp_weapon_ico);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##3", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_weapon_i_settings");

                if (ImGui::BeginPopup("##esp_weapon_i_settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::SliderFloat("Font size##w_i", &g_Options.esp_settings[esp_target_num].esp_weapon_ico_size, 5.0f, 30.0f, "%.2f"); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color Weapon##2323232", g_Options.esp_settings[esp_target_num].weapon_ico_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion
#pragma region weapon_text
                  ImGui::Checkbox("Weapon Text", &g_Options.esp_settings[esp_target_num].esp_weapon);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##4", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_weapon__settings");

                if (ImGui::BeginPopup("##esp_weapon__settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::SliderFloat("Font size##w_", &g_Options.esp_settings[esp_target_num].esp_weapon_size, 5.0f, 30.0f, "%.2f"); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color Weapon##wewe", g_Options.esp_settings[esp_target_num].weapon_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion
#pragma region ammo_text
                  ImGui::Checkbox("Ammo Text", &g_Options.esp_settings[esp_target_num].esp_ammo_text);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##5", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_ammo__settings");

                if (ImGui::BeginPopup("##esp_ammo__settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::SliderFloat("Font size##ammo_", &g_Options.esp_settings[esp_target_num].esp_ammo_size, 5.0f, 30.0f, "%.2f"); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color Ammo##retrtertertr", g_Options.esp_settings[esp_target_num].ammo_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion
#pragma region health_text
                  ImGui::Checkbox("Health Text", &g_Options.esp_settings[esp_target_num].esp_health_text);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##6", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_health__settings");

                if (ImGui::BeginPopup("##esp_health__settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::SliderFloat("Font size##health_", &g_Options.esp_settings[esp_target_num].esp_health_size, 5.0f, 30.0f, "%.2f"); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color health##retrtertertr", g_Options.esp_settings[esp_target_num].health_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion
#pragma region money_text
                  ImGui::Checkbox("Money Text", &g_Options.esp_settings[esp_target_num].esp_money);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##7", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_money__settings");

                if (ImGui::BeginPopup("##esp_money__settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::SliderFloat("Font size##money_", &g_Options.esp_settings[esp_target_num].esp_money_size, 5.0f, 30.0f, "%.2f"); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color money##retrtertertr", g_Options.esp_settings[esp_target_num].money_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion
#pragma region flashed_text
                  ImGui::Checkbox("Flashed Text", &g_Options.esp_settings[esp_target_num].esp_flashed);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##8", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_flashed__settings");

                if (ImGui::BeginPopup("##esp_flashed__settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::SliderFloat("Font size##flashed_", &g_Options.esp_settings[esp_target_num].esp_flashed_size, 5.0f, 30.0f, "%.2f"); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color flashed##retrtertertr", g_Options.esp_settings[esp_target_num].flashed_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion
#pragma region reloading_text
                  ImGui::Checkbox("Reloading Text", &g_Options.esp_settings[esp_target_num].esp_reloading);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##9", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_reloading__settings");

                if (ImGui::BeginPopup("##esp_reloading__settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::SliderFloat("Font size##reloading_", &g_Options.esp_settings[esp_target_num].esp_reloading_size, 5.0f, 30.0f, "%.2f"); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color reloading##retrtertertr", g_Options.esp_settings[esp_target_num].reloading_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion
#pragma region scoped_text
                  ImGui::Checkbox("Scoped Text", &g_Options.esp_settings[esp_target_num].esp_scoped);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##10", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_scoped__settings");

                if (ImGui::BeginPopup("##esp_scoped__settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::SliderFloat("Font size##scoped_", &g_Options.esp_settings[esp_target_num].esp_scoped_size, 5.0f, 30.0f, "%.2f"); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color scoped##retrtertertr", g_Options.esp_settings[esp_target_num].scoped_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion
#pragma region armor_text
                  ImGui::Checkbox("Armor Text", &g_Options.esp_settings[esp_target_num].esp_armor);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##11", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_armor__settings");

                if (ImGui::BeginPopup("##esp_armor__settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::SliderFloat("Font size##armor_", &g_Options.esp_settings[esp_target_num].esp_armor_size, 5.0f, 30.0f, "%.2f"); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color armor##retrtertertr", g_Options.esp_settings[esp_target_num].armor_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion
#pragma region defuser_text
                 ImGui::Checkbox("Defuser Text", &g_Options.esp_settings[esp_target_num].esp_defuser);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##12", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_defuser__settings");

                if (ImGui::BeginPopup("##esp_defuser__settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                    ImGui::SliderFloat("Font size##defuser_", &g_Options.esp_settings[esp_target_num].esp_defuser_size, 5.0f, 30.0f, "%.2f"); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color defuser##retrtertertr", g_Options.esp_settings[esp_target_num].defuser_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion
#pragma region Skeleton
                ImGui::Checkbox("Skeleton", &g_Options.esp_settings[esp_target_num].skeleton);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##13", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_skeleton_settings");

                if (ImGui::BeginPopup("##esp_skeleton_settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color Skeleton", g_Options.esp_settings[esp_target_num].skeleton_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
#pragma endregion

                ImGui::Checkbox("Health Bar", &g_Options.esp_settings[esp_target_num].esp_health);

                ImGui::Checkbox("Sound Esp##esp_sound_enable", &g_Options.esp_settings[esp_target_num].sound_esp);
                ImGui::SameLine(120.f);
                if (ImGui::Button("+##14", ImVec2(15, 15)))
                    ImGui::OpenPopup("##esp_sound_settings");

                if (ImGui::BeginPopup("##esp_sound_settings")) {
                    ImGui::PushItemWidth(200.f);
                    ImGui::Spacing();
                     ImGui::Text("Color");
                    ImGui::SameLine(180.f);
                    ImGui::ColorEdit3("Color Sound Esp", g_Options.esp_settings[esp_target_num].sound_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                    ImGui::Spacing();
                    ImGui::PopItemWidth();
                    ImGui::EndPopup();
                }
                ImGui::EndCombo();
            }

            ImGui::Checkbox("Out Of Fov", &g_Options.esp_settings[esp_target_num].oof);
            ImGui::SameLine(220.f);
            if (ImGui::Button("+##15", ImVec2(15, 15)))
                ImGui::OpenPopup("##oof_settings");

            if (ImGui::BeginPopup("##oof_settings")) {
                ImGui::PushItemWidth(200.f);
                ImGui::Spacing();
                 ImGui::Text("Color");
                ImGui::SameLine(180.f);
                ImGui::ColorEdit3("Color OOF", g_Options.esp_settings[esp_target_num].oof_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Combo("Style##oof_style", &g_Options.esp_settings[esp_target_num].oof_style, "Arc\0\rArrow\0\0");
                ImGui::Spacing();
                ImGui::PopItemWidth();
                ImGui::EndPopup();
            }

            ImGui::Checkbox("Hit Arrow", &g_Options.hit_arrow);
            ImGui::SameLine(220.f);
            if (ImGui::Button("+##16", ImVec2(15, 15)))
                ImGui::OpenPopup("##Hit_settings");

            if (ImGui::BeginPopup("##Hit_settings")) {
                ImGui::PushItemWidth(200.f);
                ImGui::Spacing();
                ImGui::Checkbox("Only Visible", &g_Options.hit_arrow_only_visible);
                ImGui::Text("Color");
                ImGui::SameLine(180.f);
                ImGui::ColorEdit3("Arrow Color", g_Options.hit_arrow_clr, color_edit_flags); ImGui::SameLine(); ImGui::Text(" ");
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Combo("Style##arrow_style", &g_Options.hit_arrow_style, "Arc\0\rArrow\0\0");
                ImGui::Spacing();
                ImGui::PopItemWidth();
                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();
        }
        else if (active_esp_tab == 1) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
            ImGui::Columns(3, nullptr, false);
            ImGui::SetColumnOffset(1, group_w / 3.0f);
            ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
            ImGui::SetColumnOffset(3, group_w);

            if (!g_Options.safe_mode)
            {
                ImGui::Checkbox("Enabled", &g_Options.glow_enabled);
            }
            ImGui::Checkbox("Team check", &g_Options.glow_enemies_only);
            ImGui::Checkbox("Players", &g_Options.glow_players);
            ImGui::Checkbox("Chickens", &g_Options.glow_chickens);
            ImGui::Checkbox("C4 Carrier", &g_Options.glow_c4_carrier);
            ImGui::Checkbox("Planted C4", &g_Options.glow_planted_c4);
            ImGui::Checkbox("Defuse Kits", &g_Options.glow_defuse_kits);
            ImGui::Checkbox("Weapons", &g_Options.glow_weapons);

            ImGui::NextColumn();

            ImGui::PushItemWidth(100);
            ImGuiEx::ColorEdit3("Ally", &g_Options.color_glow_ally);
            ImGuiEx::ColorEdit3("Enemy", &g_Options.color_glow_enemy);
            ImGuiEx::ColorEdit3("Chickens", &g_Options.color_glow_chickens);
            ImGuiEx::ColorEdit3("C4 Carrier", &g_Options.color_glow_c4_carrier);
            ImGuiEx::ColorEdit3("Planted C4", &g_Options.color_glow_planted_c4);
            ImGuiEx::ColorEdit3("Defuse Kits", &g_Options.color_glow_defuse);
            ImGuiEx::ColorEdit3("Weapons", &g_Options.color_glow_weapons);
            ImGui::PopItemWidth();

            ImGui::NextColumn();

            ImGui::Columns(1, nullptr, false);
            ImGui::PopStyleVar();
        }
        else if (active_esp_tab == 2) 
{
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
            
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnOffset(1, group_w / 2.0f);
            ImGui::SetColumnOffset(2, 2 * group_w / 2.0f);


            ImGui::BeginGroupBox("Players");
            {
                static int32_t iChamsGroup = 0;
               
                static const char* Targets[] =
                {
                    "Enemy Visible",
                    "Enemy Invisble",
                    "Team Visible",
                    "Team Invisible",
                    "Local",
                    "Arms",
                    "Weapon"
                };

                ImGui::Combo("Target", &iChamsGroup,  Targets, IM_ARRAYSIZE(Targets));
               
                ImGui::Checkbox("Enable Chams", &g_Options.m_aChamsSettings[iChamsGroup].m_bRenderChams);
                ImGuiEx::ColorEdit4("##qweqwe", &g_Options.m_aChamsSettings[iChamsGroup].m_Color);

                ImGui::Combo("Material", &g_Options.m_aChamsSettings[iChamsGroup].m_iMainMaterial, g_Chams->names);

                ImGui::Checkbox("Enable Glow", &g_Options.m_aChamsSettings[iChamsGroup].m_aModifiers[0]);
                ImGuiEx::ColorEdit4("##512414 color", &g_Options.m_aChamsSettings[iChamsGroup].m_aModifiersColors[0]);

                ImGui::Checkbox("Enable Ghost", &g_Options.m_aChamsSettings[iChamsGroup].m_aModifiers[1]);
                ImGuiEx::ColorEdit4("##235235 color", &g_Options.m_aChamsSettings[iChamsGroup].m_aModifiersColors[1]);

                g_DrawModel->SetChamsSettings(g_Options.m_aChamsSettings[iChamsGroup]);
            }
            ImGui::EndGroupBox();
            ImGui::NextColumn();
            ImGui::BeginGroupBox("MaterialGenerator");
            {
                static char material_name[64];

                static bool model = false;
                static bool flat = false;
                static bool nocull = false;
                static bool halflambert = false;
                static bool nofog = false;
                static bool ignorez = false;
                static bool znearer = false;
                static bool wireframe = false;
                static bool selfillum = false;
                static std::string envmap = " ";
                static  std::string base_texture = " ";
                static float pearlescent = 0.f;
                static float phong = 0.f;
                static float phongexponent = 0.f;
                static float phongboost = 0.f;
                static float rimlight = 0.f;
                static float rimlightexponent = 0.f;
                static float rimlightboost = 0.f;
                static float additive = 0.f;
                static float alpha = 0.f;
                static float envmapfresnel = 0.f;

                ImGui::InputText("Material Name", material_name, 64);
                ImGui::Checkbox("model", &model);
                ImGui::Checkbox("flat", &flat);
                ImGui::Checkbox("nocull", &nocull);
                ImGui::Checkbox("halflambert", &halflambert);
                ImGui::Checkbox("nofog", &nofog);
                ImGui::Checkbox("ignorez", &ignorez);
                ImGui::Checkbox("znearer", &znearer);
                ImGui::Checkbox("wireframe", &wireframe);
                ImGui::Checkbox("selfillum", &selfillum);
                ImGui::SliderFloat("pearlescent", &pearlescent, 0, 1);
                ImGui::SliderFloat("phong", &phong, 0, 1);
                ImGui::SliderFloat("phongexponent", &phongexponent, 0, 1);
                ImGui::SliderFloat("phongboost", &phongboost, 0, 1);
                ImGui::SliderFloat("rimlight", &rimlight, 0, 1);
                ImGui::SliderFloat("rimlightexponent", &rimlightexponent, 0, 1);
                ImGui::SliderFloat("additive", &additive, 0, 1);
                ImGui::SliderFloat("alpha", &alpha, 0, 1);
                ImGui::SliderFloat("envmapfresnel", &envmapfresnel, 0, 1);

                g_Chams->generated_material->FindVar("$model", nullptr, false)->SetValue(Menu::Get().bool_to_int(model));
                g_Chams->generated_material->FindVar("$flat", nullptr, false)->SetValue(Menu::Get().bool_to_int(flat));
                g_Chams->generated_material->FindVar("$nocull", nullptr, false)->SetValue(Menu::Get().bool_to_int(nocull));
                g_Chams->generated_material->FindVar("$halflambert", nullptr, false)->SetValue(Menu::Get().bool_to_int(halflambert));
                g_Chams->generated_material->FindVar("$nofog", nullptr, false)->SetValue(Menu::Get().bool_to_int(nofog));
                g_Chams->generated_material->FindVar("$ignorez", nullptr, false)->SetValue(Menu::Get().bool_to_int(ignorez));
                g_Chams->generated_material->FindVar("$znearer", nullptr, false)->SetValue(Menu::Get().bool_to_int(znearer));
                g_Chams->generated_material->FindVar("$wireframe", nullptr, false)->SetValue(Menu::Get().bool_to_int(wireframe));
                g_Chams->generated_material->FindVar("$selfillum", nullptr, false)->SetValue(Menu::Get().bool_to_int(selfillum));
                g_Chams->generated_material->FindVar("$pearlescent", nullptr, false)->SetValue(pearlescent);
                g_Chams->generated_material->FindVar("$phong", nullptr, false)->SetValue(phong);
                g_Chams->generated_material->FindVar("$phongexponent", nullptr, false)->SetValue(phongexponent);
                g_Chams->generated_material->FindVar("$phongboost", nullptr, false)->SetValue(phongboost);
                g_Chams->generated_material->FindVar("$rimlight", nullptr, false)->SetValue(rimlight);
                g_Chams->generated_material->FindVar("$rimlightexponent", nullptr, false)->SetValue(rimlightexponent);
                g_Chams->generated_material->FindVar("$additive", nullptr, false)->SetValue(additive);
                g_Chams->generated_material->FindVar("$alpha", nullptr, false)->SetValue(alpha);
                g_Chams->generated_material->FindVar("$envmapfresnel", nullptr, false)->SetValue(envmapfresnel);

                if (ImGui::Button("Add Material"))
                {
                    g_Chams->AddOwnMaterial(g_Chams->generated_material, material_name);  
                }
            }
            ImGui::EndGroupBox();
            ImGui::Columns(1, nullptr, false);
            ImGui::PopStyleVar();
        }
    }
    ImGui::EndGroupBox();
}

void RenderMiscTab()
{
    auto& style = ImGui::GetStyle();

    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
        ImGui::Combo("Hit Sound", &g_Options.misc_hitsound, "Disabled\0Metallic\0Cod\0Bubble\0Stapler\0Bell\0Flick\0Custom\0\0");
        ImGui::InputText("Custom Path", g_Options.custom_hit_sound, 128);
        ImGui::Spacing();
        ImGui::Checkbox("Anti Afk Kick", &g_Options.misc_antiafk_kick);
        ImGui::Spacing();
        ImGui::Checkbox("Spectator List", &g_Options.misc_spectator_list);
        ImGui::Checkbox("Bomb Timer", &g_Options.bomb_timer);
        ImGui::Checkbox("Watermark", &g_Options.misc_watermark);
        ImGui::Checkbox("Binds List", &g_Options.misc_binds_list);
        ImGui::Spacing();
        ImGui::Checkbox("Bunny hop", &g_Options.misc_bhop);
        ImGui::SliderInt("Chance", &g_Options.bhop_chance, 0.f, 100.f);
        ImGui::SliderInt("Min hops", &g_Options.max_bhop_limit_min, 0.f, 15.f);
        ImGui::SliderInt("Max hops", &g_Options.max_bhop_limit_max, 0.f, 15.f);
        ImGui::Combo("Strafe", &g_Options.strafe_type, "Disabled\0Legit\0Directional\0\0");
        ImGui::Checkbox("Edge Jump", &g_Options.edge_jump);
        ImGui::Keybind("e_j bind", g_Options.edge_jump_key, g_Options.edge_jump_mode);

        ImGui::Spacing(); 
        ImGui::Checkbox("Remove Scope", &g_Options.remove_scope);
        ImGui::Checkbox("Remove Shadows", &g_Options.remove_shadows);
        ImGui::Checkbox("Remove Post Processing", &g_Options.misc_no_post_processing);

        ImGui::Checkbox("Remove Flash", &g_Options.misc_no_flash);
        if (!g_Options.safe_mode)
        {
            ImGui::Checkbox("Remove Smoke", &g_Options.misc_no_smoke);
         
        }
        ImGui::Checkbox("Remove Panorama Blur", &g_Options.remove_panorama_blur);
        ImGui::Spacing();


        if (!g_Options.safe_mode)
        {
            ImGui::Checkbox("Auto Accept", &g_Options.auto_accept);
            ImGui::Checkbox("Rank reveal", &g_Options.misc_showranks);
        }
        ImGui::Checkbox("Sniper Crosshair", &g_Options.misc_force_crosshair);

        ImGui::Spacing();
        ImGui::Checkbox("Fov Changer", &g_Options.fov_changer);
        ImGui::SliderFloat("Camera", &g_Options.camera_fov, 90, 140);

        ImGui::Spacing();
        ImGui::Checkbox("Viewmodel Offsets", &g_Options.view_offsets);
        ImGui::SliderFloat("X", &g_Options.viewmodel_x, -20, 20);
        ImGui::SliderFloat("Y", &g_Options.viewmodel_y, -20, 20);
        ImGui::SliderFloat("Z", &g_Options.viewmodel_z, -20, 20);
        ImGui::SliderFloat("Roll", &g_Options.viewmodel_roll, -180, 180);

        ImGui::Spacing();
        ImGui::Checkbox("Aspect Ratio", &g_Options.do_saspect_ratio);
        ImGui::SliderFloat("Value", &g_Options.aspect_ratio_value, 0.1f, 5.f);

        ImGui::Spacing();
        ImGui::Checkbox("Third Person", &g_Options.misc_thirdperson);
        ImGui::Keybind("th_p bind", g_Options.third_person_key, g_Options.third_person_mode);
        ImGui::SliderFloat("Distance", &g_Options.misc_thirdperson_dist, 0.f, 150.f);

        static const char* aSkyboxList[] =
        {
            "None",
            "Tibet",
            "Baggage",
            "Italy",
            "Aztec",
            "Vertigo",
            "Daylight",
            "Daylight 2",
            "Clouds",
            "Clouds 2",
            "Gray",
            "Clear",
            "Canals",
            "Cobblestone",
            "Assault",
            "Clouds dark",
            "Night",
            "Night 2",
            "Night flat",
            "Dusty",
            "Rainy"
        };
        ImGui::Spacing();
        ImGui::Combo("Skybox", &g_Options.skybox_num, aSkyboxList, IM_ARRAYSIZE(aSkyboxList));
        ImGui::Spacing();

        ImGui::Checkbox("Hit Effect", &g_Options.misc_hiteffect);
        ImGui::SliderFloat("Duration", &g_Options.misc_hiteffect_duration, 0.01f, 5.f);

        ImGui::Spacing();
        ImGui::Checkbox("World Color", &g_Options.misc_world_color);
        ImGui::ColorEdit3("Color World", g_Options.color_walls);
        ImGui::ColorEdit3("Color Static Props", g_Options.color_props);
        ImGui::ColorEdit3("Color Sky", g_Options.color_sky);

        ImGui::Spacing();
        ImGui::Checkbox("Post Processing", &g_Options.misc_post_processing);
        ImGui::ColorEdit3("Color post_pr", g_Options.color_post_processing);

        ImGui::Spacing();
        ImGui::Checkbox("Custom Sunset", &g_Options.custom_shadows);
        ImGui::SliderFloat("X##shadows", &g_Options.shadows_x, -100, 100);
        ImGui::SliderFloat("Y##shadows", &g_Options.shadows_y, -100, 100);
        ImGui::SliderFloat("Z##shadows", &g_Options.shadows_z, -100, 100);

        ImGui::Spacing();
        ImGui::Checkbox("World Hit Marker", &g_Options.misc_hit_marker_1);
        ImGui::ColorEdit3("Whm Color", g_Options.color_hit_marker_1);

        ImGui::Spacing();
        ImGui::Checkbox("Screen Hit Marker", &g_Options.misc_hit_marker_2);
        ImGui::ColorEdit3("Shm Color", g_Options.color_hit_marker_2);

        ImGui::Spacing();
        ImGui::Checkbox("Damage Indicator", &g_Options.misc_damage_indicator);
        ImGui::ColorEdit3("Color Damage Indicator", g_Options.color_damage_indicator);
        ImGui::SliderFloat("Speed", &g_Options.damage_indicator_speed, 0.0f, 5.0f, "%.2f");
        ImGui::SliderFloat("Font size", &g_Options.damage_indicator_size, 10.0f, 25.0f, "%.2f");

        ImGui::Spacing();
        ImGui::Checkbox("Fog", &g_Options.fog_enable);
        ImGui::ColorEdit3("Color Fog", g_Options.fog_color);
        ImGui::SliderFloat("End Dist.", &g_Options.fog_start, 0.0f, 3000.0f, "%.2f");
        ImGui::SliderFloat("Start Dist.", &g_Options.fog_end, 0.0f, 3000.0f, "%.2f");
        ImGui::SliderFloat("Density", &g_Options.fog_density, 0.0f, 1.0f, "%.2f");

        if (ImGui::Button("Unload", ImVec2{ 100, 25 })) {
            g_Unload = true;
        }
      
        ImGui::PopStyleVar();
    }
    ImGui::EndGroupBox();
}

void RenderEmptyTab()
{
    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::Checkbox("Enable", &g_Options.legit_enable);
        ImGui::Combo("Weapon", &Utils::cur_weapon, legitbot::weapon_data, IM_ARRAYSIZE(legitbot::weapon_data));
        ImGui::Checkbox("Resize FOV When Zoom", &g_Options.legit_resize_fov);
        ImGui::Checkbox("Only In Scope", &g_Options.legit_onlyscope);
        ImGui::Spacing();
        ImGui::SliderFloat("Fov", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_fov, 0.f, g_Options.safe_mode ? 4.f : 20.f, "%.2f");
        ImGui::Checkbox("Dynamic FOV", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_dynamic_fov);
        ImGui::SliderFloat("Smooth", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_smooth, g_Options.safe_mode ? 1.f : 0.f, 40.f, "%.2f");
        ImGui::Spacing();
        ImGui::Checkbox("Draw Fov", &g_Options.visuals_draw_fov);
        ImGui::Combo("Style", &g_Options.fov_draw_type, "Circle\0Filled\0\0");
        ImGui::Spacing();
        if (!g_Options.safe_mode)
        {
            ImGui::Checkbox("Legit Checks", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_checks);
            ImGui::Spacing();
        }
        ImGui::SliderFloat("Kill Delay", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_delay_after_kill, 0.f, 50.f, "%.2f");
        ImGui::SliderFloat("Shot Delay", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_delay_shot, 0.f, 50.f, "%.2f");
        ImGui::SliderFloat("RCS FOV", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_fov_after, 0.f, 20.f, g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_fov_after > 0 ? "%.2f" : "Off");
        ImGui::SliderFloat("RCS Smooth", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_smooth_after, 0.f, 40.f, g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_smooth_after > 0 ? "%.2f" : "Off");
        ImGui::SliderInt("RCS X", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_rcs_x, 0, 100);
        ImGui::SliderInt("RCS Y", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_rcs_y, 0, 100);
        ImGui::Spacing();
        ImGui::Checkbox("Nearest", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_nearest);

        if (!g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_nearest)
        {
            const std::vector log_type{ "Head", "Body", "Pelvis" };
            if (ImGui::BeginCombo("Hitboxes", "Select"))
            {
                for (int i = 0; i < log_type.size(); i++)
                    ImGui::Checkbox(log_type[i], &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_hitbox[i]);

                ImGui::EndCombo();
            }
        }
        if (!g_Options.safe_mode)
        {
            ImGui::Checkbox("Auto Wall", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_autowall);
            ImGui::SliderFloat("Min Damage", &g_Options.legitbot_for_weapon[Utils::cur_weapon].legit_mindamage, 0.f, 100.f, "%.2f");
        }
    }
    ImGui::EndGroupBox();
}

void RenderConfigTab()
{
    static char cfg_name[64];
    static std::string showing_cfg_name = "";
    g_Config->config_files();
    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::Text("Selected config: "); ImGui::SameLine(); ImGui::Text(showing_cfg_name.c_str());
        
        ImGui::BeginChild("Config List", { 300.f, 190.f }, g_Config->files.size() > 10 ? false : true); {
            ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); {
                for (size_t i = 0; i < g_Config->files.size(); i++) {
                    if (ImGui::Button(g_Config->files[i].c_str(), { 300.f, 20.f }))
                    {
                        g_cheat.cur_cfg_idx = i;
                        showing_cfg_name = g_Config->files[i];
                    }
                }
            }
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        ImGui::InputText("Config Name", cfg_name, 64);
        if (ImGui::Button("Save", ImVec2(150, 20))) 
        {
            if (cfg_name != "")
            {
                g_Config->save(cfg_name);
                g_cheat.cfg_name = cfg_name;
                ZeroMemory(&cfg_name, sizeof(cfg_name));
            }
        }
        ImGui::Spacing();

        if (ImGui::Button("Load", ImVec2(150, 20))) 
        {
            g_Config->load(g_Config->files.at(g_cheat.cur_cfg_idx));
            g_cheat.cfg_name = g_Config->files.at(g_cheat.cur_cfg_idx);
        }
        if (ImGui::Button("Remove", ImVec2(150, 20)))
        {
            g_Config->remove(g_Config->files.at(g_cheat.cur_cfg_idx));
        }
        if (ImGui::Button("Open CFG Folder", ImVec2(150, 20))) {
            CreateDirectory(L"C:/umbrella_csgo/configs/", NULL);
            ShellExecute(NULL, L"open", L"C:/umbrella_csgo/configs/", NULL, NULL, SW_SHOWNORMAL);
        }
    }
    ImGui::EndGroupBox();
}

void Menu::Initialize()
{
    CreateStyle();

    Utils::toggled_keys[0x2D] = true;
}

void Menu::Shutdown()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Menu::OnDeviceLost()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
    ImGui_ImplDX9_CreateDeviceObjects();
}

template<typename T> static inline T Lerp(T a, T b, float t) { return (T)(a + (b - a) * t); }

void Menu::Render()
{
    if (!IsVisible())
        return;

    if (g_Options.safe_mode)
    {
        if (g_Options.buy_bot_first_buy_delay < 100)
            g_Options.buy_bot_first_buy_delay = 100;

        if (g_Options.buy_bot_active_buy_delay < 200)
            g_Options.buy_bot_active_buy_delay = 200;

        if (g_Options.bhop_chance > 50)
            g_Options.bhop_chance = 50;

        if (g_Options.max_bhop_limit_max > 3 || g_Options.max_bhop_limit_max == 0)
            g_Options.max_bhop_limit_max = 3;

        if (g_Options.strafe_type == 2)
            g_Options.strafe_type = 1;

        if (g_Options.glow_enabled)
            g_Options.glow_enabled = false;

        for (int i = 0; i < 34; i++)
        {
            if (g_Options.legitbot_for_weapon[i].legit_fov > 4.f)
                g_Options.legitbot_for_weapon[i].legit_fov = 4.f;

            if (g_Options.legitbot_for_weapon[i].legit_smooth < 1.f)
                g_Options.legitbot_for_weapon[i].legit_smooth = 1.f;

            if (!g_Options.legitbot_for_weapon[i].legit_checks)
                g_Options.legitbot_for_weapon[i].legit_checks = true;

            if (g_Options.legitbot_for_weapon[i].legit_autowall)
                g_Options.legitbot_for_weapon[i].legit_autowall = false;
        }

        for (int i = 0; i < 34; i++)
        {
            if (g_Options.trigger_for_weapon[i].trigger_chance > 50.f)
                g_Options.trigger_for_weapon[i].trigger_chance = 50.f;

            if (!g_Options.trigger_for_weapon[i].legit_checks)
                g_Options.trigger_for_weapon[i].legit_checks = true;
        }
    }

    ImVec2 menu_pos;
    const auto sidebar_size = get_sidebar_size();
    static int active_sidebar_tab = 0;

    ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiSetCond_Once);
    ImGui::SetNextWindowSize(ImVec2{ 650, 400 }, ImGuiSetCond_Once);

    if (ImGui::Begin("UMBRELLA", &Utils::toggled_keys[0x2D], ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {

        menu_pos = ImGui::GetCurrentWindow()->Pos;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        {
            ImGui::BeginGroupBox("##sidebar", sidebar_size);
            {
                render_tabs(sidebar_tabs, active_sidebar_tab, 100, 35, false);
                ImGui::Checkbox("CFG Auto Save", &g_cheat.config_autosave);
                ImGui::Checkbox("Safe Mode", &g_Options.safe_mode);
            }
            ImGui::EndGroupBox();
        }
        ImGui::PopStyleVar();
        ImGui::SameLine();

        auto size = ImVec2{ 0.0f, sidebar_size.y };

        ImGui::BeginGroupBox("##body", size);
        if (active_sidebar_tab == TAB_TRIGGER) {
            RenderTriggetBotTab();
        }
        if (active_sidebar_tab == TAB_ESP) {
            RenderEspTab();
        }
        else if (active_sidebar_tab == TAB_AIMBOT) {
            RenderEmptyTab();
        }
        else if (active_sidebar_tab == TAB_MISC) {
            RenderMiscTab();
        }
        else if (active_sidebar_tab == TAB_CONFIG) {
            RenderConfigTab();
        }
        else if (active_sidebar_tab == TAB_RADAR) {
            RenderRadarTab();
        }
        else if (active_sidebar_tab == TAB_GRENADES) {
            RenderGrenadesTab();
        }
        else if (active_sidebar_tab == TAB_WEAPON) {
            RenderWeaponTab();     
        }
        else if (active_sidebar_tab == TAB_BUYBOT) {
            RenderBuyBotTab();
        }
        ImGui::EndGroupBox();
        ImGui::End();
    }
    
    ImGui::SetNextWindowPos(ImVec2(menu_pos.x + 660, menu_pos.y));
    ImGui::SetNextWindowSize(ImVec2(300, 400));
    ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
    {
        const auto context{ *GImGui };
        auto mouse_pos = context.IO.MouseDelta;

        if (g_DrawModel->GetTexture())
        {
            ImGui::GetWindowDrawList()->AddImage(
                g_DrawModel->GetTexture()->pTextureHandles[0]->lpRawTexture,
                ImVec2(menu_pos.x + 660 , menu_pos.y),
                ImVec2(menu_pos.x + 660 + 300, menu_pos.y + 400),
                ImVec2(0, 0), ImVec2(1, 1),
                ImColor(1.0f, 1.0f, 1.0f, 1.0f));
        }

        ImGui::SetCursorPos(ImVec2(0,0));
        ImGui::InvisibleButton("preview", ImVec2(300, 400));

        static float_t rotation = 180.0f;

        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
            rotation += mouse_pos.x;

        g_cheat.m_flModelRotation = Lerp(g_cheat.m_flModelRotation, rotation, 8.0f * context.IO.DeltaTime);
    }
    ImGui::End();
    
}

void Menu::Toggle()
{
    Utils::toggled_keys[0x2D] = !Utils::toggled_keys[0x2D];
}

void Menu::CreateStyle()
{
    ImGui::StyleColorsDark();
    ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);
    _style.FrameRounding = 0.f;
    _style.WindowRounding = 0.f;
    _style.ChildRounding = 0.f;
    _style.Colors[ImGuiCol_Button] = ImVec4(0.260f, 0.590f, 0.980f, 0.670f);
    _style.Colors[ImGuiCol_Header] = ImVec4(0.260f, 0.590f, 0.980f, 0.670f);
    _style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.260f, 0.590f, 0.980f, 1.000f);
    _style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.30f, 1.0f);
    _style.Colors[ImGuiCol_WindowBg] = ImVec4(0.000f, 0.009f, 0.120f, 0.940f);
    _style.Colors[ImGuiCol_PopupBg] = ImVec4(0.076f, 0.143f, 0.209f, 1.000f);
    ImGui::GetStyle() = _style;
}
