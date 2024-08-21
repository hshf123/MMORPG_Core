#include "pch.h"
#include "MathUtil.h"

bool MathUtil::CircularSectorRangeCheck(const float& theta, const float& radius, const Vec2& myPos, const Vec2& forward, const Vec2& targetPos)
{
	// 내 위치와 타겟의 거리가 범위보다 크면 애초에 탈락
	if (Vec2::Distance(myPos, targetPos) > radius)
		return false;

	// 방향벡터 구하기
	Vec2 forwardVec = forward - myPos;		// 시전자가 바라보는 기하벡터
	Vec2 targetVec = targetPos - myPos;		// 타겟 방향의 기하벡터

	forwardVec.Normalize();
	targetVec.Normalize();

	float t = forwardVec.Dot(targetVec);	// 내적
	float ac = DirectX::XMScalarACos(t);	// acos값
	return DirectX::XMConvertToDegrees(ac) <= (theta / 2);	// 2로 나누는 이유는 각도가 60이라 했을 때 방향기준 오른쪽 30도 왼쪽 30도 체크하기 위함
}

bool MathUtil::IsBehindPos(const Vec2& myPos, const Vec2& forward, const Vec2& targetPos, const Vec2& targetForward, const float& marginOfErrTheta)
{
	Vec2 forwardVec = forward - myPos;			// 시전자가 바라보는 기하벡터
	Vec2 targetVec = targetForward - myPos;		// 타겟이 바라보는 방향의 기하벡터

	// 방향벡터 구하기
	float t = forwardVec.Dot(targetVec);	// 내적
	float ac = DirectX::XMScalarACos(t);	// acos값
	return DirectX::XMConvertToDegrees(ac) <= (marginOfErrTheta / 2);	// 2로 나누는 이유는 각도가 60이라 했을 때 방향기준 오른쪽 30도 왼쪽 30도 체크하기 위함
}
