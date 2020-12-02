#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>

int get_score();
int *get_scores(short total);
// int array_length(int *arr); // does not work for dynamically allocated arrays
void print_chart(int *scores, int num_scores);
void print_bar(int score, int score_number);

int main(void)
{
    int num_scores = get_int("Number of scores to collect: ");
    int *scores = get_scores(num_scores);
    print_chart(scores, num_scores);

    free(scores);
}

void print_chart(int *scores, int num_scores)
{
    for (int i = 0; i < num_scores; i++)
    {
        int score = scores[i];
        print_bar(score, i + 1);
    }
}

void print_bar(int score, int score_number)
{
    char bar[score + 2]; // the final char array to print

    // build the bar string
    for (int i = 0; i < score; i++)
    {
        bar[i] = '#';
    }

    bar[score] = '\n';
    bar[score + 1] = '\0';

    // print the line
    printf("Score %i: ", score_number);
    printf("%s", bar);
}

int *get_scores(short n)
{
    // dynamically allocate so it can be passed out of scope
    int *scores = malloc(n * sizeof(int));

    // request the scores one at a time from the user
    for (int i = 0; i < n; i ++)
    {
        scores[i] = get_score(i + 1);
    }

    return scores;
}

int get_score(int i)
{
    return get_int("Score %i: ", i);
}

// this does not work for dynamically allocated arrays
// int array_length(int *arr)
// {
//     return sizeof(arr) / sizeof(arr[0]);
// }
