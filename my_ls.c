#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> // Ajout de cette ligne pour inclure time.h
#include "my_string.h"
#include <pwd.h>
#include <grp.h>


#define MAX_LEN 256

// Vos autres fonctions continuent ici...

int setFlags(int ac, char** av, bool* flagA, bool* flagT, bool* flagl, bool* flagR, bool* flagr, bool* flagd) {
    int count = 0;
    for (int i = 1; i < ac; i++) {
        if (strcmp(av[i], "-a") == 0) {
            *flagA = true;
            count++;
        } else if (strcmp(av[i], "-t") == 0) {
            *flagT = true;
            count++;
        } else if (strcmp(av[i], "-l") == 0) {
            *flagl = true;
            count++;
        } else if (strcmp(av[i], "-R") == 0) {
            *flagR = true;
            count++;
        } else if (strcmp(av[i], "-r") == 0) {
            *flagr = true;
            count++;
        } else if (strcmp(av[i], "-d") == 0) {
            *flagd = true;
            count++;
        } else if (strcmp(av[i], "-A") == 0) {
            *flagA = true;
            count++;
        } else if (strcmp(av[i], "-l") == 0) {
            *flagl = true;
            count++;
        }
    }
    return count;
}

void swap(char* str1, char* str2) {
    char temp[MAX_LEN];
    my_str_copy(temp, str1);
    my_str_copy(str1, str2);
    my_str_copy(str2, temp);
}
void timelex_sort(char* dirStr, char* first, char* second) {
    // Building the path to the first and second file
    struct stat buffer;
    struct stat buffer2;
    char dirStrCopy[MAX_LEN] = "";
    char dirStrCopy2[MAX_LEN] = "";
    my_str_cat(dirStrCopy, dirStr);
    my_str_cat(dirStrCopy, "/");
    my_str_cat(dirStrCopy2, dirStr);
    my_str_cat(dirStrCopy2, "/");

    // Getting the seconds of the files
    my_str_cat(dirStrCopy, first);
    lstat(dirStrCopy, &buffer);
    time_t sec = buffer.st_mtime;

    my_str_cat(dirStrCopy2, second);
    lstat(dirStrCopy2, &buffer2);
    time_t sec2 = buffer2.st_mtime;
    
    if (sec2 > sec) {
        swap(first, second);
    } else if (sec2 == sec) {
        if (my_str_cmp(first, second) > 0) {
            swap(first, second);
        }   
    }
}

void lex_or_timelex_sort(char* dirStr, char array[][MAX_LEN], int str_count, bool flagT) {
    for (int i = 0; i < str_count - 1; i++) {
        for (int j = 0; j < str_count - i - 1; j++) {
            if (flagT) {
               timelex_sort(dirStr, array[j], array[j+1]);
            } else {
                if (my_str_cmp(array[j], array[j+1]) > 0) {
                    swap(array[j], array[j + 1]);
                }
            }
        }
    }
}

void setAllFilesAndDirsCount(int* fileCount, int* dirCount, int countFlags, int ac,  char av[][MAX_LEN]) {
    DIR* dir;
    for (int indexOperand = countFlags+1; indexOperand < ac; indexOperand++) {
        dir = opendir(av[indexOperand]);
        if (dir == NULL) {
            *fileCount = *fileCount + 1;
        } else {
            *dirCount = *dirCount + 1;
        }
        closedir(dir);
    }
}

void fillFileAndDirArrays(char files[][MAX_LEN], char dirs[][MAX_LEN], int countFlags, int ac, char av[][MAX_LEN]) {
    DIR* dir;
    int fileIndex = 0;
    int dirIndex = 0;
    for (int indexOperand = countFlags+1; indexOperand < ac; indexOperand++) {
        dir = opendir(av[indexOperand]);
        if (dir == NULL) {
            my_str_copy(files[fileIndex], av[indexOperand]);
            fileIndex++;
        } else {
            my_str_copy(dirs[dirIndex], av[indexOperand]);
            dirIndex++;
        }
        closedir(dir);
    }
}



void setDirCount(int* dirCount, char* dirStr, bool flagA) {
    struct dirent* entry;
    DIR* dir = opendir(dirStr);

    while ((entry = readdir(dir)) != NULL) {
        char* name = entry->d_name;
        if (!flagA && name[0] == '.') {
            continue;
        }
        *dirCount = *dirCount + 1;
    }
    closedir(dir);
}

void fillDirArray(char files[][MAX_LEN], char* dirStr, bool flagA) {
    struct dirent* entry;
    DIR* dir = opendir(dirStr);
    int dirIndex = 0;

    while ((entry = readdir(dir)) != NULL) {
        char* name = entry->d_name;
        if (!flagA && name[0] == '.') {
            continue;
        }
        my_str_copy(files[dirIndex], name);
        dirIndex++;
    }
    closedir(dir);
}

void printDirEntries(char* dirStr, bool flagA, bool flagT) {
    int dirCount = 0;
    setDirCount(&dirCount, dirStr, flagA);
    char dir_array[dirCount][MAX_LEN];
    fillDirArray(dir_array, dirStr, flagA);
    lex_or_timelex_sort(dirStr, dir_array, dirCount, flagT);
    print_str_array(dir_array, dirCount);
}

void printDirEntriesReverse(char* dirStr, bool flagA) {
    int dirCount = 0;
    setDirCount(&dirCount, dirStr, flagA);
    char dir_array[dirCount][MAX_LEN];
    fillDirArray(dir_array, dirStr, flagA);
    lex_or_timelex_sort(dirStr, dir_array, dirCount, false);

    // Imprimer le contenu du répertoire dans l'ordre inverse
    for (int i = dirCount - 1; i >= 0; i--) {
        printf("%s\n", dir_array[i]);
    }
}

void printDirEntriesRecursive(char* dirStr, bool flagA) {
    printf("%s:\n", dirStr);

    int dirCount = 0;
    setDirCount(&dirCount, dirStr, flagA);
    char dir_array[dirCount][MAX_LEN];
    fillDirArray(dir_array, dirStr, flagA);
  lex_or_timelex_sort(dirStr, dir_array, dirCount, false);

    
    for (int i = 0; i < dirCount; i++) {
        char* entry = dir_array[i];
        if (strcmp(entry, ".") == 0 || strcmp(entry, "..") == 0) {
            continue;
        }
        
        char entryPath[MAX_LEN];
        sprintf(entryPath, "%s/%s", dirStr, entry);
        struct stat st;
        if (stat(entryPath, &st) == 0 && S_ISDIR(st.st_mode)) {
            printDirEntriesRecursive(entryPath, flagA);
        } else {
            printf("%s\n", entry);
        }
    }
}

/*
void printFileDetails(const char *filePath) {
    struct stat st;
    if (stat(filePath, &st) == 0) {
        // Autorisations
        printf((S_ISDIR(st.st_mode)) ? "d" : "-");
        printf((st.st_mode & S_IRUSR) ? "r" : "-");
        printf((st.st_mode & S_IWUSR) ? "w" : "-");
        printf((st.st_mode & S_IXUSR) ? "x" : "-");
        printf((st.st_mode & S_IRGRP) ? "r" : "-");
        printf((st.st_mode & S_IWGRP) ? "w" : "-");
        printf((st.st_mode & S_IXGRP) ? "x" : "-");
        printf((st.st_mode & S_IROTH) ? "r" : "-");
        printf((st.st_mode & S_IWOTH) ? "w" : "-");
        printf((st.st_mode & S_IXOTH) ? "x " : "-");

        // Nombre de liens
        printf("%lu ", st.st_nlink);

        // Propriétaire et groupe
        struct passwd *pwd = getpwuid(st.st_uid);
        struct group *grp = getgrgid(st.st_gid);
        if (pwd != NULL) {
            printf("%s ", pwd->pw_name);
        } else {
            printf("%d ", st.st_uid);
        }
        if (grp != NULL) {
            printf("%s ", grp->gr_name);
        } else {
            printf("%d ", st.st_gid);
        }

        // Taille du fichier
        printf("%ld ", st.st_size);

        // Date de dernière modification
        struct tm *tm = localtime(&st.st_mtime);
        char date[20];
        strftime(date, sizeof(date), "%b %d %H:%M", tm);
        printf("%s ", date);

        // Nom du fichier
        printf("%s\n", filePath);
    }
}

#define MAX_PATH_LENGTH 256

void printFilesInDirectory(const char* directoryPath) {
    DIR *dir;
    struct dirent *entry;

    // Ouvrir le répertoire
    dir = opendir(directoryPath);
    if (dir == NULL) {
        fprintf(stderr, "Cannot open directory '%s'\n", directoryPath);
        return;
    }

    // Parcourir le répertoire
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char filePath[MAX_PATH_LENGTH];
            snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);
            printFileDetails(filePath);
        }
    }

    // Fermer le répertoire
    closedir(dir);
}

*/

int main(int ac, char** av) {
    bool flagA = false;
    bool flagT = false;
    bool flagl = false;
    bool flagR = false;
    bool flagr = false;
    bool flagd = false;
    int allFilesCount = 0;
    int allDirsCount = 0;

    int countFlags = setFlags(ac, av, &flagA, &flagT, &flagl, &flagR, &flagr, &flagd);

    //char *directoryPath = av[1];  // Récupérez le chemin du répertoire à partir des arguments de la ligne de commande

    // Vérifiez si le répertoire est fourni en argument
    bool hasOperands = ac > countFlags + 1;
    if (hasOperands) {
        char arguments[ac][MAX_LEN];
        for (int i = 0; i < ac; i++) {
            my_str_copy(arguments[i], av[i]);
        }
        setAllFilesAndDirsCount(&allFilesCount, &allDirsCount, countFlags, ac, arguments);
        char allFiles_array[allFilesCount][MAX_LEN];
        char allDirs_array[allDirsCount][MAX_LEN];
        fillFileAndDirArrays(allFiles_array, allDirs_array, countFlags, ac, arguments);

        lex_or_timelex_sort(".", allDirs_array, allDirsCount, flagT);
        for (int i = 0; i < allDirsCount; i++) {
            char* dir = allDirs_array[i];
            if (allDirsCount > 1) {
                printf("%s:\n", dir);
            }
            if (flagR) {
                printDirEntriesRecursive(dir, flagA); // Utilisation de la fonction récursive
            } else if (flagr) {
                printDirEntriesReverse(dir, flagA); // Utilisation de la fonction de tri inversé
            } else if (flagl) {
             //  printFilesInDirectory(directoryPath);  // Appel de la fonction pour imprimer les détails des fichiers dans le répertoire
            } else {
                printDirEntries(dir, flagA, flagT);
            }
            if (i < allDirsCount - 1) {
                printf("\n");
            } 
        }
    } else {
        if (flagR) {
            printDirEntriesRecursive(".", flagA); // Utilisation de la fonction récursive
        } else if (flagr) {
            printDirEntriesReverse(".", flagA); // Utilisation de la fonction de tri inversé
        } else if (flagl) {
        //   printFilesInDirectory(directoryPath);  // Appel de la fonction pour imprimer les détails des fichiers dans le répertoire
        } else {
            printDirEntries(".", flagA, flagT);
        }
    }
    return 0;
}

