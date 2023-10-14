# Compile each source file
g++ -std=c++17 -c src/main.cpp -o main.o
g++ -std=c++17 -c src/CommandExecutor.cpp -o CommandExecutor.o
g++ -std=c++17 -c src/Command.cpp -o Command.o
g++ -std=c++17 -c src/Job.cpp -o Job.o
g++ -std=c++17 -c src/JobHandler.cpp -o JobHandler.o

# Link the object files together
g++ main.o CommandExecutor.o Command.o Job.o JobHandler.o -lreadline -o 2_shell
rm -rf main.o CommandExecutor.o Command.o Job.o JobHandler.o

# Run program
./2_shell
