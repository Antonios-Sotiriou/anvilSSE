# Compile all c files to anvil executable!

CC = gcc
CFLAGS = -Werror -g# -pg -O2 -Os -Og -O0
OBJ = anvil
LINKS = -lX11 -lm
INTRINSICS = #-msse4.2 #-mavx2 #-mavx512f #-msse4.2 #-msse #-msse2 #-msse2avx #-msse3 -msse4 #-msse4.1 -msse4a -msse5 -msseregparm -mssse3 -msse-check=warning -msse2avx -march=CPU,i7 
FILENAME = collision_detection.c
DIRECTORY = c_files/

# install depended libraries.
install:
	./install.sh;

uninstall:
	sudo apt remove libx11-dev;

all:
	$(CC) $(CFLAGS)\
		$(DIRECTORY)main.c\
		$(DIRECTORY)locale.c\
		$(DIRECTORY)matrices.c\
		$(DIRECTORY)quaternions.c\
		$(DIRECTORY)kinetics.c\
		$(DIRECTORY)clipping.c\
		$(DIRECTORY)grafik_pipeline.c\
		$(DIRECTORY)shadow_pipeline.c\
		$(DIRECTORY)camera.c\
		$(DIRECTORY)vectors_math.c\
		$(DIRECTORY)scene_objects.c\
		$(DIRECTORY)general_functions.c\
		$(DIRECTORY)draw_functions.c\
		$(DIRECTORY)lighting.c\
		$(DIRECTORY)terrain_functions.c\
		$(DIRECTORY)physics.c\
		$(DIRECTORY)collision_detection.c\
		$(DIRECTORY)mesh_loader.c\
		$(DIRECTORY)database.c\
		$(DIRECTORY)bbox_loader.c\
		$(DIRECTORY)exec_time.c\
		$(DIRECTORY)test_shapes.c\
		$(DIRECTORY)logging.c\
		$(DIRECTORY)click_select.c\
		-o\
	$(OBJ) $(LINKS)\

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


