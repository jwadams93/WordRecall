#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Linked list
typedef struct Node {
  char *word;
  struct Node *next;
} Node;

Node* alphabet[26] = {NULL};

Node *createNode(char *word) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->word = strdup(word);
  newNode->next = NULL;

  return newNode;
}

int buildWordSearchStructure() {

  //file input text
  FILE *file = fopen("words_alpha.txt", "r");
  char buffer[35];

  if (file == NULL) {
    printf("Failed to open file.\n");
    return 1;
  }

  Node *temp = createNode(" ");
  char currentChar = 'a';
  int arrPos = 0;

  while (fgets(buffer, sizeof(buffer), file) != NULL) {

      //printf("Read: %s\n", buffer);
      //printf("First position of buffer: %c\n", buffer[0]);
      //printf("buffer[0] = '%c' (ASCII: %d)\n", buffer[0], buffer[0]);

    if (buffer[0] == currentChar) {
      Node *nextNode = createNode(buffer);
      //printf("Created new node with word: %s in the %d index of the alphabet array\n", nextNode->word, arrPos);
      temp = nextNode->next;
      alphabet[arrPos] = nextNode;
    } else {
      //New char, need to load these into next aray index
      currentChar = buffer[0];
      //calculating the position by subtracting a.
      //ASCII val works out to a-a = 0, b-a = 1, etc
      arrPos = buffer[0] - 'a';

      Node *nextNode = createNode(buffer);
      //printf("Created new node with word: %s in the %d index of the alphabet array\n", nextNode->word, arrPos);
      temp = nextNode->next;
      alphabet[arrPos] = nextNode;
    }
  }

  if(feof(file)) {
    //printf("End of file reached.\n");
  } else if (ferror(file)) {
    printf("An error occured.\n");
  }
  
  fclose(file);
  return 0;
}

char getRandLetter() {
  //Could just gen a random number from .. ascii a to ascii z?
  // then translate that int into a char
  int lower_bound = 97;
  int upper_bound = 122;

  // mod the random number to get a number between [0, 26] (rand % (122-97+1 = 26))
  // then, adding the lower_bound shifts it to [97, 123]
  return rand() % (upper_bound - lower_bound + 1) + lower_bound;

}

void buildWithSpinner() {
  char spinChars[] = {'|', '/', '-', '\\'};
  int spinIdx = 0;

  //hide cursor
  printf("\033[?25l");

  for (int i = 0; i < 50; i++) {
    printf("\r%c Building word database... %d%%", spinChars[spinIdx % 4], (i * 100) / 50);
    fflush(stdout);
    usleep(90000);
    spinIdx++;
  }
  printf("\r######## Database built! ########\n");
}

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
  srand(time(NULL));
  //TODO 
  //  Add logic to handle user input 
  //  Add timer that will automatically end the game 
  //    when it runs out of time
  //  Add logic to score player (take each entered word and validate its in the wordSearchStructure)
  //  Add logic display the score (Maybe use cool ascii art thing) and save score to DB file 
  buildWithSpinner();
  buildWordSearchStructure();
  printf("\nYour letter is.....\n");
  printf("\n%c!\n", getRandLetter());
  return 0;
}
