#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_BUFFER 1024
#define DEFAULT_PAGE "index.html"
#define WEB_DIR "./www"
#define NOT_FOUND_PAGE "404.html"

typedef struct http_info_s {
	
	// Requete
	char* methode;
	char* cible;
	char* version;
	// Réponse
	char* serveur;
	char* contenu_type;
	char* date;
	int code;
	
} http_info_t;

char* analyser_format(char* format) {

	/*
	/ On pourrait également analyser le fichier ciblé s'il existe pour savoir si c'est du texte ou du binaire. Une facon simple est de répéré la présence de \n et la non présence de \n
	/ Fichier de config avec les MIME pris en charge ?
	*/

	if(!strcmp(format,"none")) return strdup("application/octet-stream"); // Type par défaut
	if(!strcmp(format,"txt")) return strdup("text/plain");
	if(!strcmp(format,"html")) return strdup("text/html");
	if(!strcmp(format,"css")) return strdup("text/css");
	if(!strcmp(format,"js")) return strdup("application/javascript");
	if(!strcmp(format,"jpg") || !strcmp(format,"jpeg")) return strdup("image/jpeg");
	if(!strcmp(format,"gif")) return strdup("image/gif");
	if(!strcmp(format,"png")) return strdup("image/png");
	if(!strcmp(format,"pdf")) return strdup("application/pdf");
	
	return strdup("application/octet-stream");
}

char* date_actuelle(void) {

	time_t timestamp;
	char buffer[256];

	time(&timestamp);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %X GMT", localtime(&timestamp));
	
	return strdup(buffer);

}

/*
/ Prends en paramètres : 
/ > socket : pointeur de FILE* représentant le socket (on pourra changer en int pour juste le fd)
/ > req : pointeur de http_info_req permettant le stockage des infos de la requète et de la réponse
/ Retour :
/ > int : 1 (OK), 0 fichier non présent
*/

int traiter_requete(FILE* socket, http_info_t* req) {

	char buffer[MAX_BUFFER];
	char methode[16];
	char cible[128];
	char version[16];
	char chemin[128];
	char* temp;
	char* format;
	int code;
	struct stat fstat;
	
	if(fgets(buffer,MAX_BUFFER,socket) == NULL) return 0;
	if(sscanf(buffer,"%s %s %s",methode,cible,version) != 3) return 0;
	if(strcmp(cible,"/") == 0) sprintf(cible,"/%s",DEFAULT_PAGE);
	
	// On vérifie la présence du fichier
	
	sprintf(chemin,"%s%s",WEB_DIR,cible);
	
  	if(stat(chemin,&fstat) != 0 || !S_ISREG(fstat.st_mode)) {
    	sprintf(chemin,"%s/%s",WEB_DIR,NOT_FOUND_PAGE);
    	strcpy(cible,NOT_FOUND_PAGE);
    	code = 404;
    }
    else {
 		code = 200;
    }

	// Analyse du type de contenu

	if((temp = strrchr(cible,'.')) != NULL) {
		format = temp+1;
	}
	else {
		format = "none";
	}
	
	// Affectation à la structure
	
	req->code = code;
	req->methode = strdup(methode);
	req->cible = strdup(chemin);
	req->version = strdup(version);
	req->serveur = strdup("Choctaw HTTP Server");
	req->contenu_type = analyser_format(format);
	req->date = date_actuelle();
	
	// Lecture du reste (on en fait quoi ?)
	
	while(fgets(buffer,MAX_BUFFER,socket) != NULL) {
		if(strcmp(buffer,"\n") == 0) break;
		printf("%s",buffer);
	}
	
	return 1;

}

void free_http_info(http_info_t* r) {

	free(r->methode);
	free(r->cible);
	free(r->version);
	free(r->serveur);
	free(r->contenu_type);
	free(r->date);
	
}

int main(void) {

	FILE* s = fopen("request_http","r");
	http_info_t req;
	traiter_requete(s,&req);
	
	printf("---------------------------\n");
	printf("Methode\t: %s\nCible\t: %s\nVersion\t: %s\nServeur\t: %s\nFormat\t: %s\nDate\t: %s\nCode\t: %d\n",req.methode,req.cible,req.version,req.serveur,req.contenu_type,req.date,req.code);
	
	free_http_info(&req);

	return 0;
	
}
