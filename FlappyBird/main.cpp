#include "FlappyBird.h"

int main()
{
	FlappyBird flappy;
	flappy.constructConsole(80, 140, 5, 5);
	flappy.start();
}