#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_WORD_LENGTH 19
#define MAX_LETTER_SOLUTION 10
#define ROWS 8
#define COLS 6

char** read_board(char* argv[]);
char* read_word(FILE* file);
char** strand_solver(char** board);
void backtrack(char** board, char** colors, int n, int r, int c, int** selected);
int word_exists(char* string);
int coords_in_selected(int r, int c, int** selected, int n);
void print_board(char** board, char** colors, int** selected, int n);
char* get_selected_str(char** board, int** selected, int n);
char** set_colors();

int main(int argc, char* argv[])
{
	char** board = read_board(argv);
	strand_solver(board);
	for (int i = 0; i < ROWS; i++)
		free(board[i]);
	free(board);
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
		board[r][ROWS] = '\0';
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
			printf("We have ran into a word that is the maximum length! This word might be too long.\n");
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
char** strand_solver(char** board)
{
	char** colors = set_colors();
	
	// Use backtracking to come up with possible words.
	// As a rule of thumb, don't try to come up with
	// words that are longer than 10 letters, and
	// use lexicographical sorting as you're adding
	// letters on the words. Meaning do a binary search
	// for the letter you're on, comparing it to the word
	// list.
	print_board(board, colors, NULL, 0);
	// Initialize selected letters in partial solution
	int** selected = (int**) malloc(sizeof(int*) * MAX_LETTER_SOLUTION); // Max selected letters
	for (int i = 0; i < MAX_LETTER_SOLUTION; i++)
	{
		selected[i] = (int*) malloc(sizeof(int) * 2); // Coordinate pair, row and col
	}
	backtrack(board, colors, 0, 0, 0, selected);
	for (int i = 0; i < MAX_LETTER_SOLUTION; i++)
	{
		free(selected[i]);
	}
	free(selected);
	for (int i = 0; i < 7; i++)
	{
		free(colors[i]);
	}
	free(colors);
	return NULL;
}

void backtrack(char** board, char** colors, int n, int r, int c, int** selected)
{
	sleep(0.5);
	print_board(board, colors, selected, n);
	// Here is where we try and see if our current partial solution could be a real word
	char* string = get_selected_str(board, selected, n);
	if (word_exists(string))
	{
		printf("Could this be a word? |%s|\n", string);
	}
	
	free(string);
	// Base case
	if (n == MAX_LETTER_SOLUTION) return;
	
	// Try to go in every direction
	
	// North
	if (r-1 >= 0 && !coords_in_selected(r-1, c, selected, n))
	{
		selected[n][0] = r-1;
		selected[n][1] = c;
		backtrack(board, colors, n+1, r-1, c, selected);
	}
	// East
	if (c+1 < 6 && !coords_in_selected(r, c+1, selected, n))
	{
		selected[n][0] = r;
		selected[n][1] = c+1;
		backtrack(board, colors, n+1, r, c+1, selected);
	}
	// South
	if (r+1 < 8 && !coords_in_selected(r+1, c, selected, n))
	{
		selected[n][0] = r+1;
		selected[n][1] = c;
		backtrack(board, colors, n+1, r+1, c, selected);
	}
	// West
	if (c-1 >= 0 && !coords_in_selected(r, c-1, selected, n))
	{
		selected[n][0] = r;
		selected[n][1] = c-1;
		backtrack(board, colors, n+1, r, c-1, selected);
	}
	// North East
	if (r-1 >= 0 && c+1 < 6 && !coords_in_selected(r-1, c+1, selected, n))
	{
		selected[n][0] = r-1;
		selected[n][1] = c+1;
		backtrack(board, colors, n+1, r-1, c+1, selected);
	}
	// South East
	if (r+1 < 8 && c+1 < 6 && !coords_in_selected(r+1, c+1, selected, n))
	{
		selected[n][0] = r+1;
		selected[n][1] = c+1;
		backtrack(board, colors, n+1, r+1, c+1, selected);
	}
	// South West
	if (r+1 < 8 && c-1 >= 0 && !coords_in_selected(r+1, c-1, selected, n))
	{
		selected[n][0] = r+1;
		selected[n][1] = c-1;
		backtrack(board, colors, n+1, r+1, c-1, selected);
	}
	// North West
	if (r-1 >= 0 && c-1 >= 0 && !coords_in_selected(r-1, c-1, selected, n))
	{
		selected[n][0] = r-1;
		selected[n][1] = c-1;
		backtrack(board, colors, n+1, r-1, c-1, selected);
	}
}

// Checks the word list for the string parameter
int word_exists(char* string)
{
	return 0;
	FILE* file = fopen("./en.txt", "r");
	if (file == NULL)
	{
		printf("Error opening the file!\n");
		return 1;
	}
	// binary search here
	fclose(file);
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

void print_board(char** board, char** colors, int** selected, int n)
{
	printf("%s+", colors[4]);
	printf("%s-----------%s+\n", colors[5], colors[4]);
	for (int r = 0; r < ROWS; r++)
	{
		printf("%s|", colors[5]);
		for (int c = 0; c < COLS; c++)
		{
			int color_to_print = 2;
			if (n != 0 && coords_in_selected(r, c, selected, n)) color_to_print = 0;
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
	printf("currently selected letters: %s\n", word);
	free(word);
}

char* get_selected_str(char** board, int** selected, int n)
{
	char* str = (char*) malloc(sizeof(char) * (n+1));
	for (int i = 0; i < n; i++)
	{
		str[i] = board[ (selected[i][0]) ][ (selected[i][1]) ];
	}
	str[n+1] = '\0';
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

