#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <libgen.h>


// function prototype
int copyfile1(char* infilename, char* outfilename);
struct timeval * gettimeelapsed(struct timeval * start, struct timeval * end);
void printtime(struct timeval * tm);

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
  printf("Usage: %s infile outfile functionmode buffersize\n", program_name);
  printf("functionmode must be between 1 and 3.\n");
  printf("buffersize is only applicable with function mode 3.\n");
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
  int functionmode = 1;
  int bufSize = 2048;
  int returnVal=-1;
  struct stat sb;
  struct timeval * tmStart = (struct timeval*)malloc(sizeof(struct timeval));
  struct timeval * tmEnd = (struct timeval*)malloc(sizeof(struct timeval));
  struct timeval * tmDiff = (struct timeval*)malloc(sizeof(struct timeval));

  if (argc < 4) {
    usage(argv[0]); // Must have exactly 2 arguments.
    return 1;
  }

  infilename = argv[1];
  outfilename = argv[2];
  functionmode = atoi(argv[3]);
  // bufSize = atoi(argv[4]);

  if((functionmode>3)|(functionmode<1)){
    usage(argv[0]);
    return 1;
  }

 if (functionmode == 3 && argc == 5){
    bufSize = atoi(argv[4]);
  }

  char* outdir;
  char* filename;
  if (stat(argv[2], &sb) == 0 && S_ISDIR(sb.st_mode)){
    // printf("It's a directory!\n");
    outdir = argv[2];
    filename = basename(infilename);
    outfilename = (char*)malloc(sizeof(char) * (strlen(outdir) + strlen(filename) + 2));
    strcat(outfilename, outdir);
    strcat(outfilename, "/");
    strcat(outfilename, filename);
    strcat(outfilename, "\0");
  } else {
    printf("It's a-me, Mario!\n");
    outfilename = argv[2];
  }

  gettimeofday(tmStart, NULL);

  // Perform the copying
  switch (functionmode){
    case 1:
        returnVal = copyfile1(infilename, outfilename);
        break;
    case 2:
        returnVal = copyfile2(infilename, outfilename);
        break;
    case 3:
        returnVal = copyfile3(infilename, outfilename, bufSize);
        break;
    }
    gettimeofday(tmEnd, NULL);
    tmDiff = gettimeelapsed(tmStart, tmEnd);

    printf("Time started: ");
    printtime(tmStart);

    printf("Time ended: ");
    printtime(tmEnd);

    printf("Time elapsed: ");
    printtime(tmDiff);
    free(tmStart);
    free(tmEnd);
    free(tmDiff);
  return returnVal;
}

void printtime(struct timeval * tm){
  printf("%ld seconds, %ld microseconds\n", tm->tv_sec, tm->tv_usec);
}

struct timeval * gettimeelapsed(struct timeval * start, struct timeval * end){
    struct timeval * diff = (struct timeval *)malloc(sizeof(struct timeval));
    diff->tv_sec = end->tv_sec - start->tv_sec;
    diff->tv_usec = end->tv_usec - start->tv_usec;
    
    //Handle borrowing.
    if (diff->tv_usec < 0){
        diff->tv_sec -= 1;
        diff->tv_usec += 1000000;
    }

    return diff;
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

/** Copies one file to another using open, close, one character at a time.
 @param infilename Name of input file
 @param outfilename Name of output file
 @return 0 if successful, 1 if error.
*/
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

/** Copies one file to another using open, close, with variable buffer size.
 @param infilename Name of input file
 @param outfilename Name of output file
 @param bufSize Size of buffer to use.
 @return 0 if successful, 1 if error.
*/
int copyfile3(char* infilename, char* outfilename, int bufSize){
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
  char buf[bufSize];
  while (byte = read(infile, buf, bufSize) > 0){
    write(outfile, buf, byte);
  }

  close(infile);
  close(outfile);
}

//Using copyfile1 to copy emacs-x
// time ./cptest /usr/bin/emacs-x emacs-x1

// real  0m0.215s
// user  0m0.140s
// sys 0m0.012s

//Using copyfile2 to copy emacs-x 
// time ./cptest /usr/bin/emacs-x emacs-x1

// real  0m37.412s
// user  0m5.968s
// sys 0m27.806s

//Using copyfile3 to copy emacs-x
// time ./cptest /usr/bin/emacs-x emacs-x1

// real  0m0.048s
// user  0m0.008s
// sys 0m0.032s