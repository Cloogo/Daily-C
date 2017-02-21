#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "sql.h"

#define SERVER "localhost"
#define CLIENT "Cloogo"
#define CLIENT_PASSWD "1234"
#define DB_NAME "tankdb"

#define SUCCESS 1
#define FAILED 0
#define TRUE 1
#define FALSE 0

#define QUERY_BUF_SIZE 128
#define MAX_COMBATANTS_NUM 8
#define MEMBER_NUM_PER_TEAM 4
#define FIELD_VAL_LEN 11

#define UP 0
#define DOWN 1
#define CHALLENGER_Y 0
#define GUARDER_Y 648

#define QUERY_CHECK(query_str) do{\
    Trace_APS(query_str);\
    if(mysql_query(conn,query_str)){\
        return FAILED;\
    }\
}while(0)

#define MKQUERY(format,...) char query[QUERY_BUF_SIZE];sprintf(query,format,__VA_ARGS__)

enum _TEAM_INDEX{
    TEAM_NAME,
    TEAM_PASSWD,
    TEAM_CAPTAIN,
    TEAM_SOLDIER1,
    TEAM_SOLDIER2,
    TEAM_SOLDIER3,
    TEAM_LEVEL,
    TEAM_VS_TIMES,
    TEAM_WIN_TIMES,
    TEAM_STATUS,
    TEAM_RIVAL,
    TEAM_INVITATION
};

enum _USER_INDEX{
    USER_NAME,
    USER_TEAM,
    USER_PASSWD,
    USER_TITLE,
    USER_ID,
    USER_STATUS,
    USER_LEVEL,
    USER_BLOOD,
    USER_OFFENSE,
    USER_DEFENSE,
    USER_EXP,
    USER_X,
    USER_Y,
    USER_BULLET,
    USER_DIRECTION
};

static MYSQL *conn;

static char**
alloc_mem(int n,size_t size){
    Trace_E();
    char **mem=malloc(n*sizeof(char *));
    if(mem==NULL){
        Trace_L();
        return NULL;
    }
    for(int i=0;i<n;i++){
        mem[i]=malloc(sizeof(char)*size);
        if(mem[i]==NULL){
        }
        memset(mem[i],0,sizeof(char)*size);
    }
    Trace_L();
    return mem;
}

static void
free_mem(char*** mem,int n){
    Trace_E();
    if(*mem!=NULL){
        for(int i=0;i<n;i++){
            if((*mem)[i]!=NULL){
                free((*mem)[i]);
            }
        }
        free(*mem);
        *mem=NULL;
    }
    Trace_L();
}

static Status
is_exists(char* query){
    Trace_E();
    MYSQL_RES *res=mysql_store_result(conn);
    if(res==NULL){
        Trace_L();
        return TRUE;
    }
    long long int num=mysql_num_rows(res);
    mysql_free_result(res);
    if(num){
        Trace_L();
        return TRUE;
    }
    Trace_L();
    return FALSE;
}

static Status
retrieve_field(char* query,int field_index,char* field_val){
    Trace_E();
    Trace_APS(query);
    Trace_API(field_index);
    QUERY_CHECK(query);
    MYSQL_RES *res;
    res=mysql_store_result(conn);
    if(res==NULL){
        Trace_L();
        return FALSE;
    }
    MYSQL_ROW row;
    row=mysql_fetch_row(res);
    strcpy(field_val,row[field_index]?row[field_index]:"NULL");
    Trace_APS(field_val);
    mysql_free_result(res);
    Trace_L();
    return SUCCESS;
}

static Status
retrieve_members_name(char* query,char** names){
    Trace_E();
    Trace_APS(query);
    for(int i=TEAM_CAPTAIN;i<TEAM_SOLDIER3+1;i++){
        if(retrieve_field(query,i,names[i-TEAM_CAPTAIN])){
        }else{
            Trace_L();
            return FAILED;
        }
    }
    Trace_L();
    return SUCCESS;
}

static Status
join(char* dst,char** src,int nitems,char delim){
    Trace_E();
    Trace_API(nitems);
    char* p=dst;
    for(int i=0;i<nitems;i++){
        strcpy(p,src[i]);
        p+=strlen(src[i]);
        *p++=delim;
        Trace_APS(src[i]);
    }
    *--p='\0';
    Trace_APS(dst);
    Trace_L();
    return SUCCESS;
}

static Status
retrieve_teams(char* query,char* teams,int* nteams){
    Trace_E();
    QUERY_CHECK(query);
    MYSQL_RES* res;
    res=mysql_store_result(conn);
    if(res==NULL){
        Trace_L();
        return FAILED;
    }
    MYSQL_ROW row;
    static int index[]={TEAM_NAME,TEAM_LEVEL,TEAM_VS_TIMES,TEAM_WIN_TIMES};
    static int nidx=sizeof(index)/sizeof(index[0]);
    char** buf=alloc_mem(nidx,FIELD_VAL_LEN);
    char** teams_buf=alloc_mem(100,FIELD_VAL_LEN);
    while((row=mysql_fetch_row(res))&&*nteams<100){
        for(int i=0;i<nidx;i++){
            strcpy(buf[i],row[index[i]]);
        }
        join(teams_buf[*nteams],buf,nidx,'/');
        ++*nteams;
    }
    join(teams,teams_buf,*nteams,':');
    free_mem(&buf,nidx);
    free_mem(&teams_buf,100);
    mysql_free_result(res);
    Trace_API(*nteams);
    Trace_APS(teams);
    Trace_L();
    return SUCCESS;
}

static Status
set_user_status(char* team_name,char* member_name,char* status){
    Trace_E();
    MKQUERY("update users set status=\"%s\" where binary name=\"%s\" and team=\"%s\"",status,member_name,team_name);
    QUERY_CHECK(query);
    Trace_L();
    return SUCCESS;
}

static Status
set_team_status(char* team,char* status){
    Trace_E();
    MKQUERY("update teams set status=\"%s\" where binary name=\"%s\"",status,team);
    QUERY_CHECK(query);
    Trace_L();
    return SUCCESS;
}

static Status
set_position(char* team,char* name,int x,int y,int direction){
    Trace_E();
    MKQUERY("update users set x=%d,y=%d,direction=%d where team=\"%s\" and binary name=\"%s\"",x,y,direction,team,name);
    QUERY_CHECK(query);
    Trace_L();
    return SUCCESS;
}

static Status
set_bullet_status(char* team,char* name,char* status){
    Trace_E();
    MKQUERY("update users set bullet=\"%s\" where team=\"%s\" and binary name=\"%s\"",status,team,name);
    QUERY_CHECK(query);
    Trace_L();
    return SUCCESS;
}

static Status
pack_user_data(char* team,char* name,int* fieldsidx,int nidx,char** usersdata,int* nthuser){
    Trace_E();
    MKQUERY("select * from users where team=\"%s\" and binary name=\"%s\"",team,name);
    QUERY_CHECK(query);
    MYSQL_RES *res;
    res=mysql_store_result(conn);
    if(res==NULL){
        return FAILED;
    }
    MYSQL_ROW row;
    char** buf=alloc_mem(nidx,FIELD_VAL_LEN);
    size_t size=0;
    row=mysql_fetch_row(res);
    for(int i=0;i<nidx;i++){
        size+=strlen(row[fieldsidx[i]])+1;
        strcpy(buf[i],row[fieldsidx[i]]);
    }
    size++;
    usersdata[*nthuser]=malloc(size);
    join(usersdata[*nthuser],buf,nidx,'/');
    ++*nthuser;
    free_mem(&buf,nidx);
    mysql_free_result(res);
    Trace_L();
    return SUCCESS;
}

static Status
pack_users_data(char* team,int *fieldsidx,int nidx,char** usersdata,int *nthuser,int update_bullet){
    Trace_E();
    MKQUERY("select * from teams where binary name=\"%s\"",team);
    char** names=alloc_mem(MEMBER_NUM_PER_TEAM,FIELD_VAL_LEN);
    if(retrieve_members_name(query,names)){
        for(int i=0;i<MEMBER_NUM_PER_TEAM;i++){
            if(pack_user_data(team,names[i],fieldsidx,nidx,usersdata,nthuser)){
                if(update_bullet){
                    set_bullet_status(team,names[i],"FIRED");
                }
            }
        }
        free_mem(&names,MEMBER_NUM_PER_TEAM);
    }else{
        free_mem(&names,MEMBER_NUM_PER_TEAM);
        Trace_L();
        return FAILED;
    }
    Trace_L();
    return SUCCESS;
}

static Status
pack_team_battle_data(char* team,char** usersdata,int *nthuser){
    Trace_E();
    static int fieldsidx[]={USER_X,USER_Y,USER_BULLET,USER_DIRECTION};
    static int nidx=sizeof(fieldsidx)/sizeof(fieldsidx[0]);
    Trace_L();
    return pack_users_data(team,fieldsidx,nidx,usersdata,nthuser,1);
}

static Status
pack_team_static_data(char* team,char**usersdata,int *nthuser){
    Trace_E();
    static int fieldsidx[]={USER_NAME,USER_ID,USER_LEVEL,USER_BLOOD,USER_EXP,USER_OFFENSE,USER_DEFENSE};
    static int nidx=sizeof(fieldsidx)/sizeof(fieldsidx[0]);
    Trace_L();
    return pack_users_data(team,fieldsidx,nidx,usersdata,nthuser,0);
}

static Status
update_times(char* team,char* fieldsname,int idx,int more){
    Trace_E();
    MKQUERY("select * from teams where binary name=\"%s\"",team);
    char times_str[10]={'\0'};
    if(retrieve_field(query,idx,times_str)){
    }else{
        Trace_L();
        return FAILED;
    }
    int times_int=atoi(times_str);
    sprintf(query,"update teams set %s=%d where binary name=\"%s\"",fieldsname,times_int+more,team);
    QUERY_CHECK(query);
    Trace_L();
    return SUCCESS;
}

static Status
set_invitation_fields(char* team,char* status){
    Trace_E();
    MKQUERY("update teams set invitation=\"%s\" where binary name=\"%s\"",status,team);
    QUERY_CHECK(query);
    Trace_L();
    return SUCCESS;
}

static Status
get_invitation_fields(char* team,char* status){
    Trace_E();
    MKQUERY("select * from teams where binary name=\"%s\"",team);
    Trace_L();
    return retrieve_field(query,TEAM_INVITATION,status);
}

static Status
set_rival_fields(char* team,char* rival){
    Trace_E();
    MKQUERY("update teams set rival=\"%s\" where binary name=\"%s\"",rival,team);
    QUERY_CHECK(query);
    Trace_L();
    return SUCCESS;
}

static Status
clear_rival(char* team){
    Trace_E();
    return set_rival_fields(team,"NULL");
}

static Status
clear_invitation(char* team){
    Trace_E();
    return set_invitation_fields(team,"EMPTY");
}

Status
init_sql_handler(){
    Trace_E();
    conn=mysql_init(NULL);
    if(conn==NULL){
        Trace_L();
        return FAILED;
    }
    if(mysql_real_connect(conn,SERVER,CLIENT,CLIENT_PASSWD,DB_NAME,0,NULL,0)==NULL){
        Trace_L();
        return FAILED;
    }
    Trace_L();
    return SUCCESS;
}

Status
close_sql_handler(){
    Trace_E();
    mysql_close(conn);
}

Status
add_team(char** fields){
    Trace_E();
    MKQUERY("select * from teams where binary name=\"%s\"",fields[1]);
    QUERY_CHECK(query);
    if(is_exists(query)){
        return FAILED;
    }
    //teams table header:
    //name passwd captain soldier1 soldier2 solider3 level vs_times win_times status rival invitation
    sprintf(query,"insert into teams values(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",1,0,0,\"OFFLINE\",NULL,\"EMPTY\")",\
            fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]);
    QUERY_CHECK(query);
    //users table header:
    //name team passwd title id status level blood offense defense exp x y bullet
    sprintf(query,"insert into users values(\"%s\",\"%s\",\"%s\",\"captain\",0,\"ONLINE\",1,100,10,10,0,0,0,\"FIRED\",%d)",\
            fields[3],fields[1],fields[2],UP);
    QUERY_CHECK(query);
    for(int i=4;i<7;i++){
        sprintf(query,"insert into users values(\"%s\",\"%s\",\"%s\",\"soldier\",%d,\"OFFLINE\",1,50,5,5,0,0,0,\"FIRED\",%d)",\
                fields[i],fields[1],fields[2],i-3,UP);
        QUERY_CHECK(query);
    }
    Trace_L();
    return SUCCESS;
}

Status
query_team_list(char** fields,char* teams,int *nteams){
    Trace_E();
    MKQUERY("select * from teams where binary name=\"%s\"",fields[1]);
    char levelstr[5]={'\0'};
    int level=0;
    if(retrieve_field(query,TEAM_LEVEL,levelstr)){
        level=atoi(levelstr);
        sprintf(query,"select * from teams where status=\"ONLINE\"&&%d<level&&level<%d",level-50,level+50);
        if(retrieve_teams(query,teams,nteams)){
            Trace_L();
            return SUCCESS;
        }
    }
    Trace_L();
    return FAILED;
}

Status
is_all_online(char** fields,char* absent_members){
    Trace_E();
    MKQUERY("select * from teams where binary name=\"%s\"",fields[1]);
    char **names=alloc_mem(MEMBER_NUM_PER_TEAM,FIELD_VAL_LEN);
    int nmems=0;
    char *p=absent_members;
    if(retrieve_members_name(query,names)){
        for(int i=0;i<MEMBER_NUM_PER_TEAM;i++){
            char status[FIELD_VAL_LEN]={'\0'};
            sprintf(query,"select * from users where team=\"%s\" and binary name=\"%s\"",fields[1],names[i]);
            if(retrieve_field(query,USER_STATUS,status)){
                if(strcmp(status,"ONLINE")==0){
                    sprintf(p,"%s:ONLINE/",names[i]);
                    p+=strlen(names[i])+strlen(":ONLINE/");
                }else{
                    sprintf(p,"%s:OFFLINE/",names[i]);
                    p+=strlen(names[i])+strlen(":OFFLINE/");
                    nmems++;
                }
            }else{
                Trace_L();
                return FAILED;
            }
            free(names[i]);
        }
        free(names);
    }else{
        Trace_L();
        return FAILED;
    }
    if(nmems!=0){
        Trace_L();
        return FALSE;
    }
    Trace_L();
    return set_team_status(fields[1],"ONLINE");
}

Status
is_invited(char** fields,char* challenger){
    Trace_E();
    MKQUERY("select * from teams where binary name=\"%s\"",fields[1]);
    if(retrieve_field(query,TEAM_RIVAL,challenger)){
        if(strcmp(challenger,"NULL")!=0){
            Trace_L();
            return TRUE;
        }
    }else{
        Trace_L();
        return FAILED;
    }
    Trace_L();
    return FALSE;
}

Status
set_rival(char** fields){
    Trace_E();
    if(set_rival_fields(fields[1],fields[2])&&set_rival_fields(fields[2],fields[1])){
    }else{
        Trace_L();
        return FAILED;
    }
    Trace_L();
    return set_invitation_fields(fields[1],"WAIT");
}

Status
set_invitation_status(char** fields){
    Trace_E();
    if(strcmp(fields[3],"ACCEPT")==0){
        return set_invitation_fields(fields[1],"ACCEPT")&&set_invitation_fields(fields[2],"ACCEPT");
    }else{
        return set_rival_fields(fields[1],"NULL")&&set_rival_fields(fields[2],"NULL")&&set_invitation_fields(fields[2],"EMPTY");
    }
}

Status
get_invitation_status(char** fields,char* status){
    Trace_E();
    return get_invitation_fields(fields[1],status);
}

Status
set_members_onbattle(char** fields){
    Trace_E();
    MKQUERY("select * from teams where binary name=\"%s\"",fields[1]);
    char **names=alloc_mem(MEMBER_NUM_PER_TEAM,FIELD_VAL_LEN);
    if(retrieve_members_name(query,names)){
        int team_y;
        int direction;
        if(strcmp(fields[2],"GUARDER")==0){
            team_y=GUARDER_Y;
            direction=DOWN;
        }else if(strcmp(fields[2],"CHALLENGER")==0){
            team_y=CHALLENGER_Y;
            direction=UP;
        }else{
            //some measures may be taken in case of data missing.
        }
       for(int i=0;i<MEMBER_NUM_PER_TEAM;i++){
            if(set_user_status(fields[1],names[i],fields[2])){
                int x=(i+1)*72+36;
                if(set_position(fields[1],names[i],x,team_y,direction)){
                }else{
                    free_mem(&names,MEMBER_NUM_PER_TEAM);
                    Trace_L();
                    return FAILED;
                }
            }else{
                //reset online status?
                free_mem(&names,MEMBER_NUM_PER_TEAM);
                Trace_L();
                return FAILED;
            }
        }
        free_mem(&names,MEMBER_NUM_PER_TEAM);
    }else{
        free_mem(&names,MEMBER_NUM_PER_TEAM);
        Trace_L();
        return FAILED;
    }
    Trace_L();
    return set_team_status(fields[1],"ONBATTLE");
}

Status
update_battle_result(char** fields){
    Trace_E();
    return update_times(fields[1],"vs_times",TEAM_VS_TIMES,1)\
    &&update_times(fields[1],"win_times",TEAM_WIN_TIMES,strcmp(fields[2],"winner")==0)\
    &&set_team_status(fields[1],"ONLINE")\
    &&clear_rival(fields[1])\
    &&clear_invitation(fields[1]);
}

Status
set_xy(char** fields){
    Trace_E();
    return set_position(fields[1],fields[2],atoi(fields[3]),atoi(fields[4]),atoi(fields[5]));
}

Status
load_bullet(char** fields){
    Trace_E();
    return set_bullet_status(fields[1],fields[2],"LOADED");
}

Status
pack_battle_data(char** fields,char* data){
    Trace_E();
    char** usersdata=malloc(MAX_COMBATANTS_NUM*sizeof(char* ));
    int nthuser=0;
    pack_team_battle_data(fields[1],usersdata,&nthuser);
    pack_team_battle_data(fields[2],usersdata,&nthuser);
    join(data,usersdata,nthuser,'/');
    for(int i=0;i<MAX_COMBATANTS_NUM;i++){
        free(usersdata[i]);
    }
    free(usersdata);
    Trace_L();
    return SUCCESS;
}

Status
pack_combatants_attr(char** fields,char* data){
    Trace_E();
    char**usersdata=malloc(MAX_COMBATANTS_NUM*sizeof(char* ));
    int nthuser=0;
    pack_team_static_data(fields[1],usersdata,&nthuser);
    pack_team_static_data(fields[2],usersdata,&nthuser);
    join(data,usersdata,nthuser,'/');
    for(int i=0;i<MAX_COMBATANTS_NUM;i++){
        free(usersdata[i]);
    }
    free(usersdata);
    Trace_L();
    return SUCCESS;
}

Status
set_combatants_attr(char** fields){
    Trace_E();
    MKQUERY("update users set level=%s,blood=%s,exp=%s,offense=%s,defense=%s,bullet=\"FIRED\" where team=\"%s\" and binary name=\"%s\"",fields[3],fields[4],fields[5],fields[6],fields[7],fields[1],fields[2]);
    QUERY_CHECK(query);
    Trace_L();
    return set_user_status(fields[1],fields[2],"ONLINE");
}

Status
set_user_online(char** fields){
    Trace_E();
    MKQUERY("select * from users where binary name=\"%s\" and team=\"%s\" and passwd=\"%s\" and title=\"%s\"",fields[3],fields[1],fields[2],fields[4]);
    QUERY_CHECK(query);
    if(is_exists(query)){
        Trace_L();
        return set_user_status(fields[1],fields[3],"ONLINE");
    }else{
        Trace_L();
        return FAILED;
    }
}

Status
set_user_offline(char** fields){
    Trace_E();
    return set_user_status(fields[1],fields[2],"OFFLINE");
}

Status
is_summoned(char** fields,char* role){
    Trace_E();
    MKQUERY("select * from users where team=\"%s\" and binary name=\"%s\"",fields[1],fields[2]);
    char status[10]={'\0'};
    retrieve_field(query,USER_STATUS,status);
    if(strcmp(status,"ONLINE")!=0){
        strcpy(role,status);
        Trace_L();
        return TRUE;
    }
    Trace_L();
    return FALSE;
}

Status
get_rival(char** fields,char* rival){
    Trace_E();
    MKQUERY("select * from teams where binary name=\"%s\"",fields[1]);
    return retrieve_field(query,TEAM_RIVAL,rival);
}
