#pragma once

#include "Recv.hpp"

class ClientClass;
class IClientNetworkable;

typedef IClientNetworkable* (*CreateClientClassFn)(int entnum, int serialNum);
typedef IClientNetworkable* (*CreateEventFn)();

class ClientClass
{
public:
    CreateClientClassFn      m_pCreateFn;
    CreateEventFn            m_pCreateEventFn;
    char*                    m_pNetworkName;
    RecvTable*               m_pRecvTable;
    ClientClass*             m_pNext;
    ClassId                  m_ClassID;
};

struct ServerClass
{
	const char* m_pNetworkName;
	void** m_pTable;
	ServerClass* m_pNext;
	int							m_ClassID;
	int							m_InstanceBaselineIndex;
};

class IServerGameDLL
{
public:
	virtual bool			ReplayInit(CreateInterfaceFn fnReplayFactory) = 0;
	virtual bool			GameInit(void) = 0;
	virtual bool			LevelInit(char const* pMapName, char const* pMapEntities, char const* pOldLevel, char const* pLandmarkName, bool loadGame, bool background) = 0;
	virtual void			ServerActivate(void* pEdictList, int edictCount, int clientMax) = 0;
	virtual void			GameFrame(bool simulating) = 0;
	virtual void			PreClientUpdate(bool simulating) = 0;
	virtual void			LevelShutdown(void) = 0;
	virtual void			GameShutdown(void) = 0;
	virtual void			DLLShutdown(void) = 0;
	virtual float			GetTickInterval(void) const = 0;
	virtual ServerClass* GetAllServerClasses(void) = 0;
	virtual const char* GetGameDescription(void) = 0;
	virtual void			CreateNetworkStringTables(void) = 0;
};