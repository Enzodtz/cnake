main:
	eval cc main.c `pkg-config --libs --cflags raylib` -o SnakeGame
