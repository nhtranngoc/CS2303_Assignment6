#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// function prototype
int copyfile1(char* infilename, char* outfilename);

/** cptest.cpp
 * A file copying program.
 * Derived partially from caesar.cpp by Horstmann and Budd from big C++
 */

/**
   Prints usage instructions.
   @param program_name the name of this program
*/
void usage(char* program_name)
{  
  printf("Usage: %s infile outfile\n", program_name);
}

/**
   Prints file opening error message
   @param filename the name of the file that could not be opened
*/
void open_file_error(char* filename)
{  
  printf("Error opening file %s: %d: %s\n", filename, errno, strerror(errno));
}

/** Main program: copies a file.
    @param argc Number of command-line arguments (including program name).
    @param argv Array of pointers to character arrays holding arguments.
    @return 0 if successful, 1 if fail.
*/
int main(int argc, char* argv[])
{  
  char* infilename; // Names of files.
  char* outfilename;

  if (argc != 3) {
    usage(argv[0]); // Must have exactly 2 arguments.
    return 1;
  }

  infilename = argv[1];
  outfilename = argv[2];

  // Perform the copying
  int returnstatus = copyfile1(infilename, outfilename);
  
  return returnstatus;
}

/** Copies one file to another using formatted I/O, one character at a time.
 @param infilename Name of input file
 @param outfilename Name of output file
 @return 0 if successful, 1 if error.
*/
int copyfile1(char* infilename, char* outfilename) {
  FILE* infile; //File handles for source and destination.
  FILE* outfile;

  infile = fopen(infilename, "r"); // Open the input and output files.
  if (infile == NULL) {
    open_file_error(infilename);
    return 1;
  }

  outfile = fopen(outfilename, "w");
  if (outfile == NULL) {
    open_file_error(outfilename);
    return 1;
  }

  int intch;  // Character read from input file. must be an int to catch EOF.
  unsigned char ch; // Character stripped down to a byte.

  // Read each character from the file, checking for EOF.
  // Note: fgetc() returns unsigned char, cast to a signed int,
  // so the charaxcter is in the low-order byte of the int.
  while ((intch = fgetc(infile)) != EOF) {
    ch = (unsigned char) intch; // Convert to one-byte char.
    fputc(ch, outfile); // Write out.
  }

  // All done--close the files and return success code.
  fclose(infile);
  fclose(outfile);

  return 0; // Success!
}

int copyfile2(char* infilename, char* outfilename){
  int infile = open(infilename, O_RDONLY);
  if (infile < 0){
    printf("Error %d in cptest.c: %s\n", errno, strerror(errno));
    return 1;
  }
  int outfile= open(outfilename,  O_CREAT | O_WRONLY,
                                  S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

  if (outfile < 0){
    printf("Error %d in cptest.c: %s\n", errno, strerror(errno));
    return 1;
  }

  int byte;
  char buf[1024];
  while (byte = read(infile, buf, 1) > 0){
    write(outfile, buf, byte);
  }

  close(infile);
  close(outfile);
}

//Using copyfile2 to copy emacs-x 
// time ./cptest /usr/bin/emacs-x emacs-x1

// real  0m37.412s
// user  0m5.968s
// sys 0m27.806s

//Using copyfile1 to copy emacs-x