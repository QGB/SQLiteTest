#include "set.h"
#include "qgb.c"
sqlite3 * gp_sqlite3 = 0;

void show_main_menu();
void add_info();
void del_info();
int find_info(int ai_col, char* apc_keyword);
void sort_info();
int select_callback_list(void * data, int col_count, char ** col_values, char ** col_Name);

void sqlite_connect();
int db_isNull();
void sqlite_create_table();
int select_callback(void * data, int col_count, char ** col_values, char ** col_Name);
int sqlite_save(int ai_length,char *aps[]);
sqlite3_stmt* sqlite_get_stmt();
char* sql_input_keyword();


/************************ Control functions ****************************/
int main(int argc, char * argv[]){
    color(S_DEF);
    sqlite_connect();
    //find_info(-1,"");
    //exit(1);
    show_main_menu();
    return 0;
}

void show_main_menu(){
    cls();
    padln('=',80);
    pad(' ',26);
    printf("Address Book For Students V1.0\n");
    color(S_DEF);
    pad(' ',70);
    printf("By QGBCS\n");
    pad('=',80);
    printf("%d Add \n",ICMD_ADD);//infomation
    printf("%d Delete\n",ICMD_DEL);
    printf("%d Scan \n",ICMD_SCA);//infomation
    printf("%d Find \n",ICMD_FIN);//infomation
    printf("%d Sort\n",ICMD_SOR);
    printf("%d Count\n",ICMD_COUNT);
    printf("%d EXIT!\n",ICMD_EXIT);

    char yc[2];
    int ia=0;
    while(1){
        scanf("%s",yc);
        ia=atoi(yc);
        printf("input is %d\n",ia);
        if(ia<ICMD_MIN||ia>ICMD_MAX){
            printf("Illegal requests, please retry!\n");
            pause();
            show_main_menu();
        }else{
            break;
        }
    }
    switch(ia){
    case ICMD_ADD:
        add_info();
        show_main_menu();
        break;
    case ICMD_DEL:
        del_info();
        show_main_menu();
        break;
    case ICMD_FIN:
        find_info(-1,"");
        show_main_menu();
        break;
    case ICMD_SCA:
        find_info(0,"%");
        show_main_menu();
        break;
    case ICMD_SOR:
        sort_info();
        show_main_menu();
        break;
    case ICMD_COUNT:
        printf("The total number of records is %d\n",sqlite_get_row_count());
        pause();
        show_main_menu();
    case ICMD_EXIT:
        sqlite3_close(gp_sqlite3);/*****close database !*****/
        color(S_RED);
        printf("Program Exit!\n");
        pause();
        exit(0);
        break;
    }
}

void sort_info(){
    cls();
    printf("please select a column to sort:(input a number)\n");
    printf("1(Sorting by "SC_id") \n");
    printf("2(Sorting by "SC_nam") \n");
    pad('-',80);
    char yc[1],*pc;
    int ia=0;
    while(1){
        scanf("%s",&yc);
        ia=atoi(yc);
        if(ia!=1&&ia!=2){
            printf("Illegal requests, please retry!\n");
            pause();
            cls();
            sort_info();
            return;
        }else{
            break;
        }
    }
    ia=ia-1;/**input is 1-2**/
    sqlite3_stmt *pstmt=sqlite_get_stmt();
    pc=sqlite3_column_name(pstmt,ia);
    printf("You selected %d(%s)\n",ia+1,pc);

    char ysql[50],ysql_p[]={"select * from "STABLE_NAME" order by %s asc;"};
    sprintf(ysql,ysql_p,pc);
    if(IS_TEST)printf("ysql=(%s)\n",ysql);
    pad('*',33);
    printf("Sorting by %-4s",pc);
    pad('*',32);
    int i=0,icols=sqlite_get_col_count();
    for(i=0;i<icols;i++){
        pc=sqlite3_column_name(pstmt,i);
        printf( "%-12s|", pc);

    }
    printf("\n");
    pad('-',80);
    char * pErrMsg = 0;
    sqlite3_exec(gp_sqlite3,ysql, select_callback_list, 0, &pErrMsg);
    pad('*',36);
    printf("  End  ");
    padln('*',36);
    pause();
}
/**每条记录回调一次该函数，有多少条就回调多少次**/
int select_callback_list(void * data, int col_count, char ** col_values, char ** col_Name){
    int i;
    for( i=0; i < col_count; i++){
        printf( "%-12.12s|", col_values[i] == 0 ? "NULL" : col_values[i] );
    }
    printf("\n");
    return 0;
}

void del_info(){
    cls();
    char *pc=find_info(-1,"");
    char check,ysql[50];
    printf("Are you sure you want to delete above infomation?(y/n)\n");
    setbuf(stdin, NULL);/**清空缓冲区**/
    scanf("%c",&check);
    if(check=='y'||check=='Y'){//qgb:testedv
        printf("Input is %c\n",check);
        sprintf(ysql,"delete from "STABLE_NAME"%s",pc);
        if(IS_TEST) printf("ysql=[%s]\n",ysql);
        int ir=sqlite_exec(ysql);
        switch (ir){
        case SQLITE_OK:
            color(S_PINK);
            printf("Delete success!\n");
            pause();
            color(S_DEF);
            return;
            break;
        default:
            printf("Error:Unknown.code=%d \n",ir);
            return;
            break;
        }
    }else{
        color(S_LIGHT_BLUE);
        printf("Input is %c\nDelete abort!\n",check);
        pause();
        color(S_DEF);
        return;
    }
}
int find_info(int ai_col, char* apc_keyword){
    cls();//if(IS_TEST==IFALSE)
    char ysql[50],ysql_part[100];
    sqlite3_stmt *pstmt;//;
    int icols;
    pstmt=sqlite_get_stmt();
    icols=sqlite_get_col_count();
    strcpy(ysql_part,"select * from "STABLE_NAME" where %s like '%s';");
    //char ybbc2[1];/**TODO: 必须要有一个无关 字符数组 否则 ysql为空.为什么?**///char yc3[1];
    char *pcin,*pc,*ypc[icols],yc[20];

    int itmp=ITRUE;

    if(ai_col>=0&&ai_col<icols ){
        itmp=IFALSE;
    }
    const int is_user=itmp;

    int i=0;
    if(is_user){/** User input start!**/
        printf("please select a column to find:(input a number)\n");
        for(i=0;i<icols;i++){
            pc=sqlite3_column_name(pstmt,i);
            printf("%d(%s)\n",i+1,pc);/**0 is likely to lead to ambiguity.**/
        }
        pad('-',80);
        int ia=0;
        while(1){
            setbuf(stdin, NULL);/**清空缓冲区**/
            scanf("%s",yc);
            //ia=yc-'0';
            ia=atoi(yc) ;
            if(ia<1||ia>icols){
                printf("Illegal requests, please retry!\n");
                pause();
                cls();
                find_info(-1,"");
                return;
            }else{
                break;
            }
        }
        ia=ia-1;/**input is 1-6**/
        pc=sqlite3_column_name(pstmt,ia);
        printf("You selected %d(%s)\n",ia+1,pc);
        pad('-',80);
        printf("please input keyword:(This parameter supports wildcards '%%'.)\n");
        pcin=sql_input_keyword();
        sprintf(ysql,ysql_part,pc,pcin);
        if(IS_TEST)printf("ysql=[%s]\n",ysql);
    }/** User input end!**/
    else{
        pc=sqlite3_column_name(pstmt,ai_col);
        sprintf(ysql,ysql_part,pc,apc_keyword);
    }
    if(IS_TEST)printf("ysql:%s\n",ysql);
    char * pErrMsg = 0;
    pad('*',35);
    printf("  Result  ");
    padln('*',35);
    sqlite3_exec(gp_sqlite3,ysql, select_callback, 0, &pErrMsg);
    pad('*',35);
    printf("  End  ");
    padln('*',37);
    pause();
    if(is_user){
        char yc_return[]=" where %s like '%s';";
        sprintf(ysql,yc_return,pc,pcin);
        char* pcr=(char*)calloc(50,sizeof(char*));
        strcpy(pcr,ysql);
        return pcr;
    }
}

void add_info(){
    sqlite3_stmt *pstmt=sqlite_get_stmt();//;
    char *pc,*psql;
    const int icols=sqlite_get_col_count();
    char *ypc[icols],ycheck[1];
    cls();
    printf("columns count is %d\n",icols);
    int i=0;
    for(i=0;i<icols;i++){
        pc=sqlite3_column_name(pstmt,i);
        printf("please input column%d %s:\n",i,pc);
        ypc[i]=sql_input_keyword();
    }
    pad('-',80);
    for(i=0;i<icols;i++){
        pc=sqlite3_column_name(pstmt,i);
        printf("column%d %-7s is:%s\n",i,pc,ypc[i]);
    }
    pad('-',80);
    printf("Save this ?(y/n)\n");
    scanf("%s",&ycheck);
    if(ycheck[0]=='y'||ycheck[0]=='Y'){//qgb:tested
        sqlite_save(icols,ypc);
    }else{
        color(S_LIGHT_BLUE);
        printf("Input is %s\nSave abort!\n",ycheck);
    }
    pause();
    color(S_DEF);
}
/**************************   Sql functions  *****************************************/
void sqlite_connect(){/** 连接数据库**/
    int result = 0;
    int ret = sqlite3_open(SDB_PATH, &gp_sqlite3);
    if( ret != SQLITE_OK ) {
        fprintf(stderr, "Can not open database： %s\n", sqlite3_errmsg(gp_sqlite3));
        exit(IOPEN_DB_ERROR);
    }else{
        if(IS_TEST)
            printf("sqlite connect success！\n");
    }
    if(db_isNull()){
        printf("d");
        sqlite_create_table();
    }else{
        if(IS_TEST)printf("db_not Null!\n");
    }
}

char* sql_input_keyword(){
    setbuf(stdin, NULL);/**清空缓冲区**/
    char c='\0',yc[50]={0};
    char* pcr=(char*)calloc(50,sizeof(char*));
    int i=0,ia=0,ibreak=IFALSE,inputNULL=ITRUE;
    for(i=0;i<50;i++){
        c=getchar();
        if(c=='\''){
            printf("Error:%s %c is not a valid character\n",card_to_ord(i+1),c);
            continue;
        }
        if(c=='\n'){
            if(inputNULL==ITRUE){
                continue;/**FIXED at 2014-06-27 20:52:47 防止输入为空**/
            }
            break;
        }
        if(inputNULL==ITRUE){
            inputNULL=IFALSE;
        }
        yc[ia++]=c;
    }
    strcpy(pcr,yc);
    return pcr;
}

sqlite3_stmt* sqlite_get_stmt(){
    sqlite3_stmt *pstmt;
    char *psql;
    psql="select * from "STABLE_NAME";";
    sqlite3_prepare_v2(gp_sqlite3,psql,strlen(psql),&pstmt,NULL);
    return pstmt;
}

int sqlite_get_col_count(){
    int icols=sqlite3_column_count(sqlite_get_stmt());
    return icols;
}

int sqlite_get_row_count(){
    char *pc_ErrMsg = 0;
    int ir=0;
    char **ppc_r;
    int i_row=0;
    int i_col=0;
    ir=sqlite3_get_table(gp_sqlite3,"select * from "STABLE_NAME";",&ppc_r,&i_row,&i_col,&pc_ErrMsg);
    return i_row;
}

int db_isNull(){
    char *pc_ErrMsg = 0;
    int ir=0;
    char **ppc_r;
    int i_row=0;
    int i_col=0;
    ir=sqlite3_get_table(gp_sqlite3,"select * from "STABLE_NAME";",&ppc_r,&i_row,&i_col,&pc_ErrMsg);
    if(ir==0){
        if(i_col==0)
            return 1;
        else
            return 0;
    }else{
         if(IS_TEST){
            fprintf(stderr, "SQL error or missing database: %s|错误码=%d\n", sqlite3_errmsg(gp_sqlite3),ir);
         }
         return 1;
    }
    return 1;
}

void sqlite_create_table(){
    const char * sSQL1 = "create table "STABLE_NAME"("SC_id" varchar(20) PRIMARY KEY,"
                                                        SC_nam" varchar(10),"
                                                        SC_sex" varchar(1),"
                                                        SC_add" varchar(100),"
                                                        SC_tel" varchar(12), "
                                                        SC_qq" varchar(15));";
    char * pErrMsg = 0;
    int result=sqlite3_exec( gp_sqlite3, sSQL1, 0, 0, &pErrMsg );
    if( result != SQLITE_OK ){
        fprintf(stderr, "建表错误: %s|错误码=%d\n", pErrMsg,result);
        sqlite3_free(pErrMsg);
    }
}

int sqlite_save(int ai_length,char *ayps[]){
    char ysql[99]="insert into "STABLE_NAME" values(";
    int i=0;
    for(i=0;i<ai_length;i++){
        if(i==0){
            sprintf(ysql,"%s'%s'",ysql,ayps[i]);
        }else{
            sprintf(ysql,"%s,'%s'",ysql,ayps[i]);
        }
   }
    sprintf(ysql,"%s%s",ysql,");");
    if(IS_TEST)printf("%s\n",ysql);
    int ir=sqlite_exec(ysql);
    switch (ir){
    case SQLITE_OK:
        printf("Save success!\n");
        break;
    case SQLITE_CONSTRAINT:
        printf("Error:Record already exists!\n");
        break;
    default:
        printf("Error:Unknown.code=%d \n",ir);
    }
}
int sqlite_exec(char *apsql){
    char * pErrMsg = 0;
    int result=sqlite3_exec( gp_sqlite3, apsql, 0, 0, &pErrMsg );
    if( result != SQLITE_OK ){
        if(IS_TEST)fprintf(stderr, "sql exec error: %s|code=%d\n", pErrMsg,result);
        sqlite3_free(pErrMsg);
    }else{
    }
    return result;
}
/**每条记录回调一次该函数，有多少条就回调多少次**/
int select_callback(void * data, int col_count, char ** col_values, char ** col_Name){
    int i;
    for( i=0; i < col_count; i++){
        printf( "%-7s= %s\n", col_Name[i], col_values[i] == 0 ? "NULL" : col_values[i] );
    }
    padln('-',35);
    return 0;
}

