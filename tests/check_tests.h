#pragma once
#include "bitboard.h"

namespace Panzer
{
	class CheckTests
	{
		private:
			Board_Bit *board;
		public:
			bool ValidateCheckLogic();
	};
}