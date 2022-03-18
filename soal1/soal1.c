#include "stdio.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "json-c/json.h"
#include "unistd.h"
#include "wait.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "dirent.h"

//wget --no-check-certificate -q 'https://drive.google.com/uc?id=1xYYmsslb-9s8-4BDvosym7R4EmPi6BHp&export=download' -O db_character.zip

//wget --no-check-certificate -q 'https://drive.google.com/uc?id=1XSkAqqjkNmzZ0AdIZQt_eWGOZ0eJyNlT&export=download' -O db_weapon.zip


// mengambil total file yang ada di database
int getTotalDatabase(char *database)
{
	int result = -1;
	DIR *databaseDirectory = opendir(database);
	
	if (databaseDirectory != NULL)
	{
		struct dirent *fileData;
		int totalFile = 0;
		
		while((fileData = readdir(databaseDirectory)) != NULL)
		{
			if (strcmp(fileData->d_name, ".") != 0 && strcmp(fileData->d_name, "..") != 0)
			{
				totalFile += 1;
			}
		}
		
		result = totalFile;
		
		closedir(databaseDirectory);
	}
	
	return result;
}

// menyimpan semua nama file yang ada di database ke dalam array
void getFileNameInDatabase(char **arrayDatabase, char *database)
{
	DIR *databaseDirectory = opendir(database);
	
	if (databaseDirectory != NULL)
	{
		struct dirent *fileData;
		int totalFile = 0;
		
		while((fileData = readdir(databaseDirectory)) != NULL)
		{
			if (strcmp(fileData->d_name, ".") != 0 && strcmp(fileData->d_name, "..") != 0)
			{
				strcpy(arrayDatabase[totalFile], fileData->d_name);
				totalFile += 1;				
			}
		}
		
		closedir(databaseDirectory);
	}
}

// mengalokasikan array string secara dinamis
char **allocateArrayForDatabase(int arraySize, int bufferSize)
{
	char **array;
	
	array = (char**)malloc(sizeof(char*) * arraySize);
	
	for(int i = 0; i < bufferSize; i++)
	{
		array[i] = (char*)malloc(sizeof(char) * bufferSize);
	}
	
	return array;
}

// dealokasi array string (sangat penting, jangan lupa dipanggil bila melakukan alokasi)
// kalau tidak akan memory leak
void deallocateArrayOfDatabase(char **arrayDatabase)
{
	int length = sizeof(arrayDatabase) / sizeof(arrayDatabase[0]);
	
	for (int i = 0; i < length; i++)
	{
		free(arrayDatabase[i]);
	}
	free(arrayDatabase);
}

void zipGachaFile()
{
	if(chdir("gacha_gacha") < 0)
	{
		exit(EXIT_FAILURE);
	}
	
	DIR *directory = opendir("./");
	
	if (directory == NULL)
	{
		exit(EXIT_FAILURE);
	}
	
	struct dirent *fileChecker;
	
	int totalFile = 0;
	
	while((fileChecker = readdir(directory)) != NULL)
	{
		if (strncmp("total_gacha_", fileChecker->d_name, 12) == 0)
		{
			totalFile += 1;
		}
	}
	closedir(directory);

	char **zipArgv = allocateArrayForDatabase(totalFile + 6, 128);
	strcpy(zipArgv[0], "zip");
	strcpy(zipArgv[1], "-P");
	strcpy(zipArgv[2], "satuduatiga");
	strcpy(zipArgv[3], "-r");
	strcpy(zipArgv[4], "not_safe_for_wibu.zip");
	zipArgv[totalFile + 5] = NULL;
	
	directory = opendir("./");
	
	if (directory == NULL)
	{
		exit(EXIT_FAILURE);
	}
		
	int fileIndex = 5;
	while((fileChecker = readdir(directory)) != NULL)
	{
		if (strncmp("total_gacha_", fileChecker->d_name, 12) == 0)
		{
			strcpy(zipArgv[fileIndex], fileChecker->d_name);
			fileIndex += 1;
		}
	}
	closedir(directory);
	
	execv("/bin/zip", zipArgv);
}

void clearGachaGachaDirectory()
{
	if(chdir("gacha_gacha") < 0)
	{
		exit(EXIT_FAILURE);
	}
	
	DIR *directory = opendir("./");
	
	if (directory == NULL)
	{
		exit(EXIT_FAILURE);
	}
	
	struct dirent *fileChecker;
	
	pid_t pidRemoveDirectoryProcess = 0;
	int status;
	
	while((fileChecker = readdir(directory)) != NULL)
	{
		if (strncmp("total_gacha_", fileChecker->d_name, 12) == 0)
		{
			pidRemoveDirectoryProcess = 0;
			pidRemoveDirectoryProcess = fork();
			
			if (pidRemoveDirectoryProcess == 0)
			{
				char *removeDirectoryArgv[] = {"rm", "-rf", fileChecker->d_name, NULL};
				execv("/bin/rm", removeDirectoryArgv);
			}
			else
			{
				while(wait(&status) > 0);
			}
		}
	}
	exit(EXIT_SUCCESS);
}

int main()
{
	pid_t pidDaemonProcess = 0, sidDaemonProcess;
	
	pidDaemonProcess = fork();
	
	if (pidDaemonProcess < 0)
	{
		exit(EXIT_FAILURE);
	}
	else if (pidDaemonProcess > 0)
	{
		exit(EXIT_SUCCESS);
	}
	
	// masuk dalam daemon
	
	umask(0);
	
	sidDaemonProcess = setsid();
	
	if (sidDaemonProcess < 0)
	{
		exit(EXIT_FAILURE);
	}
	
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	pid_t pidDownloadDatabaseCharacter = 0;
	
	pidDownloadDatabaseCharacter = fork();
	
	if (pidDownloadDatabaseCharacter < 0)
	{
		exit(EXIT_FAILURE);
	}
	
	if (pidDownloadDatabaseCharacter == 0)
	{
		// process untuk mengunduh zip database weapon dan character
		// dan kemudian mengextract zip tersebut
		
		pid_t pidDownloadDatabaseWeapon = 0;
		
		pidDownloadDatabaseWeapon  = fork();
		
		if (pidDownloadDatabaseWeapon < 0)
		{
			exit(EXIT_FAILURE);
		}
		
		if (pidDownloadDatabaseWeapon == 0)
		{
			// mengunduh zip database character
			// dan mengextract zip tersebut
			
			pid_t pidExtractFileDBWeapon = 0;
			
			pidExtractFileDBWeapon = fork();
			
			if (pidExtractFileDBWeapon < 0)
			{
				exit(EXIT_FAILURE);
			}
			
			if (pidExtractFileDBWeapon == 0)
			{
				// mengunduh zip database character
				
				char *argvDownloadWeaponDB[] = {
					"wget", 
					"--no-check-certificate", 
					"-q", 
					"https://drive.google.com/uc?id=1XSkAqqjkNmzZ0AdIZQt_eWGOZ0eJyNlT&export=download", 
					"-O", 
					"db_weapon.zip", NULL
				};
			
				execv("/bin/wget", argvDownloadWeaponDB);
			}
			else
			{
				// mengextract zip database character
				
				int statusProcessDownloadDBWeapon;
				while(wait(&statusProcessDownloadDBWeapon) > 0);
				
				char *unzipWeaponDBArgv[] = {"unzip", "-q",  "db_weapon.zip", NULL};
				execv("/bin/unzip", unzipWeaponDBArgv);
			}
			
		}
		else
		{
			// mengunduh zip database weapon
			// dan kemudian mengextract zip tersebut
		
			int statusProcessDBWeapon;
			while(wait(&statusProcessDBWeapon) > 0);
			
			pid_t pidExtractFileDBCharacter = 0;
			
			pidExtractFileDBCharacter = fork();
			
			if (pidExtractFileDBCharacter < 0)
			{
				exit(EXIT_FAILURE);
			}
			
			if (pidExtractFileDBCharacter == 0)
			{
				// mengunduh zip database weapon
				
				char *argvDownloadCharacterDB[] = {
					"wget", 
					"--no-check-certificate", 
					"-q", 
					"https://drive.google.com/uc?id=1xYYmsslb-9s8-4BDvosym7R4EmPi6BHp&export=download", 
					"-O", 
					"db_character.zip", 
					NULL
				};
				
				execv("/bin/wget", argvDownloadCharacterDB);
			}
			else
			{
				// mengextract zip database weapon
				
				int statusProcessDownloadDBCharacter;
				while(wait(&statusProcessDownloadDBCharacter) > 0);
				
				char *unzipCharacterDBArgv[] = {"unzip", "-q", "db_character.zip", NULL};
				execv("/bin/unzip", unzipCharacterDBArgv);
				
			}
			
		}
	}
	else
	{
		int status;
		while(wait(&status) > 0);
		
		// membuat child process yang bertugas membuat
		// directory "gacha_gacha"
		
		pid_t pidCreateGachaDirectory = 0;
		pidCreateGachaDirectory = fork();
		
		if (pidCreateGachaDirectory < 0)
		{
			exit(EXIT_FAILURE);
		}
		
		if (pidCreateGachaDirectory == 0)
		{
			// membuat directory gacha_gacha
			
			char *createGachaDirectoryArgv[] = {"mkdir", "gacha_gacha", NULL};
			execv("/bin/mkdir", createGachaDirectoryArgv);
		}
		else
		{
			while(wait(&status) > 0);
			
			// proccess daemon program gacha
			
			time_t currentTime; // mendapatkan waktu dalam second
			struct tm currentLocalTime; // menyimpan data waktu yang telah dibagi menjadi tahun, bulan, hari, jam, menit, detik
			
			// jam, menit, dan detik mulai dan berhenti proccess diconvert ke detik
			// agar mudah dilakukan perbandingan
			// dengan jam, menit, dan detik saat sekarang
			// prevHourSecond untuk menyimpan waktu sebelumnya karena program diminta menciptakan file
			// gacha txt dengan waktu yang berbeda setiap 1 detik
			int startHourSecond = 4 * 3600 + 44 * 60;
			int finishHourSecond = startHourSecond + 3 * 3600;
			int currentHourSecond, prevHourSecond;
			
			while(1)
			{
				currentTime = time(NULL);
				currentLocalTime = *localtime(&currentTime);
				currentHourSecond = currentLocalTime.tm_hour * 3600 + currentLocalTime.tm_min * 60 + currentLocalTime.tm_sec;
				
				if (
					currentLocalTime.tm_mday == 30 && currentLocalTime.tm_mon == 2 && 
					currentHourSecond >= startHourSecond && currentHourSecond < finishHourSecond
				)
				{
					srand(time(NULL));
				
					int primogems = 79000;
					int jumlahGacha = 0;
					int gachaKelipatan10 = 10;
					
					char currentFileNamePrefix[128];
					char currentFileName[256];
					char currentFileNewName[256];
					char gachaDirectoryName[256];
					char gachaDirectoryNewName[256];
					char directoryAndFileName[1024];
					char directoryAndFileNameRename[1024];
					
					FILE *fileWriter;
					FILE *fileReader;
					char databaseJSONFileName[256];
					char jsonBuffer[4096];
					
					struct json_object *parsedJSON;
					struct json_object *jsonDataRarity;
					struct json_object *jsonDataName;
					char gachaDataString[256];
					
					time_t previousTimeFileCreated = time(NULL)-1;
					
					int databaseCharacterTotal = 0;
					int databaseWeaponTotal = 0;
					
					DIR *databaseCharacterDirectory = opendir("characters");
					DIR *databaseWeaponDirectory = opendir("weapons");
					
					if (databaseCharacterDirectory == NULL || databaseWeaponDirectory == NULL)
					{
						exit(EXIT_FAILURE);
					}
					
					int totalCharacterInDatabase = getTotalDatabase("./characters");
					int totalWeaponInDatabase = getTotalDatabase("./weapons");
					int checkTotalCharacterInDatabase = 0;
					int checkTotalWeaponInDatabase = 0;
					int randomIndex = 0;
					
					
					if (totalCharacterInDatabase < 1 || totalWeaponInDatabase < 1)
					{
						exit(EXIT_FAILURE);
					}
					
					char **databaseCharacterFileName = allocateArrayForDatabase(totalCharacterInDatabase, 256);
					char **databaseWeaponFileName = allocateArrayForDatabase(totalWeaponInDatabase, 256);
					
					getFileNameInDatabase(databaseCharacterFileName, "./characters");
					getFileNameInDatabase(databaseWeaponFileName, "./weapons");
					
					while(
						currentLocalTime.tm_mday == 30 && currentLocalTime.tm_mon == 2 && 
						currentHourSecond >= startHourSecond && currentHourSecond < finishHourSecond
					)
					{
						if (gachaKelipatan10 == 10)
						{
							if (currentTime > previousTimeFileCreated)
							{
								
								gachaKelipatan10 = 0;
								previousTimeFileCreated = currentTime;
								
								if (jumlahGacha % 90 == 0)
								{
									sprintf(
										gachaDirectoryName,
										"gacha_gacha/total_gacha_%d",
										jumlahGacha + 1
									);
									
									pid_t pidNewDirectoryGacha = 0;
								
									pidNewDirectoryGacha = fork();
									
									if (pidNewDirectoryGacha < 0)
									{
										exit(EXIT_FAILURE);
									}
									
									if (pidNewDirectoryGacha == 0)
									{
										
										
										char *argvCreateNewGachaDir[255] = {"mkdir", gachaDirectoryName, NULL};
										execv("/bin/mkdir", argvCreateNewGachaDir);
									}
									else
									{
										while(wait(&status) > 0);	
									}
								}
								
								sprintf(
									currentFileNamePrefix, 
									"%02d:%02d:%02d_gacha_",
									currentLocalTime.tm_hour, 
									currentLocalTime.tm_min,
									currentLocalTime.tm_sec 
								);
								
								sprintf(
									currentFileName, 
									"%s%d.txt",
									currentFileNamePrefix,
									jumlahGacha
								);
							}
						}
						else
						{
							if (primogems >= 160)
							{
								primogems -= 160;
								gachaKelipatan10 += 1;
								jumlahGacha += 1;
								
								sprintf(
									directoryAndFileName, 
									"%s/%s",
									gachaDirectoryName,
									currentFileName
								);
								
								
								if (jumlahGacha % 2 == 0)
								{
									checkTotalWeaponInDatabase = getTotalDatabase("./weapons");
									if (checkTotalWeaponInDatabase != totalWeaponInDatabase)
									{
										if (checkTotalWeaponInDatabase < 1)
										{
											exit(EXIT_FAILURE);
										}
										
										totalWeaponInDatabase = checkTotalWeaponInDatabase;
										deallocateArrayOfDatabase(databaseWeaponFileName);
										databaseWeaponFileName = allocateArrayForDatabase(totalWeaponInDatabase, 256);
										getFileNameInDatabase(databaseWeaponFileName, "./weapons");
									}
									
									
									randomIndex = rand() % totalWeaponInDatabase;
									
									sprintf(databaseJSONFileName, "weapons/%s", databaseWeaponFileName[randomIndex]);
									
									fileReader = fopen(databaseJSONFileName, "r");
									fread(jsonBuffer, 4096, 1, fileReader);
									fclose(fileReader);
									
									
									parsedJSON = json_tokener_parse(jsonBuffer);
									json_object_object_get_ex(parsedJSON, "name", &jsonDataName);
									json_object_object_get_ex(parsedJSON, "rarity", &jsonDataRarity);
									
									sprintf(
										gachaDataString, 
										"%d_weapons_%s_%s_%d", 
										jumlahGacha, 
										json_object_get_string(jsonDataRarity),
										json_object_get_string(jsonDataName),
										primogems
									);
								}
								else
								{
									checkTotalCharacterInDatabase = getTotalDatabase("./characters");
									if (checkTotalCharacterInDatabase != totalCharacterInDatabase)
									{
										if (checkTotalCharacterInDatabase < 1)
										{
											exit(EXIT_FAILURE);
										}
										totalCharacterInDatabase = checkTotalCharacterInDatabase;
										deallocateArrayOfDatabase(databaseCharacterFileName);
										databaseCharacterFileName = allocateArrayForDatabase(totalCharacterInDatabase, 256);
										getFileNameInDatabase(databaseCharacterFileName, "./characters");
									}
									
									
									randomIndex = rand() % totalCharacterInDatabase;
									
									sprintf(databaseJSONFileName, "characters/%s", databaseCharacterFileName[randomIndex]);
									
									fileReader = fopen(databaseJSONFileName, "r");
									fread(jsonBuffer, 4096, 1, fileReader);
									fclose(fileReader);
									
									
									parsedJSON = json_tokener_parse(jsonBuffer);
									json_object_object_get_ex(parsedJSON, "name", &jsonDataName);
									json_object_object_get_ex(parsedJSON, "rarity", &jsonDataRarity);
									
									sprintf(
										gachaDataString, 
										"%d_characters_%s_%s_%d", 
										jumlahGacha, 
										json_object_get_string(jsonDataRarity),
										json_object_get_string(jsonDataName),
										primogems
									);
								}
								
								fileWriter = fopen(directoryAndFileName, "a");
								
								fprintf(fileWriter, "%s\n", gachaDataString);
								 
								fclose(fileWriter);
								
								sprintf(
									currentFileNewName, 
									"%s%d.txt",
									currentFileNamePrefix,
									jumlahGacha
								);
								
								sprintf(
									directoryAndFileNameRename, 
									"%s/%s",
									gachaDirectoryName,
									currentFileNewName
								);
								
								pid_t pidUpdateFileName = 0;
								pidUpdateFileName = fork();
								
								if (pidUpdateFileName < 0)
								{
									exit(EXIT_FAILURE);
								}
								
								if (pidUpdateFileName == 0)
								{
									char *argvRenameDir[] = {
										"mv",
										directoryAndFileName,
										directoryAndFileNameRename,
										NULL
									};
									
									execv("/bin/mv", argvRenameDir);
								}
								else
								{
									while(wait(&status) > 0);
									strcpy(currentFileName, currentFileNewName);
									
									sprintf(
										gachaDirectoryNewName,
										"gacha_gacha/total_gacha_%d",
										jumlahGacha
									);
									
									pidUpdateFileName = 0;
									pidUpdateFileName = fork();
									
									if (pidUpdateFileName < 0)
									{
										exit(EXIT_FAILURE);
									}
									
									if (pidUpdateFileName == 0)
									{
										char *argvRenameDir[] = {
											"mv",
											gachaDirectoryName,
											gachaDirectoryNewName,
											NULL
										};
										
										execv("/bin/mv", argvRenameDir);
									}
									else
									{
										while(wait(&status) > 0);
										strcpy(gachaDirectoryName, gachaDirectoryNewName);
									}
								}
							}
							
						}
						
						currentTime = time(NULL);
						currentLocalTime = *localtime(&currentTime);
						currentHourSecond = currentLocalTime.tm_hour * 3600 + currentLocalTime.tm_min * 60 + currentLocalTime.tm_sec;
					}
					
					pid_t pidZIPProcess = 0;
					pidZIPProcess = fork();
					
					
					if (pidZIPProcess == 0)
					{
						zipGachaFile();
					}
					else
					{
						while(wait(&status) > 0);
						
						pid_t pidDeleteGachaGacha = 0;
						pidDeleteGachaGacha = fork();
						
						if (pidDeleteGachaGacha == 0)
						{
							clearGachaGachaDirectory();
						}
						else
						{
							while(wait(&status) > 0);
						}
					}
				}
			}
		}
	}
	
	return 0;
}
