#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

#include "tokenization.hpp"
#include "parser.hpp"
#include "generation.hpp"



int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "xwin <input.xw>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens));
    std::optional<NodeExit> tree = parser.parse();

    if (!tree.has_value()) {
        std::cerr << "No exit statement found" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(tree.value());

    {
        std::fstream file("/home/xwindow/dev/xWinCompiler/build/out.asm", std::ios::out);
        file << generator.generate();
    }

    system("nasm -felf64 /home/xwindow/dev/xWinCompiler/build/out.asm");
    system("ld -o /home/xwindow/dev/xWinCompiler/build/out /home/xwindow/dev/xWinCompiler/build/out.o");

    return EXIT_SUCCESS;
}