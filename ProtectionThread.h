#pragma once
#include "Common.h"
#include <wininet.h>
#include <string>
#include <vector>
#include <utility>
namespace CShield {

	class ProtectionThread
	{
	public:
		static VOID VMwareDetection();
		static VOID AntiDebugProtection();
	};
}