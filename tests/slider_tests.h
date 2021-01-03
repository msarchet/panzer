#include "../sliding-attacks/sliders.cpp";

namespace Panzer
{
    class SlidingTests
    {
        Sliders *sliders = new Sliders();
        public:
            void Setup();
            bool CanInitialize();
            bool ValidateBishopAttacks();
            bool ValidateRookAttacks();
            bool ValidateQueenAttacks();
        private:
            bitboard GenerateBoard(std::vector<square> squares);
    };

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
        const bitboard empty_occupancy = 0ULL;
        const bitboard empty_attacks = sliders->GetBishopAttacks(A1, empty_occupancy);
        const bitboard expected_empty_attacks = GenerateBoard({ B2, C3, D4, E5, F6, G7, H8});
        assert(empty_attacks == expected_empty_attacks);

        auto blocked_occupancy = GenerateBoard({B2});
        auto blocked_attacks = sliders->GetBishopAttacks(A1, blocked_occupancy);
        assert(blocked_attacks == blocked_occupancy);
    }

    bool SlidingTests::ValidateRookAttacks()
    {

    }

    bool SlidingTests::ValidateQueenAttacks()
    {

    }
}