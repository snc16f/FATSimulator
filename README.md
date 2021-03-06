# FATSimulator
Objective/Project Implementation :
The purpose of this project is to design and implement a simple, user-space, shell-like utility that is capable of interpreting a FAT32 file system image.

Compilation:
To compile the project follow the commands below:

/> make
/> ./simulator fat32.img

When running the make command, the executable for our project is created. In this case it is named 'simulator'. To run the program correctly, include the image file as a command line argument - fat32.img.

File Listing:
1) commands.c - This file essentially contains all of the function definitions for all of the 14 main commands we need to implement.
2) commands.h - This file contains all of the function declarations for each of the 14 main commands. It also includes the declarations for the extra credit commands and any other functions used throughout the project.
3) fat32.c - This file contains all of the function definitions for any functions that require the use of reading and using the fat32.img.
4) main.c - This file is responsible for carrying out the main shell-like utility. It reads the input from the command like and also reads in the fat32.img file that we are to interpret.

Division of Labor:
The members of this project are Samantha Chaves, Tyler Gurley, and Justin Richman. We made an effort to meet as a group and work on the project together. Each group member put in an equal amount of work.


Bugs:
No known bugs.

Non-implemented commands due to some time constraints and COVID-19:
   - mv
   - cp
   - write


GitHub Public Repository:
https://github.com/snc16f/FATSimulator
