#ifndef _SQL_H
#define _SQL_H
#include <mysql.h>
typedef int Status;

#ifdef __cplusplus
extern "C"{
#endif

extern Status init_sql_handler();
extern Status close_sql_handler();

/*captain only*/
extern Status add_team(char** fields);
extern Status query_team_list(char** fields,char* teams,int* nteams);
extern Status is_all_online(char** fields,char* absent_members);
extern Status is_invited(char** fields,char* challenger);
extern Status set_rival(char** fields);
extern Status set_invitation_status(char** fields);
extern Status get_invitation_status(char** fields,char* status);
extern Status set_members_onbattle(char** fields);
extern Status update_battle_result(char** fields);
/*both captain and soldier*/
extern Status set_xy(char** fields);
extern Status load_bullet(char** fields);
extern Status pack_battle_data(char** fields,char* data);
extern Status pack_combatants_attr(char** fields,char* data);
extern Status set_combatants_attr(char** fields);
extern Status set_user_online(char** fields);
extern Status set_user_offline(char** fields);
/*soldier only*/
extern Status is_summoned(char** fields,char* role);
extern Status get_rival(char** fields,char* rival);
#ifdef __cplusplus
}
#endif
#endif
