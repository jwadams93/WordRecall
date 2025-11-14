#include <stdio.h>
#include <string.h>

/**
*
* I will want to take in user input
* search the dictionary to make sure the user provided word is valid
* and do this for an amount of time before ending the test and returning
* the users score. Maybe that score can be added to a file with a date
*
* I think I could know a word is ready to submit to the dictrionary when they separate with a new line character
*
* Need to prompt the user with a random letter at the start
**/ 

int main() {
  int alphabet[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
  
  buildWordSearchStructure();
}

//Linked list
typedef struct Node {
  //Word
  char *word;
  struct Node *next;
} Node;

int buildWordSearchStructure() {

  int wordC = 0;

  //file input text
  FILE *file = fopen("words_alpha.txt", "r");
  char buffer[35];

  if (file == NULL) {
    printf("Failed to open file.\n");
    return 1; 
  }

  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    printf("Read: %s", buffer);
    wordC++;
  }

  if(feof(file)) {
    printf("End of file reached.\n");
    printf("Found %d words\n", wordC);
  } else if (ferror(file)) {
    printf("An error occured.\n");
  }
  
  fclose(file);
  return 0;
}
