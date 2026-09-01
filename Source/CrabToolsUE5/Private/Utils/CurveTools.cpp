#include "Utils/CurveTools.h"

FLinearVelocityCurve::FLinearVelocityCurve(FVector InitStart, FVector InitEnd, float InitTime)
: Start(InitStart),
	End(InitEnd),
	Time(InitTime)
{
	this->Initialize();
}

FLinearVelocityCurve::FLinearVelocityCurve(const FLinearVelocityCurve& InCurve)
: Start(InCurve.Start),
	End(InCurve.End),
	Time(InCurve.Time),
	Direction(InCurve.Direction)
{
}

void FLinearVelocityCurve::Initialize()
{
	FVector Displacement = End - Start;
	Direction = Displacement/Time;
}

FVector FLinearVelocityCurve::Step(float DeltaTime)
{
	CurrentTime += DeltaTime;

	if (CurrentTime >= Time)
	{
		return End;
	}
	else
	{
		FVector Displacement = Direction * CurrentTime;
		FVector Result = Start + Displacement;

		return Result;
	}
}



FVector FArcPointCurve::ComputeVelocityVector(const FVector& Start, const FVector& End, float Gravity, float Time)
{
	return (End - Start - FVector(0, 0, Gravity) * Time * Time / 2) / Time;
}

FArcPointCurve::FArcPointCurve(FVector InitStart, FVector InitEnd, float InitGravity, float InitTime)
: Start(InitStart),
	End(InitEnd),
	Gravity(InitGravity),
	Time(InitTime),
	Velocity(ComputeVelocityVector(InitStart, InitEnd, InitGravity, InitTime))
{
}

FVector FArcPointCurve::Step(float DeltaTime)
{
	CurrentTime += DeltaTime;

	if (CurrentTime >= Time)
	{
		return End;
	}
	else
	{
		return Start + Velocity * CurrentTime + FVector(0, 0, Gravity) * CurrentTime * CurrentTime / 2;
	}
}

bool FArcPointCurve::IsFinished() const
{
	return CurrentTime >= Time;
}

void UCurveToolsHelperLibrary::InitLinearVelocityCurve(FLinearVelocityCurve& Curve)
{
	Curve.Initialize();
}

FVector UCurveToolsHelperLibrary::ComputeVelocityVector(const FVector& Start, const FVector& End, float Gravity, float Time)
{
	return FArcPointCurve::ComputeVelocityVector(Start, End, Gravity, Time);
}