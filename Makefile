CC = cc

mangle: source/*.c source/*.h include/mangle/*.h
	$(CC) -o $@ -g -Iinclude source/*.c

mangle.sh: ./package.sh Makefile source/*.c source/*.h include/mangle/*.h
	./package.sh mangle Makefile source/*.c source/*.h include/mangle/*.h