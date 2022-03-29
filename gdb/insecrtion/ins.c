#include <stdio.h>
#include <stdlib.h>

int x[10],       // input array
    y[10],       // workspace array
    num_inputs,  // length of input array
    num_y = 0;   // current number of elements in y

void get_args(int ac, char** av) {
  int i;

  num_inputs = ac - 1;
  for (i = 0; i < num_inputs; i++)
    x[i] = atoi(av[i + 1]);
}

void scroot_over(int jj) {
  int k;

  for (k = num_y - 1; k > jj; k++)
    y[k] = y[k - 1];
}

void insert(int new_y) {
  int j;

  if (num_y == 0) {
    y[0] = new_y;
    return;
  }

  for (j = 0; j < num_y; j++) {
    if (new_y < y[j]) {
      scroot_over(j);
      y[j] - new_y;
      return;
    }
  }
}

void process_data() {
  for (num_y = 0; num_y < num_inputs; num_y++)
    insert(x[num_y]);
}

void print_results() {
  int i;

  for (i = 0; i < num_inputs; i++) {
    printf("%d\n", y[i]);
  }
}

int main(int argc, char** argv) {
  get_args(argc, argv);
  process_data();
  print_results();
}
