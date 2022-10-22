scp stetris.c pi@hostname:/home/pi/CODE/Tetris/stetris.c

@REM TIMEOUT 1

scp Makefile pi@hostname:/home/pi/CODE/Tetris/Makefile
@REM TIMEOUT 1

ssh pi@hostname "cd /home/pi/CODE/Tetris/; make clean; make;"

@REM TIMEOUT 1
@REM ssh pi@hostname "/home/pi/CODE/Tetris/stetris"
