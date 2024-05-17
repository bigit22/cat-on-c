#include "s21_cat.h"

int main(int argc, char **argv) {
  s21_cat_main(argc, argv);
  return 0;
}

void s21_cat_main(int argc, char **argv) {
  if (argc > 1) {
    options configuration = {0};
    if (scanOptions(argc, argv, &configuration)) {
      for (int x = (argc - configuration.numberFiles); x < argc; x += 1) {
        FILE *file = fopen(argv[x], "r");
        if (file != NULL)
          fclose(printData(file, &configuration));
        else
          fprintf(stderr, ERROR_01, argv[x]);
      }
    }
  }
}

int scanOptions(int argc, char **argv, options *configuration) {
  int temp_index = 1, status_code = 1, x = 1;

  for (; (x < argc && argv[x][0] == '-'); temp_index = (x += 1)) {
    if (!strcmp(argv[x], "-b") || !strcmp(argv[x], "--number-nonblank")) {
      configuration->b = (configuration->n = 0) + 1;
    } else if (!strcmp(argv[x], "-s") || !strcmp(argv[x], "--squeeze-blank")) {
      configuration->s = 1;
    } else if (!strcmp(argv[x], "-n") || !strcmp(argv[x], "--number")) {
      configuration->n = configuration->b ? 0 : 1;
    } else if (!strcmp(argv[x], "-T")) {
      configuration->t = 1;
    } else if (!strcmp(argv[x], "-v")) {
      configuration->v = 1;
    } else if (!strcmp(argv[x], "-t")) {
      configuration->t = 1;
      configuration->v = 1;
    } else if (!strcmp(argv[x], "-e")) {
      configuration->e = 1;
      configuration->v = 1;
    } else if (!strcmp(argv[x], "-E")) {
      configuration->e = 1;
    } else {
      fprintf(stderr, ERROR_02, argv[x][1]);
      status_code = 0;
    }
  }

  configuration->numberFiles = argc - temp_index;

  return status_code;
}

FILE *printData(FILE *file, options *configuration) {
  for (char symbol = '0'; (symbol = getc(file)) != EOF;) {
    configuration->emptyLine = 0;
    if (configuration->s && configuration->counterS == 0 && symbol == '\n') {
      configuration->counterS += 1;
    } else if (configuration->counterS != 0 && symbol == '\n') {
      configuration->counterS += 1;
      configuration->emptyLine = 1;
    } else if (configuration->counterS > 1 && symbol != '\n') {
      configuration->counterS = 0;
      configuration->e ? printf("$\n") : printf("\n");
      if (configuration->n != 0) printf("%6d\t", configuration->n++);
    } else {
      configuration->counterS = 0;
    }
    if (configuration->n != 0 || configuration->b != 0) {
      if (configuration->newLine == 1 && !(configuration->newLine = 0))
        printf("%6d\t", configuration->n++);
      if (configuration->n == 1) printf("%6d\t", configuration->n++);
      if (configuration->b == 1) printf("%6d\t", configuration->b++);
      if (symbol == '\n' && configuration->n != 0 && configuration->emptyLine == 0)
        configuration->newLine = 1;
      if (symbol == '\n' && configuration->b != 0) configuration->counterB += 1;
      if (symbol != '\n' && configuration->counterB != 0 && configuration->counterS == 0)
        if (!(configuration->counterB = 0)) printf("%6d\t", configuration->b++);
    }
    if (symbol == '\n' && configuration->e && configuration->emptyLine == 0) printf("$");
    if (configuration->v) {
      if (symbol < 32 && symbol != 9 && symbol != 10)
        if (symbol += 64) printf("^");
      if (symbol == 127)
        if ((symbol = '?')) printf("^");
    }
    if (configuration->t && symbol == '\t')
      if ((symbol = 'I')) printf("^");
    if (configuration->emptyLine == 0) printf("%c", symbol);
  }
  return file;
}
