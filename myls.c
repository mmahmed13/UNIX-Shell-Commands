#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <sys/ioctl.h>
#include <math.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

struct stat statbuf;
struct dirent *dp;
struct winsize wbuf;
char pwd[100];
char **mystr;
static int count = 0;
	
struct List 
{
	long linkCount, size, inode;
	char* name;
	char pmns[11];
	char* owner;
	char* group;
	long mtime; 
};
void allocate (char*, int);
void display (int);
int findLargest ();
void displayList (char*);
void checkPerm (long, char*);
void findLargestList (struct List*, int*, int*, int*, int*, int*, int*, int*);

int main (int argc , char *argv[])
{
	int i, largest;		
	DIR *dir;
			
	if (argc == 1 || (argc == 2 && strcmp(argv[1], "-li") == 0))
	{
		i=0;
		getcwd(pwd, 50);		
		dir = opendir(pwd);
		while((dp = readdir(dir)) != NULL)
		{
			if (dp->d_name[0] != '.')
				count++;					//counts number of files and dir
		}
		dir = opendir(pwd);
		mystr = (char**) malloc (sizeof(char*)*count);
		while ((dp = readdir(dir)) != NULL)
		{
			if (dp->d_name[0] != '.')
			{
				allocate (dp->d_name, i);
				i++;
			}
		}
		sort();
		if (argc == 1)
		{
			largest = findLargest();
			display(largest+3);
		}
		else
			displayList(pwd);
	}
	else
	{
		if (strcmp(argv[1], "-li") == 0)
			i=2;
		else
			i=1;
		for (; i<argc; i++)
		{
			dir = opendir(argv[i]);
			if (argv[i] == ".")
				getcwd(argv[i], 100);
			
			if (dir == NULL)
				printf ("no such directory exists\n");
			
			else
			{
				while((dp = readdir(dir)) != NULL)
				{
					if (dp->d_name[0] != '.')
						count++;
				}
				dir = opendir(argv[i]);
				if (i!=1)
					printf ("\n");
				printf ("%s:\n", argv[i]);
				mystr = (char**) malloc (sizeof(char*)*count);
				int j=0;
				while((dp = readdir(dir)) != NULL)
				{
					if (dp->d_name[0] != '.')
					{
						allocate (dp->d_name, j);
						j++;
					}
				}
				sort();
				if (strcmp(argv[1], "-li") != 0)
				{
					largest = findLargest();
					display(largest+3);
				}
				else
				{
					char copy[50];
					stpcpy (copy, argv[i]);
					displayList(copy);
				}
				
			}	
		}
	}
	return 0;
}
void allocate (char* in, int j)
{
	mystr[j] = (char*) malloc (sizeof(char*)*50);
	strcpy (mystr[j], in);
}
void display (int colSize)
{
	if(ioctl(0, TIOCGWINSZ, &wbuf) == -1)
	{
		perror("Error in ioctl");
		exit(1);
	}
	int i, j, c, width = wbuf.ws_col, cols, rows;
	cols = width/colSize;
	rows = ceil((double)count/cols);
	
	for (i=0,c=0; i<count; c++)
	{
		for (j=0; j<rows && i<count; j++)
		{
			printf ("\033[%dG%s\n", (colSize*c), mystr[i++]);
		}
		if (i<count)
			printf ("\033[%dA", rows);
	}
	for (;j<rows; j++)
		printf ("\n");
	
	for (i=0; i<count; i++)
		free (mystr[i]);
	free (mystr);
	count = 0;
}
void sort ()
{
	int i,j;
	char temp[50];
	for (i=0; i<count; i++) 
	{
		for (j=0; j<count-1; j++) 
		{
			if (strcasecmp((tolower(mystr[j])), (tolower(mystr[j + 1]))) > 0) 
			{
				strcpy(temp, mystr[j]);
				strcpy(mystr[j], mystr[j + 1]);
				strcpy(mystr[j + 1], temp);
			}
		}
   }
}
int findLargest ()
{
	int l=0, i;
	for (i=0; i<count; i++)
	{
		if (strlen(mystr[i]) > l)
			l = strlen(mystr[i]);
	}
	return l;
}

void displayList (char* path)
{
	struct stat info;
	struct List l1[count];
	int len = strlen (path);
	int i, mode, uid, gid;
	if (path[len-1] != '/')
		strcat (path, "/");
	
	for (i=0; i<count; i++)
	{
		l1[i].name = (char*) malloc(30);
		l1[i].group = (char*) malloc(20);
		l1[i].owner = (char*) malloc(20);
		
		char newPath[50];
		strcpy (newPath, path);
		strcat (newPath, mystr[i]);
		
		stat (newPath, &info);
		stpcpy (l1[i].name, mystr[i]);
		mode = info.st_mode;
		l1[i].linkCount = info.st_nlink;
		uid = info.st_uid;
		gid = info.st_gid;
		l1[i].size = info.st_size;
		l1[i].mtime = info.st_mtime;
		l1[i].inode = info.st_ino;
		
		struct passwd * pwd = getpwuid(uid);
		stpcpy (l1[i].owner, pwd->pw_name);
		struct group * grp = getgrgid(gid);
		strcpy (l1[i].group, grp->gr_name);
		checkPerm (mode, l1[i].pmns);
		
/*		printf ("\nName : %s\n", l1[i].name);
		printf ("Link : %ld\n", l1[i].linkCount);
		printf ("Perm : %s\n", l1[i].pmns);
		printf ("Owner : %s\n", l1[i].owner);
		printf ("Group : %s\n", l1[i].group);
		printf ("Size : %ld\n", l1[i].size);
		printf ("Node : %ld\n", l1[i].inode);
		printf ("Time : %ld\n", l1[i].mtime);
		
		free (l1[i].name);
		free (l1[i].owner);
		free (l1[i].group);	*/	
	}
	int in=0, li=0, o=0, g=0, t=0, n=0, s=0;
	findLargestList (l1, &in, &li, &o, &g, &t, &n, &s);
	
	for (i=0; i<count; i++)
	{
		printf ("%-*ld %s %-*ld ", in, l1[i].inode, l1[i].pmns, li, l1[i].linkCount);
		printf ("%-*s %-*s %-*ld %-*ld %-*s\n", o, l1[i].owner, g, l1[i].group, s, l1[i].size, t, l1[i].mtime, n, l1[i].name);
	}
	
	for (i=0; i<count; i++)
		free (mystr[i]);
	free (mystr);
	count = 0;
}
void findLargestList (struct List* list, int* in, int* li, int* o, int* g, int* t, int* n, int* s)
{
	int i;
	for (i=0; i<count; i++)
	{
		if (strlen(list[i].name) > *n)
			*n = strlen(list[i].name);
		if (strlen(list[i].owner) > *o)
			*o = strlen(list[i].owner);
		if (strlen(list[i].group) > *g)
			*g = strlen(list[i].group);
		char buf[40];
		sprintf(buf, "%lu", list[i].inode);
		if (strlen(buf) > *in)
			*in = strlen(buf);
		sprintf(buf, "%lu", list[i].linkCount);
		if (strlen(buf) > *li)
			*li = strlen(buf);
		sprintf(buf, "%lu", list[i].mtime);
		if (strlen(buf) > *t)
			*t = strlen(buf);
		sprintf(buf, "%lu", list[i].size);
		if (strlen(buf) > *s)
			*s = strlen(buf);
	}
}
void checkPerm (long m, char* p)
{
	int i;
	for (i=0; i<10; i++)
		p[i] = '-';
	p[10] = '\0';
	if ((m & 0170000) == 0010000)
		p[0] = 'p';
	else if ((m & 0170000) == 0020000)
		p[0] = 'c';
	else if ((m & 0170000) == 0040000)
		p[0] = 'd';
	else if ((m & 0170000) == 0060000)
		p[0] = 'b';
	else if ((m & 0170000) == 0100000)
		p[0] = '-';
	else if ((m & 0170000) == 0120000)
		p[0] = 'l';
	else if ((m & 0170000) == 0140000)
		p[0] = 's';
	
	if ((m & 0000400) == 0000400)
		p[1] = 'r';
	if ((m & 0000200) == 0000200)
		p[2] = 'w';
	if ((m & 0000100) == 0000100)
		p[3] = 'x';
	if ((m & 0000040) == 0000040)
		p[4] = 'r';
	if ((m & 0000020) == 0000020)
		p[5] = 'w';
	if ((m & 0000010) == 0000010)
		p[6] = 'x';
	if ((m & 0000004) == 0000004)
		p[7] = 'r';
	if ((m & 0000002) == 0000002)
		p[8] = 'w';
	if ((m & 0000001) == 0000001)
		p[9] = 'x';
	
	if ((m & 0004000) == 0004000)
		p[3] = 's';
	if ((m & 0002000) == 0002000)
		p[6] = 's';
	if ((m & 0001000) == 0001000)
		p[9] = 't';
}