#include "../includes/server.hpp"

int main(int ac, char **av){
    if (ac != 3){
        std::cout << "Bad arguments" << std::endl;
        exit(EXIT_FAILURE);
    }
    try
    {
        server sv(av[1], av[2]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what();
    }   
}