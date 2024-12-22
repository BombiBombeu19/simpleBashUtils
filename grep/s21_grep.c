#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Flags {
  bool flag_e;
  int flag_i;
  bool flag_v;
  bool flag_c;
  bool flag_l;
  bool flag_n;
  bool flag_h;
  bool flag_s;
  bool flag_f;
  bool flag_o;

  char pattern[4096];
  int len_pattern;
} Flags;

void add_pattern(Flags* flags, char* pattern);

void add_file(Flags* f, char* filepath) {
  FILE* file = fopen(filepath, "r");
  if (file == NULL) {
    if (!f->flag_s) perror(filepath);
    exit(1);
  }

  char* line = NULL;
  size_t memline = 0;
  int read = getline(&line, &memline, file);

  while (read != -1) {
    if (line[read - 1] == '\n') line[read - 1] = '\0';
    add_pattern(f, line);
    read = getline(&line, &memline, file);
  }
  free(line);
  fclose(file);
}

void add_pattern(Flags* flags, char* pattern) {
  if (flags->len_pattern != 0) {
    strcat(flags->pattern + flags->len_pattern, "|");
    flags->len_pattern += 1;
  }

  flags->len_pattern +=
      sprintf(flags->pattern + flags->len_pattern, "(%s)", pattern);
}

Flags GrepReadFlags(int argc, char* argv[]) {
  Flags flags = {0};
  int currentFlag;

  while ((currentFlag = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (currentFlag) {
      break;
      case 'e':
        flags.flag_e = true;
        add_pattern(&flags, optarg);

        break;
      case 'i':
        flags.flag_i = REG_ICASE;

        break;
      case 'v':
        flags.flag_v = true;

        break;
      case 'c':
        flags.flag_c = true;

        break;
      case 'l':
        flags.flag_l = true;

        break;
      case 'n':
        flags.flag_n = true;

        break;
      case 'h':
        flags.flag_h = true;

        break;
      case 's':
        flags.flag_s = true;

        break;
      case 'f':
        flags.flag_f = true;
        add_file(&flags, optarg);

        break;
      case 'o':
        flags.flag_o = true;
    }
  }

  if (flags.len_pattern == 0) {  //переход на другой флаг, если некорректный
    add_pattern(&flags, argv[optind]);
    optind += 1;
  }

  if (argc - optind == 1)
    flags.flag_h = 1;  //Проверка на количество проверяемых файлов

  return flags;
};

void output_line(char* line, int n) {
  for (int i = 0; i < n; i++) {
    putchar(line[i]);
  }
  if (line[n - 1] != '\n') putchar('\n');
}

void print_match(regex_t* re, char* line) {
  regmatch_t math;
  int offset = 0;

  while (1) {
    int result = regexec(re, line + offset, 1, &math, 0);

    if (result != 0) break;

    for (int i = math.rm_so; i < math.rm_eo; i++) {
      putchar(line[i]);
    }
    putchar('\n');
    offset += math.rm_eo;
  }
}

void processFile(Flags f, char* path, regex_t* reg) {
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    if (!f.flag_s) perror(path);
    exit(1);
  }

  int line_count = 1;
  int c_count = 0;
  char* line = NULL;
  size_t memline = 0;
  int read = getline(&line, &memline, file);

  while (read != -1) {
    int result = regexec(reg, line, 0, NULL, 0);

    if ((result == 0 && !f.flag_v) || (result != 0 && f.flag_v)) {
      if (!f.flag_c && !f.flag_l) {
        if (!f.flag_h) printf("%s:", path);

        if (f.flag_n) printf("%d:", line_count);

        if (f.flag_o)
          print_match(reg, line);

        else
          output_line(line, read);
      }
      c_count += 1;
    }

    read = getline(&line, &memline, file);

    line_count += 1;
  }

  free(line);

  if (f.flag_c && !f.flag_l) {
    if (!f.flag_h) printf("%s:", path);
    printf("%d\n", c_count);
  }

  if (f.flag_l && c_count > 0) printf("%s\n", path);
  fclose(file);
}

void output(Flags f, int argc, char** argv) {
  regex_t re;

  int error = regcomp(&re, f.pattern, REG_EXTENDED | f.flag_i);

  if (error) perror("ERROR");

  for (int i = optind; i < argc; i++) {
    processFile(f, argv[i], &re);
  }

  regfree(&re);
}

int main(int argc, char* argv[]) {
  Flags f = GrepReadFlags(argc, argv);
  output(f, argc, argv);

  return 0;
}
