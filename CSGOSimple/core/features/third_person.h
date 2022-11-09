#include "visuals.hpp"

class C_ThirdPerson
{
public:
	virtual void Instance();
private:
	float_t m_flThirdpersonDistance = 0.0f;
};

inline C_ThirdPerson* g_ThirdPerson = new C_ThirdPerson();