#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Flags {
  bool non_blank_rows;  //-b
  bool show_endl;       //-E
  bool all_rows;        //-n
  bool squeezed;        //-s
  bool show_tabs;       //-T
  bool show_all;        //-v
} Flags;

Flags CatReadFlags(int argc, char *argv[]) {
  Flags flags = {0};

  struct option long_options[] = {{"number-nonblank", 0, NULL, 'b'},
                                  {"number", 0, NULL, 'n'},
                                  {"squeeze-blank", 0, NULL, 's'},
                                  {NULL, 0, NULL, 0}};

  int currentFlag;
  while ((currentFlag = getopt_long(argc, argv, "bevEnstT", long_options, 0)) !=
         -1) {
    switch (currentFlag) {
      break;
      case 'b':
        flags.non_blank_rows = true;

        break;
      case 'e':
        flags.show_endl = true;
        flags.show_all = true;

        break;
      case 'v':
        flags.show_all = true;

        break;
      case 'E':
        flags.show_endl = true;

        break;
      case 'n':
        flags.all_rows = true;

        break;
      case 's':
        flags.squeezed = true;

        break;
      case 't':
        flags.show_all = true;
        flags.show_tabs = true;

        break;
      case 'T':
        flags.show_tabs = true;
    }
  }
  return flags;
}

char flag_v(char ch) {
  if (ch == '\n' || ch == '\t') return ch;

  if (ch < 0) {
    printf("M-");
    ch = ch & 0x7F;
  }

  if (ch > 0 && ch <= 31) {
    putchar('^');
    ch += 64;
  }

  else if (ch == 127) {
    putchar('^');
    ch = '?';
  }

  return ch;
}

void outline(Flags *f, char *line, int n) {
  for (int i = 0; i < n; i++) {
    if (f->show_tabs && line[i] == '\t')
      printf("^I");
    else {
      if (f->show_endl && line[i] == '\n') putchar('$');

      if (f->show_tabs && line[i] == '\t') printf("^I");

      if (f->show_all) line[i] = flag_v(line[i]);

      putchar(line[i]);
    }
  }
}

void output(Flags *f, char **argv) {
  FILE *file = fopen(argv[optind], "r");
  char *line = NULL;
  size_t memline = 0;

  int line_num = 1;
  int empty_count = 0;

  int read = getline(&line, &memline, file);

  while (read != -1) {
    if (line[0] == '\n')
      empty_count += 1;
    else
      empty_count = 0;
    if (f->squeezed && empty_count > 1) {
    }

    else {
      if (f->all_rows || f->non_blank_rows) {
        if (f->non_blank_rows && line[0] != '\n') {
          printf("%6d\t", line_num);
          line_num += 1;
        }

        else if (f->all_rows) {
          printf("%6d\t", line_num);
          line_num += 1;
        }
      }
      outline(f, line, read);
    }

    read = getline(&line, &memline, file);
  }

  free(line);
  fclose(file);
}

int main(int argc, char *argv[]) {
  Flags f = CatReadFlags(argc, argv);
  output(&f, argv);

  return 0;
}
