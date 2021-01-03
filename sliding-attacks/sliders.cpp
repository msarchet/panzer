#include <fstream>
#include <vector>
#include "../bitboard_constants.h"

namespace Panzer
{
    class Sliders
    {
        magics_array rook_magics;
        std::shared_ptr<std::array<std::shared_ptr<std::vector<bitboard>>, 64>> rook_attacks;
        mask_array rook_masks;
        int rook_shifts[64];

        magics_array bishop_magics;
        std::shared_ptr<std::array<std::shared_ptr<std::vector<bitboard>>, 64>> bishop_attacks;
        mask_array bishop_masks;
        int bishop_shifts[64];

        public:
            void Initialize();
            bitboard GetRookAttacks(square s, bitboard occupancy);
            bitboard GetBishopAttacks(square s, bitboard occupancy);
            bitboard GetQueenAttacks(square s, bitboard occupancy);
        private:
            int GetAttackIndex(bitboard occupancy, mask occupancy_mask, mask magic, int shift) 
            {
                return ((occupancy & occupancy_mask) * magic) >> (64 - shift);
            }
    };

    void Sliders::Initialize()
    {
        rook_magics = std::make_shared<mask_array>();
        bishop_magics = std::make_shared<mask_array>();
        rook_attacks = std::make_shared<std::array<std::shared_ptr<std::vector<bitboard>>, 64>>();
        bishop_attacks = std::make_shared<std::array<std::shared_ptr<std::vector<bitboard>>, 64>>();

        std::string rook_magics_file("rook_magics.dat");
        std::string rook_attacks_file("rook_attacks.dat");
        std::string rook_masks_file("rook_masks.dat");
        std::string rook_shifts_file("rook_shifts.dat");

        std::string bishop_magics_file("bishop_magics.dat");
        std::string bishop_attacks_file("bishop_attacks.dat");
        std::string bishop_masks_file("bishop_masks.dat");
        std::string bishop_shifts_file("bishop_shifts.dat");


        std::ifstream rook_magics_stream (rook_magics_file, std::ios_base::in);
        std::ifstream bishop_magics_stream (bishop_magics_file, std::ios_base::in);
        std::ifstream rook_attacks_stream (rook_attacks_file, std::ios_base::in);
        std::ifstream bishop_attacks_stream (bishop_attacks_file, std::ios_base::in);
        std::ifstream rook_shifts_stream (rook_shifts_file, std::ios_base::in);
        std::ifstream bishop_shifts_stream (bishop_shifts_file, std::ios_base::in);
        std::ifstream rook_masks_stream (rook_masks_file, std::ios_base::in);
        std::ifstream bishop_masks_stream (bishop_masks_file, std::ios_base::in);

        for (int i = 0; i < 64; i++)
        {
            rook_magics_stream >> rook_magics->at(i);
            bishop_magics_stream >> bishop_magics->at(i);

            rook_shifts_stream >> rook_shifts[i];
            bishop_shifts_stream >> bishop_shifts[i];

            rook_masks_stream >> rook_masks[i];
            bishop_masks_stream >> bishop_masks[i];
            auto rook_attacks_vector = std::make_shared<std::vector<bitboard>>();
            rook_attacks->at(i) = rook_attacks_vector;

            const std::string terminator = "---";
            std::string line;
            while(rook_attacks_stream >> line)
            {
                if (terminator.compare(line) == 0)
                {                        
                    break;
                }

                auto attack = std::strtoull(line.c_str(), NULL, 10);
                rook_attacks_vector->push_back(attack);
            }

            auto bishop_attacks_vector = std::make_shared<std::vector<bitboard>>();
            bishop_attacks->at(i) = bishop_attacks_vector;

            while(bishop_attacks_stream >> line)
            {
                if (terminator.compare(line) == 0)
                {                        
                    break;
                }

                auto attack = std::strtoull(line.c_str(), NULL, 10);
                bishop_attacks_vector->push_back(attack);
            }
        }
    }

    bitboard Sliders::GetRookAttacks(square s, bitboard occupancy)
    {
        return rook_attacks->at(s)->at(GetAttackIndex(occupancy, rook_masks.at(s), rook_magics->at(s), rook_shifts[s]));
    }

    bitboard Sliders::GetBishopAttacks(square s, bitboard occupancy)
    {
        return bishop_attacks->at(s)->at(GetAttackIndex(occupancy, bishop_masks.at(s), bishop_magics->at(s), bishop_shifts[s]));
    }

    bitboard Sliders::GetQueenAttacks(square s, bitboard occupancy)
    {
        return GetRookAttacks(s, occupancy) | GetBishopAttacks(s, occupancy);
    }
} // namespace Panzer