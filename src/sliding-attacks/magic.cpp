#include <algorithm>
#include <climits>
#include <cmath>
#include <fstream>
#include <random>
#include <set>
#include <vector>

#include <bitboard_constants.h>
#include <board_utils.h>
#include <sliders.h>

namespace Panzer {

void Sliders::MagicGenerator::MakeMagic(Magics &rookMagic,
                                        Magics &bishopMagic) {
  MakeGenerator();
  std::vector<Direction> rook_directions = {
      {Directions::N},
      {Directions::W},
      {Directions::S},
      {Directions::E},
  };

  std::vector<Direction> bishop_directions = {
      {Directions::NW},
      {Directions::SW},
      {Directions::SE},
      {Directions::NE},
  };

  rookMagic.shifts = {
      12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12,
  };

  bishopMagic.shifts = {6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5,
                        5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5,
                        5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5,
                        5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6};

  mask_array rook_masks = {0ULL};
  mask_array bishop_masks = {0ULL};
  for (int i = 0; i < 64; i++) {
    auto rook_mask = CreateMask(rook_directions, i);
    rookMagic.masks.at(i) = rook_mask;
    auto bishop_mask = CreateMask(bishop_directions, i);
    bishopMagic.masks.at(i) = bishop_mask;
  }

  GenerateMagic(rook_directions, rookMagic);
  GenerateMagic(bishop_directions, bishopMagic);
}
uint8_t Sliders::MagicGenerator::Direction::Row(square s) { return s / 8; };

uint8_t Sliders::MagicGenerator::Direction::Column(square s) { return s % 8; };

bool Sliders::MagicGenerator::Direction::IsBoardEdge(Directions d, square s) {
  switch (d) {
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

uint8_t Sliders::MagicGenerator::Direction::Inverse(uint8_t value) {
  return 7 - value;
}

std::vector<bitboard>
Sliders::MagicGenerator::Direction::GenerateOccupancies(square s) {
  std::vector<bitboard> occupancies = {};
  auto number_of_squares = this->DistanceToEdge(s) - 1;

  if (number_of_squares <= 0) {
    return occupancies;
  }

  const unsigned number_of_occupancies = 1 << number_of_squares;
  for (unsigned occupancy = 0U; occupancy < number_of_occupancies;
       ++occupancy) {
    bitboard occ_bb = 0ULL;
    square next_index = s;
    for (mask target_mask = 1U; target_mask <= occupancy; target_mask <<= 1) {
      next_index = this->GetNextSquare(next_index);
      occ_bb |=
          (static_cast<bitboard>(!!(target_mask & occupancy)) << next_index);
    }

    occupancies.push_back(occ_bb);
  }

  return occupancies;
}

bitboard Sliders::MagicGenerator::Direction::GenerateAttacks(bitboard occupany,
                                                             square s) {
  bitboard attacks = 0ULL;
  for (square next = this->GetNextSquare(s); next != NO_SQUARE;
       next = this->GetNextSquare(next)) {
    attacks |= ONE_BIT << next;
    if (attacks & occupany) {
      // Found a blocker
      break;
    }
  }

  return attacks;
}

uint8_t Sliders::MagicGenerator::Direction::DistanceToEdge(square s) {
  switch (this->d) {
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

square Sliders::MagicGenerator::Direction::GetNextSquare(square start) {
  auto row = Row(start);
  auto column = Column(start);

  switch (d) {
  case Directions::N:
    row++;
    break;
  case Directions::NE:
    row++;
    column++;
    break;
  case Directions::E:
    column++;
    break;
  case Directions::SE:
    row--;
    column++;
    break;
  case Directions::S:
    row--;
    break;
  case Directions::SW:
    row--;
    column--;
    break;
  case Directions::W:
    column--;
    break;
  case Directions::NW:
    row++;
    column--;
    break;
  }

  if (row > 7 || row < 0 || column > 7 || column < 0) {
    return NO_SQUARE;
  }

  return row * 8 + column;
}

std::vector<bitboard>
Sliders::MagicGenerator::CombineBitboards(const std::vector<bitboard> &left,
                                          const std::vector<bitboard> &right) {
  std::vector<bitboard> combined = {};

  if (left.empty()) {
    return right;
  }

  if (right.empty()) {
    return left;
  }

  for (auto one : left) {
    for (auto two : right) {
      combined.push_back(one | two);
    }
  }

  return combined;
}

bitboard
Sliders::MagicGenerator::CreateMask(const std::vector<Direction> &directions,
                                    square s) {
  bitboard mask = 0ULL;

  for (auto d : directions) {
    square current_square = d.GetNextSquare(s);
    while (current_square != NO_SQUARE &&
           !d.IsBoardEdge(d.GetDirection(), current_square)) {
      mask |= ONE_BIT << current_square;
      current_square = d.GetNextSquare(current_square);
    }
  }

  return mask;
}
void Sliders::MagicGenerator::MakeGenerator() {
  // Magic number 624: The number of unsigned ints the MT uses as state
  auto seed_data = Utils::GetSeedData();
  std::seed_seq seeds(seed_data.begin(), seed_data.end());
  std::mt19937 seededEngine(seeds);
  engine = seededEngine;
  dist = std::uniform_int_distribution<uint64_t>(1ULL, ULLONG_MAX);
}

uint64_t Sliders::MagicGenerator::random64() {
  return dist(engine) & dist(engine) & dist(engine);
}

void Sliders::MagicGenerator::GenerateMagic(
    const std::vector<Direction> &directions, Magics &magics) {
  // Generate all Occupancies for this square with the
  // given directions
  for (int i = 0; i < 64; i++) {

    std::vector<bitboard> occupancies = {};
    for (auto direction : directions) {
      auto occs =
          CombineBitboards(direction.GenerateOccupancies(i), occupancies);
      occupancies = occs;
    }

    // generate all "attacks" in these directions
    // by generating bitboards that move to the first
    // obstruction in a give occupancy
    std::vector<bitboard> temp_attacks = {};

    for (auto occupancy : occupancies) {
      bitboard square_attacks = 0ULL;
      for (auto direction : directions) {
        square_attacks |= direction.GenerateAttacks(occupancy, i);
      }

      temp_attacks.push_back(square_attacks);
    }

    // assert(temp_attacks.size() == occupancies.size());

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
    while (!generated_magic) {
      bool collision = false;
      std::vector<bitboard> temp_attack_magiced(1ULL << magics.shifts.at(i),
                                                ~0ULL);
      // more zero bits
      uint64_t magic = random64();

      for (int o_i = 0; o_i < occupancies.size(); o_i++) {
        const auto occupancy = occupancies.at(o_i);
        const auto attack = temp_attacks.at(o_i);
        const int index = (occupancy * magic) >> (64 - magics.shifts.at(i));
        if (temp_attack_magiced.at(index) == ~0ULL) {
          temp_attack_magiced.at(index) = attack;
        } else {
          collision = true;
          break;
        }
      }
      if (!collision) {
        generated_magic = true;
        magics.attacks.at(i) = temp_attack_magiced;
        magics.magics.at(i) = magic;
      }
    }
  }
}

void WriteToFile(std::string piece_name, const Magics &magics) {
  std::ofstream shift_file;
  std::ofstream mask_file;
  std::ofstream magics_file;
  std::ofstream attacks_file;
  shift_file.open(piece_name + "_shifts.dat", std::ios_base::app);
  mask_file.open(piece_name + "_masks.dat", std::ios_base::app);
  magics_file.open(piece_name + "_magics.dat", std::ios_base::app);
  attacks_file.open(piece_name + "_attacks.dat", std::ios_base::app);

  for (int i = 0; i < 64; i++) {
    shift_file << magics.shifts[i] << std::endl;
    mask_file << magics.masks.at(i) << std::endl;
    magics_file << magics.magics.at(i) << std::endl;
    auto attacks_vector = magics.attacks.at(i);
    for (auto attack : attacks_vector) {
      attacks_file << attack << std::endl;
    }
    attacks_file << "---" << std::endl;
  }
  shift_file.close();
  mask_file.close();
  magics_file.close();
  attacks_file.close();
}

} // namespace Panzer
