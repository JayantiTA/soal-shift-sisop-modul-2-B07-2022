#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

struct Drakor {
  char raw_title[100];
  char title[100];
  char year[10];
  char category[30];
  bool max;
};

char *path = "/home/oem/shift2/drakor";

void move_to_folder(struct Drakor drakor);
void to_list(struct Drakor drakor[], int counter);

int main() {
  struct Drakor drakor[100];

  pid_t child_id_1, child_id_2;
  char *path_zip = "/home/oem/Downloads/drakor.zip";

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
    char file_name[100][100];
    dir_path = opendir(path);
    int counter = 0;

    while ((dir = readdir(dir_path))) {
      if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
        strcpy(file_name[count], dir->d_name);
        ++count;
      }
    }

    char title[10][100];
    for (int i = 0; i < count; ++i) {
      int semicolon = 0;
      int underscore = 0;
      int index = 0, idx = 0;
      int title_count = 0;
      char temp[100], temp2[100];

      for (int j = 0; j < strlen(file_name[i]); ++j) {
        if (file_name[i][j] == '_') {
          temp[index] = '\0';
          strcpy(title[underscore], temp);
          index = 0;
          ++underscore;
        } else if (j == strlen(file_name[i]) - 4 && file_name[i][j] == '.') {
          temp[index] = '\0';
          strcpy(title[underscore], temp);
          index = 0;
          break;
        } else {
          temp[index] = file_name[i][j];
          ++index;
        }
      }
      for (int k = 0; k <= underscore; ++k) {
        // printf("%s\n", title[k]);
        // printf("tes\n");
        strcpy(drakor[counter].raw_title, file_name[i]);
        if (k == underscore) drakor[counter].max = true;
        for (int l = 0; l <= strlen(title[k]); ++l) {
          if (title[k][l] == ';') {
            temp2[idx] = '\0';
            // printf("%s\n", temp2);
            if (semicolon == 0) {
              strcpy(drakor[counter].title, temp2);
            } else if (semicolon == 1) {
              strcpy(drakor[counter].year, temp2);
              strcpy(temp2, "");
            }
            idx = 0;
            ++semicolon;
          } else if (l == strlen(title[k])) {
            temp2[idx] = '\0';
            idx = 0;
            strcpy(drakor[counter].category, temp2);
            semicolon = 0;
            ++counter;
            break;
          } else {
            temp2[idx] = title[k][l];
            ++idx;
          }
        }
      }
    }
    // for (int m = 0; m < counter; m++) {
    //   move_to_folder(drakor[m]);
    // }
    to_list(drakor, counter);
  }

  return 0;
}

void to_list(struct Drakor drakor[], int counter) {
  struct Drakor temp;

}

void move_to_folder(struct Drakor drakor) {
  char dest_path[100];
  strcpy(dest_path, path);
  strcat(dest_path, "/");
  strcat(dest_path, drakor.category);
  strcat(dest_path, "/");
  pid_t child_id_1;
  int status1;

  child_id_1 = fork();

  if (child_id_1 == 0) {
    char *argv[] = {"mkdir", "-p", dest_path, NULL};
    execv("/bin/mkdir", argv);
  } else {
    while ((wait(&status1)) > 0);

    char oldfile[100];
    strcpy(oldfile, path);
    strcat(oldfile, "/");
    strcat(oldfile, drakor.raw_title);

    char newfile[100];
    strcpy(newfile, dest_path);
    strcat(newfile, drakor.title);
    strcat(newfile, ".png");
    printf("%s\n", newfile);

    pid_t child_id_2;
    int status2;
    child_id_2 = fork();
    if (child_id_2 == 0) {
      if (drakor.max) {
        char *argv[] = {"mv", oldfile, newfile, NULL};
        execv("/bin/mv", argv);
        exit(0);
      } 
      else {
        char *argv[] = {"cp", oldfile, newfile, NULL};
        execv("/bin/cp", argv);
        exit(0);
      }
    } else {
      return;
    }
  }
}

