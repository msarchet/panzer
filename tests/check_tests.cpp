#include <cassert>
#include "check_tests.h"
#include "bitboard_constants.h"
#include "bitboard.h"

namespace Panzer
{
	bool CheckTests::ValidateCheckLogic()
	{
		board = new Board();
		board->FillSquare(A1, KING, WHITE);
		board->FillSquare(A2, QUEEN, BLACK);

		assert(board->IsChecked(WHITE));

		std::cout << "Simple check" << std::endl;

		board = new Board();
		board->FillSquare(A1, KING, WHITE);
		board->FillSquare(A3, BISHOP, BLACK);

		assert(!board->IsChecked(WHITE));

		std::cout << "Not Checked" << std::endl;

		board = new Board();
		board->FillSquare(A1, KING, WHITE);
		board->FillSquare(B2, PAWN, BLACK);

		assert(board->IsChecked(WHITE));
		std::cout << "Checked by pawn white";

		board = new Board();
		board->FillSquare(A1, PAWN, WHITE);
		board->FillSquare(B2, KING, BLACK);

		assert(board->IsChecked(BLACK));
		std::cout << "Checked by pawn black";

		board = new Board();
		board->FillSquare(A1, KING, WHITE);
		board->FillSquare(B3, KNIGHT, BLACK);

		assert(board->IsChecked(WHITE));

		std::cout << "Checked by Knight" << std::endl;

		board = new Board();
		board->FenToBoard("8/8/8/8/2q5/8/1PPP4/1PKP4");

		assert(!board->IsChecked(WHITE));

		board->FenToBoard("8/8/8/8/8/8/3p4/2K5");

		assert(board->IsChecked(WHITE));

		std::cout << "Not Checked, blocked" << std::endl;
		return true;
	}
}