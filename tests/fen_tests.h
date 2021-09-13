#pragma once
#include "../bitboard.h"

namespace Panzer
{
	class FenTests
	{
		private:
			Board_Bit *board;
		public:
			bool TestBoardToFen();
			bool TestFenToBoard();
	};
}