#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

char *path = "/home/oem/shift2/drakor";

void move_to_folder(char file_name[], bool max, char title[], char category[]);

int main() {
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

    while ((dir = readdir(dir_path))) {
      if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
        strcpy(file_name[count], dir->d_name);
        ++count;
      }
    }

    for (int i = 0; i < count; ++i) {
      int semicolon = 0;
      int index = 0;
      int title_count = 0;
      bool dot = false;
      char temp[100], title[10][100], category[100];

      for (int j = 0; j < strlen(file_name[i]); ++j) {
        if (file_name[i][j] == ';') {
          temp[index] = '\0';
          index = 0;
          if (semicolon % 2 == 0) {
            strcpy(title[title_count], temp);
            ++title_count;
          }
          ++semicolon;
        } else if (j == strlen(file_name[i]) - 4 || file_name[i][j] == '.') {
          if (file_name[i][j] == '.') dot = true;
          temp[index] = '\0';
          index = 0;
          if (dot) {
            strcpy(category, temp);
            strcat(category, "/");
            index = 0; 
            semicolon = 0;
            dot = false;
            continue;
          } else {
            break;
          }
        } else {
          temp[index] = file_name[i][j];
          ++index;
        }
      }
      bool max = false;
      for (int k = 0; k < title_count; ++k) {
        if (k == title_count - 1) max = true;
        move_to_folder(file_name[i], max, title[k], category);
        strcpy(title[i], "");
      }
    }
  }

  return 0;
}

void move_to_folder(char file_name[], bool max, char title[], char category[]) {
  pid_t child_id_1;
  int status1;
  child_id_1 = fork();

  char dest_path[100];
  strcpy(dest_path, path);
  strcat(dest_path, "/");
  strcat(dest_path, category);

  if (child_id_1 == 0) {
    char *argv[] = {"mkdir", "-p", dest_path, NULL};
    execv("/bin/mkdir", argv);
  } else {
    while ((wait(&status1)) > 0);

    char oldfile[100];
    strcpy(oldfile, path);
    strcat(oldfile, "/");
    strcat(oldfile, file_name);

    char newfile[100];
    strcpy(newfile, dest_path);
    strcat(newfile, title);
    strcat(newfile, ".png");

    pid_t child_id_2;
    int status2;
    child_id_2 = fork();
    if (child_id_2 == 0) {
      if (max == true) {
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
