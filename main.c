#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>

#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_GREEN  "\x1b[32m"
#define ANSI_COLOR_RESET  "\x1b[0m"

//Linked list
typedef struct Node {
  char *word;
  struct Node *next;
} Node;

Node* alphabet[26] = {NULL};
char* usedWords[50];
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

char* toLowerString(char* userInput) {
  int i = 0;
  while (userInput[i] != '\0') {
    userInput[i] = tolower(userInput[i]);
    i++;
  }
}

char* trimWhitespace(char* userInput) {
  char* end = userInput + strlen(userInput) - 1;
  while (end > userInput && isspace(*end)) {
    *end = '\0';
    end--;
  }
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

    trimWhitespace(buffer);
    if (buffer[0] == currentChar) {
      Node *nextNode = createNode(buffer);
      currentNode->next = nextNode;
      currentNode = nextNode;
    } else {
      currentChar = buffer[0];

      //calculating the position by subtracting a.
      //ASCII val works out to a-a = 0, b-a = 1, etc
      arrPos = buffer[0] - 'a';
      currentNode = createNode(buffer);
      alphabet[arrPos] = currentNode;
    }
  }

  if(feof(file)) {
    printf("Lets play!.\n");
  } else if (ferror(file)) {
    printf("An error occured.\n");
  }
  
  fclose(file);
  return 0;
}

char getRandLetter() {
  int lower_bound = 97;
  int upper_bound = 122;

  // mod the random number to get a number between [0, 26] (rand % (122-97+1 = 26))
  // then, adding the lower_bound shifts it to [97, 123]
  randomLetter = rand() % (upper_bound - lower_bound + 1) + lower_bound;
  return randomLetter; 
}

//TODO Not needed.
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
  bool alreadyGuessed;

  //TODO Honestly might be worth adding the alreadyGuessed check here, so I 
  //don't check O(n) entries of the linked list before finding the word was already guessed

  while (current != NULL) {
    alreadyGuessed = false; 
    if (strcmp(current->word, usersWord) == 0) {
      
      for (int i = 0; i < count; i++) {
        if (strcmp(usersWord, usedWords[i]) == 0) {
          printf(ANSI_COLOR_RED "%s was already guessed! Try again.\n" ANSI_COLOR_RESET, usersWord);
          alreadyGuessed = true;
          break;
        }
      }

      if (!alreadyGuessed) {
        printf(ANSI_COLOR_GREEN "Valid word!\n" ANSI_COLOR_RESET);
        count++;
        // Bug existed here. Original code set usedWords[0] to usersWord
        // which pointed to the same buffer each time.
        // So after one duplicate guess, every guess afterwards
        // existed in the usedWords array. Fix: use strdup to create a copy
        usedWords[count - 1] = strdup(usersWord);
      }
    }
    current = current->next;
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
        toLowerString(userInput);
        trimWhitespace(userInput);

        while (userInput != NULL) {
          if (userInput[0] == randomLetter) {
            //check the db
            search(alphabet[randomLetter - 'a'], userInput); 
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
}

int main() {
  srand(time(NULL));

  //TODO 
  //  Add logic to score player (take each entered word and validate its in the wordSearchStructure)
  //  Add logic display the score (Maybe use cool ascii art thing) and save score to DB file 
  //
  //  Add logic to keep track of valid guesses and check them to prevent multiple of the same guess
  buildWordSearchStructure();
  printf("\nYour letter is.....\n");
  usleep(900000);
  printf("\n\t%c\n", getRandLetter());
  
  displayTimer();

  printf("Looks like you typed %d words\n", count);
  return 0; 
}
