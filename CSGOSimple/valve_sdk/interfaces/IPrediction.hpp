#pragma once

#include "../Math/QAngle.hpp"
#include "../Misc/CUserCmd.hpp"
#include "IMoveHelper.hpp"


#define get_virtual_fn( index, function_name, type, ... )  auto function_name { return CallVFunction<type>( this, index )( this, __VA_ARGS__ ); };
#define get_static_offset( type, func, offset ) type& func() { return *reinterpret_cast< type* >( reinterpret_cast< uintptr_t >( this ) + offset ); }


class CMoveData
{
public:
    bool    m_bFirstRunOfFunctions : 1;
    bool    m_bGameCodeMovedPlayer : 1;
    int     m_nPlayerHandle;        // edict index on server, client entity handle on client=
    int     m_nImpulseCommand;      // Impulse command issued.
    Vector  m_vecViewAngles;        // Command view angles (local space)
    Vector  m_vecAbsViewAngles;     // Command view angles (world space)
    int     m_nButtons;             // Attack buttons.
    int     m_nOldButtons;          // From host_client->oldbuttons;
    float   m_flForwardMove;
    float   m_flSideMove;
    float   m_flUpMove;
    float   m_flMaxSpeed;
    float   m_flClientMaxSpeed;
    Vector  m_vecVelocity;          // edict::velocity        // Current movement direction.
    Vector  m_vecAngles;            // edict::angles
    Vector  m_vecOldAngles;
    float   m_outStepHeight;        // how much you climbed this move
    Vector  m_outWishVel;           // This is where you tried 
    Vector  m_outJumpVel;           // This is your jump velocity
    Vector  m_vecConstraintCenter;
    float   m_flConstraintRadius;
    float   m_flConstraintWidth;
    float   m_flConstraintSpeedFactor;
    float   m_flUnknown[5];
    Vector  m_vecAbsOrigin;
};

class C_BasePlayer;

class IGameMovement
{
public:
    virtual			~IGameMovement(void) {}

    virtual void	          ProcessMovement(C_BasePlayer *pPlayer, CMoveData *pMove) = 0;
    virtual void	          Reset(void) = 0;
    virtual void	          StartTrackPredictionErrors(C_BasePlayer *pPlayer) = 0;
    virtual void	          FinishTrackPredictionErrors(C_BasePlayer *pPlayer) = 0;
    virtual void	          DiffPrint(char const *fmt, ...) = 0;
    virtual Vector const&	  GetPlayerMins(bool ducked) const = 0;
    virtual Vector const&	  GetPlayerMaxs(bool ducked) const = 0;
    virtual Vector const&   GetPlayerViewOffset(bool ducked) const = 0;
    virtual bool		        IsMovingPlayerStuck(void) const = 0;
    virtual C_BasePlayer*   GetMovingPlayer(void) const = 0;
    virtual void		        UnblockPusher(C_BasePlayer *pPlayer, C_BasePlayer *pPusher) = 0;
    virtual void            SetupMovementBounds(CMoveData *pMove) = 0;
};

class CGameMovement
    : public IGameMovement
{
public:
    virtual ~CGameMovement(void) {}
};

class IPrediction
{
public:
    bool InPrediction()
    {
        typedef bool(__thiscall* oInPrediction)(void*);
        return CallVFunction<oInPrediction>(this, 14)(this);
    }

    void RunCommand(C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *moveHelper)
    {
        typedef void(__thiscall* oRunCommand)(void*, C_BasePlayer*, CUserCmd*, IMoveHelper*);
        return CallVFunction<oRunCommand>(this, 19)(this, player, ucmd, moveHelper);
    }

    void SetupMove(C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *moveHelper, void* pMoveData)
    {
        typedef void(__thiscall* oSetupMove)(void*, C_BasePlayer*, CUserCmd*, IMoveHelper*, void*);
        return CallVFunction<oSetupMove>(this, 20)(this, player, ucmd, moveHelper, pMoveData);
    }

    void FinishMove(C_BasePlayer *player, CUserCmd *ucmd, void*pMoveData)
    {
        typedef void(__thiscall* oFinishMove)(void*, C_BasePlayer*, CUserCmd*, void*);
        return CallVFunction<oFinishMove>(this, 21)(this, player, ucmd, pMoveData);
    }

    void CheckMovingGround(C_BaseEntity* player, double frametime)
    {
        typedef void(__thiscall* oCheckMovingGround)(void*, C_BaseEntity*, double);
        return CallVFunction<oCheckMovingGround>(this, 18)(this, player, frametime);
    }
    get_virtual_fn(3, Update(int iServerTick, bool bIsValid, int iAcknowledged, int iOutgoingCmd), void(__thiscall*)(void*, int, bool, int, int), iServerTick, bIsValid, iAcknowledged, iOutgoingCmd);
};