#include <stdio.h>
#include <assert.h>

int main() {
  printf("test fopen\n");
  FILE *fp = fopen("/share/texts/num", "r+");
  printf("test fopen1\n");
  assert(fp);

  printf("test fopen2\n");
  fseek(fp, 0, SEEK_END);
  printf("test fopen3\n");
  long size = ftell(fp);
  assert(size == 5000);
  printf("test fopen4\n");

  fseek(fp, 500 * 5, SEEK_SET);
  printf("test fopen5\n");
  int i, n;
  for (i = 500; i < 1000; i ++) {
//  printf("test fopen6\n");
    fscanf(fp, "%d", &n);
//  printf("test fopen6 the i is %d, the n is %d\n", i, n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fprintf(fp, "%4d\n", i + 1 + 1000);
  }

  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
  printf("test fopen6 the i is %d, the n is %d\n", i, n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }

  fclose(fp);

  printf("PASS!!!\n");

  return 0;
}
