#include	<stdio.h>
#include	<time.h>

int is_leap_year(int year);
void calendar(int mon);

char wday[][7]={
		"Sun",
		"Mon",
		"Tue",
		"Wed",
		"Thu",
		"Fri",
		"Sat"
};

int mday[]={
		31,
		0,
		31,
		30,
		31,
		30,
		31,
		31,
		30,
		31,
		30,
		31
};

int
main(){
	for(int i=1;i<=12;i++)
		calendar(i);
}

void
calendar(int mon){
	struct tm *time_s;
	time_t time_val;
	time(&time_val);
	time_s=localtime(&time_val);
	time_s->tm_mday=1;
	time_s->tm_mon=mon-1;
	if(mktime(time_s)<0)
		perror("mktime");
	printf("%d月\n",1+time_s->tm_mon);
	for(int i=0;i<7;i++)
		printf("%s\t",wday[i]);
	printf("\n");
	for(int i=0;i<time_s->tm_wday;i++)
		printf("%s\t"," ");
	int f_line=7-time_s->tm_wday;
	for(int i=1;i<=f_line;i++)
		printf("%d\t",i);
	printf("\n");
	int count=0;
	if(is_leap_year(1900+time_s->tm_year))
		mday[1]=29;
	else
		mday[1]=28;
	for(int i=f_line+1;i<=mday[time_s->tm_mon];i++){
		printf("%d\t",i);
		count++;
		if(count%7==0)
			printf("\n");
	}
	printf("\n\n");
}

int
is_leap_year(int year){
	if(year%4==0&&year%100||year%400==0)
		return 1;
	else
		return 0;
}
