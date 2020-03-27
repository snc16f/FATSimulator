simulator: main.o commands.o fat32.o
	tar -xvf fat32.img.tar.gz && gcc -g -o simulator main.o commands.o fat32.o -lm

main.o: main.c
	gcc -c -g main.c -lm

commands.o: commands.c
	gcc -c -g commands.c -lm

fat32.o: fat32.c
	gcc -c -g fat32.c -lm

clean:
	rm *.o simulator fat32.img
