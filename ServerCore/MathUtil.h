#pragma once
#include "MathPch.h"

class MathUtil
{
public:
	/// <summary>
	/// 부채꼴 범위 체크
	/// </summary>
	/// <param name="theta">부채꼴 각도</param>
	/// <param name="radius">반지름 범위</param>
	/// <param name="myPos">내 위치벡터</param>
	/// <param name="forward">방향벡터를 구하기 위한 위치벡터</param>
	/// <param name="targetPos">타겟의 위치벡터</param>
	/// <returns></returns>
	static bool CircularSectorRangeCheck(const float& theta, const float& radius, const Vec2& myPos, const Vec2& forward, const Vec2& targetPos);
	
	/// <summary>
	/// 내가 상대방 뒤에 있는지 체크
	/// </summary>
	/// <param name="myPos">내 위치벡터</param>
	/// <param name="forward">방향벡터를 구하기 위한 위치벡터</param>
	/// <param name="targetPos">타겟 위치벡터</param>
	/// <param name="targetForward">방향벡터를 구하기 위한 위치벡터</param>
	/// <param name="marginOfError">오차범위(각도)</param>
	/// <returns></returns>
	static bool IsBehindPos(const Vec2& myPos, const Vec2& forward, const Vec2& targetPos, const Vec2& targetForward, const float& marginOfErrTheta);
};

