#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h> 
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int do_more(FILE *, int, char*);
int  get_input(FILE*,int);
void canonical_echo();
void nonCanonical_nonEcho();

long total_size(FILE*);
long total_lines = 0;
long lines_displayed=0;
int PAGELEN=0;
int LINELEN=0;
char * name;
int file_num=0;
struct winsize w;

int main(int argc , char *argv[])
{
	nonCanonical_nonEcho();		//setting mode	
	int i=0,ex=0;
	if (argc == 1)
	{	
		do_more(stdin,1,argv[1]);
   	}
   	FILE * fp;
	
    ioctl(0, TIOCGWINSZ, &w);
	PAGELEN=w.ws_row-1;
	LINELEN=w.ws_col;
	
   	while(++i < argc && ex==0)
	{
		fp = fopen(argv[i] , "r");
		if (fp == NULL)
		{
			perror("Can't open file");
			exit (1);
		}
//add fol one line
		total_lines = total_size(fp);
		name=argv[i];
		file_num++;
		ex=do_more(fp,argc, argv[1]);
		fclose(fp);
	}
	canonical_echo();	//resetting mode
	
	return 0;
}

int do_more(FILE *fp, int n, char* fileName)
{
	char buffer[LINELEN]; 
	int rv;
	int num_of_lines=0;
	int oneline=0;
	
	if(lines_displayed<PAGELEN)
		num_of_lines=lines_displayed;
	else
		num_of_lines=PAGELEN;
	
	lines_displayed=0;
	
	
	FILE* fp_tty = fopen("/dev/tty", "r");
	if (fp_tty == NULL)
		exit(1);
	while (fgets(buffer, LINELEN, fp))
	{

		if (oneline==1 || num_of_lines == PAGELEN || (file_num>=2 && lines_displayed==0))
		{
			rv = get_input(fp_tty,n);
			if (rv == 0)
			{
				printf("\033[1A\033[2K\033[1G");
				return 1; //1 means quit the program
			}
			else if (rv == 1)
			{//user pressed space bar
				
				printf("\033[1A\033[2K \033[1G");
				num_of_lines -= PAGELEN;
				oneline=0;
			}
			else if (rv == 2)
			{//user pressed return/enter
				printf("\033[1A\033[2K \033[1G");
				num_of_lines -= 1; //show one more line            
				oneline=1;
			}
			else if (rv == 3)
			{ //invalid character
				printf("\033[1A\033[2K\033[1G");
				break; 
			}
			else if (rv == 4) //open vim
			{
				int cpid = -1,status;
				cpid = fork();

				if (cpid == -1)
				{
					perror("Fork Failed");
					exit (1);
				}
				if (cpid == 0) //child 
				{
					execlp("/usr/bin/vi","openVim",fileName,NULL);
					perror("exec Failed");
					exit(1);
				}
				else       //parent
					wait(&status);
			}
			else if (rv == 5)
			{
				int cpid = -1,status;
				cpid = fork();
   
				if (cpid == -1)
				{
					perror("Fork Failed");
					exit (1);
				}
				if (cpid == 0) //child 
				{
					execlp("/usr/bin/vi","help","help.txt",NULL);
					perror("exec Failed");
					exit(1);
				}
				else       //parent
					wait(&status);
			}
			ioctl(0, TIOCGWINSZ, &w);
			PAGELEN=w.ws_row-1;
			LINELEN=w.ws_col;
		}
		if(n>2 && lines_displayed==0)
		{
			printf(":::::::::::::::\n");
			printf("%s\n",name);
			printf(":::::::::::::::\n");
			num_of_lines+=3;
			if(num_of_lines>=PAGELEN)
			{
				num_of_lines=PAGELEN-1;
			}
		}

		fputs(buffer, stdout);
		num_of_lines++;
		lines_displayed++; //for calculating percentage
	}
	
	return 0;
}

/* print message, wait for response, return # of lines to advance q means no, space means yes, CR means one line */
int get_input(FILE * cmdstream,int n)
{
	int c;
	int percentage = 100*lines_displayed/total_lines;
	
	if(n>=2 && lines_displayed==0)
	{
		printf("\033[7m--More--(Next file : %s) \033[m",name);			
	}
	else
	{
		printf("\033[7m--More--(%d%%)\033[m", percentage);		
	}
	while((c=getc(cmdstream)) != EOF)
	{
		if(c == 'q')
			return 0;
		if ( c == ' ' )			
			return 1;
		if ( c == 'n' )	
			return 2;
		if (c == 'v')
			return 4;
		if (c == 'h')
			return 5;
		return 3;
	}
	return 0;
}

long total_size(FILE* fp)
{
	long lines = 0;
	char buff[LINELEN];
	while(fgets(buff,LINELEN, fp))
		lines++;
	fseek(fp, 0, SEEK_SET);
	return lines;
}
void nonCanonical_nonEcho ()
{
	char command[50];
   	strcpy(command, "stty -icanon && stty -echo" );
   	system(command);
}


void canonical_echo ()
{
	char command[50];
   	strcpy(command, "stty icanon && stty echo" );
   	system(command);
}

