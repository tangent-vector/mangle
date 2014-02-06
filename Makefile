CC = cc

mangle: source/*.c source/*.h
	$(CC) -o $@ -g -Iinclude source/*.c
