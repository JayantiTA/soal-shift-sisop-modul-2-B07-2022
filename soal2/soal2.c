#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>

struct Drakor {
  char raw_title[100];
  char title[100];
  int year;
  char category[30];
  bool max;
};

char *path = "/home/oem/shift2/drakor";
char *path_zip = "/home/oem/Downloads/drakor.zip";

int compare_by_category(const void *drakor1, const void *drakor2);
void move_to_folder(struct Drakor drakor, bool is_first);

int main() {
  struct Drakor drakor[100];

  pid_t child_id_1, child_id_2;

  child_id_1 = fork();
  int status1;

  if (child_id_1 == 0) {
    child_id_2 = fork();
    int status2;
    
    if (child_id_2 == 0) {
      char *argv[] = {"mkdir", "-p", path, NULL};
      execv("/bin/mkdir", argv);
    } else {
      while ((wait(&status2)) > 0);
      char *argv[] = {"unzip", path_zip, "-d", path, "-x", "*/*", NULL};
      execv("/usr/bin/unzip", argv);
    }
  } else {
    while ((wait(&status1)) > 0);
    DIR *dir_path;
    struct dirent *dir;
    int count = 0;
    int title_count = 0;
    char file_name[100][100];
    dir_path = opendir(path);

    while ((dir = readdir(dir_path))) {
      if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
        strcpy(file_name[count], dir->d_name);
        ++count;
      }
    }

    for (int i = 0; i < count; ++i) {
      int semicolon = 0;
      int underscore = 0;
      int index = 0;
      char temp[100];

      for (int j = 0; j < strlen(file_name[i]); ++j) {
        if (file_name[i][j] == '_' || file_name[i][j] == '.') {
          temp[index] = '\0';
          index = 0;
          semicolon = 0;
          strcpy(drakor[title_count].raw_title, file_name[i]);
          strcpy(drakor[title_count].category, temp);

          if (file_name[i][j] == '_') ++underscore;
          if (file_name[i][j] == '.') {
            if (underscore == 0) {
              drakor[title_count].max = true;
            } else {
              if (strcmp(drakor[title_count].category, drakor[title_count-1].category) > 0)
                drakor[title_count].max = true;
              else 
                drakor[title_count-1].max = true;
            }
            underscore = 0;
          }
          ++title_count;
        } else if (file_name[i][j] == ';') {
          temp[index] = '\0';
          index = 0;
          if (semicolon > 0) {
            drakor[title_count].year = atoi(temp);
          } else {
            strcpy(drakor[title_count].title, temp);
          }
          ++semicolon;
        } else {
          temp[index] = file_name[i][j];
          ++index;
        }
      }
    }
    bool is_first = false;
    qsort(drakor, title_count, sizeof(struct Drakor), compare_by_category);
    for (int i = 0; i < title_count; ++i) {
      if (i == 0 || (i > 0 && 
        strcmp(drakor[i].category, drakor[i-1].category) > 0)) is_first = true;
      move_to_folder(drakor[i], is_first);
      is_first = false;
    }
  }
  return 0;
}

int compare_by_category(const void *drakor1, const void *drakor2) {
  struct Drakor *d1 = (struct Drakor *)drakor1;
  struct Drakor *d2 = (struct Drakor *)drakor2;
  int category_compare = strcmp(d2->category, d1->category);
  if (category_compare == 0) {
    return d1->year - d2->year;
  } else {
    return -category_compare;
  }
}

void move_to_folder(struct Drakor drakor, bool is_first) {
  char dest_path[100];
  strcpy(dest_path, path);
  strcat(dest_path, "/");
  strcat(dest_path, drakor.category);
  strcat(dest_path, "/");

  FILE *fileptr;
  char path_txt[100];
  strcpy(path_txt, dest_path);
  strcat(path_txt, "data.txt");

  pid_t child_id_1;
  int status1;

  child_id_1 = fork();

  if (child_id_1 == 0) {
    char *argv[] = {"mkdir", "-p", dest_path, NULL};
    execv("/bin/mkdir", argv);
  } else {
    while ((wait(&status1)) > 0);
    
    fileptr = fopen(path_txt, "a");
    if (is_first) {
      fprintf(fileptr, "kategori: %s\n", drakor.category);
    }
    fprintf(fileptr, "\nnama: %s\nrilis: tahun %d\n", drakor.title, drakor.year);
    fclose(fileptr);

    char oldfile[100];
    strcpy(oldfile, path);
    strcat(oldfile, "/");
    strcat(oldfile, drakor.raw_title);

    char newfile[100];
    strcpy(newfile, dest_path);
    strcat(newfile, drakor.title);
    strcat(newfile, ".png");

    pid_t child_id_2;
    int status2;
    child_id_2 = fork();
    if (child_id_2 == 0) {
      if (drakor.max) {
        char *argv[] = {"mv", oldfile, newfile, NULL};
        execv("/bin/mv", argv);
        exit(0);
      } else {
        char *argv[] = {"cp", oldfile, newfile, NULL};
        execv("/bin/cp", argv);
        exit(0);
      }
    } else {
      return;
    }
  }
}
