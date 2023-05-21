
COMPILER = gcc
COMPILER_FLAGS = -c -g --std=c99 -pedantic -Wall -Wextra -Wno-unused-parameter -D_POSIX_C_SOURCE=200809L -I./include

LINKER = gcc
LINKER_FLAGS = -g --std=c99 -pedantic -Wall -Wextra -Wno-unused-parameter -D_POSIX_C_SOURCE=200809L -lm