#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sql.h"
#include "trace.h"

#define IS_SERVICE(cmd) (strcmp(fields[0],cmd)==0)
#define SERVER_PORT 8088
#define MAX_PLAYER_NUM 2000
#define REQ_BUF_SIZE 512
#define RESP_BUF_SIZE 4096
#define MAX_FIELD_NUM 10

#define OK "200"
#define ERR "404"

#define RESP(str) send(client,str,strlen(str)+1,0)

void
regist(int client,char **fields){
    Trace_E();
    if(add_team(fields)){
        RESP(OK);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

void
login(int client,char **fields){
    Trace_E();
    if(set_user_online(fields)){
        RESP(OK);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

int
logout(int client,char **fields){
    Trace_E();
    if(set_user_offline(fields)){
        close_sql_handler();
        RESP(OK);
        close(client);
        return 1;
    }else{
        RESP(ERR);
        return 0;
    }
    Trace_L();
}

void
check_members_status(int client,char **fields){
    Trace_E();
    char members_status[RESP_BUF_SIZE]={'\0'};
    if(is_all_online(fields,members_status)){
        RESP(members_status);
        Trace_P("%s","Is all online.\n");
    }else{
        RESP(members_status);
        Trace_P("%s","someone absent.\n");
    }
    Trace_L();
}

void
get_team_list(int client,char **fields){
    Trace_E();
    int nteams=0;
    char teams[RESP_BUF_SIZE]={'\0'};
    if(query_team_list(fields,teams,&nteams)){
        RESP(teams);
    }else{
        RESP(ERR);
    }
    Trace_L();
}


void
has_battle(int client,char **fields){
    Trace_E();
    char role[11]={'\0'};
    if(is_summoned(fields,role)){
        RESP(role);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

void
get_rival_name(int client,char** fields){
    Trace_E();
    char rival[11]={'\0'};
    if(get_rival(fields,rival)){
        RESP(rival);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

void
has_invitation(int client,char **fields){
    Trace_E();
    char challenger[11]={'\0'};
    if(is_invited(fields,challenger)){
        RESP(challenger);
    }else{
        RESP(challenger);
    }
    Trace_L();
}

void
respond_invitation(int client,char **fields){
    Trace_E();
    if(set_invitation_status(fields)){
        RESP(OK);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

void
send_invitation(int client,char **fields){
    Trace_E();
    if(set_rival(fields)){
        RESP(OK);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

void
listen_invitation(int client,char **fields){
    Trace_E();
    char status[RESP_BUF_SIZE];
    memset(status,0,RESP_BUF_SIZE);
    if(get_invitation_status(fields,status)){
        RESP(status);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

void
summon(int client,char **fields){
    Trace_E();
    if(set_members_onbattle(fields)){
        RESP(OK);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

void
update_xy(int client,char **fields){
    Trace_E();
    if(set_xy(fields)){
        RESP(OK);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

void
update_bullet(int client,char **fields){
    Trace_E();
    if(load_bullet(fields)){
        RESP(OK);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

//x.y.direction.bullet
void
get_battle_data(int client,char **fields){
    Trace_E();
    char battle_data[RESP_BUF_SIZE];
    memset(battle_data,0,RESP_BUF_SIZE);
    if(pack_battle_data(fields,battle_data)){
        RESP(battle_data);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

//life level and exp etc.
void
get_combatants_attr(int client,char **fields){
    Trace_E();
    char attrdata[RESP_BUF_SIZE];
    memset(attrdata,0,RESP_BUF_SIZE);
    if(pack_combatants_attr(fields,attrdata)){
        RESP(attrdata);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

void
update_combatants_attr(int client,char **fields){
    Trace_E();
    if(set_combatants_attr(fields)){
        RESP(OK);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

void
record_battle_result(int client,char **fields){
    Trace_E();
    if(update_battle_result(fields)){
        RESP(OK);
    }else{
        RESP(ERR);
    }
    Trace_L();
}

void
parse_server(int client,char **fields,int nfields){
    Trace_E();
    if(IS_SERVICE("register")){
        //fields:register/team-name/team-passwd/captain1/solider1/solider2/solider3/
        if(nfields!=7){
        }else{
            regist(client,fields);
        }
    }else if(IS_SERVICE("login")){
        //fields:login/team-name/team-passwd/member-name/title/
        if(nfields!=5){
        }else{
            login(client,fields);
        }
    }else if(IS_SERVICE("check_members_status")){
        //fields:check_members_status/team-name/
        if(nfields!=2){
        }else{
            check_members_status(client,fields);
        }
    }else if(IS_SERVICE("get_team_list")){
        //fields:get_team_list/team-name
        if(nfields!=2){
        }else{
            get_team_list(client,fields);
        }
    }else if(IS_SERVICE("has_battle")){
        //fields:has_battle/team-name/member-name/
        if(nfields!=3){
        }else{
            has_battle(client,fields);
        }
    }else if(IS_SERVICE("get_rival_name")){
        //fields:get_rival_name/team-name
        if(nfields!=2){
        }else{
            get_rival_name(client,fields);
        }
    }else if(IS_SERVICE("has_invitation")){
        //fields:has_invitation/team-name/
        if(nfields!=2){
        }else{
            has_invitation(client,fields);
        }
    }else if(IS_SERVICE("respond_invitation")){
        //fields:respond_invitation/team-name/rival-name/respond/
        if(nfields!=4){
        }else{
            respond_invitation(client,fields);
        }
    }else if(IS_SERVICE("send_invitation")){
        //fields:send_invitation/team-name/rival-name/
        if(nfields!=3){
        }else{
            send_invitation(client,fields);
        }
    }else if(IS_SERVICE("listen_invitation")){
        //fields:listen_invitation/team-name/
        if(nfields!=2){
        }else{
            listen_invitation(client,fields);
        }
    }else if(IS_SERVICE("summon")){
        //fields:summon/team-name/role
        if(nfields!=3){
        }else{
            summon(client,fields);
        }
    }else if(IS_SERVICE("update_xy")){
        //fields:update_xy/team-name/combatant-name/x/y/direction
        if(nfields!=6){
        }else{
            update_xy(client,fields);
        }
    }else if(IS_SERVICE("update_bullet")){
        //fields:update_bullet/team-name/combatant-name/
        if(nfields!=3){
        }else{
            update_bullet(client,fields);
        }
    }else if(IS_SERVICE("get_battle_data")){
        //fields:get_battle_data/team-name/rival-name/
        if(nfields!=3){
        }else{
            get_battle_data(client,fields);
        }
    }else if(IS_SERVICE("get_combatants_attr")){
        //fields:get_combatants_attr/team-name/rival-name/
        if(nfields!=3){
        }else{
            get_combatants_attr(client,fields);
        }
    }else if(IS_SERVICE("update_combatants_attr")){
        //fields:update_combatants_attr/team-name/combatant-name/level/blood/exp/offense/defense/
        if(nfields!=8){
        }else{
            update_combatants_attr(client,fields);
        }
    }else if(IS_SERVICE("record_battle_result")){
        //fields:record_battle_result/team-name/"winner" or "losser"/
        if(nfields!=3){
        }else{
            record_battle_result(client,fields);
        }
    }else{
        RESP(ERR);
    }
    Trace_L();
}

char **
parse_fields(char *req,int *nfields){
    Trace_E();
    Trace_APS(req);
    Trace_API(*nfields);
    char *p=NULL;
    char **fields=malloc(MAX_FIELD_NUM*sizeof(char *));
    for(size_t size=0;p=strtok(req,"/");req=NULL){
        size=strlen(p)+1;
        fields[*nfields]=malloc(size);
        memset(fields[*nfields],0,size);
        strcpy(fields[*nfields],p);
        fields[*nfields][size-1]='\0';
        Trace_API(*nfields);
        Trace_APS(fields[*nfields]);
        ++*nfields;
    }
    Trace_L();
    return fields;
}

void
free_fields(char*** fields,int nfields){
    Trace_E();
    if(*fields!=NULL){
        for(int i=0;i<nfields;i++){
            if((*fields)[i]!=NULL){
                free((*fields)[i]);
            }
        }
        free(*fields);
        *fields=NULL;
    }
    Trace_L();
}


void
handle_request(int client){
    Trace_E();
    char req[REQ_BUF_SIZE];
    char **fields=NULL;
    int nfields;
    if(init_sql_handler()){
        Trace_P("%s","Connect to database.\n");
    }else{
        Trace_EP("%s","Failed to connect to database.\n");
    }
    int can_logout=0;
    while(!can_logout){
        memset(req,0,REQ_BUF_SIZE);
        recv(client,req,REQ_BUF_SIZE,0);
        Trace_APS(req);
        nfields=0;
        fields=parse_fields(req,&nfields);
        if(IS_SERVICE("logout")){
            //fields:logout/team-name/member-name/
            if(nfields!=3){
            }else{
                can_logout=logout(client,fields);
            }
        }else{
            parse_server( client,fields,nfields);
        }
        free_fields(&fields,nfields);
    }
    Trace_L();
}

int
startup(){
    Trace_E();
    int httpd=0;
    struct sockaddr_in server_addr;
    httpd=socket(AF_INET,SOCK_STREAM,0);
    if(httpd==-1){
        Trace_EP("%s","Failed to build up socket!\n");
    }
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(SERVER_PORT);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(httpd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){
        Trace_EP("%s","Failed to bind!\n");
    }
    if(listen(httpd,MAX_PLAYER_NUM)<0){
        Trace_EP("%s","Failed to listen!\n");
    }
    Trace_P("Start server on port:%d\n",SERVER_PORT);
    Trace_L();
    return httpd;
}

int
main(int argc,char *argv[]){
    Trace_E();
    int server=startup();
    int client=-1;
    struct sockaddr_in client_addr;
    int client_len=sizeof(client_addr);

    for(;;){
        client=accept(server,(struct sockaddr *)&client_addr,&client_len);
        if(client==-1){
            Trace_EP("%s","Failed to accept!\n");
            break;
        }
        switch(fork()){
        case 0:
            close(server);
            handle_request(client);
            exit(0);
        case -1:
            Trace_EP("%s","Failed to fork!\n");
            break;
        }
    }
    close(server);
    Trace_L();
}
