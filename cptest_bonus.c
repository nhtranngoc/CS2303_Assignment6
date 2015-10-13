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
#include <ctype.h>

#define MAX_FILE_LENGTH 128
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
void usage(char* program_name){
  printf("Usage: %s infile outfile functionmode buffersize\n", program_name);
  printf("functionmode must be between 1 and 3.\n");
  printf("buffersize is only applicable with function mode 3.\n");
}

/**
   Prints file opening error message
   @param filename the name of the file that could not be opened
*/
void open_file_error(char* filename){  
  printf("Error opening file %s: %d: %s\n", filename, errno, strerror(errno));
}

/** Main program: copies a file.
    @param argc Number of command-line arguments (including program name).
    @param argv Array of pointers to character arrays holding arguments.
    @return 0 if successful, 1 if fail.
*/
int main(int argc, char** argv){  
  int functionmode = 1;
  int bufSize = 1;
  int returnVal=-1;
  char* infilename; // Names of files.
  char* outfilename;
  struct timeval * tmStart = (struct timeval*)malloc(sizeof(struct timeval));
  struct timeval * tmEnd = (struct timeval*)malloc(sizeof(struct timeval));
  struct timeval * tmDiff = (struct timeval*)malloc(sizeof(struct timeval));

  if (argc < 2){
    usage(argv[0]); // Must have exactly 2 arguments.
    return 1;
  }

  int opt;
  int index=0;
  char *cvalue = (char *)malloc(sizeof(char));
  while((opt = getopt(argc, argv, "b:c:")) != -1)
    switch(opt){
      case 'c': 
        functionmode = atoi(optarg); 
        printf("Mode C\n");
      break;

      case 'b': 
        printf("Mode B\n");
        bufSize = atoi(optarg); 
      break;
      
      case '?':
        if ((optopt == 'c') | (optopt == 'b'))
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
        return 1;

      default:
        abort();
    }
  char** argList = (char**)malloc(10 * sizeof(char*));
  int count=0;
  for (index = optind; index < argc; index++){
    argList[count] = (char*)malloc(MAX_FILE_LENGTH * sizeof(char));
    argList[count] = argv[index];
    // printf("Argument: %s\n", argList[count]);
    count++;
  }
  printf("Count: %d\n", count);
  outfilename=argList[count-1];
  printf("outfilename: %s\n", outfilename);
  // generateOutName(infilename, outfilename);
  gettimeofday(tmStart, NULL);
  // Perform the copying
  // returnVal = copy();
  int i;
  struct stat sb;
  char* outdir;
  char* filename;
  for(i=0;i<count-1;i++){
    char* outfilenameFinal;
    infilename=argList[i];
    printf("infilename: %s\n", infilename);
    if (stat(outfilename, &sb) == 0 && S_ISDIR(sb.st_mode)){
    // printf("It's a directory!\n");
      outdir = outfilename;
      filename = basename(infilename);
      outfilename = (char*)malloc(sizeof(char) * (strlen(outdir) + strlen(filename) + 2));
      strcat(outfilename, outdir);
      strcat(outfilename, "/");
      strcat(outfilename, filename);
      strcat(outfilename, "\0");
      returnVal=copy(infilename, outfilename, functionmode, bufSize);
    }
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

/**
 * @brief Print time from timeval struct.
 * @param timeval Struct to print time.
 */
void printtime(struct timeval * tm){
  printf("%ld seconds, %ld microseconds\n", tm->tv_sec, tm->tv_usec);
}

/**
 * @brief Calculate time elapsed and put it to a timeval struct
 * @param start Starting timeval 
 * @param end Ending timeval
 * @return A timeval struct containing difference in time interval.
 */
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

/**
 * @brief Copy based on function mode.
 * @param infilename Pointer to input file name
 * @param outfilename Pointer to output file name
 * @param functionmode Operating mode
 * @param bufSize Size of buffer
 * @return Status value
 */
int copy(char* infilename, char* outfilename, int functionmode, int bufSize){
  int returnVal;
  switch(functionmode){
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
}

/** Copies one file to another using formatted I/O, one character at a time.
 @param infilename Name of input file
 @param outfilename Name of output file
 @return 0 if successful, 1 if error.
*/
int copyfile1(char* infilename, char* outfilename){
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