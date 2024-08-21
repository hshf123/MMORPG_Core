#include "pch.h"
#include "MathUtil.h"

bool MathUtil::CircularSectorRangeCheck(const float& theta, const float& radius, const Vec2& myPos, const Vec2& forward, const Vec2& targetPos)
{
	// �� ��ġ�� Ÿ���� �Ÿ��� �������� ũ�� ���ʿ� Ż��
	if (Vec2::Distance(myPos, targetPos) > radius)
		return false;

	// ���⺤�� ���ϱ�
	Vec2 forwardVec = forward - myPos;		// �����ڰ� �ٶ󺸴� ���Ϻ���
	Vec2 targetVec = targetPos - myPos;		// Ÿ�� ������ ���Ϻ���

	forwardVec.Normalize();
	targetVec.Normalize();

	float t = forwardVec.Dot(targetVec);	// ����
	float ac = DirectX::XMScalarACos(t);	// acos��
	return DirectX::XMConvertToDegrees(ac) <= (theta / 2);	// 2�� ������ ������ ������ 60�̶� ���� �� ������� ������ 30�� ���� 30�� üũ�ϱ� ����
}

bool MathUtil::IsBehindPos(const Vec2& myPos, const Vec2& forward, const Vec2& targetPos, const Vec2& targetForward, const float& marginOfErrTheta)
{
	Vec2 forwardVec = forward - myPos;			// �����ڰ� �ٶ󺸴� ���Ϻ���
	Vec2 targetVec = targetForward - myPos;		// Ÿ���� �ٶ󺸴� ������ ���Ϻ���

	// ���⺤�� ���ϱ�
	float t = forwardVec.Dot(targetVec);	// ����
	float ac = DirectX::XMScalarACos(t);	// acos��
	return DirectX::XMConvertToDegrees(ac) <= (marginOfErrTheta / 2);	// 2�� ������ ������ ������ 60�̶� ���� �� ������� ������ 30�� ���� 30�� üũ�ϱ� ����
}
