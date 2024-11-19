hello:
	gcc src/* -w -o task_manager `pkg-config --cflags --libs gtk+-3.0`
