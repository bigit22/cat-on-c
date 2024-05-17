#include "s21_grep.h"

int main(int argc, char **argv) {
  s21_grep_main(argc, argv);
  return 0;
}

void s21_grep_main(int argc, char **argv) {
  if (argc > 1) {
    char *temp = NULL;
    options configuration = {0};

    for (int x = scanOptions(argc, argv, &configuration, &temp); x < argc; x += 1)
      s21_grep(argv[x], configuration, temp);

    if (temp != NULL) free(temp);
  }
}

int scanOptions(int argc, char **argv, options *configuration, char **template) {
  for (int symbol = 0; (symbol = getopt(argc, argv, "e:ivclnhsf:o")) != (-1);) {
    switch (symbol) {
      case 'i':
        configuration->i = 1;
        break;
      case 'v':
        configuration->v = 1;
        break;
      case 'c':
        configuration->c = 1;
        break;
      case 'l':
        configuration->l = 1;
        break;
      case 'n':
        configuration->n = 1;
        break;
      case 'h':
        configuration->h = 1;
        break;
      case 's':
        configuration->s = 1;
        break;
      case 'o':
        configuration->o = 1;
        break;
      case 'e':
        setconfigurationE(configuration, template, optarg);
        break;
      case 'f':
        setconfigurationF(configuration, template, optarg);
        break;
    }
  }
  if ((configuration->e || configuration->f) == 0) {
    createTemplate(template, argv[optind]);
    optind += 1;
  }
  setupconfiguration(configuration, argc);
  return optind;
}

void s21_grep(char *path, options configuration, char *template) {
  FILE *file = fopen(path, "r");
  if (file != NULL) {
    for (char symbol = '0'; (symbol = getc(file)) != EOF;) {
      char *line = calloc(256, 1);
      configuration.numberLine += 1;
      int length = 0;

      for (line[length] = '\0'; symbol != EOF && symbol != '\n'; symbol = getc(file)) {
        line[length] = symbol;
        line[length += 1] = '\0';
        if (length % 255 == 0) line = increaseLengthStr(line, length + 256);
      }

      printMainData(line, &configuration, template, path);
      free(line);
    }
    printfAuxData(configuration, path);
    fclose(file);
  } else if (configuration.s == 0) {
    fprintf(stderr, ERROR_01, path);
  }
}

void setconfigurationF(options *configuration, char **template, char *optarg) {
  FILE *file = fopen(optarg, "r");
  if (file != NULL) {
    for (char symbol = '0'; (symbol = getc(file)) != EOF;) {
      int length = 0;
      char *line = calloc(256, 1);

      for (line[length] = '\0'; symbol != EOF && symbol != '\n'; symbol = getc(file)) {
        line[length] = symbol;
        line[length += 1] = '\0';
        if (length % 255 == 0)
          line = (char *)increaseLengthStr(line, length + 256);
      }

      if (!(configuration->e || configuration->f))
        configuration->f = createTemplate(template, line);
      else
        addTemplate(template, line);
      free(line);
    }
    fclose(file);
  } else if (configuration->s == 0) {
    fprintf(stderr, ERROR_01, optarg);
  }
}

void setconfigurationE(options *configuration, char **template, char *optarg) {
  if (!(configuration->e || configuration->f))
    configuration->e = createTemplate(template, optarg);
  else
    addTemplate(template, optarg);
}

void printfAuxData(options configuration, char *path) {
  if (configuration.c) {
    if (configuration.l) {
      configuration.countFiles > 1 ? printf("%s:1\n", path) : printf("1\n");
    } else {
      if (configuration.countFiles > 1) printf("%s:", path);
      printf("%i\n", configuration.countMatches);
    }
  }
  if (configuration.l && configuration.countMatches) printf("%s\n", path);
}

void printMainData(char *line, options *configuration, char *template, char *path) {
  regex_t regex;
  if (regcomp(&regex, template, configuration->i ? REG_ICASE : REG_EXTENDED) == 0) {
    if (regexec(&regex, line, 0, NULL, 0) == configuration->v) {
      configuration->countMatches += 1;
      if ((configuration->c || configuration->l) == 0) {
        if (configuration->countFiles > 1 && !(configuration->h)) printf("%s:", path);
        if (configuration->n) printf("%i:", configuration->numberLine);
        if (!configuration->o)
          printf("%s\n", line);
        else
          printfconfigurationO(regex, line, *configuration);
      }
    }
    regfree(&regex);
  }
}

void printfconfigurationO(regex_t regex, char *line, options configuration) {
  while (regexec(&regex, line, 0, NULL, 0) == configuration.v) {
    char *aux = (char *)calloc(strlen(line) + 1, 1);
    strcpy(aux, line);
    int end = strlen(line);
    while (regexec(&regex, aux, 0, NULL, 0) == configuration.v) {
      end--;
      aux[strlen(aux) - 1] = 0;
    }
    aux[strlen(aux)] = line[strlen(aux)];
    int start = 0;
    while (regexec(&regex, aux, 0, NULL, 0) == configuration.v && strlen(aux) > 0) {
      start++;
      int j = 0;
      while (aux[j] != 0) {
        aux[j] = aux[j + 1];
        j++;
      }
    }
    start--;
    int i = strlen(aux);
    while (i != 0) {
      aux[i] = aux[i - 1];
      i--;
    }
    aux[0] = line[start];
    printf("%s\n", aux);
    free(aux);
    i = start + 1;
    while (line[i] != 0) {
      line[i - start - 1] = line[i];
      i++;
    }
    line[i - start - 1] = 0;
  }
}

void setupconfiguration(options *configuration, int argc) {
  if (configuration->o && (configuration->l || configuration->v || configuration->c)) configuration->o = 0;
  configuration->countFiles = argc - optind;
}

int createTemplate(char **str, char *optarg) {
  *str = calloc(strlen(optarg) + 1, 1);
  if (*str) strcpy(*str, optarg);
  return str ? 1 : 0;
}

void addTemplate(char **str, char *optarg) {
  *str = increaseLengthStr(*str, strlen(*str) + strlen(optarg) + 2);
  if (*str) strcat(strcat(*str, "|"), optarg);
}

void *increaseLengthStr(void *str, int size) {
  char *aux = realloc(str, size);
  return aux;
}
