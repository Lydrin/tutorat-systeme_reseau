#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "broadcast.h"

#define MAC_SIZE	 6
#define IPV4_SIZE	 4

#define IP_SOURCE    "172.26.79.205"
#define IP_DEST      "172.26.79.204"

#define PORT_SOURCE  4269
#define PORT_DEST    80

#define ID_TSHIRT    3

void forger_trameUDP(TrameUDP* trame, uint8_t* v_capteurs) {

	DataUDP data;
	
	trame->port_source = (uint16_t)PORT_SOURCE;
	trame->port_destination = (uint16_t)PORT_DEST;
	trame->longueur = 13;
	trame->checksum = 0;
	
	data.id_tshirt = (uint8_t)ID_TSHIRT;
	data.accel_x = v_capteurs[0];
	data.accel_y = v_capteurs[1];
	data.accel_z = v_capteurs[2];
	data.temp = v_capteurs[3];
	
	trame->data = data;

}

void forger_trameIP(TrameIP* trame, uint8_t* v_capteurs) {

	trame->version_longueur_entete = 0x45;
	trame->TOS = 0;
	trame->longueur_totale = 33;
	trame->identificateur = 0;
	trame->flags_offset = 0x4000;
	trame->TTL = 64;
	trame->protocole = 0x11;
	trame->checksum = 0;
	
	char* ips = strdup(IP_SOURCE); 
	char* ipd = strdup(IP_DEST);
	char *token1, *token2;
	uint8_t i = 0;

	while((token1 = strtok_r(ips, ".", &ips)) && (token2 = strtok_r(ipd, ".", &ipd)))
	{
		trame->adr_source[i] = (uint8_t)atoi(token1);
		trame->adr_destination[i] = (uint8_t)atoi(token2);
		i++;
	}
	
	TrameUDP trameU;
	forger_trameUDP(&trameU, v_capteurs);
	trame->data = trameU;	

}

void afficher_trame(TrameIP* trame) { // Fonction de test

	printf("TOS : %d\n",trame->TOS);
	printf("TOS : %d\n",trame->TTL);
	printf("IP_S : %d.%d.%d.%d\n",trame->adr_source[0],trame->adr_source[1],trame->adr_source[2],trame->adr_source[3]);
	printf("PORT_SOURCE : %d\n",(trame->data).port_source);
	printf("DATA (TEMP): %d\n",((trame->data).data).temp);
}


int main(void) {

    //init_printf();
    uint8_t v_capteurs[4];
    TrameIP trame;
    v_capteurs[0] = 26;
    v_capteurs[1] = 13;
    v_capteurs[2] = 189;
    v_capteurs[3] = 254;
    forger_trameIP(&trame, v_capteurs);
    afficher_trame(&trame);
    
    
    //init_trame(&trame);

/*
   
    while(1) {
    
        for(int i = 0; i <= 3; i++) {
            ad_init(i);
            v_capteurs[i] = ad_sample();
        }
        
        forger_trame(&trame, v_capteurs);
        envoyer_trame(&trame);
    }
    
*/
    
    return 0;
}