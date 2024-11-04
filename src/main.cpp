#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main()
{
    std::cout << "Hello World" << std::endl;

    std::fstream input("../inputs/test.json");
    json data = json::parse(input);

    std::cout << "The tested molecule is " << data["run"]["Molecule"] << std::endl;

    return 0;
}