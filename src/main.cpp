#include <iostream>
#include <fstream>

#include <toml++/toml.hpp>

int main()
{
    auto testFile = toml::parse_file("../assets/Molecule-Tests/CH4.toml");

    std::cout << "Hello World" << std::endl;

    std::cout << "The tested molecule is " << testFile["Run"]["Molecule"].value_or("") << std::endl;

    return 0;
}