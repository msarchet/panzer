#include <memory>
#include <iostream>
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
}