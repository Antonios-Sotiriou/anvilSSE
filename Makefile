# Compile all c files to anvil executable!

CC = gcc
CFLAGS = -Werror -g# -pg -O2 -Os -Og -O0
OBJ = anvil
LINKS = -lX11 -lm -lGLEW -lGLU -lGL -I "/usr/include/postgresql" -lpq
INTRINSICS = #-msse4.2 #-mavx2 #-mavx512f #-msse4.2 #-msse #-msse2 #-msse2avx #-msse3 -msse4 #-msse4.1 -msse4a -msse5 -msseregparm -mssse3 -msse-check=warning -msse2avx -march=CPU,i7 
FILENAME = matrices.c
DIRECTORY = source/

# install depended libraries.
install:
	./install.sh;

uninstall:
	sudo apt remove libx11-dev;

all:
	$(CC) $(CFLAGS)\
		main.c\
		$(DIRECTORY)shaders/mainShader.c\
		$(DIRECTORY)shaders/shadowShader.c\
		$(DIRECTORY)shaders/displayShader.c\
		$(DIRECTORY)shaders/clickSelectShader.c\
		$(DIRECTORY)components/matrices.c\
		$(DIRECTORY)components/quaternions.c\
		$(DIRECTORY)components/kinetics.c\
		$(DIRECTORY)components/camera.c\
		$(DIRECTORY)components/vectors_math.c\
		$(DIRECTORY)components/scene_objects.c\
		$(DIRECTORY)components/general_functions.c\
		$(DIRECTORY)components/terrain_functions.c\
		$(DIRECTORY)components/physics.c\
		$(DIRECTORY)components/collision_detection.c\
		$(DIRECTORY)components/mesh_loader.c\
		$(DIRECTORY)components/database.c\
		$(DIRECTORY)components/bbox_loader.c\
		$(DIRECTORY)components/exec_time.c\
		$(DIRECTORY)components/test_shapes.c\
		$(DIRECTORY)components/logging.c\
		$(LINKS)\
	    -o $(OBJ)\

exec:
	./$(OBJ)

# Create assembly code for the given file.
assembly:
	$(CC) -S $(DIRECTORY)$(FILENAME);

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


