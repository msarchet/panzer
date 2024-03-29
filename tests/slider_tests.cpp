#include <iostream>
#include <cassert>
#include "slider_tests.h"
#include "board_utils.h"
#include "sliders.h"
#include "bitboard.h"

namespace Panzer
{
    const bitboard empty_occupancy = 0ULL;
        
   
    bitboard SlidingTests::GenerateBoard(std::vector<square> squares)
    {
        bitboard created = 0ULL;
        for (auto s: squares)
        {
            created |= ONE_BIT << s;
        }
        return created;
    }

    void SlidingTests::Setup()
    {
        sliders->Initialize();
    }

    bool SlidingTests::CanInitialize()
    {
        return true;
    }

    bool SlidingTests::ValidateBishopAttacks()
    {
        const bitboard empty_attacks = sliders->GetBishopAttacks(A1, empty_occupancy);
        const bitboard expected_empty_attacks = GenerateBoard({ B2, C3, D4, E5, F6, G7, H8});
#ifdef DEBUG_PRINT
        Panzer::Utils::PrintBoard(empty_attacks);
#endif
        assert(empty_attacks == expected_empty_attacks);
        auto blocked_occupancy = GenerateBoard({B2});
        auto blocked_attacks = sliders->GetBishopAttacks(A1, blocked_occupancy);
#ifdef DEBUG_PRINT
        Panzer::Utils::PrintBoard(blocked_attacks);
#endif
        assert(blocked_attacks == blocked_occupancy);

        std::cout << "bishop attacks test passed";
        return true;
    }

    bool SlidingTests::ValidateRookAttacks()
    {
        const bitboard empty_attacks = sliders->GetRookAttacks(A1, empty_occupancy);
        const bitboard expected_empty_attacks = GenerateBoard({ B1, C1, D1, E1, F1, G1, H1, A2, A3, A4, A5, A6, A7, A8});
#ifdef DEBUG_PRINT
        Panzer::Utils::PrintBoard(empty_attacks);
#endif
        assert(empty_attacks == expected_empty_attacks);

        const bitboard blocked_occupancy = GenerateBoard({ A3, C1 });
        const bitboard blocked_attacks = sliders->GetRookAttacks(A1, blocked_occupancy);
        const bitboard expected_blocked_attacks = GenerateBoard({ B1, C1, A2, A3});
#ifdef DEBUG_PRINT
        Panzer::Utils::PrintBoard(blocked_attacks);
#endif
        assert(blocked_attacks == expected_blocked_attacks);

        const bitboard starting_occupnacy = GenerateBoard({
                A8, B8, C8, D8, E8, F8, G8, H8,
                A7, B7, C7, D7, E7, F7, G7, H7, 
                A2, B2, C2, D2, E2, F2, G2, H2, 
                A1, B1, C1, D1, E1, F1, G1, H1
        });
        const bitboard starting_attacks = sliders->GetRookAttacks(A8, starting_occupnacy);
        const bitboard expected_starting_attacks = GenerateBoard({ A7 , B8 });
#ifdef DEBUG_PRINT
        Panzer::Utils::PrintBoard(starting_attacks);
#endif
        assert(starting_attacks == expected_starting_attacks);

        std::cout << "Rook attacks passed";

        const auto board = new Panzer::Board();
        board->FenToBoard("r3k2r/p1ppqpb1/bn3np1/3NN3/1p2P3/5Q2/PPPBBPpP/1R2K2R w Kkq ");
        auto possible = sliders->GetRookAttacks(H1, board->GetOccupancy()) & ~board->GetOccupancy();
        Panzer::Utils::PrintBoard(possible);

        return true;
    }

    bool SlidingTests::ValidateQueenAttacks()
    {
		return true;
    }

    bool SlidingTests::ValidateIsChecked()
    {
        auto board = new Board();
        return true;
    }
}