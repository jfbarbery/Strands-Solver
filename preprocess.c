#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_WORDS 370105
#define NUM_DIGITS 8

// The point of this program is to take the words_alpha.txt text file and generate a parallel
// file containing the offsets of each word using fseek. The problem at hand is that we can use
// fseek but each word in words_alpha.txt is a different length, and you can't jump through the
// file by line. The work around is to have a file that is 1:1 with words_alpha.txt that has the
// offsets for each word contained in it. Additionally, each index in the parallel file will be
// comprised of the same number of digits so that you can offset precisely when you do a jump.
// Meaning: If we have 100,000,000 entries, the first 10 will be compried of 1 digit, the next 90
// of 2 digits, the next 900 of 3, etc. This is a problem because fseeks jumps by characters not
// by line. By referring to all of them as 000,000,100 we can just say if you want to cut through
// half the entries, jump by the total # of entries * 9 / 2, 9 for the number of digits per entry,
// divided by 2 to cut by half.

int read_word_offset(FILE* file);
char* int_to_formatted_str(int n);
char* get_init_offset();

int main(void)
{
	FILE* preprocess_ptr = fopen("preprocessed.txt", "w");
	if (preprocess_ptr == NULL) printf("ERROR PREPROCESS\n");
	FILE* words_ptr = fopen("./alphabetic_words.txt", "r");
	if (words_ptr == NULL) printf("ERROR WORDS\n");
	int cumulative = 0;
	char* initial_offset = get_init_offset();
	fprintf(preprocess_ptr, initial_offset);
	free(initial_offset);
	for (int i = 0; i < TOTAL_WORDS; i++)
	{
		int offset = read_word_offset(words_ptr);
		cumulative += offset + 1;
		char* offset_as_str = int_to_formatted_str(cumulative);
		fprintf(preprocess_ptr, offset_as_str);
		fprintf(preprocess_ptr, "\n");
		free(offset_as_str);
	}
	fclose(preprocess_ptr);
	fclose(words_ptr);
	return 0;
}

// Reads a single word from the file
int read_word_offset(FILE* file)
{
	int i = 0;
	char ch;
	while ((ch = fgetc(file)) != '\n')
	{
		if (ch == EOF) break;
		i++;
	}
	return i;
}

char* int_to_formatted_str(int n)
{
	if (n < 0)
	{
		printf("Error! Bad offset.\n");
		return NULL;
	}
	char* str = (char*) malloc(sizeof(char) * (NUM_DIGITS + 1));
	for (int i = 0; i < NUM_DIGITS; i++)
	{
		str[i] = '0';
	}
	str[NUM_DIGITS] = '\0';
	int index = NUM_DIGITS - 1;
	int m = n;
	while (m > 0)
	{
		// Take the last digit and turn it into the ascii version
		char ch = (char)((m%10) + 48);
		str[index] = ch;
		index--;
		m = m / 10;
	}
	return str;
}

char* get_init_offset()
{
	char* offset = (char*) malloc(sizeof(char) * (NUM_DIGITS + 1 + 1));
	for (int i = 0; i < NUM_DIGITS; i++)
	{
		offset[i] = '0';
	}
	// Adding a new line because I'll need to put one in the file anyway
	offset[NUM_DIGITS] = '\n';
	offset[NUM_DIGITS+1] = '\0';
	return offset;
}