# Name of the project
PROJ_NAME=vrp

# .cpp files
CPP_SOURCE=$(wildcard ./source/*.cpp)

# .h files
H_SOURCE=$(wildcard ./source/*.h)

# Object files
OBJ=$(subst .cpp,.o,$(subst source,objects,$(CPP_SOURCE)))

# Compiler and linker
CC=g++

# Flags for compiler
CC_FLAGS=-c       \
	       -Wall    

# Command used at clean target
RM = rm -rf

#
# Compilation and linking
#
all: objFolder $(PROJ_NAME)

$(PROJ_NAME): $(OBJ)
		@ echo 'Building binary using G++ linker: $@'
		$(CC) $^ -o $@
		@ echo 'Finished building binary: $@'
		@ echo ' '

./objects/%.o: ./source/%.cpp ./source/%.h
		@ echo 'Building target using G++ compiler: $<'
		$(CC) $< $(CC_FLAGS) -o $@
		@ echo ' '

./objects/main.o: ./source/main.cpp $(H_SOURCE)
		@ echo 'Building target using G++ compiler: $<'
		$(CC) $< $(CC_FLAGS) -o $@
		@ echo ' '

objFolder:
		@ mkdir -p objetos

clean:
		@ $(RM) ./objetos/*.o $(PROJ_NAME) *~
		@ rmdir objetos

.PHONY: all clean
