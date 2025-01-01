#pragma once

class UStateMachine;
class UStateName;
class UStateNode;

namespace Naming
{
	CRABTOOLSUE5_API FName GenerateStateNodeName(UStateMachine* Outer, FName StateName);
	CRABTOOLSUE5_API FName GenerateStateNodeName(UStateNode* Node, FName StateName);
}