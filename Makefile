# Compile all c files to anvil executable!

CC = gcc
CFLAGS = -Wall -Werror -g# -pg -O2 -Os -Og -O0
OBJ = anvil
RM = rm
LINKS = -lX11 -lm
INTRINSICS = #-msse4.2 #-mavx2 #-mavx512f #-msse4.2 #-msse #-msse2 #-msse2avx #-msse3 -msse4 #-msse4.1 -msse4a -msse5 -msseregparm -mssse3 -msse-check=warning -msse2avx -march=CPU,i7 
FILENAME = kinetics.c

# install depended libraries.
install:
	./install.sh;

uninstall:
	sudo apt remove libx11-dev;

all:
	$(CC) $(CFLAGS) $(INTRINSICS) main.c locale.c matrices.c quaternions.c kinetics.c clipping.c grafik_pipeline.c shadow_pipeline.c frustum_map.c camera.c vectors_math.c world_objects.c general_functions.c draw_functions.c lighting.c obj_parser.c database.c exec_time.c test_shapes.c logging.c -o $(OBJ) $(LINKS);

exec:
	./$(OBJ)

# Create assembly code for the given file.
assembly:
	$(CC) -S $(FILENAME);

clean:
	sudo apt autoremove -y;

# Removes all assembly generated files.
clearAsm:
	rm *.s;

# Removes all Profiling generated files.
clearProf:
	rm *.out;

# Removes all Profiling generated files.
clearPerf:
	rm *.data*;


