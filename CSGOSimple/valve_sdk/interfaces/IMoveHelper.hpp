#pragma once

class IClientEntity;

class IMoveHelper
{
public:
    virtual void UnknownVirtual() = 0;
public:
    virtual void SetHost(IClientEntity* host) = 0;
private:
    virtual void pad00() = 0;
    virtual void pad01() = 0;
public:
    virtual void process_impacts() = 0;
};
