# Compile each source file
g++ -std=c++17 -c src/main.cpp -o main.o
g++ -std=c++17 -c src/exec.cpp -o exec.o
g++ -std=c++17 -c src/parser.cpp -o parser.o

# Link the object files together with the readline library to produce the final executable
g++ main.o exec.o parser.o -lreadline -o 2_shell
rm -rf main.o exec.o parser.o