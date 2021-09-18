#include <memory>
#include <iostream>
#include <vector>
#include "slider_tests.h"
#include "check_tests.h"
#include "../bitboard.h"
#include "../bitboard_constants.h"

int main()
{
    auto slider_tests = new Panzer::SlidingTests();
    std::cout << "Running Tests" << std::endl;
    std::cout << "DERP";
    slider_tests->Setup();
    slider_tests->ValidateBishopAttacks();
    slider_tests->ValidateRookAttacks();
    auto check_tests = new Panzer::CheckTests();
    check_tests->ValidateCheckLogic();
    auto test_vector = new std::vector<int>();
    test_vector->reserve(10);
    test_vector->emplace_back(1);
    test_vector->emplace_back(1);
    test_vector->emplace_back(1);
    for (auto it = test_vector->begin(); it != test_vector->end(); it++)
    {
        std::cout << *it << std::endl;
    }
}