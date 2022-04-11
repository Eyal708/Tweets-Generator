#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000
#define PRINT_TWEET "Tweet %lu: "
#define INVALID_ARGS_MSG "Usage: seed, num of tweets, file_path, num of words"
#define INVALID_FILE_MSG "Error: problem reading file!"
#define ALLOCATION_FAIL_MSG "Allocation failure: problem allocating memory!"
#define BASE 10
#define EXIT_FAILURE 1
#define MIN_NUM_ARGS 4
#define MAX_NUM_ARGS 5

//each word has WordStruct of it's own
typedef struct WordStruct {
    char *word; //a pointer to the content of the word
    struct WordProbability *prob_list; //dynamic array of all possible words
    //after word
    //a parameter indicating how many times this word appears in text
    //... Add your own fields here
    int frequency; // how many times this word appears in text
    int capacity; //capacity of prob_list
} WordStruct;

//this is a property of WordStruct
typedef struct WordProbability {
    struct WordStruct *word_struct_ptr;//a pointer to the next possible word
    //... Add your own fields here
    int reps;
    //a parameter indicating how many times this word appears after word 1
} WordProbability;

/************ LINKED LIST ************/
typedef struct Node {
    WordStruct *data;
    struct Node *next;
} Node;

typedef struct LinkList {
    Node *first;
    Node *last;
    int size;
} LinkList;
/**
 * checks if a word ends with "."
 * @param word a pointer to the content of the word
 * @return 0 if word ends with ".", else 1
 */
/**

 *checks if malloc/realloc/calloc succeded. if not exits program
 * @param ptr a pointer to a memory slot that was allocated by ma/rea/ca lloc.
 */
void check_allocation_success (void *ptr)
{
  if (ptr == NULL)
    {
      printf (ALLOCATION_FAIL_MSG);
      exit (EXIT_FAILURE);

    }
}
int check_end_sentence (char *word)
{
  if (word[strlen (word) - 1] == '.')
    {
      return 0;
    }
  return 1;
}
/**
 * Create a word struct to a new word
 * @param word a word the Word Struct represents
 * @return a pointer to the new Word Struct
 */
WordStruct *create_word_struct (char *word)
{
  WordStruct *new_word = (WordStruct *) malloc (sizeof (WordStruct));
  new_word->word = malloc ((strlen (word) + 1) * sizeof (char));
  check_allocation_success (new_word->word);
  //check malloc success
  new_word->prob_list = NULL;
  strcpy (new_word->word, word);
  new_word->capacity = 0;
  new_word->frequency = 1;
  return new_word;
}
LinkList *create_link_list (void)
{
  LinkList *new_list = malloc (sizeof (LinkList));
  check_allocation_success (new_list);
  //check malloc success
  new_list->first = NULL;
  new_list->last = NULL;
  new_list->size = 0;
  return new_list;

}

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */


int add (LinkList *link_list, WordStruct *data)
{
  Node *new_node = malloc (sizeof (Node));
  if (new_node == NULL)
    {
      return 1;
    }
  *new_node = (Node) {data, NULL};

  if (link_list->first == NULL)
    {
      link_list->first = new_node;
      link_list->last = new_node;
    }
  else
    {
      link_list->last->next = new_node;
      link_list->last = new_node;
    }

  link_list->size++;
  return 0;
}
/**
 * checks if a word is in dictionary. if it is, updated frequency, else adds it
 * @param word a pointer to the content of the word
 * @param dict link list to check
 * @return a pointer to the word struct of word
 */

WordStruct *check_word_in_dict (char *word, LinkList *dict)
{
  int i = 0;
  Node *cur_node = dict->first;
  while (i < dict->size)
    {
      WordStruct *word_struct = cur_node->data;
      if (strcmp (word_struct->word, word) == 0)
        {
          word_struct->frequency++;
          return word_struct;
        }
      cur_node = cur_node->next;
      i++;
    }
  // if we got here, the word was not found in dict
  WordStruct *new_word = create_word_struct (word);
  add (dict, new_word);
  return new_word;

}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number (int max_number)
{
  int num = rand ();
  return (num % max_number);
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct *get_first_random_word (LinkList *dictionary)
{

  while (1)
    {
      int num = get_random_number (dictionary->size);
      Node *node = dictionary->first;
      for (int i = 0; i < num; i++)
        {
          node = node->next;
        }
      if (node->data->prob_list!=NULL)
        {
          return node->data;
        }
    }

}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word (WordStruct *word_struct_ptr)
{
  if (word_struct_ptr->prob_list == NULL)
    {
      return NULL;
    }
  int max_num = word_struct_ptr->capacity;//how many words are in prob_list
  int rand_num = get_random_number (word_struct_ptr->frequency);
  //a number in range [0,frequency)
  int low = 0, high = word_struct_ptr->prob_list[0].reps;
  int i = 0;
  for (; i < max_num - 1; i++)
    {
      if (low <= rand_num && rand_num <= high)
        {
          return word_struct_ptr->prob_list[i].word_struct_ptr;
        }
      int temp = high;
      low = high + 1;
      high = temp + word_struct_ptr->prob_list[i + 1].reps;
    }
  //if we got here, the last word in prob_list was chosen
  return word_struct_ptr->prob_list[i].word_struct_ptr;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence (LinkList *dictionary)
{
  WordStruct *cur_word = get_first_random_word (dictionary);
  int count = 0; //how many words in sentence
  while (count < MAX_WORDS_IN_SENTENCE_GENERATION && cur_word != NULL)
    {
      count++;
      printf ("%s ", cur_word->word);
      if (cur_word->prob_list == NULL)
        {
          return count;
        }
      //check if cur_word ends with '.' if so break loop
      cur_word = get_next_random_word (cur_word);
      //insert word to string

    }
  return count;
}
/**
 * checks if word2 is in word1 prob_list, if it's there adds one to reps.
 * @param word1 a pointer to WordStruct
 * @param word2 a pointer to WordStruct
 * @return 0 if word2 in in word1 prob_list, else 1
 */
int check_word_in_prob_list (WordStruct *word1, WordStruct *word2)
{
  for (int i = 0; i < word1->capacity; i++)
    {
      char *data = word1->prob_list[i].word_struct_ptr->word;
      if (strcmp (data, word2->word) == 0)
        {//word2 is in word1 prob_list
          word1->prob_list[i].reps++;
          return 0;
        }
    }
  return 1;
}

/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list (WordStruct *first_word,
                                  WordStruct *second_word)
{
  if (first_word->capacity == 0) //prob_list was empty
    {
      first_word->prob_list = malloc (sizeof (WordProbability));
      check_allocation_success (first_word->prob_list);
      //check malloc
      first_word->prob_list[0].word_struct_ptr = second_word;
      first_word->prob_list[0].reps = 1;
      first_word->capacity++;
      return 1;//word2 was not in word's 1 prob_list
    }
  //check if word2 is in word 1 prob_list
  if (check_word_in_prob_list (first_word, second_word) == 0)
    {
      //check_word_in_prob_list already updated probability value
      return 0;
    }
  first_word->capacity++;
  first_word->prob_list = realloc (first_word->prob_list,
                                   sizeof (WordProbability)
                                   * first_word->capacity);
  check_allocation_success (first_word->prob_list);
  //check realloc success
  //update attributes
  first_word->prob_list[(first_word->capacity)
                        - 1].word_struct_ptr = second_word;
  first_word->prob_list[(first_word->capacity) - 1].reps = 1;
  return 1;

}
/**
 * reads one lime from text for fill_dictionary
 * @param fp pinter to a file
 * @param words_to_read number of words left to read from the file
 * @param dictionary to fill
 * @return number of words that were read
 */
int read_line (FILE *fp, int words_to_read, LinkList *dictionary)
{
  //read line from file- save it on a string.
  char line[MAX_SENTENCE_LENGTH];//this will store one line from file
  if (fgets (line, MAX_SENTENCE_LENGTH, fp) == NULL)
    {//done reading file
      return -1;
    }
  char *cur_word;
  cur_word = strtok (line, " ,\r,\n");
  //now word contains the first word of the string
  WordStruct *cur_word_struct = check_word_in_dict (cur_word, dictionary);
  int count_words = 1; //counts how many word we already read
  while (count_words < words_to_read || words_to_read == -1)
    {
      char *next_word = strtok (NULL, " ,\r,\n");
      if (next_word == NULL)
        {
          return count_words;
          //we finished reading the line and need to call fgets again
        }

      WordStruct *next_word_struct =
          check_word_in_dict (next_word, dictionary);
      //now it holds a pointer to the struct of next word
      if (check_end_sentence (cur_word_struct->word) != 0)
        {
          add_word_to_probability_list (cur_word_struct, next_word_struct);
        }
      cur_word_struct = next_word_struct;

      count_words++;
    }
  return count_words;

}

/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */
void fill_dictionary (FILE *fp, int words_to_read, LinkList *dictionary)
{
  while (words_to_read > 0 || words_to_read == -1)
    {
      int words_read = read_line (fp, words_to_read, dictionary);
      if (words_read == -1)
        {
          //done reading file
          return;
        }
      if (words_to_read != -1)
        {
          words_to_read -= words_read;
        }
    }
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary (LinkList *dictionary)
{
  Node * node=dictionary->first;
  for(int i=0; i<dictionary->size;i++)
    {
      free(node->data->prob_list);
      free(node->data->word);
      free(node->data);
      Node *temp=node->next;
      free(node);
      node=temp;
    }
  free(dictionary);
}


/**
 * checks the validity of args given to the program
 * @param argc the number of args given
 * @return 0 if number of args is 4 or 5, else 1
 */
int check_args (int argc)
{
  if (argc != MIN_NUM_ARGS && argc != MAX_NUM_ARGS)
    {
      return 1;
    }
  return 0;

}

int main (int argc, char *argv[])
{
  if (check_args (argc) == 1)
    {
      printf (INVALID_ARGS_MSG);
      return EXIT_FAILURE;
    }
  FILE *fp = fopen (argv[3], "r");
  if (fp == NULL)
    {
      printf (INVALID_FILE_MSG);
      return EXIT_FAILURE;
    }

  LinkList *dict = create_link_list ();
  if (argc == 4)
    {
      fill_dictionary (fp, -1, dict);
    }
  else
    {
      long words_to_read = strtol (argv[4], NULL, BASE);
      fill_dictionary (fp, words_to_read, dict);
    }
  fclose (fp);//done reading file, now we close it
  long seed = strtol (argv[1], NULL, BASE);
  srand (seed);
  long tweets_num = strtol (argv[2], NULL, BASE);
  for (long i = 1; i <= tweets_num; i++)
    {
      printf (PRINT_TWEET, i);
      generate_sentence (dict);
      printf ("\n");
    }
  //now we can free dictionary
  free_dictionary (dict);
  dict = NULL;
  return 0;

}

