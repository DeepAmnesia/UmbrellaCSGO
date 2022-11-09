#pragma once

#include "../Math/VMatrix.hpp"

class IPanel;
class C_BaseEntity;

enum class ClearFlags_t
{
    VIEW_CLEAR_COLOR = 0x1,
    VIEW_CLEAR_DEPTH = 0x2,
    VIEW_CLEAR_FULL_TARGET = 0x4,
    VIEW_NO_DRAW = 0x8,
    VIEW_CLEAR_OBEY_STENCIL = 0x10,
    VIEW_CLEAR_STENCIL = 0x20,
};


enum class MotionBlurMode_t
{
    MOTION_BLUR_DISABLE = 1,
    MOTION_BLUR_GAME = 2,
    MOTION_BLUR_SFM = 3
};

class CViewSetup
{
public:
    int			iX;
    int			iUnscaledX;
    int			iY;
    int			iUnscaledY;
    int			iWidth;
    int			iUnscaledWidth;
    int			iHeight;
    int			iUnscaledHeight;
    bool		bOrtho;
    std::byte	pad0[0x8F];
    float		flFOV;
    float		flViewModelFOV;
    Vector		vecOrigin;
    QAngle		angView;
    float		flNearZ;
    float		flFarZ;
    float		flNearViewmodelZ;
    float		flFarViewmodelZ;
    float		flAspectRatio;
    float		flNearBlurDepth;
    float		flNearFocusDepth;
    float		flFarFocusDepth;
    float		flFarBlurDepth;
    float		flNearBlurRadius;
    float		flFarBlurRadius;
    float		flDoFQuality;
    int			nMotionBlurMode;
    float		flShutterTime;
    Vector		vecShutterOpenPosition;
    QAngle		vecShutterOpenAngles;
    Vector		vecShutterClosePosition;
    QAngle		vecShutterCloseAngles;
    float		flOffCenterTop;
    float		flOffCenterBottom;
    float		flOffCenterLeft;
    float		flOffCenterRight;
    bool		bOffCenter;
    bool		bRenderToSubrectOfLargerScreen;
    bool		bDoBloomAndToneMapping;
    bool		bDoDepthOfField;
    bool		bHDRTarget;
    bool		bDrawWorldNormal;
    bool		bCullFontFaces;
    bool		bCacheFullSceneState;
    bool		bCSMView;
};

class IClientMode
{
public:
    virtual             ~IClientMode() {}
    virtual int         ClientModeCSNormal(void *) = 0;
    virtual void        Init() = 0;
    virtual void        InitViewport() = 0;
    virtual void        Shutdown() = 0;
    virtual void        Enable() = 0;
    virtual void        Disable() = 0;
    virtual void        Layout() = 0;
    virtual IPanel*     GetViewport() = 0;
    virtual void*       GetViewportAnimationController() = 0;
    virtual void        ProcessInput(bool bActive) = 0;
    virtual bool        ShouldDrawDetailObjects() = 0;
    virtual bool        ShouldDrawEntity(C_BaseEntity *pEnt) = 0;
    virtual bool        ShouldDrawLocalPlayer(C_BaseEntity *pPlayer) = 0;
    virtual bool        ShouldDrawParticles() = 0;
    virtual bool        ShouldDrawFog(void) = 0;
    virtual void        OverrideView(CViewSetup *pSetup) = 0;
    virtual int         KeyInput(int down, int keynum, const char *pszCurrentBinding) = 0;
    virtual void        StartMessageMode(int iMessageModeType) = 0;
    virtual IPanel*     GetMessagePanel() = 0;
    virtual void        OverrideMouseInput(float *x, float *y) = 0;
    virtual bool        CreateMove(float flInputSampleTime, void* usercmd) = 0;
    virtual void        LevelInit(const char *newmap) = 0;
    virtual void        LevelShutdown(void) = 0;
};