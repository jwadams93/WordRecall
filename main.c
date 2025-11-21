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

int checkScores() {
  FILE *scoreFile = fopen("high_scores.txt", "r+");
  char scoreBuffer[35];

  printf("Checking score...");
  
  if (scoreFile == NULL) {
    printf("Failed to open or create the high scores file.\n");
    return 1;
  }

  int arrPos = 0;
  int* scoreAlphabet[26] = {0};
  bool foundOldScore = false;
  int i = 0;

  while (fgets(scoreBuffer, sizeof(scoreBuffer), scoreFile) != NULL) {
    printf("am I even in here? HELLO?\n");
    //load the scoreAlphabet arr to be used to populate the high score file
    //trimWhitespace(buffer);
    arrPos = scoreBuffer[0] - 'a';
    int actualValue = scoreBuffer[3] - '0';

    printf("scorebuffer[0]: %c == randomLetter: %c is evaluating to: %b", scoreBuffer[0], randomLetter, scoreBuffer == randomLetter);
    printf("count: %d > actualValue %d is evaluating to: %b", count, actualValue, count > actualValue);
   
    printf("scorebuffer[0] = '%c' (ASCII: %d)\n", scoreBuffer[0], scoreBuffer[0]);
    printf("randomLetter = '%c' (ASCII: %d)\n", randomLetter, randomLetter);
    printf("Are they equal? %d\n", scoreBuffer[0] == randomLetter);

    printf("Found this in buffer[3]: %d for buffer[0] %d\n", actualValue, scoreBuffer[0]);
    if (scoreBuffer[0] == randomLetter) {
      printf("Found entry for %s, writing new score of %d in place of buffer[3]\n", scoreBuffer[0], count, actualValue);
      if (count > actualValue) {
        scoreAlphabet[arrPos] = count;
      }
      foundOldScore = true;
    } else {
      scoreAlphabet[arrPos] = actualValue;
    }
  }


  printf("Found.... nothing in the file??? I guess????\n");

  fflush(scoreFile);
  while (i < 26) {
    printf("Building high score file\n");
    int score;
    if (scoreAlphabet[i] != 0) {
      printf("Found scoreAlphabet[%d] of %d\n", i, scoreAlphabet[i]);
      score = (int) scoreAlphabet[i];
    } else {
      printf("Found scoreAlphabet[%d] of null I guess?\n", i, scoreAlphabet[i]);
      score = 0;
    }
    printf("Attempting to write to the file with values i + 'a' == %d and score == %d\n", i + 'a', score);

    fprintf(scoreFile, "%c\t %d\n", i + 'a', score);
    i++;
  }
  
  if(feof(scoreFile)) {
    if(!foundOldScore){
      printf("Updated high score for: %c\n", randomLetter);
    }
  } else if (ferror(scoreFile)) {
    printf("An error occured.\n");
  }

  fclose(scoreFile);
  return 0;
}

int main() {
  srand(time(NULL));

  //TODO 
  //  Add logic to score player (take each entered word and validate its in the wordSearchStructure)
  buildWordSearchStructure();
  printf("\nYour letter is.....\n");
  usleep(900000);
  printf("\n\t%c\n", getRandLetter());
  
  displayTimer();

  printf("Looks like you typed %d words\n", count);

  //write to a high score file.
  //Should probably read the scores in, and for the current letter, update if new score > old score
  checkScores();
  return 0; 
}
