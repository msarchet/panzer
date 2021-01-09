#include <iostream>
#include "slider_tests.h"

int main()
{
    auto slider_tests = new Panzer::SlidingTests();
    std::cout << "Running Tests" << std::endl;
    slider_tests->Setup();
    slider_tests->ValidateBishopAttacks();
    slider_tests->ValidateRookAttacks();

    std::cout << "Done!";
}