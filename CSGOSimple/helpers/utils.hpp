#pragma once

#define NOMINMAX
#include <Windows.h>
#include <string>
#include <initializer_list>
#include "../valve_sdk/sdk.hpp"
#include <array>
#include "math.hpp"

namespace Utils 
{
	std::vector<char> HexToBytes(const std::string& hex);
	std::string BytesToString(unsigned char* data, int len);
	std::vector<std::string> Split(const std::string& str, const char* delim);
	unsigned int FindInDataMap(datamap_t * pMap, const char * name);

    inline int rgb_to_int(const int red, const int green, const int blue) 
    {
        int c = red;
        c = (c << 8) | green;
        c = (c << 8) | blue;
        return c;
    }

    inline DWORD find_hud_element(const char* name) 
    {
        typedef DWORD(__thiscall* find_hud_element_t)(void*, const char*);
        const PDWORD _this = *reinterpret_cast<DWORD**>(g_cheat.element_this);
        const auto find_hud_element = reinterpret_cast<find_hud_element_t>(g_cheat.element_fn);
        const DWORD dw_buff = find_hud_element(_this, name);
        return dw_buff;
    }

    inline void modern_clamp_shadows(float& target, float to, float step)
    {
        if (target > to)
        {
            target = Math::clamp(target - step, to, 100.f);

        }
        else if (target < to)
        {
            target = Math::clamp(target + step, -100.f, to);

        }
    }

    inline void modern_clamp_world(float& target, float to, float step)
    {
        if (target > to)
        {
            target = Math::clamp(target - step, to, 1.f);

        }
        else if (target < to)
        {
            target = Math::clamp(target + step, 0.f, to);

        }
    }

    inline int cur_weapon = 0;
   
	void AttachConsole();
    void DetachConsole();
    bool ConsolePrint(const char* fmt, ...);
    char ConsoleReadKey();
    int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules);
    std::uint8_t* PatternScan(void* module, const char* signature);
    void SetClantag(const char* tag);
    void SetName(const char* name);

    constexpr uint64_t BASIS = 0x811c9dc5;
    constexpr uint64_t PRIME = 0x1000193;

    inline constexpr uint32_t get_const(const char* txt, const uint32_t value = BASIS) noexcept 
    {
        return (txt[0] == '\0') ? value :
            get_const(&txt[1], (value ^ uint32_t(txt[0])) * PRIME);
    }

    inline bool line_goes_through_smoke(Vector start, Vector end) 
    {
        static auto line_goes_through_smoke_fn = reinterpret_cast<bool(*)(Vector, Vector)>(g_cheat.get_line_goes_through_smoke);
        return line_goes_through_smoke_fn(start, end);
    }

    inline uint32_t get(const char* txt) 
    {
        uint32_t ret = BASIS;
        uint32_t length = strlen(txt);
        for (auto i = 0u; i < length; ++i) 
        {
            ret ^= txt[i];
            ret *= PRIME;
        }

        return ret;
    }

    inline std::array< bool, 256 > holded_keys = { };
    inline std::array< bool, 256 > toggled_keys = { };

    inline bool key_bind(int key, int select_mode)
    {
        if (key <= 0)
        {
            if (select_mode == 3)
                return true;

            return false;
        }

        switch (select_mode)
        {
        case 0:
            return toggled_keys[key];
        case 1:
            return holded_keys[key];
        case 2:
            return !holded_keys[key];
        case 3:
            return true;
        }

        return false;
    }
}

#define get_hash( str ) Utils::get( str )