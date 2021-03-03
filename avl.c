#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct node {
    int val;
    int height;
    struct node *left;
    struct node *right;
};