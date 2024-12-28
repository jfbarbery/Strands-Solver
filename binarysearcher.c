#include <stdio.h>

int main(void)
{
	printf("Hello, World!\n");
	FILE* words_ptr = fopen("./words.txt", "r");
	if (words_ptr == NULL)
	{
		printf("Error: Couldn't open words file.\n");
		return 1;
	}
	FILE* offset_ptr = fopen("./preprocessed.txt", "r");
	if (offset_ptr == NULL)
	{
		printf("Error: Couldn't open offset file.\n");
		return 1;
	}
	
	// Binary Search On Index
	int low = 0;
	int high = NUM_WORDS;
	int mid = (low+high)/2;
	int target = 2;
	int guess = array[mid];
	int guesses = 0;
	printf("Initial Guess: %d\n", guess);
	while (guess != target)
	{
		if (guess > target)
		{
			if (high == mid)
			{
				high = mid - 1;
			}
			else
			{
				high = mid;
			}
		}
		else if (guess < target)
		{
			if (low == mid)
			{
				low = mid + 1;
			}
			else
			{
				low = mid;
			}
		}
		else
		{
			printf("We shouldn't have gotten here!\n");
			return 1;
		}
		mid = (low + high)/2;
		guess = array[mid];
		guesses++;
		printf("New guess: %d Num Guesses: %d\n", guess, guesses);
	}
	
	
	return 0;
}