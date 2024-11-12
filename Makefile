hello:
	gcc main.c -o task_manager `pkg-config --cflags --libs gtk4`
