#define _CRT_SECURE_NO_WARNINGS
#include "dirent.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TWO 2
#define THREE 3
#define ONE 1
#define ZERO 0
#define MAX_SIZE 50
#define TWENTY_PERCENT 0.2
typedef struct dataFiles
{
	char* pathOfFile;
	int location;
}dataFiles;

typedef struct AllFiles
{
	dataFiles** eachFileData;
	int amountOfFiles;
}AllFiles;


DIR* openDir(char* dirName);
void writeToFile(char* direct, char* userChoice, AllFiles* filesToCheckVirus);
void addingDataToFile(char* data, AllFiles* filesToCheckVirus);
void addFilesToStruct(AllFiles* filesToCheckVirus, char** arrayOfFiles, char* directPath, int lenArray);
void swap(char** arrayOfFiles, int replaceOne, int replaceTwo);
void orderFiles(char** arrayOfFiles, int size);
void normalScan(AllFiles* filesToCheckVirus, char* fileStr, char* virusStr, int counterAmountOfFiles);
void quickScan(AllFiles* filesToCheckVirus, char* fileStr, char* virusStr, int counterAmountOfFiles);
char* menu(char* folder, char* virus);
char* createFullPath(char* direct, char* filePath);
void runOverFiles(DIR* direct, AllFiles* filesToCheckVirus, char* directPath);
void checkEachFile(AllFiles* filesToCheckVirus, char* userChoice);
void addNewFileToStruct(AllFiles* filesToCheckVirus, dataFiles* newFile);
dataFiles* createNewFile(char* pathOfFile);
void printIfInfected(AllFiles* filesToCheckVirus, char* nameOfFile, int indicator, int typeScan);
char* openFiles(AllFiles* fileToOpen, int index);
int checkIfVirusInFile(char* fileStr, char* virusStr, int lenVirus, int lenFile, int startValueCount);
void checkOpenDir(DIR* direct);
void checkAllocation(void* ptr);
void freeMemory(AllFiles* fileToClose);
void checkFileOpen(void* file, char* fileName);

int main(int argc, char** argv)
{
	DIR* direct = NULL;
	AllFiles filesToCheckVirus = { NULL, ZERO };
	char* userChoice = NULL;
	if (argc == THREE)
	{
		userChoice = menu(argv[ONE], argv[TWO]);
		direct = openDir(argv[ONE]);
		addNewFileToStruct(&filesToCheckVirus, createNewFile(argv[TWO]));
		runOverFiles(direct, &filesToCheckVirus, argv[ONE]);
		writeToFile(argv[ONE], userChoice, &filesToCheckVirus);
		checkEachFile(&filesToCheckVirus, userChoice);
		freeMemory(&filesToCheckVirus);
	}
	else
	{
		printf("USAGE: <dirPath>.....<virusPath>");
	}
	getchar();
	return 0;
}
/*
this function will add the first part of the data the user need after the scanning in to file
input: direct, virus, userChoice, filesToCheckVirus
output: none
*/
void writeToFile(char* direct, char* userChoice, AllFiles* filesToCheckVirus)
{
	FILE* file = fopen(filesToCheckVirus->eachFileData[filesToCheckVirus->amountOfFiles - ONE]->pathOfFile, "w");
	checkFileOpen(file, filesToCheckVirus->eachFileData[filesToCheckVirus->amountOfFiles - ONE]->pathOfFile);
	fputs("Anti-virus began! Welcome!\n\nFolder to scan:\n", file);
	fputs(direct, file);
	fputs("\nVirus signature:\n", file);
	fputs(filesToCheckVirus->eachFileData[ZERO]->pathOfFile, file);
	fputs("\n\nScanning option:\n", file);
	fputs(userChoice, file);
	fputs("\n\nResults:\n", file);
	fclose(file);
}
/*
this function will receive the user choice (quick or normal scan) and return it
input: folder, virus
output: userChoice
*/
char* menu(char* folder, char* virus)
{
	char* userChoice = NULL;
	int normalOrQuick = ZERO;
	printf("Welcome to my Virus scan!\n\nFolder to scan: %s\nVirus signature: %s\n\nPress 0 for normal scan and any other key for a quick scan: ", folder, virus);
	scanf("%d", &normalOrQuick);
	getchar();
	if (normalOrQuick == ZERO)
	{
		userChoice = "Normal scan";
	}
	else
	{
		userChoice = "Quick Scan";
	}
	printf("Scanning began...\nThis process may take several minutes...\n\nScanning:\n");
	return userChoice;
}
/*
this function will open direct using dirName and return it open.
input: dirName
output: none
*/
DIR* openDir(char* dirName)
{
	DIR* direct = NULL;
	direct = opendir(dirName);
	checkOpenDir(direct);
	return direct;
}
/*
this function will create a new typed dataFiles struct and return it. it will also add it's pathOfFile
input: pathOfFile
output: newFile
*/
dataFiles* createNewFile(char* pathOfFile)
{
	dataFiles* newFile = (dataFiles*)malloc(sizeof(dataFiles));
	checkAllocation(newFile);
	newFile->pathOfFile = pathOfFile;
	return newFile;
}

/*
this function will add new file struct to filesToCheckVirus as the size of filesToCheckVirus->eachFileData
input: filesToCheckVirus, newFile
output: none
*/
void addNewFileToStruct(AllFiles* filesToCheckVirus, dataFiles* newFile)
{
	filesToCheckVirus->amountOfFiles++;
	filesToCheckVirus->eachFileData = (dataFiles**)realloc(filesToCheckVirus->eachFileData, filesToCheckVirus->amountOfFiles * sizeof(dataFiles*));
	checkAllocation(filesToCheckVirus->eachFileData);
	filesToCheckVirus->eachFileData[filesToCheckVirus->amountOfFiles - ONE] = newFile;
}
/*
this function will run over all the files from the direct it received and send them to create full path
and add them to an array of files path.
input: direct, filesToCheckVirus, directPath
output: none
*/
void runOverFiles(DIR* direct, AllFiles* filesToCheckVirus, char* directPath)
{
	struct dirent* dir = NULL;
	char** arrayOfAllFiles = NULL;
	char* pathOfFile = NULL;
	int counterFiles = 0;
	while ((dir = readdir(direct)) != NULL)
	{
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..") && dir->d_type != DT_DIR) // run over each file from dir direct
		{
			pathOfFile = createFullPath(directPath, dir->d_name); // creating full path for each file
			counterFiles++;
			arrayOfAllFiles = (char**)realloc(arrayOfAllFiles, counterFiles * sizeof(char*)); // adding each full path to array of files
			checkAllocation(arrayOfAllFiles);
			arrayOfAllFiles[counterFiles - ONE] = pathOfFile;
		}
	}
	addFilesToStruct(filesToCheckVirus, arrayOfAllFiles, directPath, counterFiles); // call to function that will add all files from file array to structs
	closedir(direct);
}

/*
this function will receive array of files, order it and add it to new struct which will be added
to filesToCheckVirus.
input: filesToCheckVirus, arrayOfFiles, directPath, lenArray
output: none
*/
void addFilesToStruct(AllFiles* filesToCheckVirus, char** arrayOfFiles, char* directPath, int lenArray)
{
	int counter = 0;
	char* pathOfFile = NULL;
	dataFiles* newFile = NULL;
	orderFiles(arrayOfFiles, lenArray);
	for (counter = ZERO; counter < lenArray; counter++) // create new struct from type dataFiles for each file name in arrayOfFiles and add it to filesToCheckVirus
	{
		newFile = createNewFile(arrayOfFiles[counter]); 
		addNewFileToStruct(filesToCheckVirus, newFile);
	}
	pathOfFile = createFullPath(directPath, "AntiVirusLog.txt"); // add the file the data of the virus will be added to at the end of the program
	newFile = createNewFile(pathOfFile);
	addNewFileToStruct(filesToCheckVirus, newFile);
	free(arrayOfFiles);
}
/*
this function will order the files by alphabetic order
input: arrayOfFiles, size
output: none
*/
void orderFiles(char** arrayOfFiles, int size)
{
	int counter = ZERO;
	int secondCounter = ZERO;
	for (counter = ZERO; counter < size; counter++) // compare each letter from the counter array with each letter from the secondCounter array and swap if needed
	{
		for (secondCounter = ZERO; secondCounter < size; secondCounter++)
		{
			if (strcmp(*(arrayOfFiles + counter), *(arrayOfFiles + secondCounter)) < ZERO) // check if needs to swap between the values
			{
				swap(arrayOfFiles, counter, secondCounter); // swap file strings
			}
		}
	}
}
/*
this function will create full path of a file and return it
input: direct, filePath
output: fullPath - the full path
*/
char* createFullPath(char* direct, char* filePath)
{
	char* fullPath = (char*)malloc(strlen(direct) * sizeof(char) + strlen(filePath) * sizeof(char) + strlen("/") * sizeof(char) + ONE); // creating full path
	checkAllocation(fullPath);
	strncpy(fullPath, direct, (strlen(direct) + ONE));
	strcat(fullPath, "/");
	strncat(fullPath, filePath, strlen(filePath));
	return fullPath;
}
/*
this function will compare each char from the virus file and the file to check. if the virus is inside the
file it will return 1, else it will return 0. also the function start checking from the start value it receives
input: fileStr, virusStr, lenVirus, lenFile, startValueCount
output: indicatorVirus - if the virus inside the file to check or not
*/
int checkIfVirusInFile(char* fileStr, char* virusStr, int lenVirus, int lenFile, int startValueCount)
{
	int counter = startValueCount;
	int index = ZERO;
	int indicatorVirus = ZERO;
	do {
		if (virusStr[index] == fileStr[counter]) // compare each char from virusStr and fileStr, if equal add to index + 1
		{
			index++;
		}
		else
		{
			index = ZERO; // if they are not equal go back to the start of the virus string and check from the start of it
		}
		if (index == lenVirus)
		{
			indicatorVirus = ONE; // if the length of the virus equal to index the indicator of the virus in the file will turn on and the loop will stop
		}
		counter++;
	} while (indicatorVirus == ZERO && counter < (lenFile + startValueCount));
	return indicatorVirus;
}

/*
this function will print if the file is infected or not and if its infected on the first 20% last 20% or just infected
will also send this data to function that will add it to data file of the virus and all files
input: filesToCheckVirus, nameOfFile, indicator, typeScan, indexOfFile
output: none
*/
void printIfInfected(AllFiles* filesToCheckVirus, char* nameOfFile, int indicator, int typeScan)
{
	char* infectedOrNot = NULL;
	char* firstOrLast = NULL;
	if (indicator == ONE)
	{
		printf("%s  Infected!", nameOfFile);
		infectedOrNot = " Infected!";
		if (typeScan == ONE)
		{
			printf(" (first 20%%)");
			firstOrLast = " (first 20%)";
		}
		else if (typeScan == TWO)
		{
			printf(" (last 20%%)");
			firstOrLast = " (first 20%)";
		}
		printf("\n");
	}
	else
	{
		printf("%s  Clean\n", nameOfFile);
		infectedOrNot = "  Clean\n";
	}
	addingDataToFile(nameOfFile, filesToCheckVirus); // adding the name of the file to the data file 
	addingDataToFile(infectedOrNot, filesToCheckVirus); // adding if the file is infected or not
	addingDataToFile(firstOrLast, filesToCheckVirus); // if the file is infected adding if it was in the first or last 20%
	if (!strcmp(infectedOrNot, " Infected!"))
	{
		addingDataToFile("\n", filesToCheckVirus); 
	}
}
/*
this function will add data to the file that all the data about the virus and files are from
input: filesToCheckVirus, data
output: none
*/
void addingDataToFile(char* data, AllFiles* filesToCheckVirus)
{
	FILE* file = fopen(filesToCheckVirus->eachFileData[filesToCheckVirus->amountOfFiles - ONE]->pathOfFile, "a");
	checkFileOpen(file, filesToCheckVirus->eachFileData[filesToCheckVirus->amountOfFiles - ONE]->pathOfFile);
	if (data != NULL)
	{
		fputs(data, file);
	}
	fclose(file);
}

/*
this function will run over every file and by the choice from the user decide whether call to quick scan or normal scan
it will also get the data from the virus file and the file to check
input: filesToCheckVirus,  userChoice
output: none
*/
void checkEachFile(AllFiles* filesToCheckVirus, char* userChoice)
{
	char* fileStr = NULL;
	char* virusStr = NULL;
	int counterAmountOfFiles = 0;
	virusStr = openFiles(filesToCheckVirus, ZERO); // open virus file
	for (counterAmountOfFiles = ONE; counterAmountOfFiles < filesToCheckVirus->amountOfFiles - ONE; counterAmountOfFiles++) // run over all files from filesToCheckVirus
	{
		fileStr = openFiles(filesToCheckVirus, counterAmountOfFiles); // open each file in filesToCheckVirus
		if (!strcmp(userChoice, "Normal scan"))
		{
			normalScan(filesToCheckVirus, fileStr, virusStr, counterAmountOfFiles); 
		}
		else
		{
			quickScan(filesToCheckVirus, fileStr, virusStr, counterAmountOfFiles);
		}
		free(fileStr);
	}
	free(virusStr);
	printf("Scan Completed.\nSee log path for results: %s\n", filesToCheckVirus->eachFileData[filesToCheckVirus->amountOfFiles - ONE]->pathOfFile);
}
/*
this function will make the normal scan, run over all the file and check if the virus is in the file
input: filesToCheckVirus, fileStr, counterAmountOfFiles
output: none
*/
void normalScan(AllFiles* filesToCheckVirus, char* fileStr, char* virusStr, int counterAmountOfFiles)
{
	int indicatorVirus;
	indicatorVirus = checkIfVirusInFile(fileStr, virusStr, filesToCheckVirus->eachFileData[ZERO]->location, filesToCheckVirus->eachFileData[counterAmountOfFiles]->location, ZERO); // check if virus in file
	printIfInfected(filesToCheckVirus, filesToCheckVirus->eachFileData[counterAmountOfFiles]->pathOfFile, indicatorVirus, ZERO); // print if virus in file or not
}

/*
this function will make the quick scan, it will start check for the virus in the first 20% of the data in the file, then last 20% and then
full scan if it couldnt find anything in those 20%
input: filesToCheckVirus, fileStr, counterAmountOfFiles
output: none
*/
void quickScan(AllFiles* filesToCheckVirus, char* fileStr, char* virusStr, int counterAmountOfFiles)
{
	int indicatorVirus;
	int keepLocationLastChars = ZERO;
	indicatorVirus = checkIfVirusInFile(fileStr, virusStr, filesToCheckVirus->eachFileData[ZERO]->location, (int)(filesToCheckVirus->eachFileData[counterAmountOfFiles]->location * TWENTY_PERCENT), ZERO); // check first 20% of the file
	if (indicatorVirus == ONE)
	{
		printIfInfected(filesToCheckVirus, filesToCheckVirus->eachFileData[counterAmountOfFiles]->pathOfFile, indicatorVirus, ONE); // if the first 20% of the file has the virus it will print it
	}
	else
	{
		keepLocationLastChars = (int)(filesToCheckVirus->eachFileData[counterAmountOfFiles]->location - (filesToCheckVirus->eachFileData[counterAmountOfFiles]->location * TWENTY_PERCENT)) + ONE;
		indicatorVirus = checkIfVirusInFile(fileStr, virusStr, filesToCheckVirus->eachFileData[ZERO]->location, (int)(filesToCheckVirus->eachFileData[counterAmountOfFiles]->location * TWENTY_PERCENT), keepLocationLastChars); // check last 20% of the file
		if (indicatorVirus == ONE)
		{
			printIfInfected(filesToCheckVirus, filesToCheckVirus->eachFileData[counterAmountOfFiles]->pathOfFile, indicatorVirus, TWO); // if the last 20% of the file has the virus it will print it
		}
		else
		{
			normalScan(filesToCheckVirus, fileStr, virusStr, counterAmountOfFiles); // run over all the file to check the virus
		}
	}
}

/*
this function will open files and return the data in the file as string
input: fileToOpen, index
output: sourceData - all file data as str
*/
char* openFiles(AllFiles* fileToOpen, int index)
{
	int lenOfFile = 0;
	char* data = NULL;
	FILE* file = fopen(fileToOpen->eachFileData[index]->pathOfFile, "rb");
	checkFileOpen(file, fileToOpen->eachFileData[index]->pathOfFile);
	fseek(file, ZERO, SEEK_END); // go to end of file
	lenOfFile = ftell(file); // receive amount of chars in file file
	fileToOpen->eachFileData[index]->location = lenOfFile;
	data = (char*)malloc(lenOfFile * sizeof(char)); // set data size as the amount of chars from file
	checkAllocation(data);
	fseek(file, ZERO, SEEK_SET); // back to the start of the file
	fread(data, sizeof(char), lenOfFile, file);  // reading the data from file to data
	fclose(file);
	return data;
}

/*
this function will check if the direct oppened properly or not
input: file, fileName
output: none
*/
void checkOpenDir(DIR* direct)
{
	if (direct == NULL)
	{
		printf("error open direct make sure u typed it right!");
		exit(ONE);
	}
}

/*
this function will check if the dinamic allocation worked properly
input: ptr
output: none
*/
void checkAllocation(void* ptr)
{
	if (ptr == NULL)
	{
		printf("Memory allocation error!\n");
		exit(ONE);
	}
}
/*
this function will free the memory from fileToClose
input: fileToClose
output: none
*/
void freeMemory(AllFiles* fileToClose)
{
	int i = 0;
	free(fileToClose->eachFileData[i]); // free eachFileData[0] because the virus file is in that index and it wasnt dinamic allocated
	for (i = ONE; i < fileToClose->amountOfFiles; i++) // run over all dinamic allocation from fileToClose
	{
		free(fileToClose->eachFileData[i]->pathOfFile);
		free(fileToClose->eachFileData[i]);
	}
	free(fileToClose->eachFileData);
}
/*
this function will replace between the strings in the replaceOne and replaceTwo indexes
input: arrayOfFiles, replaceOne, replaceTwo
output: none
*/
void swap(char** arrayOfFiles, int replaceOne, int replaceTwo)
{
	char* tempStr = *(arrayOfFiles + replaceTwo); // keep the string in the replaceTwo index in temp string
	*(arrayOfFiles + replaceTwo) = *(arrayOfFiles + replaceOne); // replace strings
	*(arrayOfFiles + replaceOne) = tempStr;
}

/*
this function will check if the file oppened properly or not
input: file, fileName
output: none
*/
void checkFileOpen(void* file, char* fileName)
{
	if (file == NULL)
	{
		printf("cant open file %s\n", fileName);
		exit(ONE);
	}
}