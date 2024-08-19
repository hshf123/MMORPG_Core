#pragma once
#include "MathPch.h"

class MathUtil
{
public:
	/// <summary>
	/// ��ä�� ���� üũ
	/// </summary>
	/// <param name="theta">��ä�� ����</param>
	/// <param name="radius">������ ����</param>
	/// <param name="myPos">�� ��ġ����</param>
	/// <param name="forward">���⺤�͸� ���ϱ� ���� ��ġ����</param>
	/// <param name="targetPos">Ÿ���� ��ġ����</param>
	/// <returns></returns>
	static bool CircularSectorRangeCheck(const float& theta, const float& radius, const Vec2& myPos, const Vec2& forward, const Vec2& targetPos);
	
	/// <summary>
	/// ���� ���� �ڿ� �ִ��� üũ
	/// </summary>
	/// <param name="myPos">�� ��ġ����</param>
	/// <param name="forward">���⺤�͸� ���ϱ� ���� ��ġ����</param>
	/// <param name="targetPos">Ÿ�� ��ġ����</param>
	/// <param name="targetForward">���⺤�͸� ���ϱ� ���� ��ġ����</param>
	/// <param name="marginOfError">��������(����)</param>
	/// <returns></returns>
	static bool IsBehindPos(const Vec2& myPos, const Vec2& forward, const Vec2& targetPos, const Vec2& targetForward, const float& marginOfErrTheta);
};

