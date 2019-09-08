#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const char alpb[]="abcdefghijklmnopqrstuvwxyz+-*/";

main() {
	unsigned short num,i,j,len=strlen(alpb);
	FILE *fp;
	char str[64],*p,s1[32],s2[32];
	srand(time(NULL));
	printf("data : ");
	scanf("%hu",&num);
	getchar();
	printf("file name : ");
	fgets(str,63,stdin);
	p=str;
	while(*p) {
		if(*p=='\n') *p='\0';
		p++;
	}
	if((fp=fopen(str,"w"))==NULL) return 1;
	for(i=0;i<num;i++) {
		for(j=0;j<10;j++) {
			s1[j]=alpb[rand()%len];
		}
		s1[j]='\0';
		for(j=0;j<10;j++) {
			s2[j]=alpb[rand()%len];
		}
		s2[j]='\0';
		fprintf(fp,"%s %s 0000-%05d\n",s1,s2,i+1);
	}
	fclose(fp);
	return 0;
}
