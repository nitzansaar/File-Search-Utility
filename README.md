# Project 1: File Search Utility
This program searches for directories and files with optional search pattern matching. The search pattern matching can be either exact or partial. It works by specifying the directory for which to search and then recursively searches through that directory. By default it prints out all the directories and files it contains. However, the program takes command line arguments which allow the user to specify the following: 
"-d": Only display directories (no files).
"-e": Match search pattern exactly; no partial matches reported.
"-f": Only display files (no directories).
"-h": Display hidden files.
"-l depth-limit": Set a depth limit
"-H": Display help/usage information.
## Building
To build the program you can use the following command: gcc search.c -o search
## Running + Example Usage
To run the program you can specify the search directory and any additional options you want to use. For example if you are searching for a file that you remember contains the word 'hello' within a directory called 'my_directory' you can use the following command: ./search my_directory -f hello
## What I Learned
- Reading and understanding existing C code for file search and directory traversal.
- Working with command-line arguments and options in C programs using getopt.
- Implementing recursive directory traversal using the opendir and readdir functions.
- Filtering and matching files and directories based on user-defined options.
- Using bitwise flags to store multiple boolean options compactly.
- Memory management using dynamic memory allocation with malloc and free.