#pragma once
#include "bitboard.h"

namespace Panzer
{
	class CheckTests
	{
		private:
			Board *board;
		public:
			bool ValidateCheckLogic();
	};
}