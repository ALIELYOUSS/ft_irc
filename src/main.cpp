#include "../includes/server.hpp"

int main(int ac, char **av){
    if (ac != 3){
        std::cerr << "Bad arguments" << std::endl;
        return EXIT_FAILURE;
    }
    try
    {
        server sv(av[1], av[2]);
        sv.init();
        return EXIT_SUCCESS;
    }
    catch(const std::exception& e){
        std::cerr << e.what();
        return EXIT_FAILURE;
    }
}