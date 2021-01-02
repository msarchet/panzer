#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <fstream>
#include <set>

#include "../bitboard_constants.h"
#include "../utils/board_utils.h"

namespace Panzer
{
    enum Directions
    {
        N,
        NE,
        E,
        SE,
        S,
        SW,
        W,
        NW
    };

    uint8_t Row(square s) { return s / 8; };
    uint8_t Column(square s) { return s % 8; };

    bool IsBoardEdge(Directions d, square s)
    {
        switch (d)
        {
            case NW:
                return s / 8 == 7 || s % 8 == 0;
            case N:
                return s / 8 == 7;
            case NE:
                return s / 8 == 7 || s % 8 == 7; 
            case E:
                return s % 8 == 7;
            case SE:
                return s / 8 == 0 || s % 8 == 7;
            case S:
                return s / 8 == 0;
            case SW:
                return s / 8 == 0 || s % 8 == 0;
            case W:
                return s % 8 == 0;
        }

        return false;
    }

    uint8_t Inverse(uint8_t value) { return 7 - value; }

    class Direction
    {
        Directions d;

        public:
            Direction(Directions dir) :
                d(dir) {}
    
            square GetNextSquare(square start);
            uint8_t DistanceToEdge(square s);
            std::vector<bitboard> GenerateOccupancies(square s);
            bitboard GenerateAttacks(bitboard occupancy, square s);
    };

    std::vector<bitboard> Direction::GenerateOccupancies(square s)
    {
        std::vector<bitboard> occupancies = { };
        auto number_of_squares = this->DistanceToEdge(s) - 1;

        if (number_of_squares <= 0)
        {
            return occupancies;
        }

        const unsigned number_of_occupancies = 1 << number_of_squares;
        for (unsigned occupancy = 0U; occupancy < number_of_occupancies; ++occupancy)
        {
            bitboard occ_bb = 0ULL;
            square next_index = s;
            for (mask target_mask = 1U; target_mask <= occupancy; target_mask <<= 1)
            {
                next_index = this->GetNextSquare(next_index);
                occ_bb |= (static_cast<bitboard>(!!(target_mask & occupancy)) << next_index);
            }

            occupancies.push_back(occ_bb);
        }

        return occupancies;
    }

    bitboard Direction::GenerateAttacks(bitboard occupany, square s)
    {
        bitboard attacks = 0ULL;
        for(square next = this->GetNextSquare(s); next != NO_SQUARE; next = this->GetNextSquare(next))
        {
            attacks |= ONE_BIT << next;
            if (attacks & occupany)
            {
                // Found a blocker
                break;
            }
        }

        return attacks;
    }

    uint8_t Direction::DistanceToEdge(square s)
    {
        switch(this->d)
        {
            case NW:
                return std::min(Inverse(Row(s)), Column(s));
            case N:
                return Inverse(Row(s));
            case NE:
                return std::min(Inverse(Row(s)), Inverse(Column(s)));
            case E:
                return Inverse(Column(s));
            case SE:
                return std::min(Row(s), Inverse(Column(s)));
            case S:
                return Row(s);
            case SW:
                return std::min(Row(s), Column(s));
            case W:
                return Column(s);
        }
    }



    square Direction::GetNextSquare(square start) 
    {
        square next = start;

        if (IsBoardEdge(d, next))
        {
            return NO_SQUARE;
        }

        switch (d)
        {
            case Directions::N:
                next = start + 8;
                break;
            case Directions::NE:
                next = start + 9;
                break;
            case Directions::E:
                next = start + 1;
                break;
            case Directions::SE:
                next = start - 7;
                break;
            case Directions::S:
                next = start - 8;
                break;
            case Directions::SW:
                next = start - 9;
                break;
            case Directions::W:
                next = start - 1;
                break;
            case Directions::NW:
                next = start + 7;
                break;
        }

        if (next >= 64 || IsBoardEdge(d, next))
        {
            return NO_SQUARE;
        }

        return next;
    }

    std::vector<bitboard> CombineBitboards(std::vector<bitboard> left, std::vector<bitboard> right)
    {
        std::vector<bitboard> combined = {};

        if (left.empty())
        {
            return right;
        }

        if (right.empty())
        {
            return left;
        }

        for (auto one: left)
        {
            for (auto two: right)
            {
                combined.push_back(one | two);
            }
        }

        return combined;
    }

    bitboard CreateMask(std::vector<Direction> directions, square s)
    {
        bitboard mask = 0ULL;

        for (auto d: directions)
        {
            square current_square = d.GetNextSquare(s);
            while (current_square != NO_SQUARE)
            {
                mask |= ONE_BIT << current_square;
                current_square = d.GetNextSquare(current_square);
            }            
        }

        return mask;
    }
}


std::uniform_int_distribution<uint64_t> dist;
std::mt19937 engine;

void makeGenerator()
{
        // Magic number 624: The number of unsigned ints the MT uses as state
        auto seed_data = Panzer::Utils::GetSeedData();
        std::seed_seq seeds(seed_data->begin(), seed_data->end());
        std::mt19937 seededEngine (seeds);
        engine = seededEngine;
        dist = std::uniform_int_distribution<uint64_t>(1ULL, -1ULL);
}

uint64_t random64()
{
    return dist(engine) & dist(engine) & dist(engine);
}

void GenerateMagic(square i, 
    std::vector<Panzer::Direction> directions,
    square_bitboards attacks,
    magics_array magics,
    int occupancy_shift)
{
    // Generate all Occupancies for this square with the
    // given directions
    std::vector<bitboard> occupancies = {};
    for(auto direction: directions)
    {
        auto occs = Panzer::CombineBitboards(direction.GenerateOccupancies(i), occupancies);
        occupancies = occs;
    }

    // generate all "attacks" in these directions
    // by generating bitboards that move to the first
    // obstruction in a give occupancy
    std::vector<bitboard> temp_attacks = {};

    for (auto occupancy: occupancies)
    {
        bitboard square_attacks = 0ULL;
        for (auto direction: directions)
        {
            square_attacks |= direction.GenerateAttacks(occupancy, i);
        }

        temp_attacks.push_back(square_attacks);
    }

    assert(temp_attacks.size() == occupancies.size());

    // set some bits p for finding out magic bits
    bool generated_magic = false;

    // do this until we find a number that properly creates
    // a unique index from 0-2^occupancy_shift-1 for each
    // attack when we mulitply it by our random magic number
    // we then right shift all the non needed bits so
    // if there are 10 occupancies we shift over 54 bits
    // so we end up with effectively a 10 bit number
    // This allows us to just multiple the occupancy mask
    // by the magic number and find the associated attacks
    // index in the attacks array
    while(!generated_magic)
    {
        bool collision = false;
        std::vector<bitboard> temp_attack_magiced(1U << occupancy_shift, ~0ULL);
        // more zero bits
        uint64_t magic = random64();

        for (int o_i = 0; o_i < occupancies.size(); o_i++)
        {
            const auto occupancy = occupancies.at(o_i);
            const auto attack = temp_attacks.at(o_i);
            const int index = (occupancy * magic) >> (64 - occupancy_shift);
            if (temp_attack_magiced.at(index) == ~0ULL)
            {
                temp_attack_magiced.at(index) = attack;
            }
            else
            {
                collision = true; 
                break;
            }
        }
        if (!collision)
        {
            generated_magic = true;
            attacks->at(i) = temp_attack_magiced;
            magics->at(i) = magic;
        }
    }
}

void WriteToFile(std::string piece_name, const int shifts[], mask_array masks, magics_array magics, square_bitboards attacks)
{

    // write em all to files
    // write em all to files
    std::ofstream shift_file;
    std::ofstream mask_file;
    std::ofstream magics_file;
    std::ofstream attacks_file;
    shift_file.open(piece_name + "_shifts.dat", std::ios_base::app);
    mask_file.open(piece_name + "_masks.dat", std::ios_base::app);
    magics_file.open(piece_name + "_magics.dat", std::ios_base::app);
    attacks_file.open(piece_name + "_attacks.dat", std::ios_base::app);

    for (int i = 0; i < 64; i++)
    {
        shift_file << shifts[i] << std::endl;
        mask_file << masks.at(i) << std::endl;
        magics_file << magics->at(i) << std::endl;
        auto attacks_vector = attacks->at(i);
        for (auto attack: attacks_vector)
        {
            attacks_file << attack << std::endl;
        }
        attacks_file << "---" << std::endl;
    }
    shift_file.close();
    mask_file.close();
    magics_file.close();
    attacks_file.close();

}

int main(int argc, char *argv[])
{
    std::vector<Panzer::Direction> rook_directions =  
    {
        Panzer::Direction(Panzer::Directions::N),
        Panzer::Direction(Panzer::Directions::W),
        Panzer::Direction(Panzer::Directions::S),
        Panzer::Direction(Panzer::Directions::E),
    };

    std::vector<Panzer::Direction> bishop_directions =  
    {
        Panzer::Direction(Panzer::Directions::NW),
        Panzer::Direction(Panzer::Directions::SW),
        Panzer::Direction(Panzer::Directions::SE),
        Panzer::Direction(Panzer::Directions::NE),
    };

    const int rook_shifts[64] = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12,
    };

    const int bishop_shifts[64] = {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5, 
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6
    };

    mask_array rook_masks = { 0ULL };
    mask_array bishop_masks = { 0ULL };
    for (int i = 0; i < 64; i++)
    {
        auto rook_mask = Panzer::CreateMask(rook_directions, i);
        rook_masks[i] = rook_mask;
        auto bishop_mask = Panzer::CreateMask(bishop_directions, i);
        bishop_masks[i] = bishop_mask;
    }

    square_bitboards rook_attacks =  std::make_shared<std::array<std::vector<bitboard>, 64>>();
    square_bitboards bishop_attacks =  std::make_shared<std::array<std::vector<bitboard>, 64>>();

    magics_array rook_magics = std::make_shared<std::array<uint64_t, 64>>();
    magics_array bishop_magics = std::make_shared<std::array<uint64_t, 64>>();

    for (int i = 0; i < 64; i++)
    {
        std::cout << i << std::endl;
        GenerateMagic(i, rook_directions, rook_attacks, rook_magics, rook_shifts[i]);
        GenerateMagic(i, bishop_directions, bishop_attacks, bishop_magics, bishop_shifts[i]);
    }

    WriteToFile("rook", rook_shifts, rook_masks, rook_magics, rook_attacks);
    WriteToFile("bishop", bishop_shifts, bishop_masks, bishop_magics, bishop_attacks);
}