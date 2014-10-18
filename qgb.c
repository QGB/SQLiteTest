#include <stdio.h>

/**  Basic define  **/
#define ITRUE 1
#define IFALSE 0
#define IS_TEST IFALSE

/**  Color define  **/
#define S_BLUE "01"
#define S_GREEN "0a"
#define S_LIGHT_BLUE "0b"
#define S_RED "0c"
#define S_PINK "0D"
#define S_YELLOW "0E"
#define S_WHITE "0F"
#define S_DEF S_GREEN

void pad(char ac,int ai);
void padln(char ac,int ai);
void color(char *apc);
void pause();
void cls();
char* card_to_ord(int ai); /**cardinals_to_ordinals(基数词转序数词)**/

/**************************   basic functions  *****************************************/

void pad(char ac,int ai){
    if(ai<1){
        return ;
    }
    int i=0;
    for(i=0;i<ai;i++){
        putchar(ac);
    }
}

void padln(char ac,int ai){
    pad(ac,ai);
    printf("\n");
}

void color(char *apc){
    char pc[]={"color "};
    system(strcat(pc,apc));
}

void pause(){
    system("pause");
}

void cls(){
    system("cls");
}

char* card_to_ord(int ai){
    if(ai<0){
        return "Invalid parameter!";
    }
    char yc[20]={0};
    char* pcr=(char*)calloc(20,sizeof(char*));
    int final_d = ai%10;
    int ilb1_d=(ai/10)%10; /**last_but_one digit**/
    if(ilb1_d==1){
        if(final_d==1||final_d==2||final_d==3){
            sprintf(yc,"%d%s",ai,"th");
            strcpy(pcr,yc);
            return pcr;
        }
    }
    switch(final_d){
    case 1 :
        sprintf(yc,"%d%s",ai,"st");
        break;
    case 2 :
        sprintf(yc,"%d%s",ai,"nd");
        break;
    case 3 :
        sprintf(yc,"%d%s",ai,"rd");
        break;
    default:
        sprintf(yc,"%d%s",ai,"th");
        break;
    }
    strcpy(pcr,yc);
    return pcr;
}
