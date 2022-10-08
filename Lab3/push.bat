scp stetris.c pi@hostname:/home/pi/CODE/Tetris/stetris.c

TIMEOUT 1

scp Makefile pi@hostname:/home/pi/CODE/Tetris/Makefile

TIMEOUT 1

ssh pi@hostname "cd /home/pi/CODE/Tetris/; make clean; make;"
