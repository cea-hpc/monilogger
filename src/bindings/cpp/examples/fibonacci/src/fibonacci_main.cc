#include "fibonacci_embedding.h"
#include "fibonacci.h"
#include <stdexcept>
#include <string>

int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: fibonacci <python paths> <python_scripts> n";
        return 1;
    }

    // Retrieve the paths where Python scripts are located (can provide multiple locations).
    std::vector<std::string> python_path = { argv[1] };
    
    // Provide the Python scripts to include.
    std::vector<std::string> python_script = { argv[2] };

    int n;
    std::string arg = argv[3];
    try {
        std::size_t pos;
        n = std::stoi(arg, &pos);
    } catch (std::invalid_argument const &ex) {
        std::cerr << "Invalid number: " << arg << '\n';
        return 1;
    }
    
    initialize_embedding(python_path, python_script);

    fibonacci(n);

    return 0;
}
