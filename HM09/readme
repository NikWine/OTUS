gcc -Wall -Wextra -Wpedantic -std=c11 -c logger.c -o logger.o
ar rcs liblogger.a logger.o
gcc -Wall -Wextra -Wpedantic -std=c11 main.c -L. -llogger -o test_app
