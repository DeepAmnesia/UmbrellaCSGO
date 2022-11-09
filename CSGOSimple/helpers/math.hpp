#pragma once

#include "../valve_sdk/sdk.hpp"
#include <corecrt_math_defines.h>
#include <DirectXMath.h>
#include <corecrt_math_defines.h>
#include <array>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)
#define M_PI 3.14159265358979323846
#define PI_F	((float)(M_PI)) 
#define M_RADPI 57.295779513082f

#define TIME_TO_TICKS( flTime ) ( ( int32_t )( 0.5f + ( float_t )( ( flTime ) ) /  g_GlobalVars->interval_per_tick ) )
#define TICKS_TO_TIME( iTick ) ( float_t )( ( iTick ) * g_GlobalVars->interval_per_tick )

static const float invtwopi = 0.1591549f;
static const float twopi = 6.283185f;
static const float threehalfpi = 4.7123889f;
static const float pi = 3.141593f;
static const float halfpi = 1.570796f;
static const __m128 signmask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

static const __declspec(align(16)) float null[4] = { 0.f, 0.f, 0.f, 0.f };
static const __declspec(align(16)) float _pi2[4] = { 1.5707963267948966192f, 1.5707963267948966192f, 1.5707963267948966192f, 1.5707963267948966192f };
static const __declspec(align(16)) float _pi[4] = { 3.141592653589793238f, 3.141592653589793238f, 3.141592653589793238f, 3.141592653589793238f };

typedef __declspec(align(16)) union
{
	float f[4];
	__m128 v;
} m128;

namespace Math
{
	__forceinline void AngleMatrix(const QAngle& angView, matrix3x4_t& matOutput, const Vector& vecOrigin)
	{
		float sp, sy, sr, cp, cy, cr;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angView.x));
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angView.y));
		DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angView.z));

		matOutput.SetForward(Vector(cp * cy, cp * sy, -sp));

		const float crcy = cr * cy;
		const float crsy = cr * sy;
		const float srcy = sr * cy;
		const float srsy = sr * sy;

		matOutput.SetLeft(Vector(sp * srcy - crsy, sp * srsy + crcy, sr * cp));
		matOutput.SetUp(Vector(sp * crcy + srsy, sp * crsy - srcy, cr * cp));
		matOutput.SetOrigin(vecOrigin);
	}
	__forceinline float RemapValClamped(float val, float A, float B, float C, float D)
	{
		if (A == B)
			return val >= B ? D : C;
		float cVal = (val - A) / (B - A);
		cVal = std::clamp(cVal, 0.0f, 1.0f);

		return C + (D - C) * cVal;
	}
	template <class T>
	inline T Lerp(float flPercent, T const& A, T const& B)
	{
		return A + (B - A) * flPercent;
	}
	__forceinline float DistanceToRay(const Vector& pos, const Vector& rayStart, const Vector& rayEnd, float* along = nullptr, Vector* pointOnRay = nullptr)
	{
		Vector to = pos - rayStart;
		Vector dir = rayEnd - rayStart;
		float length = dir.Normalize();

		float rangeAlong = dir.Dot(to);
		if (along)
			*along = rangeAlong;

		float range;

		if (rangeAlong < 0.0f)
		{
			range = -(pos - rayStart).Length();

			if (pointOnRay)
				*pointOnRay = rayStart;
		}
		else if (rangeAlong > length)
		{
			range = -(pos - rayEnd).Length();

			if (pointOnRay)
				*pointOnRay = rayEnd;
		}
		else
		{
			Vector onRay = rayStart + dir * rangeAlong;

			range = (pos - onRay).Length();

			if (pointOnRay)
				*pointOnRay = onRay;
		}

		return range;
	}

	inline void rotate_triangle(std::array<Vector2D, 3>& points, float rotation)
	{
		const auto pointsCenter = (points.at(0) + points.at(1) + points.at(2)) / 3;
		for (auto& point : points)
		{
			point -= pointsCenter;

			const auto tempX = point.x;
			const auto tempY = point.y;

			const auto theta = DEG2RAD(rotation);
			const auto c = cos(theta);
			const auto s = sin(theta);

			point.x = tempX * c - tempY * s;
			point.y = tempX * s + tempY * c;

			point += pointsCenter;
		}
	}

	void inline sin_cos(float radians, float* sine, float* cosine) {
		*sine = sin(radians);
		*cosine = cos(radians);
		
	}
	void inline matrix_set_column(const Vector& in, int column, matrix3x4_t& out) {
		out[0][column] = in.x;
		out[1][column] = in.y;
		out[2][column] = in.z;
	}

	void inline angle_matrix(const QAngle& angles, matrix3x4_t& matrix) {
#ifdef _VPROF_MATHLIB
		VPROF_BUDGET("AngleMatrix", "Mathlib");
#endif

		float sr, sp, sy, cr, cp, cy;

#ifdef _X360
		fltx4 radians, scale, sine, cosine;
		radians = LoadUnaligned3SIMD(angles.Base());
		scale = ReplicateX4(M_PI_F / 180.f);
		radians = MulSIMD(radians, scale);
		SinCos3SIMD(sine, cosine, radians);

		sp = SubFloat(sine, 0);	sy = SubFloat(sine, 1);	sr = SubFloat(sine, 2);
		cp = SubFloat(cosine, 0);	cy = SubFloat(cosine, 1);	cr = SubFloat(cosine, 2);
#else
		sin_cos(DEG2RAD(angles[1]), &sy, &cy);
		sin_cos(DEG2RAD(angles[0]), &sp, &cp);
		sin_cos(DEG2RAD(angles[2]), &sr, &cr);
#endif

		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		const float crcy = cr * cy;
		const float crsy = cr * sy;
		const float srcy = sr * cy;
		const float srsy = sr * sy;
		matrix[0][1] = sp * srcy - crsy;
		matrix[1][1] = sp * srsy + crcy;
		matrix[2][1] = sr * cp;

		matrix[0][2] = (sp * crcy + srsy);
		matrix[1][2] = (sp * crsy - srcy);
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.0f;
		matrix[1][3] = 0.0f;
		matrix[2][3] = 0.0f;
	}

	inline void normalize_angles(QAngle& angles) {
		for (auto i = 0; i < 3; i++) {
			while (angles[i] < -180.0f) angles[i] += 360.0f;
			while (angles[i] > 180.0f) angles[i] -= 360.0f;
		}
	}

	void inline angle_matrix(const QAngle& angles, const Vector& position, matrix3x4_t& matrix) {
		angle_matrix(angles, matrix);
		matrix_set_column(position, 3, matrix);
	}

	template<class T, class U>
	static T clamp(const T& in, const U& low, const U& high)
	{
		if (in <= low)
			return low;

		if (in >= high)
			return high;

		return in;
	}

	__forceinline float normalize_yaw(float f)
	{
		while (f < -180.0f)
			f += 360.0f;

		while (f > 180.0f)
			f -= 360.0f;

		return f;
	}

	inline float FASTSQRT(float x)
	{
		unsigned int i = *(unsigned int*)&x;

		i += 127 << 23;
		// approximation of square root
		i >>= 1;
		return *(float*)&i;
	}

	float VectorDistance(const Vector& v1, const Vector& v2);
	QAngle CalcAngle(const Vector& src, const Vector& dst);
	float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle);

	template <typename t>
	static t interpolate(const t& t1, const t& t2, float progress)
	{
		if (t1 == t2)
			return t1;

		return t2 * progress + t1 * (1.0f - progress);
	}

	template<class T>
	void Normalize3(T& vec)
	{
		for (auto i = 0; i < 2; i++) {
			while (vec[i] < -180.0f) vec[i] += 360.0f;
			while (vec[i] >  180.0f) vec[i] -= 360.0f;
		}
		vec[2] = 0.f;
	}

    void ClampAngles(QAngle& angles);
    void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
    void AngleVectors(const QAngle &angles, Vector& forward);
    void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up);
    void VectorAngles(const Vector& forward, QAngle& angles);
    bool WorldToScreen(const Vector& in, Vector& out);
}