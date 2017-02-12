#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum Color{
    MACRO=31,
    FUNC,
    KEYWORD,
    COMMENT,
    CHAR,
    STRING,
    VARIANT,
    HEADER=MACRO,
    PUNCT=FUNC,
}Color;

void turn_color_on(Color c);
void turn_color_off();
void close_app();
void space();
void punct();
void macro();
void header();
void string();
void character();
void comment();
void word();
void highlight();

//"FILE NULL EOF" actually aren't keywords,only for the sake of distinction.
static char 
keywords[]="char short int long float double signed unsigned "\
           "static void extern volatile const auto register"\
           "if else switch case default while do for break continue goto "\
           "struct enum union "\
           "return typedef sizeof "\
           "FILE NULL EOF";

FILE* istream=NULL;
/*
 * in case of the situations such as:
 * for(int i=0;i<10;i++){}
 * in which all after <(10;i++){}) are regarded as a header file.
 * 
 * set in macro();reset in header() or string();used for justification in highlight()
 */
int isheader=0;

void
turn_color_on(Color c){
    printf("\e[1;%dm",c);
}

void 
turn_color_off(){
    printf("\e[0m");
}

void
close_app(){
    turn_color_off();
    fclose(istream);
    exit(0);
}

void
space(){
    int c;
    while((c=fgetc(istream))!=EOF&&isspace(c)){
        printf("%c",c);
    }
    if(c==EOF){
        close_app();
    }else{
        ungetc(c,istream);
    }
}

void
punct(){
    int c;
    turn_color_on(PUNCT);
    while((c=fgetc(istream))!=EOF&&(strchr("#\'\"/",c)==NULL&&ispunct(c))){
        printf("%c",c);
    }
    if(c==EOF){
        close_app();
    }else{
        ungetc(c,istream);
    }
    turn_color_off();
}

void
macro(){
    int c='#';
    turn_color_on(MACRO);
    printf("#");
    space();
    isheader=1;
    int i=0;
    while((c=fgetc(istream))!=EOF&&isalpha(c)){
        printf("%c",c);
        if(i<8){
            isheader*=("include"[i++]==c);
        }else{
            isheader=0;
        }
    }
    if(c==EOF){
        close_app();
    }else{
        ungetc(c,istream);
    }
    turn_color_off();
}

void
character(){
    int c='\'';
    turn_color_on(CHAR);
    do{
        printf("%c",c);
        if(c=='\\'){
            c=fgetc(istream);
            if(c==EOF){
                close_app();
            }else{
                printf("%c",c);
            }
        }
    }while((c=fgetc(istream))!=EOF&&c!='\'');
    if(c!=EOF){
        printf("'");
    }else{
        close_app();
    }
    turn_color_off();
}

void
string(){
    if(isheader==1){
        turn_color_on(HEADER);
        isheader=0;
    }else{
        turn_color_on(STRING);
    }
    int c='"';
    do{
        printf("%c",c);
        if(c=='\\'){
            c=fgetc(istream);
            if(c!=EOF){
                printf("%c",c);
            }else{
                close_app();
            }
        }
    }while((c=fgetc(istream))!=EOF&&c!='"');
    if(c==EOF){
        close_app();
    }else{
        printf("\"");
    }
    turn_color_off();
}

void
header(){
    int c='<';
    turn_color_on(HEADER);
    do{
        printf("%c",c);
    }while((c=fgetc(istream))!=EOF&&c!='>');
    if(c==EOF){
        close_app();
    }else{
        printf(">");
    }
    turn_color_off();
    isheader=0;
}

void
comment(){
    int c='/';
    turn_color_on(COMMENT);
    printf("/");
    if((c=fgetc(istream))!=EOF){
        //comment type://
        if(c=='/'){
            do{
                printf("%c",c);
            }while((c=fgetc(istream))!=EOF&&c!='\n');
            if(c!=EOF){
                printf("\n");
            }else{
                close_app();
            }
        }else if(c=='*'){
            //comment type:/**/
            printf("*");
            while((c=fgetc(istream))!=EOF){
                printf("%c",c);
                if(c=='*'){
                    c=fgetc(istream);
                    if(c=='/'){
                        printf("/");
                        break;
                    }else if(c==EOF){
                        close_app();
                    }else{
                        printf("%c",c);
                    }
                }
            }
            if(c==EOF){
                close_app();
            }
        }
    }else{
        close_app();
    }
    turn_color_off();
}

void
word(){
    int c;
    char wbuf[128];
    char* p=wbuf;
    while((c=fgetc(istream))!=EOF&&(c=='_'||isalnum(c))){
        *p++=c;
    }
    *p='\0';
    if(wbuf[1]!='\0'&&strstr(keywords,wbuf)!=NULL){
        turn_color_on(KEYWORD);
        printf("%s",wbuf);
        turn_color_off();
    }else{
        if(c=='('){
            turn_color_on(FUNC);
            printf("%s",wbuf);
            turn_color_off();
        }else{
        //what if isspace(c)==1 and wbuf is a function?
            turn_color_on(VARIANT);
            printf("%s",wbuf);
            turn_color_off();
        }
    }
    if(c==EOF){
        close_app();
    }else{
        ungetc(c,istream);
    }
}

void 
highlight(){
    int c;
    while((c=fgetc(istream))!=EOF){
        if(c=='#'){
            macro();
        }else if(c=='\''){
            character();
        }else if(isheader==1&&c=='<'){
            header();
        }else if(c=='/'){
            comment();
        }else if(c=='"'){
            string();
        }else if(isspace(c)){
            ungetc(c,istream);
            space();
        }else if(ispunct(c)){
            ungetc(c,istream);
            punct();
        }else{
            ungetc(c,istream);
            word();
        }
    }
    close_app();
}


/*
 * highlight the content read from the c file or stdin
 * and print them in the console
 */
int
main(int argc,char* argv[]){
    if(argc!=2){
        istream=stdin;
    }else{
        istream=fopen(argv[1],"r");
        if(istream==NULL){
            perror("fopen");
            exit(1);
        }
    }
    highlight();
}
