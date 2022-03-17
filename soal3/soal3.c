#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <wait.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>

struct Animal {
  char filename[100];
  char name[100];
  char habitat[100];
  bool is_bird;
};

char *path1 = "/home/oem/modul2/darat/";
char *path2 = "/home/oem/modul2/air/";
char *path_extract = "/home/oem/modul2/animal/";

char *path_zip = "/home/oem/Downloads/animal.zip";

void move_or_remove(struct Animal animal, bool max);
void to_list(struct Animal animal);

int main() {
  struct Animal animals[100];
  
  pid_t child_id_1, child_id_2, child_id_3;
  int status1;

  child_id_1 = fork();

  if (child_id_1 == 0) {
    child_id_2 = fork();
    int status2;
    
    if (child_id_2 == 0) {
      char *argv[] = {"mkdir", "-p", path1, NULL};
      execv("/bin/mkdir", argv);
    } else {
      while ((wait(&status2)) > 0);
      sleep(3);
      char *argv[] = {"mkdir", "-p", path2, NULL};
      execv("/bin/mkdir", argv);
    }
  } else {
    while ((wait(&status1)) > 0);
    child_id_3 = fork();
    int status3;

    if (child_id_3 == 0) {
      char *argv[] = {"unzip", path_zip, "-d", "/home/oem/modul2", NULL};
      execv("/usr/bin/unzip", argv);
    } else {
      while ((wait(&status3)) > 0);
      DIR *dir_path;
      struct dirent *dir;
      int count = 0;
      bool max = false;
      char file_name[100][100];
      dir_path = opendir(path_extract);

      while ((dir = readdir(dir_path))) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
          strcpy(file_name[count], dir->d_name);
          ++count;
        }
      }

      for (int i = 0; i < count; ++i) {
        int underscore = 0;
        int index = 0;
        int name_count = 0;
        strcpy(animals[i].filename, file_name[i]);
        char temp[100], category[100], name[10][100];
        if (i == count - 1) max = true;

        for (int j = 0; j < strlen(file_name[i]); ++j) {
          if (file_name[i][j] == '_') {
            temp[index] = '\0';
            index = 0;
            if (strcmp(temp, "darat") == 0) {
              strcpy(animals[i].habitat, "darat");
            } else if (strcmp(temp, "air") == 0) {
              strcpy(animals[i].habitat, "air");
            } else {
              if (name_count > 0) strcat(animals[i].name, "_");
              strcat(animals[i].name, temp);
              ++name_count;
            }
            if (strcmp(temp, "bird") == 0) {
              animals[i].is_bird = true;
            }
            ++underscore;
          } else if (j == strlen(file_name[i]) - 4 && file_name[i][j] == '.') {
            temp[index] = '\0';
            index = 0;
            underscore = 0;
            if (strcmp(temp, "darat") == 0) {
              strcpy(animals[i].habitat, "darat");
            } else if (strcmp(temp, "air") == 0) {
              strcpy(animals[i].habitat, "air");
            } else {
              if (name_count > 0) strcat(animals[i].name, "_");
              strcat(animals[i].name, temp);
            }
            if (strcmp(temp, "bird") == 0) {
              animals[i].is_bird = true;
            }
            break;
          } else {
            temp[index] = file_name[i][j];
            ++index;
          }
        }
        move_or_remove(animals[i], max);
      }
    }
  }
  return 0;
}

void move_or_remove(struct Animal animal, bool max) {
  char newpath[100];
  char filepath[100];
  strcpy(filepath, path_extract);
  strcat(filepath, animal.filename);
  
  pid_t child_id_1;
  int status;

  child_id_1 = fork();

  if (child_id_1 == 0) {
    if (animal.is_bird) {
      char *argv[] = {"rm", filepath, NULL};
      execv("/bin/rm", argv);
    } else if (strcmp(animal.habitat, "darat") == 0) {
      strcpy(newpath, path1);
      strcat(newpath, animal.name);
      strcat(newpath, ".jpg");
      char *argv[] = {"mv", filepath, newpath, NULL};
      execv("/bin/mv", argv);
    } else if (strcmp(animal.habitat, "air") == 0) {
      strcpy(newpath, path2);
      strcat(newpath, animal.name);
      strcat(newpath, ".jpg");
      char *argv[] = {"mv", filepath, newpath, NULL};
      execv("/bin/mv", argv);
    } else {
      char *argv[] = {"rm", filepath, NULL};
      execv("/bin/rm", argv);
    }
  } else {
    while((wait(&status)) > 0);
    if (strcmp(animal.habitat, "air") == 0) {
      to_list(animal);
    }
    if (max) {
      char *argv[] = {"rm", "-rf", path_extract, NULL};
      execv("/bin/rm", argv);
    }
    return;
  }
}

void to_list(struct Animal animal) {
  char filename[100];
  strcpy(filename, animal.name);
  strcat(filename, ".jpg");

  char path[100];
  strcpy(path, path2);

  FILE *fileptr;
  char path_txt[100];
  strcpy(path_txt, path);
  strcat(path_txt, "list.txt");
  fileptr = fopen(path_txt, "a");

  if (fileptr == NULL)
  {
    printf("Unable to create file.\n");
    exit(EXIT_FAILURE);
  }

  strcat(path, filename);

  struct stat info;
  int r;

  r = stat(path, &info);
  if (r == -1)
  {
    fprintf(stderr,"File error\n");
    exit(1);
  }

  struct passwd *pw = getpwuid(info.st_uid);

  char text[100];
  if (pw != 0) strcpy(text, pw->pw_name);
  strcat(text, "_");
  if (info.st_mode & S_IRUSR) strcat(text, "r");
  if (info.st_mode & S_IWUSR) strcat(text, "w");
  if (info.st_mode & S_IXUSR) strcat(text, "x");
  strcat(text, "_");
  strcat(text, filename);

  fprintf(fileptr, "%s\n", text);

  fclose(fileptr);
}
