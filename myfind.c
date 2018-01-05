#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <sys/ioctl.h>
#include <math.h>
#include <sys/types.h>

struct stat statbuf;
struct dirent *dp;
static int count=0;

void searchByName(char*, char*, char);
void searchByType(char*, char);

int main (int argc , char *argv[])
{
	if (argc == 4 || argc == 6)
	{
		if (strcmp(argv[2], "-name") == 0)
		{
			if (argc == 4)
				searchByName(argv[1], argv[3], '!');	
			else
				searchByName(argv[1], argv[3], argv[5][0]);
		}
		else if (strcmp(argv[2], "-type") == 0)
			searchByType(argv[1], argv[3][0]);
		else
			printf ("Invalid search type\n");
	}
	return 0;
}
void searchByName(char* path, char* search, char type)
{
	int i;		
	DIR *dir;
	struct stat info;
	char temp[100];
	char found[100];
	long mode;	
	dir = opendir(path);
	int len = strlen (path);
		
	if (dir == NULL)
	{
//		printf ("no such directory : %s\n", path);
		return;
	}
	if (path[len-1] != '/')
		strcat (path, "/");
	
	while ((dp = readdir(dir)) != NULL)
	{
		if ((strcmp(dp->d_name, ".") != 0) && (strcmp (dp->d_name, "..") != 0))
		{
			if (strcmp(dp->d_name, search) == 0)
			{
				strcpy (found, path);
				strcat (found, search);

			//	printf ("hello : %s\n", found);
				stat (found, &info);
				mode = info.st_mode;
				if (type == 'p' && (mode & 0170000) == 0010000)
				{
					printf ("%s\n", found);
				}
				else if (type == 'c' && (mode & 0170000) == 0020000)
				{
					printf ("%s\n", found);
				}
				else if (type == 'd' && (mode & 0170000) == 0040000)
				{
					printf ("%s\n", found);
				}
				else if (type == 'b' && (mode & 0170000) == 0060000)
				{
					printf ("%s\n", found);
				}
				else if (type == '-' && (mode & 0170000) == 0100000)
				{
					printf ("%s\n", found);
				}
				else if (type == 'l' && (mode & 0170000) == 0120000)
				{
					printf ("%s\n", found);
				}
				else if (type == 's' && (mode & 0170000) == 0140000)
				{
					printf ("%s\n", found);
				}
				else if (type == '!')
					printf ("%s\n", found);
			}
			
			strcpy (temp, path);
			strcat (temp, dp->d_name);
			if (stat (temp, &info) == -1)
			{
				printf ("\nError\n");
				return;
			}
			mode = info.st_mode;
			if ((mode & 0170000) == 0040000)
			{
				searchByName (temp, search, type);	//recursive function
			}
		}
	}
	return;	
}
void searchByType(char* path, char type)
{
	int i;		
	DIR *dir;
	struct stat info;
	char temp[100];
	char myDir[100];
	long mode;	
	dir = opendir(path);
	int len = strlen (path);
		
	if (dir == NULL)
	{
//		printf ("no such directory : %s\n", path);
		return;
	}
	if (path[len-1] != '/')
		strcat (path, "/");
	
	while ((dp = readdir(dir)) != NULL)
	{
		if ((strcmp(dp->d_name, ".") != 0) && (strcmp (dp->d_name, "..") != 0))
	//	if (dp->d_name[0] != '.')
		{			
			char temp[512];
			strcpy (temp, path);
			strcat (temp, dp->d_name);
			stat (temp, &info);
			mode = info.st_mode;
	//	printf ("%s\n", myDir);
			if (type == 'p' && (mode & 0170000) == 0010000)
			{
				printf ("%s\n", temp);
			}
			else if (type == 'c' && (mode & 0170000) == 0020000)
			{
				printf ("%s\n", temp);
			}
			else if (type == 'd' && (mode & 0170000) == 0040000)
			{
				printf ("%s\n", temp);
			}
			else if (type == 'b' && (mode & 0170000) == 0060000)
			{
				printf ("%s\n", temp);
			}
			else if (type == '-' && (mode & 0170000) == 0100000)
			{
				printf ("%s\n", temp);
			}
			else if (type == 'l' && (mode & 0170000) == 0120000)
			{
				printf ("%s\n", temp);
			}
			else if (type == 's' && (mode & 0170000) == 0140000)
			{
				printf ("%s\n", temp);
			}
			
			if ((mode & 0170000) == 0040000)
			{
				strcpy (myDir, path);
				strcat (myDir, dp->d_name);
				searchByType (myDir, type);
			}
		}
	}
}