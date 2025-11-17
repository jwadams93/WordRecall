#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

//Linked list
typedef struct Node {
  char *word;
  struct Node *next;
} Node;

Node* alphabet[26] = {NULL};
int timeLeft = 60;
int running = 1;
char randomLetter;
int count = 0;

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

  Node *currentNode;
  char currentChar;
  int arrPos = 0;

  while (fgets(buffer, sizeof(buffer), file) != NULL) {

    if (buffer[0] == currentChar) {
      Node *nextNode = createNode(buffer);
      //printf("Created new node with word: %s in the %d index of the alphabet array\n", nextNode->word, arrPos);
      currentNode->next = nextNode;
      currentNode = nextNode;
      //alphabet[arrPos] = nextNode;
    } else {
      //New char, need to load these into next aray index
      currentChar = buffer[0];
      //calculating the position by subtracting a.
      //ASCII val works out to a-a = 0, b-a = 1, etc
      arrPos = buffer[0] - 'a';

      currentNode = createNode(buffer);
      //printf("Created new node with word: %s in the %d index of the alphabet array\n", nextNode->word, arrPos);
      alphabet[arrPos] = currentNode;
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
  randomLetter = rand() % (upper_bound - lower_bound + 1) + lower_bound;
  return randomLetter; 
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

void* timerThread(void* arg) {
  while (timeLeft > 0 && running) {
    sleep(1);
    timeLeft--;
  }
  running = 0;
  return NULL;
}

void search(struct Node* head, char* usersWord) {
  struct Node* current = head;
  printf("Okay well the first word is uh.. %s\n", head->word);
  printf("And the word we passed in is %s and we're gonna compare it to %s\n", usersWord, current->word);
  while (current != NULL) {
    //printf("The current word is %s", current->word);
    if (strcmp(current->word, usersWord) == 0) {
      printf("holy shit IT WORKED your word was VALID\n");
      count++;
    }
    current = current->next;
  }
  printf("okay so we looked for the word it had the right starting letter but we couldn't find it\n");
}

char* trimWhitespace(char* userInput) {
  char* end = userInput + strlen(userInput) - 1;
  while (end > userInput && isspace(*end)) {
    *end = '\0';
    end--;
  }
}

void displayTimer() {
  pthread_t timer;
  char input[100];

  printf("\nYou have 60 seconds... GO!\n");
  pthread_create(&timer, NULL, timerThread, NULL);

  while (running) {
    printf("%2d: ", timeLeft);
    fflush(stdout);

    if(fgets(input, sizeof(input), stdin) != NULL && running) {
      //fgets will include a \n in the buffer, following line removes it
      input[strcspn(input, "\n")] = 0;
      if (strlen(input) > 0 && running) {
        char *userInput = strtok(input, " ");
        trimWhitespace(userInput);

        while (userInput != NULL) {
          printf("TEST: found user input %s\n", userInput);
          if (userInput[0] == randomLetter) {
            //check the db
            printf("TEST: Word began with the right letter. Checkin' the ol db\n");
            printf("TEST: lets see Paul Allen's singley linked list: %s", alphabet[randomLetter - 'a']->word);
            search(alphabet[randomLetter - 'a'], userInput); 
          } else {
            // on to the next word
            printf("TEST: Word didn't begin with right letter. Not gonna do a damn thing.\n");
          }
          userInput = strtok(NULL, " ");
        }
      }
    }

    if (timeLeft <= 0) {
      break;
    }
  }
  
  running = 0;
  pthread_join(timer, NULL);
  printf("\nTimes up!\n");
  return 0;
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
  //
  //  Add logic to keep track of valid guesses and check them to prevent multiple of the same guess
  buildWordSearchStructure();
  buildWithSpinner();
  printf("\nYour letter is.....\n");
  usleep(900000);
  printf("\n\t%c\n", getRandLetter());
  
  displayTimer();

  printf("Looks like you typed %d words\n", count);
  return 0; }
