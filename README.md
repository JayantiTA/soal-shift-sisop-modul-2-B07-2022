# Soal Shift Sisop Modul 2 2022

Repository untuk soal shift sisop modul 2 kelompok B07 tahun 2022

Anggota:

1. Hans Sean Nathanael (5025201019)
2. Jayanti Totti Andhina (5025201037)
3. Agnesfia Anggraeni (5025201059)


## Nomor 1

**Dokumentasi Pengerjaan dan Rintangan**

## Nomor 2

Soal nomor 2 terkait poster drama korea. 

- Seperti program C pada umumnya, di awal program terdapat library-library yang digunakan.
- Terdapat `struct Drakor` yang berisi nama file, judul drakor, tahun rilis, kategori, dan `boolean max`.
- Terdapat juga variabel global untuk menyimpan path folder dan file zip, serta deklarasi fungsi yang akan digunakan.

```c++
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
...
```

1. Masuk ke soal 2a, diminta untuk meng-*extract* isi file zip ke folder “/home/[user]/shift2/drakor”, tetapi yang diperlukan saja. 
- Melakukan `fork()` *process*, kemudian karena akan terdapat 2 *process*, maka dilakukan `fork()` lagi. Sehingga untuk *child process* adalah membuat folder, dan *parent process* adalah meng-*extract* zip.
- Extract zip untuk file saja dengan meng-*exclude* folder yang ada di dalam file zip menggunakan argumen `-x */*`. 

```c++
...
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
}
...
```

2. Kami menggabungkan soal 2b, 2c, 2d, 2e menjadi satu fungsi penyelesaian menggunakan data struct.
- Karena *child process* digunakan untuk `mkdir` dan `unzip`, maka kembali ke *parent process* untuk mengkategorikan file.
- Pertama, dilakukan listing folder.
```c++
...
  else {
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
...
```
- Kemudian dilakukan pemisahan string berdasarkan '_' dan ';' untuk mendapatkan data-data sesuai pada `struct Drakor`. Hal ini bertujuan untuk mempermudah pengkategorian nantinya.
```c++
...
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
...
```
- Karena juga diminta untuk membuat file `data.txt` yang berisi kategori, nama drakor, dan tahun rilis, serta harus diurutkan berdasarkan tahun rilis, maka dilakukan sorting struct menggunakan fungsi `qsort` pada C.
```c++
...
  bool is_first = false;
    qsort(drakor, title_count, sizeof(struct Drakor), compare_by_category);
...
// fungsi sorting yang digunakan:
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
...
```
- Setelah itu baru semua struct diproses untuk dipindahkan ke folder masing-masing sekaligus dibuat file `data.txt`.
```c++
...
    for (int i = 0; i < title_count; ++i) {
      if (i == 0 || (i > 0 && 
        strcmp(drakor[i].category, drakor[i-1].category) > 0)) is_first = true;
      move_to_folder(drakor[i], is_first);
      is_first = false;
    }
...
```
- Pada fungsi `move_to_folder`, dilakukan `fork()` *process*. Pada *child process*, dibuat folder berdasarkan kategori drakor. 
```c++
...
  if (child_id_1 == 0) {
    char *argv[] = {"mkdir", "-p", dest_path, NULL};
    execv("/bin/mkdir", argv);
  }
... 
```
- Kemudian pada *parent process*, dibuat file `data.txt` jika belum ada, namun jika sudah ada maka data akan di-*append* ke file tersebut.
```c++
...
    fileptr = fopen(path_txt, "a");
    if (is_first) {
      fprintf(fileptr, "kategori: %s\n", drakor.category);
    }
    fprintf(fileptr, "\nnama: %s\nrilis: tahun %d\n", drakor.title, drakor.year);
    fclose(fileptr);
...
```
- Dilakukan `fork()` kembali pada *parent process* untuk memindahkan atau menyalin poster drakor ke dalam folder kategori yang sesuai.
```c++
...
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
...
```

**Dokumentasi Pengerjaan dan Rintangan**

## Nomor 3

**Dokumentasi Pengerjaan dan Rintangan**
