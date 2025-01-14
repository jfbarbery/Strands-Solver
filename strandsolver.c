#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_WORD_LENGTH 19
#define MAX_LETTER_SOLUTION 10
#define ROWS 8
#define COLS 6
#define NUM_DIGITS 7
#define NUM_WORDS 274927

typedef struct WordList WordList;

struct WordList {
	char** list;
	// first array is parallel to the list index, second array is the index of the characters in the
	// word, third array [0] is row, [1] is col of that letter
	int*** location_list;
	int size;
	int cap;
};

WordList* create_list();
void append_word(WordList* wl, char* string, int** selected);
void remove_word(WordList* wl, char* string);
void sort_words(WordList* wl);
void expand_list(WordList* wl);

char** read_board(char* argv[]);
char* read_word(FILE* file);
void strand_solver(char** board, FILE* words_ptr, FILE* offset_ptr);
void backtrack_find(char** board, int n, int r, int c, int** selected, FILE* words_ptr, FILE* offset_ptr);
char** glue_puzzle_pieces();
void backtrack_place(char** board);
int word_exists(char* target, FILE* words_ptr, FILE* offset_ptr);
int coords_in_selected(int r, int c, int** selected, int n);
void print_board(char** board, int** selected, int n);
char* get_selected_str(char** board, int** selected, int n);
char** set_colors();
void remove_nl(char* word);
int formatted_str_to_int(char* str);
int tentopower(int n);
void tolowercase(char* str);
void reset_selected(int** selected);

WordList* wl;
char** colors;

int main(int argc, char* argv[])
{
	wl = create_list();
	colors = set_colors();
	char** board = read_board(argv);
	FILE* words_ptr = fopen("./en.txt", "r");
	FILE* offset_ptr = fopen("./preprocessed.txt", "r");
	strand_solver(board, words_ptr, offset_ptr);
	for (int i = 0; i < ROWS; i++)
		free(board[i]);
	free(board);
	fclose(words_ptr);
	fclose(offset_ptr);
	//printf("num words found: %d\n", wl->size);
	for (int i = 0; i < wl->size; i++)
	{
		printf("%s\n", wl->list[i]);
		for (int j = 0; wl->location_list[i][j]; j++)
		{
			if (wl->list[i][j] == '\0') break;
			printf("%c %d %d\n", wl->list[i][j], wl->location_list[i][j][0], wl->location_list[i][j][1]);
		}
	}
	// Another round of backtracking to place the words on the board
	//char** final_board = glue_puzzle_pieces();
	// Memory leak bad!!!
	for (int i = 0; i < wl->cap; i++)
	{
		for (int j = 0; j < MAX_LETTER_SOLUTION; j++)
		{
			free(wl->location_list[i][j]);
		}
		free(wl->list[i]);
		free(wl->location_list[i]);
	}
	
	free(wl->list);
	free(wl->location_list);
	free(wl);
	return 0;
}

// Reads the command line Strands board parameter
char** read_board(char* argv[])
{
	int offset = ROWS;
	char** board = (char**) malloc(sizeof(char*) * ROWS);
	int i = 0;
	while (i < ROWS)
	{
		board[i] = (char*) malloc(sizeof(char) * (COLS+1));
		i++;
	}
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			board[r][c] = argv[0][(r*COLS)+c+offset];
		}
		tolowercase(board[r]);
		board[r][COLS] = '\0';
	}
	return board;
}

// Reads a single word from the file
char* read_word(FILE* file)
{
	char* word = (char*) malloc(sizeof(char) * (MAX_WORD_LENGTH+1));
	int i = 0;
	char ch;
	while ((ch = fgetc(file)) != '\n')
	{
		if (ch == EOF)
		{
			ch = '\0';
			break;
		}
		if (i >= MAX_WORD_LENGTH)
		{
			//printf("We have ran into a word that is the maximum length! This word might be too long.\n");
			ch = '\0';
			break;
		}
		word[i] = ch;
		i++;
	}
	word[i] = ch;
	return word;
}

// Takes in a 2D array of characters that represent a game of Strands.
// Returns a possible list of words that may be correct words in the game.
void strand_solver(char** board, FILE* words_ptr, FILE* offset_ptr)
{
	// Uses backtracking and binary searching in a dictionary to come up with possible words
	// For this purpose, a "full solution" is the case when a word is found in the dictionary
	print_board(board, NULL, 0);
	// Initialize selected letters in partial solution
	int** selected = (int**) malloc(sizeof(int*) * MAX_LETTER_SOLUTION); // Max selected letters
	for (int i = 0; i < MAX_LETTER_SOLUTION; i++)
	{
		selected[i] = (int*) malloc(sizeof(int) * 2); // Coordinate pair, row and col
	}
	
	// Set the starting letter and backtrack
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			selected[0][0] = r;
			selected[0][1] = c;
			//printf("Selected letter: %c\n", board[r][c]);
			//sleep(5);
			backtrack_find(board, 1, r, c, selected, words_ptr, offset_ptr);
			//printf("DONE! About to start another iteration...\n");
			reset_selected(selected);
		}
	}
	
	
	for (int i = 0; i < MAX_LETTER_SOLUTION; i++)
	{
		free(selected[i]);
	}
	free(selected);
}
// Find words on the board, where n is the # of selected letters, r and c are row and col,
// selected represents the locations of the n selected letters, and the FILE ptrs are for the dictionary lookup
void backtrack_find(char** board, int n, int r, int c, int** selected, FILE* words_ptr, FILE* offset_ptr)
{
	//sleep(1);
	// See if current partial solution could be a real word
	char* string = get_selected_str(board, selected, n);
	remove_nl(string);
	//printf("'%s'\n", string);
	int exists = word_exists(string, words_ptr, offset_ptr);
	// Minimum word count to be considered a word in strands
	if (strlen(string) > 3)
	{
		if (exists == 0)
		{
			return;
		}
		if (exists == 1)
		{
			append_word(wl, string, selected);
			print_board(board, selected, n);
		}
		else if (exists == 2)
		{
			//printf("We should keep trying more letters to see if there's a word here...\n");
		}
	}
	//printf("%s\n", string);
	
	// Base case
	if (n == MAX_LETTER_SOLUTION) return;
	
	// Try to go in every direction
	
	// North
	if (r-1 >= 0 && !coords_in_selected(r-1, c, selected, n))
	{
		//printf("n %d %d\n", r-1, c);
		selected[n][0] = r-1;
		selected[n][1] = c;
		backtrack_find(board, n+1, r-1, c, selected, words_ptr, offset_ptr);
	}
	// East
	if (c+1 < 6 && !coords_in_selected(r, c+1, selected, n))
	{
		//printf("e %d %d\n", r, c+1);
		selected[n][0] = r;
		selected[n][1] = c+1;
		backtrack_find(board, n+1, r, c+1, selected, words_ptr, offset_ptr);
	}
	// South
	if (r+1 < 8 && !coords_in_selected(r+1, c, selected, n))
	{
		//printf("s\n");
		selected[n][0] = r+1;
		selected[n][1] = c;
		backtrack_find(board, n+1, r+1, c, selected, words_ptr, offset_ptr);
	}
	// West
	if (c-1 >= 0 && !coords_in_selected(r, c-1, selected, n))
	{
		//printf("w\n");
		selected[n][0] = r;
		selected[n][1] = c-1;
		backtrack_find(board, n+1, r, c-1, selected, words_ptr, offset_ptr);
	}
	// North East
	if (r-1 >= 0 && c+1 < 6 && !coords_in_selected(r-1, c+1, selected, n))
	{
		//printf("ne\n");
		selected[n][0] = r-1;
		selected[n][1] = c+1;
		backtrack_find(board, n+1, r-1, c+1, selected, words_ptr, offset_ptr);
	}
	// South East
	if (r+1 < 8 && c+1 < 6 && !coords_in_selected(r+1, c+1, selected, n))
	{
		//printf("se\n");
		selected[n][0] = r+1;
		selected[n][1] = c+1;
		backtrack_find(board, n+1, r+1, c+1, selected, words_ptr, offset_ptr);
	}
	// South West
	if (r+1 < 8 && c-1 >= 0 && !coords_in_selected(r+1, c-1, selected, n))
	{
		//printf("sw\n");
		selected[n][0] = r+1;
		selected[n][1] = c-1;
		backtrack_find(board, n+1, r+1, c-1, selected, words_ptr, offset_ptr);
	}
	// North West
	if (r-1 >= 0 && c-1 >= 0 && !coords_in_selected(r-1, c-1, selected, n))
	{
		//printf("nw\n");
		selected[n][0] = r-1;
		selected[n][1] = c-1;
		backtrack_find(board, n+1, r-1, c-1, selected, words_ptr, offset_ptr);
	}
	//printf("end of all possible options for the path '%s'\n", string);
	free(string);
}

char** glue_puzzle_pieces()
{
	// Make a board to send in
	char** board = (char**) malloc(sizeof(char*) * ROWS);
	int i = 0;
	while (i < ROWS)
	{
		board[i] = (char*) malloc(sizeof(char) * (COLS+1));
		i++;
	}
	// Initialize the board
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			board[r][c] = ' ';
		}
		board[r][COLS] = '\0';
	}
	// Track which indexes on the board have been selected
	int** selected = (int**) malloc(sizeof(int*) * (ROWS * COLS));
	for (int i = 0; i < ROWS * COLS; i++)
	{
		// Create space for the row and col of this selected letter
		selected[i] = (int*) malloc(sizeof(int) * 2);
		// Initialize the indexes to invalid locations
		selected[i][0] = -1;
		selected[i][1] = -1;
	}
	print_board(board, selected, 0);
	// Iterate through all indexes and
	backtrack_place(board, selected);
}

void backtrack_place(char** board, int** selected, int n, int r, int c)
{
	// For each cardinal direction:
	// If not in selected, ATTEMPT to place a word here
	// Unplace word
	
	// Next direction
	
	// North
	if (r-1 >= 0 && !coords_in_selected(r-1, c, selected, n))
	{
		// Iterate through all the words that have a letter at this location
		for (int i = 0; i < num_qualifying_words; i++)
		{
			int word_len = 0; // filler code here
			int fits_flag = 1;
			// Check to see if entire word would fit
			for (int j = 0; j < word_len; j++)
			{
				if (!coords_in_selected(qualifying_word_locations[i][j][0], qualifying_word_locations[i][j][1], selected, n))
				{
					// This word will not fit
					fits_flag = 0;
					break;
				}
			}
			if (fits_flag)
			{
				// This word will fit
				// Go forward with this word
				place_word_in_selected(selected, qualifying_word_locations[i]);
				backtrack_place(board, selected, n+word_len, r-1, c);
				// You don't need the word indexes themselves since you know how long the word was
				// Just remove the most recent word_len indexes from selected
				remove_word_from_selected(word_len);
			}
		}
		
		
	}
}

// Checks the word list for the string parameter
int word_exists(char* target, FILE* words_ptr, FILE* offset_ptr)
{
	// Binary Search On Line Number
	int low = 0;
	int high = NUM_WORDS-1;
	int mid = (low+high)/2;
	// Get the middle offset by multiplying the middle line number * num digits per line (+1 for nl)
	int offset_for_offset = mid * (NUM_DIGITS+1);
	fseek(offset_ptr, offset_for_offset, SEEK_SET);
	char* offset = (char*) malloc(sizeof(char) * (NUM_DIGITS+2));
	fgets(offset, 9, offset_ptr);
	remove_nl(offset);
	int int_offset = formatted_str_to_int(offset);
	free(offset);
	fseek(words_ptr, int_offset, SEEK_SET);
	char* guess = (char*) malloc(sizeof(char) * 30);
	fgets(guess, 30, words_ptr);
	remove_nl(guess);
	int guesses = 0;
	int res = 0;
	int iterations = 0;
	while (1)
	{
		if (iterations > 50)
		{
			printf("error too many iterations\n");
			return 0;
		}
		iterations++;
		res = strcmp(target, guess);
		// Target is lexographically before this guess
		if (res < 0)
		{
			high = mid;
		}
		// Target is lexographically after this guess
		else if (res > 0)
		{
			low = mid;
		}
		// Found the word
		else
		{
			//printf("We found the word '%s'!\n", guess);
			free(guess);
			return 1;
		}
		int newmid = (low + high)/2;
		// We couldn't find the word
		if (mid == newmid)
		{
			// Try and look at the word after to see if it had any further spellings
			int offset_after_guess = (mid+1) * (NUM_DIGITS+1);
			fseek(offset_ptr, offset_after_guess, SEEK_SET);
			offset = (char*) malloc(sizeof(char) * 9);
			fgets(offset, 9, offset_ptr);
			remove_nl(offset);
			int int_offset = formatted_str_to_int(offset);
			free(offset);
			fseek(words_ptr, int_offset, SEEK_SET);
			fgets(guess, 30, words_ptr);
			remove_nl(guess);
			res = strcmp(target, guess);
			free(guess);
			// If the difference between the string compares is greater than 96,
			// that means that they were the same spelling but one of them had more letters
			// meaning there are still potential words to look for
			if (res < -96 || res > 96)
			{
				//printf("There is a word to keep looking for!\n");
				return 2;
			}
			//printf("There is no possible word that could be formed from this\n");
			return 0;
		}
		mid = newmid;
		// Go to the line number in our offset file
		offset_for_offset = mid * (NUM_DIGITS+1);
		fseek(offset_ptr, offset_for_offset, SEEK_SET);
		// Get the string version of the offset for the word
		offset = (char*) malloc(sizeof(char) * 9);
		fgets(offset, 9, offset_ptr);
		// Remove the newline from fgets
		remove_nl(offset);
		// Turn the word offset (a string of decimal digits) into an int type
		int int_offset = formatted_str_to_int(offset);
		free(offset);
		// Go to the offset in the words file
		fseek(words_ptr, int_offset, SEEK_SET);
		// Erase the last guess
		free(guess);
		guess = (char*) malloc(sizeof(char) * 30);
		fgets(guess, 30, words_ptr);
		remove_nl(guess);
		guesses++;
	}
	free(guess);
	printf("We got to the end of the function somehow\n");
	return 0;
}

// Returns 1 if r and c appear as a coordinate pair in selected, 0 otherwise
int coords_in_selected(int r, int c, int** selected, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (selected[i][0] == r && selected[i][1] == c) return 1;
	}
	return 0;
}

void print_board(char** board, int** selected, int n)
{
	printf("%s+", colors[4]);
	printf("%s-----------%s+\n", colors[5], colors[4]);
	for (int r = 0; r < ROWS; r++)
	{
		printf("%s|", colors[5]);
		for (int c = 0; c < COLS; c++)
		{
			int color_to_print = 6;
			if (n != 0 && coords_in_selected(r, c, selected, n)) color_to_print = 0;
			if (n != 0 && ( (selected[0][0] == r) && (selected[0][1] == c) ) ) color_to_print = 1;
			printf("%s%c", colors[color_to_print], board[r][c]);
			if (c == COLS - 1) continue;
			printf(" ");
		}
		printf("%s|", colors[5]);
		printf("\n");
	}
	printf("%s+", colors[4]);
	printf("%s-----------%s+\n", colors[5], colors[4]);
	char* str = get_selected_str(board, selected, n);
	free(str);
}

char* get_selected_str(char** board, int** selected, int n)
{
	char* str = (char*) malloc(sizeof(char) * (n+1));
	for (int i = 0; i < n; i++)
	{
		if (selected[i][0] == -1 || selected[i][1] == -1)
		{
			str[i] = '\0';
			return str;
		}
		str[i] = board[ (selected[i][0]) ][ (selected[i][1]) ];
	}
	str[n] = '\0';
	return str;
}

char** set_colors()
{
	char** colors = (char**) malloc(sizeof(char*) * 7);
	for (int i = 0; i < 7; i++)
	{
		colors[i] = (char*) malloc(sizeof(char) * 10);
	}
	strcpy(colors[0], "\033[31m"); // red
	strcpy(colors[1], "\033[32m"); // green
	strcpy(colors[2], "\033[33m"); // yellow
	strcpy(colors[3], "\033[34m"); // dark blue
	strcpy(colors[4], "\033[35m"); // purple
	strcpy(colors[5], "\033[36m"); // aqua
	strcpy(colors[6], "\033[37m"); // white
	return colors;
}

// Removes the newline character from the return of fgets
void remove_nl(char* word)
{
	if (word == NULL) return;
	int i = 0;
	while (word[i] != '\0')
	{
		if (word[i] == '\n')
		{
			word[i] = '\0';
			return;
		}
		i++;
	}
}

// Takes a string that represents an integer and returns the int primitive
int formatted_str_to_int(char* str)
{
	if (str == NULL) return 0;
	int ret = 0;
	long unsigned int len = strlen(str);
	int digits[len];
	for (int i = 0; i < len; i++)
	{
		int single_digit = (int)(str[i] - 48);
		digits[i] = single_digit;
		ret = ret + (digits[i] * (tentopower(len - i-1)));
	}
	return ret;
}

// Returns ten to power of n
int tentopower(int n)
{
	if (n == 0) return 1;
	int ret = 1;
	int i = 0;
	while (i < n)
	{
		ret *= 10;
		i++;
	}
	return ret;
}

void tolowercase(char* str)
{
	int i = 0;
	while (str[i])
	{
		if ( ((int)str[i]) > 64 && ((int)str[i]) < 91 )
		{
			str[i] = (char)(str[i]+32);
		}
		i++;
	}
}

void reset_selected(int** selected)
{
	for (int i = 0; i < MAX_LETTER_SOLUTION; i++)
	{
		selected[i][0] = -1;
		selected[i][1] = -1;
	}
}



WordList* create_list()
{
	WordList* wl = (WordList*) malloc(sizeof(WordList));
	int init_cap = 10;
	// Space for the list
	wl->list = (char**) malloc(sizeof(char*) * init_cap);
	// Space for the location of all words in the list
	wl->location_list = (int***) malloc(sizeof(int**) * init_cap);
	for (int i = 0; i < init_cap; i++)
	{
		// Space for this word
		wl->list[i] = (char*) malloc(sizeof(char) * MAX_LETTER_SOLUTION);
		// Space for all locations (on the board) of this word
		wl->location_list[i] = (int**) malloc(sizeof(int*) * MAX_LETTER_SOLUTION);
		// Space for the row and col indexes of this letter
		for (int j = 0; j < MAX_LETTER_SOLUTION; j++)
		{
			wl->location_list[i][j] = (int*) malloc(sizeof(int) * 2);
		}
	}
	wl->size = 0;
	wl->cap = init_cap;
	return wl;
}

void append_word(WordList* wl, char* string, int** selected)
{
	if (string == NULL)
	{
		printf("Error appending word to list: string null\n");
		exit(1);
	}
	if (wl == NULL)
	{
		printf("Error appending word to list: list null\n");
		exit(1);
	}
	if (wl->size == wl->cap)
	{
		expand_list(wl);
	}
	strcpy(wl->list[wl->size], string);
	int word_len = strlen(string);
	for (int i = 0; i < word_len; i++)
	{
		wl->location_list[wl->size][i][0] = selected[i][0];
		wl->location_list[wl->size][i][1] = selected[i][1];
	}
	wl->size = wl->size + 1;
}

void remove_word(WordList* wl, char* string)
{
	if (string == NULL)
	{
		printf("Error removing word from list: string null\n");
		exit(1);
	}
	if (wl == NULL)
	{
		printf("Error removing word from list: list null\n");
		exit(1);
	}
	for (int i = 0; i < wl->size; i++)
	{
		if (strcmp(wl->list[i], string) == 0)
		{
			for (int j = i; j < wl->size - 1; j++)
			{
				wl->list[j] = wl->list[j+1];
				wl->location_list[j] = wl->location_list[j+1];
			}
			wl->size = wl->size - 1;
		}
	}
}

void sort_words(WordList* wl)
{
	
}

void expand_list(WordList* wl)
{
	if (wl == NULL)
	{
		wl = create_list();
		return;
	}
	int new_cap = (wl->cap * 2) + 1;
	//printf("expanding from %d to %d\n", wl->cap, new_cap);
	char** new_list = (char**) malloc(sizeof(char*) * new_cap);
	int*** new_location_list = (int***) malloc(sizeof(int**) * new_cap);
	for (int i = 0; i < new_cap; i++)
	{
		new_list[i] = (char*) malloc(sizeof(char) * MAX_LETTER_SOLUTION);
		new_location_list[i] = (int**) malloc(sizeof(int*) * new_cap);
		for (int j = 0; j < MAX_LETTER_SOLUTION; j++)
		{
			new_location_list[i][j] = (int*) malloc(sizeof(int) * 2);
		}
	}
	for (int i = 0; i < wl->size; i++)
	{
		strcpy(new_list[i], wl->list[i]);
		free(wl->list[i]);
		// Copy the locations from the old list to the new one
		int word_len = strlen(new_list[i]);
		for (int j = 0; j < word_len; j++)
		{
			new_location_list[i][j][0] = wl->location_list[i][j][0];
			new_location_list[i][j][1] = wl->location_list[i][j][1];
			free(wl->location_list[i][j]);
		}
		free(wl->location_list[i]);
	}
	free(wl->list);
	free(wl->location_list);
	wl->list = new_list;
	wl->location_list = new_location_list;
	wl->cap = new_cap;
}