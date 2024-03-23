#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#include <sys/types.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>




#include <sys/msg.h>

#include <sys/shm.h>

#include <errno.h>


#define SEM_A 0200 /* permission de modification */
#define SEM_R 0400

#define MSG_R 400 /* permission en lecture pour l'utilisateur */
#define MSG_W 200 /* permission en lecture pour l'utilisateur */

#define MESSAGE_TYPE 1



#define CLE 123
#define CLE2 1234
#define CLE3 1235


int cpt=0;
int mutex1=1,mutex=1,nvide=3;
int semid,msqid,Trequetes;
char *shmaddr ;
int* Fliber;
int Freponse ; 





    typedef struct Dispo {
        int N1 , N2,N3 ;
    }Dispo;

    struct instruction {
        int nb1 , nb2, nb3 , type;
    };
    typedef struct Req {
        int  id , nb1 , nb2, nb3;
    }Req ;


    typedef struct Rep {
        int nb1 ,trep;
    }Rep;

    typedef struct Lib {
        int id_p , lib1,lib2,lib3 ;
    }Lib ;

    typedef struct msg_R {
        long msg_type;
        Rep data ;
    }msg_R;

    typedef struct msg_L {
        long msg_type;
        Lib data ;
    }msg_L;

    
    // typedef  union {  
    //     struct msg_R {
    //         long msg_type;
    //         Rep data ;
    //     }msg_R;

    //     struct msg_L {
    //         long msg_type;
    //         Lib data ;
    //     }msg_L;
    //     int type ;

    // }msg;       

    struct dem {
        int dem1 ,dem2,dem3;
    };

    struct alloc {
        int  alloc1 ,alloc2 ,alloc3 ; 
    };

    struct info {
        int etate ;
        int  temp_att ; 
    };
    struct victim {
        int index ;
        int  temp_att ; 
    };

    struct info Tinfos[5];
    struct alloc Talloc[5];
    struct dem Tdem[5];

    Dispo dispo={10,10,10};





void  Creer_semaphores(){
    // -------------------------------------------------------------------------------------creatation 
    int nsems = 3;
    char *path = "./Project2.c" ;
   

    // Créer l'ensemble de sémaphores
    if ((semid = semget(ftok(path,(key_t)CLE), nsems,IPC_CREAT|IPC_EXCL|SEM_R|SEM_A)) == -1) {
        if (errno == EEXIST) {
            // L'ensemble de sémaphores existe déjà, obtenir l'identifiant existant
            semid = semget(ftok(path,(key_t)CLE), nsems, 0);
        } else {
        perror("Echec de semget") ;
        exit(1) ;
        }
    }

    printf("Identifiant de l'ensemble de sémaphores : %d\n", semid);
    // printf("cet ensemble est identifie par la cle unique : %d\n",ftok(path,(key_t)CLE)) ;
    // -------------------------------------------------------------------------------------Initialisation 
    int sem_value=0;

        if (semctl(semid, 0, SETVAL, nvide) == -1) {
            perror("semctl ");
            exit(EXIT_FAILURE);
        }
        if (semctl(semid, 1, SETVAL, mutex) == -1) {
            perror("semctl ");
            exit(EXIT_FAILURE);
        }
        if (semctl(semid, 2, SETVAL, mutex1) == -1) {
            perror("semctl ");
            exit(EXIT_FAILURE);
        }
}

// void P(int semid, int sem_num) {
void P(int sem_num) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = -1;  
    sem_op.sem_flg = 0;

    if (semop(semid, &sem_op, 1) == -1) {
        perror("semop wait");
        exit(EXIT_FAILURE);
    }
}

// void V(int semid, int sem_num) {
void V(int sem_num) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = 1;  
    sem_op.sem_flg = 0;

    if (semop(semid, &sem_op, 1) == -1) {
        perror("semop signal");
        exit(EXIT_FAILURE);
    }
}

void Creer_6_Fmsgs(){
    Fliber=(int*)malloc(5 * sizeof(int));
    // int* filemsgs = (int*)malloc(5 * sizeof(int));
    int msqid ; 
    char *path = "./Project2.c" ;
    key_t key;

    for(int i =0;i<=5;i++){
    key=ftok(path,(key_t)(CLE3+i));
    if (key == -1) {
    perror("ftok");
    exit(EXIT_FAILURE);
    }
    
    if(i==0){
            if (( Freponse = msgget(key,IPC_CREAT|IPC_EXCL|MSG_R|MSG_W)) == -1) {
            if (errno == EEXIST) {
                // L'ensemble de file msegges  existe déjà, obtenir l'identifiant existant
                Freponse = msgget(key,0);
            } else { 
            perror("Echec de msgget") ;
            exit(1) ;
            }
        }
            printf("identificateur de Freponse est : %d\n",Freponse) ;

    }else{
        if (( Fliber[i] = msgget(key,IPC_CREAT|IPC_EXCL|MSG_R|MSG_W)) == -1) {
            if (errno == EEXIST) {
                // L'ensemble de file msegges  existe déjà, obtenir l'identifiant existant
                Fliber[i] = msgget(key,0);
            } else { 
            perror("Echec de msgget") ;
            exit(1) ;
            }
        }
            printf("identificateur de la Flibre[%d]: %d\n",i,Fliber[i]) ;
        }
    }



    // printf("cette file est identifiee par la cle unique : %d\n",key) ;

    
}

void Rsend(int msgid ,msg_R message){

  if (msgsnd(msgid, &message, sizeof(Rep), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    // printf("Message Sent: %s\n", message.msg_text);
}

void Rreciver(int msgid ,msg_R message ){

    if (msgrcv(msgid, &message, sizeof(Rep), MESSAGE_TYPE, 0) == -1) {
    perror("msgrcv");
    exit(EXIT_FAILURE);
    }
    //  printf("Message Received: id_p=%d, lib1=%d, lib2=%d, lib3=%d\n", message.data.id_p, message.data.lib1, message.data.lib2, message.data.lib3);

}

void Lsend(int msgid ,msg_L message){

  if (msgsnd(msgid, &message, sizeof(Lib), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    // printf("Message Sent: %s\n", message.msg_text);
}

void Lreciver(int msgid ,msg_L message ){

    if (msgrcv(msgid, &message, sizeof(Lib), MESSAGE_TYPE, 0) == -1) {
    perror("msgrcv");
    exit(EXIT_FAILURE);
    }

    printf("Message Received: id_p=%d, lib1=%d, lib2=%d, lib3=%d\n", message.data.id_p, message.data.lib1, message.data.lib2, message.data.lib3);

}

void Creer_et_attacher_tompon(){

    int shmid ; /* identificateur de la memoire commune */
    int size = sizeof(Req) * 3 ;
    char *path="./Project2.c" ;
    char *mem;

    if (( shmid = shmget(ftok(path,(key_t)CLE2), size,IPC_CREAT|IPC_EXCL|SHM_R|SHM_W)) == -1) {
        if (errno == EEXIST) {
            // le tompon existe déjà, obtenir l'identifiant existant
            shmid = shmget(ftok(path,(key_t)CLE2), size,0);
        } else { 
        perror("Echec de shmget") ;
        exit(1) ;
        }
    }
    Trequetes= shmid;
    printf("identificateur du tompon: %d \n",Trequetes) ;
    // printf("ce segment est associe a la cle unique: %d\n",ftok(path,(key_t)CLE));
    
    //attachement du processus a la zone de memoire
    if ((shmaddr = shmat(Trequetes, 0, 0)) == (char *)-1) {
        perror("Attachement impossible");
        exit(1);
    }
    
    // Initialisation de la mémoire partagée avec des structures vides
    for (int i = 0; i < 3; i++) {
        ((Req *)(shmaddr + i * sizeof(struct Req)))->id = -1;
    }

}

void Detruite_semaphores(){
     if (semctl(semid, 0, IPC_RMID, 0) == -1) {
        perror("semctl IPC_RMID helloo");
        exit(EXIT_FAILURE);
    }else{
        printf("les semaphores est detruite avec succ \n");
    }

}
void Detruite_Fmsf(){
    
    if (msgctl(Freponse, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    for(int i =1;i<=5;i++){
    
    if (msgctl(Fliber[i], IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    }
    printf("Files de messages sont detruite avec succ \n");

    free(Fliber);

    

}



void Detruite_tompon(){
    
    
    // Détacher le processus de la zone de mémoire partagée
    if (shmdt(shmaddr) == -1) {
        perror("Détachement impossible");
        exit(1);
    }

    if (shmctl(Trequetes, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    printf("le tompon  est dtruite avec succ \n");


}

void Deposer(Req *req){
    char *mem ;

    if ((mem = shmat(Trequetes, 0, 0)) == (char *)-1) {
        perror("Attachement impossible");
        exit(1);
    }


    // Section critique


    // Trouver un emplacement libre dans la mémoire partagée
    int i = 0;
    while (i < 3 && ((struct Req *)(mem + i * sizeof(struct Req)))->id != -1) {
        i++;
    }


    if (i < 3) {
        // Copier la structure dans la mémoire partagée
        memcpy(mem + i * sizeof(struct Req), req, sizeof(struct Req));
        // printf("Déposé : id=%d nb1=%d nb2=%d nb3=%d\n", req->id, req->nb1, req->nb2, req->nb3);
    }
     else {
        printf("Mémoire pleine, impossible de déposer.\n");
    }


    // Détacher le processus de la zone de mémoire partagée
    if (shmdt(mem) == -1) {
        perror("Détachement impossible");
        exit(1);
    }

}

void Prelever(Req * req2){
      char *mem;

    // Attacher le processus à la zone de mémoire partagée
    if ((mem = shmat(Trequetes, 0, 0)) == (char *)-1) {
        perror("Attachement impossible");
        exit(1);
    }

    // Section critique


    for (int i = 0; i < 3; i++) {
        struct Req *req = (struct Req *)(mem + i * sizeof(struct Req));
        if (req->id != -1) {
            memcpy(req2, req,sizeof(struct Req));
            // printf("Prélevé : id=%d nb1=%d nb2=%d nb3=%d\n", req->id, req->nb1, req->nb2, req->nb3);
            // Marquer l'emplacement comme vide
            req->id = -1;
        }
    }


    // Détacher le processus de la zone de mémoire partagée
    if (shmdt(mem) == -1) {
        perror("Détachement impossible");
        exit(1);
    }
}



// void p1(){
//     P(2);
//     printf("hello i'm one \n");
//     exit(0);
// }
// void p2(){

//     sleep(2);
//     printf("hello i'm two \n");
//     V(2);
//     exit(0);
// }

int satisfy(Req *req){

    struct victim Tvictim[4];
    int indx ,nb=0,k=0;
    bool satisfied=0 ;
    Dispo copyDispo = dispo;

    if( dispo.N1-req->nb1 >=0 && dispo.N2-req->nb2 >=0 && dispo.N3-req->nb3 >=0){
        return 1;
    };

    //construier le table victim 
    for(int i=1;i<=5;i++){
        if(Tinfos[i].etate==1 ){
            Tvictim[nb].index=i;
            nb=nb+1;
        }
    };

     //trier le table victime 
    for (int i = 0; i < nb-1; i++) {
        for (int j = 0; j < nb - i - 1; j++) {
            if (Tvictim[j].temp_att > Tvictim[j + 1].temp_att) {
                struct victim temp = Tvictim[j];
                Tvictim[j] = Tvictim[j + 1];
                Tvictim[j + 1] = temp;
            }
        }
    };



    while(satisfied==0 && k < nb ){
        indx=Tvictim[k].index;
        int besoinN1,besoinN2,besoinN3;
        besoinN1=copyDispo.N1-req->nb1;
        besoinN2=copyDispo.N2-req->nb2;
        besoinN3=copyDispo.N3-req->nb3;
        if(Talloc[indx].alloc1+besoinN1 >=0 &&Talloc[indx].alloc2+besoinN2 >=0 &&Talloc[indx].alloc3+besoinN3 >=0){
            int newdem1,newdem2,newdem3;
            int newdispo1,newdispo2,newdispo3;

            newdem1=(besoinN1 >= 0) ? 0 : Talloc[indx].alloc1+besoinN1;
            newdem2=(besoinN2 >= 0) ? 0 : Talloc[indx].alloc2+besoinN2;
            newdem3=(besoinN3>= 0) ? 0 : Talloc[indx].alloc3+besoinN3;

            Tdem[indx].dem1= Tdem[indx].dem1 +newdem1  ;
            Tdem[indx].dem2=Tdem[indx].dem2 + newdem2 ;
            Tdem[indx].dem3=Tdem[indx].dem3 +newdem3 ;

            Talloc[indx].alloc1=Talloc[indx].alloc1-newdem1;
            Talloc[indx].alloc2=Talloc[indx].alloc2-newdem2;
            Talloc[indx].alloc3=Talloc[indx].alloc3-newdem3;


            newdispo1=(besoinN1 > 0) ? besoinN1  : 0;
            newdispo2=(besoinN2 > 0) ? besoinN2  : 0;
            newdispo3=(besoinN3 > 0) ? besoinN3  : 0;

            copyDispo.N1 = newdispo1;
            copyDispo.N2 = newdispo2;
            copyDispo.N3 = newdispo3;

            satisfied =1 ;
        }else{
            int SN1,SN2,SN3;
            //
            SN1=(Talloc[indx].alloc1+besoinN1 < 0) ? Talloc[indx].alloc1 : Talloc[indx].alloc1+besoinN1;/////////??????????????????????????????
            SN2=(Talloc[indx].alloc2+besoinN2 < 0) ? Talloc[indx].alloc2 : Talloc[indx].alloc2+besoinN2;/////////??????????????????????????????
            SN3=(Talloc[indx].alloc3+besoinN3 < 0) ? Talloc[indx].alloc3 : Talloc[indx].alloc3+besoinN3;/////////??????????????????????????????

            //update despo copy 
            copyDispo.N1=copyDispo.N1+SN1;
            copyDispo.N2=copyDispo.N2+SN2;
            copyDispo.N3=copyDispo.N3+SN3;

            //update table alloc de la processus victim 
            Talloc[indx].alloc1=(Talloc[indx].alloc1+besoinN1 < 0) ? 0 : Talloc[indx].alloc1+besoinN1;
            Talloc[indx].alloc2=(Talloc[indx].alloc2+besoinN2 < 0) ? 0 : Talloc[indx].alloc2+besoinN2;
            Talloc[indx].alloc3=(Talloc[indx].alloc3+besoinN3 < 0) ? 0 : Talloc[indx].alloc3+besoinN3;

            k=k+1;
        }

    }
    if(satisfied){
        dispo=copyDispo;
        return 1;
    }else{
        return 0;
    }

   
}

void satisfy_others(){
    struct victim Tbloqui[4];
    int indx ,nb=0,k=0;
    bool satisfied_others=1 ;

    //construier le table bloques 
    for(int i=1;i<=5;i++){
        if(Tinfos[i].etate==1 ){
            Tbloqui[nb].index=i;
            nb=nb+1;
        }
    };

     //trier le table bloques 
    for (int i = 0; i < nb-1; i++) {
        for (int j = 0; j < nb - i - 1; j++) {
            if (Tbloqui[j].temp_att < Tbloqui[j + 1].temp_att) {
                struct victim temp = Tbloqui[j];
                Tbloqui[j] = Tbloqui[j + 1];
                Tbloqui[j + 1] = temp;
            }
        }
    };

    while(satisfied_others && k < nb){
        indx=Tbloqui[k].index;
        if(dispo.N1-Tdem[indx].dem1 >= 0 && dispo.N2-Tdem[indx].dem2 >= 0 && dispo.N3-Tdem[indx].dem3 >= 0 ){
            //update letate  de proccessue
            Tinfos[indx].etate=0;
            Tinfos[indx].temp_att=0;

            msg_R message3;
            Rep rep={indx,1};
            message3.data=rep;
            message3.msg_type = MESSAGE_TYPE;
            Rsend(Freponse,message3);

            
            //update table d'allocation 
            Talloc[indx].alloc1=Talloc[indx].alloc1+Tdem[indx].dem1;
            Talloc[indx].alloc2=Talloc[indx].alloc2+Tdem[indx].dem2;
            Talloc[indx].alloc3=Talloc[indx].alloc3+Tdem[indx].dem3;

            //update despo
            dispo.N1=dispo.N1-Tdem[indx].dem1;
            dispo.N2=dispo.N2-Tdem[indx].dem2;
            dispo.N3=dispo.N3-Tdem[indx].dem3;

            //update la table de dommande
            Tdem[indx].dem1=0;
            Tdem[indx].dem2=0;
            Tdem[indx].dem3=0;

            k=k+1;
        }else{
            satisfied_others=0;
        }
    }
}

void gerant(){
    int nbproc=5,type;
    msg_L message;
    Req requests;
    // initialiser la table Tinfos
    for(int i=0;i<5;i++){
        Tinfos[i].etate=0;
        Tinfos[i].temp_att=0;
    }

    // initialiser la table Talloc
    for(int i=0;i<5;i++){
        Talloc[i].alloc1=0;
        Talloc[i].alloc2=0;
        Talloc[i].alloc3=0;
    }

    // initialiser la table Tdem
    for(int i=0;i<5;i++){
        Tdem[i].dem1=0;
        Tdem[i].dem2=0;
        Tdem[i].dem3=0;
    }

    while(nbproc!=0){
        int trouve=0;
        while(trouve==0){
            P(2);
            if(cpt!=0){
                trouve=1;
                V(2);
                Prelever(&requests);
                type=2;
                printf("gerant trouve dans le tempon un requests de processes n %d",requests.id);
                P(2);
                cpt=-1;
                // cpt-=1;
                V(2);
                V(0);
            }
            else{
                V(2);
                // charche dans les files des massage  
                    for (int i = 1; i <= 5; i++) {
                        if (msgrcv(Fliber[i], &message, sizeof(Lib), MESSAGE_TYPE, IPC_NOWAIT) == -1) {
                            if (errno == ENOMSG) {
                                // printf("Aucun message disponible pour la file d attente %d\n", Fliber[i]);
                            } else {
                                perror("msgrcv");
                                exit(EXIT_FAILURE);
                            }
                        } else {
                            printf("Message reçu de la file d attente %d: ", Fliber[i]);
                            if(message.data.lib1 ==0 && message.data.lib2==0 && message.data.lib3==0){
                                type=4;
                            }else{
                                type=3; 
                            }
                            trouve=1;
                            break;
                        }
                    }
                }

                
            }
        
        switch (type){
            case 2:
                if(satisfy(&requests)){
                    //update table d'allocation 
                    Talloc[requests.id].alloc1=Talloc[requests.id].alloc1+requests.nb1;
                    Talloc[requests.id].alloc2=Talloc[requests.id].alloc2+requests.nb2;
                    Talloc[requests.id].alloc3=Talloc[requests.id].alloc3+requests.nb3;

                    //update despo

                    dispo.N1=dispo.N1-requests.nb1;
                    dispo.N2=dispo.N2-requests.nb2;
                    dispo.N3=dispo.N3-requests.nb3;

                    //envoyer une reponse au processus
                    msg_R message3;
                    Rep rep={requests.id,1};
                    message3.data=rep;
                    message3.msg_type = MESSAGE_TYPE;
                    Rsend(Freponse,message3);
                }else{
                    //update table Tinfos 
                    Tinfos[requests.id].etate=1 ;
                    Tinfos[requests.id].temp_att=0; //temp_att =0
                    
                    //update table Tdem 
                    Tdem[requests.id].dem1=requests.nb1;
                    Tdem[requests.id].dem2=requests.nb2;
                    Tdem[requests.id].dem3=requests.nb3;

                    //envoyer une reponse au processus
                    msg_R message3;
                    Rep rep={requests.id,0};            
                    message3.data=rep;
                    message3.msg_type = MESSAGE_TYPE;
                    Rsend(Freponse,message3);

                };
    
                break;
            case 3:
                // update despo 
                dispo.N1=dispo.N1+message.data.lib1;
                dispo.N2=dispo.N2+message.data.lib2;
                dispo.N3=dispo.N3+message.data.lib3;

                //update table alloc 
                Talloc[message.data.id_p].alloc1=Talloc[message.data.id_p].alloc1-message.data.lib1;
                Talloc[message.data.id_p].alloc2=Talloc[message.data.id_p].alloc2-message.data.lib2;
                Talloc[message.data.id_p].alloc3=Talloc[message.data.id_p].alloc3-message.data.lib3;
                satisfy_others();
                break;
            case 4:
                // update despo 
                dispo.N1=dispo.N1+Talloc[message.data.id_p].alloc1;
                dispo.N2=dispo.N2+Talloc[message.data.id_p].alloc2;
                dispo.N3=dispo.N3+Talloc[message.data.id_p].alloc3;

                //update table alloc 
                Talloc[message.data.id_p].alloc1=0;
                Talloc[message.data.id_p].alloc2=0;
                Talloc[message.data.id_p].alloc3=0;
                nbproc-=nbproc;
                satisfy_others();
                break;

    };
    // upadte le temp d attente
    for(int i=0 ; i<=5 ;i++){
        if( Tinfos[i].etate=1){
            Tinfos[i].temp_att+1;
        }
    }
    }
    exit(0);
}



void calcule(int i){
    FILE *f;
    struct instruction myStruct;
    msg_L message2;

 
    switch (i){
    case 1:
        f = fopen("Fichier1.txt", "r");
        break;
    case 2:
        f = fopen("Fichier2.txt", "r");
        break;
    case 3:
        f = fopen("Fichier3.txt", "r");
        break;
    case 4:
        f = fopen("Fichier4.txt", "r");
        break;
    case 5:
        f = fopen("Fichier5.txt", "r");
        break;
    };

    do{ 
        fscanf(f, "%d %d %d %d ", &myStruct.type, &myStruct.nb1 ,  &myStruct.nb2 , &myStruct.nb3);
            
        switch (myStruct.type){
            case 1:
                sleep(2);
                printf("le processus n: %d faire un instruction normal \n",i);
                break;
            case 2:
                Req requests ={i ,myStruct.nb1 ,myStruct.nb2, myStruct.nb3 };  
                msg_R message;
                message.msg_type = MESSAGE_TYPE;
                P(0);
                P(1);
                Deposer(&requests);
                V(1);
                P(2);
                cpt+=1;
                V(2);
                printf("le processus n: %d demmande de ressources, attendre la reponse \n ",i);
                Rreciver(Freponse,message);
                if(message.data.trep==0){
                    printf("le processus n: %d  le processus est bloqué. \n ",i);
                    Rreciver(Freponse,message); //bloque
                    printf("le processus n: %d  le processus est debloqué. \n ",i);
                }
                break;
            case 3:
                Lib lib1={i, myStruct.nb1, myStruct.nb2, myStruct.nb3};
                message2.data=lib1;
                message2.msg_type = MESSAGE_TYPE;
                printf("le processus n: %d liberer de ressources \n",i);
                Lsend(Fliber[i],message2);
                break;
            case 4:
                Lib lib2={i, 0,0,0};
                message2.data=lib2;
                message2.msg_type = MESSAGE_TYPE;
                printf("le processus n: %d est terminé .\n",i);
                Lsend(Fliber[i],message2);
                break;
        };
    }   while(myStruct.type!=4);


    fclose(f);
    exit(0);
}

int main(){

    
    Creer_et_attacher_tompon();
    Creer_semaphores();
    Creer_6_Fmsgs();




    int id;

    
    id=fork();
    if(id==0){

        gerant();
        };
    for(int i=1;i<=5;i++){
        id=fork();
        if(id==0){calcule(i);}
    }
    for(int i;i<=6 ; i++){
        wait(0);
    }

    Detruite_semaphores();
    Detruite_tompon();
    Detruite_Fmsf();
    return 0;


}






