#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#define TAILLE_SECTEUR 512

/**
@Created by : Aghiles Azzoug
@Date : 23 / 03 / 2017
*/

int isDisque(const char * dir) {
    if (strlen(dir) != 3) return 0;
    if ((dir[0] == 'h' || dir[0] == 's') && dir[1] == 'd') return 1;
    return 0;
}

int list_partitions(const char * dir) {
    struct dirent * lecture;
    DIR * rep;
    rep = opendir("/dev");
    int ind = 0;
    while ((lecture = readdir(rep))) {
        if (strstr(lecture -> d_name, dir) != NULL && strcmp(lecture -> d_name, dir)) {
            printf("\t\t[+] Partition %d : %s\n", ind++, lecture -> d_name);
        }
    }
    closedir(rep);
}

int list_disques() {
    /***
      retourne le nombre de disques
    */
    struct dirent * lecture;
    DIR * rep;
    rep = opendir("/dev");
    int ind = 0;
    while ((lecture = readdir(rep))) {
        if (isDisque(lecture -> d_name)) {
            printf("\n\t[+] Disque N° %d : %s\n", ind++, lecture -> d_name);
            list_partitions(lecture -> d_name);
            printf("\n");
        }

    }
    closedir(rep);
    return ind;
}

void afficher_buffer(const unsigned char * buffer,
    const unsigned long numeroSecteur) {
    int i;

    for (i = 0; i < TAILLE_SECTEUR / 16; ++i) {
        printf("\n%03X : ", i * 16 + numeroSecteur * TAILLE_SECTEUR);
        int j;

        for (j = 0; j < 16; ++j) {
            printf("%02x ", buffer[i * 16 + j]);
        }
    }
    printf("\n\n");

}

int afficherSecteur(const unsigned long numeroSecteur,
    const char * cheminDisque) {

    FILE * disque;
    unsigned char buffer[TAILLE_SECTEUR];
    strcpy(buffer, "");
    if ((disque = fopen(cheminDisque, "rb")) == NULL) {
        printf("\n[-] Impossible de lire le disque demandé.\n");
        return -1;
    } else {

        if (fseek(disque, TAILLE_SECTEUR * numeroSecteur, SEEK_SET)) {
            printf("\n[-] Secteur innexistant.\n");
            return -1;
        }

        fread(buffer, TAILLE_SECTEUR, 1, disque);

        afficher_buffer(buffer, numeroSecteur);
    }
}

int existe(const unsigned char * buffer,
    const unsigned char * chaineinfo) {
    int i = 0;
    int j = 0;
    while (i <= (TAILLE_SECTEUR - strlen(chaineinfo))) {
        while (buffer[i] == chaineinfo[j]) {
            j++;
            i++;
        }
        if (j == strlen(chaineinfo)) {
            return 1;
        } else {
            j = 0;
        }
        i++;
    }
    return 0;

}

//Rechercher une chaine de caractère
int * rechercher_secteur(const char * chaineinfo,
    const unsigned long secteur_debut,
        const unsigned long secteur_fin,
            const char * cheminDisque) {

    if (strlen(chaineinfo) > TAILLE_SECTEUR) {
        printf("\n[-] La longueur de la chaine est suppérieure à celle d'un secteur.\n");
        return NULL; //NULL
    }

    FILE * disque;
    if ((disque = fopen(cheminDisque, "rb")) == NULL) {
        printf("\n[-] impossible d'ouvrir le disque.\n");
        return NULL;
    }

    int ind;
    char buffer[TAILLE_SECTEUR];

    for (ind = secteur_debut; ind < secteur_fin + 1; ind++) {

        //memcpy(buffer,0,TAILLE_SECTEUR);
        if (fseek(disque, TAILLE_SECTEUR * ind, SEEK_SET)) {
            printf("\n[-] Secteur innexistant.\n");
            return NULL;
        }
        size_t read = fread(buffer, TAILLE_SECTEUR, 1, disque);

        if (read == 0) {
            return -1;
        }

        //printf("\nvaleur de existe = %d\n",existe(buffer,chaineinfo));
        if (existe(buffer, chaineinfo)) {
            printf("\n\n\t\t[+] Occurrence trouvée dans le secteur : < %d >\n", ind);

        } else {
            printf("\n\n\t\t[-] Aucune occurrence trouvée.\n");
        }

    }
}

//retourne 1 si la partition est de type FAT
int estFAT(const unsigned char octet) {
    if (octet == 0x01 || octet == 0xef || octet == 0xc6 || octet == 0xc4 || octet == 0xc1 || octet == 0x86 || octet == 0x1E ||
        octet == 0x1C || octet == 0x1B || octet == 0x16 || octet == 0x14 || octet == 0x11 || octet == 0x0E || octet == 0x0C ||
        octet == 0x0B || octet == 0x06 || octet == 0x04 || octet == 0x07 || octet == 0x17) {
        return 1;
    } else return 0;
}

//formatage d'une taille donnée en octets
int afficherTaille(const unsigned long taille) {
    if (taille >= pow(2, 30) / TAILLE_SECTEUR) {
        printf("%2.3f Go\n", (float)(taille) / (2 * 1024 * 1024));
        return 0;
    }

    if (taille >= pow(2, 20) / TAILLE_SECTEUR) {
        printf("%2.3f Mo\n", (float)(taille) / (2 * 1024));
        return 0;
    }

    if (taille >= pow(2, 10) / TAILLE_SECTEUR) {
        printf("%2.3f Ko\n", (float)(taille) / (2));
        return 0;
    }

    printf("%d o\n", taille);
    return 0;

}

//retourne le système de fichier
char * sysFichier(const unsigned char octet) {
    char * res = malloc(10);
    strcpy(res, "unknown");
    if (octet == 0xee) {
        strcpy(res, "EFI GPT");
        return res;

    }

    if (octet == 0x83) {
        strcpy(res, "EXT");
        return res;

    }
    if (octet == 0x82) {
        strcpy(res, "Linux Swap");
        return res;

    }

    if (octet == 0x00) {
        strcpy(res, "Vide");
        return res;

    }
    if (octet == 0x05) {
        strcpy(res, "Etendue");
        return res;

    }
    if (octet == 0x06) {
        strcpy(res, "FAT16");
        return res;

    }
    if (octet == 0x0b) {
        strcpy(res, "FAT32");
        return res;
    }
    if (octet == 0x07) {
        strcpy(res, "NTFS");
        return res;

    }
    if (octet == 0x01) {
        strcpy(res, "FAT12");
        return res;

    }
    return res;
}

//retourne 1 Si la partition est étandue
int estEtendue(const unsigned char type) {

    if (type == 0x05 || type == 15 || type == 21 || type == 31 || type == 34 || type == 59 || type == 66 ||
        type == 145 || type == 155 || type == 155 || type == 207 || type == 213) {
        return 1; //si la partition est étendue
    }
    return 0; //sinon
}

//lit le secteur "numeroSecteur" du disque "cheminDisque" 
unsigned char * lire_secteur(const unsigned long numeroSecteur,
    const char * cheminDisque) {
    FILE * disque;
    unsigned char * buffer = malloc(TAILLE_SECTEUR);
    strcpy(buffer, "");
    if ((disque = fopen(cheminDisque, "rb")) == NULL) {
        printf("\n[-] Impossible de lire le disque demandé.\n");
        return -1;
    } else {

        if (fseek(disque, TAILLE_SECTEUR * numeroSecteur, SEEK_SET)) {
            printf("\n[-] Secteur innexistant.\n");
            return -1;
        }

        fread(buffer, TAILLE_SECTEUR, 1, disque);
        return buffer;

    }
    fclose(disque);

}

//affiche les caractéristiques du disque
void caractDisque(const char * cheminDisque) {

    FILE * disque;
    unsigned char * buffer;
    int nbPartitionPrincipales = 0;
    int nbPartitionLogiques = 0;
    int tailleDisque = 0;
    unsigned char * bufferEBR;

    buffer = lire_secteur(0, cheminDisque);

    printf("\n\n\n\t\t\t[+] Information disque !\n\n");
    printf("\n[+]L'indentifiant du disque est : 0x%X%X%X%X\n", buffer[0x1bb], buffer[0x1ba], buffer[0x1b9], buffer[0x1b8]);

    int i;
    printf("\n\t[+]Affichage de la table des partitions en Hexa : \n");

    for (i = 0; i < 4; ++i) {
        printf("\n%03X : ", i * 16 + 446);
        int j;

        for (j = 0; j < 16; ++j) {
            printf("%02x ", buffer[i * 16 + j + 0x1BE]);
            if (j == 0 || j == 3 || j == 4 || j == 7 || j == 11) {
                printf(" ");
            }
        }
        printf("\n");
    }

    printf("\n\n\t[+] Details sur les partitions : \n\n");
    for (i = 0; i < 4; ++i) { //pour chaque partition !

        if (buffer[i * 16 + 0x1BE + 4] != 0x00) //si la partition n'est pas vide !
        {

            printf("\n\t\t[+] Partition N %d :\n\n", i);
            if (estEtendue(buffer[i * 16 + 0x1BE + 4]))
                printf("\n\t\t\t[+] Type : Etendue \n\n");
            else
                printf("\n\t\t\t[+] Type : Principale \n\n");
            printf("\t\t\t[+] Systeme de fichier : %s\n", sysFichier(buffer[i * 16 + 0x1BE + 4])); //type ou systeme de fichier

            //taille
            printf("\n\t\t\t[+] Taille : ");
            afficherTaille((unsigned long)(buffer[i * 16 + 0x1BE + 15] * 16 * 16 * 16 * 16 * 16 * 16 + buffer[i * 16 + 0x1BE + 14] * 16 * 16 * 16 * 16 + buffer[i * 16 + 0x1BE + 13] * 16 * 16 + buffer[i * 16 + 0x1BE + 12]));
            printf("\n");

            //adrDebutPArtition
            unsigned long adrDebutPartition = (unsigned long) buffer[i * 16 + 0x1BE + 11] * 16 * 16 * 16 * 16 * 16 * 16 + buffer[i * 16 + 0x1BE + 10] * 16 * 16 * 16 * 16 + buffer[i * 16 + 0x1BE + 9] * 16 * 16 + buffer[i * 16 + 0x1BE + 8];
            printf("\t\t\t[+] Adresse debut : %d\n", adrDebutPartition);

            //lire_secteur ebr
            int k = 0;

            if (estFAT(buffer[k * 16 + 0x1BE + 4])) {
                printf("\n");
                unsigned long adresseDebutFAT = buffer[k * 16 + 0x1BE + 11] * 16 * 16 * 16 * 16 * 16 * 16 + buffer[i * 16 + 0x1BE + 10] * 16 * 16 * 16 * 16 + buffer[i * 16 + 0x1BE + 9] * 16 * 16 + buffer[i * 16 + 0x1BE + 8];
                adresseDebutFAT += adrDebutPartition;
                printf("\t\t\t\t\t[+] Adresse debut de la fat : %d\n", adresseDebutFAT);

                unsigned char * bufferFAT;
                bufferFAT = lire_secteur(adrDebutPartition, cheminDisque);

                printf("\t\t\t\t\t[+] Taille cluster = %4.2f Ko\n", (float)(bufferFAT[13] * TAILLE_SECTEUR) / 1024);
                printf("\t\t\t\t\t[+] Adresse Debut FAT = %d", bufferFAT[14] + bufferFAT[15] * 16 * 16 + adresseDebutFAT);

            }

            if (estEtendue(buffer[i * 16 + 0x1BE + 4])) {

                printf("\n\t\t\t   [+] Affichage des partitions logiques : \n");
                bufferEBR = lire_secteur(adrDebutPartition, cheminDisque);

                k = 0;
                while (!strcmp(sysFichier(bufferEBR[k * 16 + 0x1BE + 4]), "Vide") == 0) { //tant que la partition n'est pas vide
                    printf("\t\t\t\t[+] Partition logique N : %d\n", k);
                    printf("\t\t\t\t\t[+] Type: %s\n", sysFichier(bufferEBR[k * 16 + 0x1BE + 4]));

                    printf("\n\t\t\t\t\t[+] Taille : ");
                    afficherTaille((unsigned long)(bufferEBR[k * 16 + 0x1BE + 15] * 16 * 16 * 16 * 16 * 16 * 16 + bufferEBR[k * 16 + 0x1BE + 14] * 16 * 16 * 16 * 16 + bufferEBR[k * 16 + 0x1BE + 13] * 16 * 16 + bufferEBR[k * 16 + 0x1BE + 12]));
                    printf("\n");
                    printf("\t\t\t\t\t[+] Adresse Debut : %d\n", bufferEBR[k * 16 + 0x1BE + 11] * 16 * 16 * 16 * 16 * 16 * 16 + bufferEBR[k * 16 + 0x1BE + 10] * 16 * 16 * 16 * 16 + bufferEBR[k * 16 + 0x1BE + 9] * 16 * 16 + bufferEBR[k * 16 + 0x1BE + 8]);

                    if (estFAT(bufferEBR[k * 16 + 0x1BE + 4])) {
                        printf("\n");
                        unsigned long adresseDebutFAT = bufferEBR[k * 16 + 0x1BE + 11] * 16 * 16 * 16 * 16 * 16 * 16 + bufferEBR[i * 16 + 0x1BE + 10] * 16 * 16 * 16 * 16 + bufferEBR[i * 16 + 0x1BE + 9] * 16 * 16 + bufferEBR[i * 16 + 0x1BE + 8];
                        adresseDebutFAT += adrDebutPartition;
                        printf("\t\t\t\t\t[+] Adresse debut de la fat : %d\n", adresseDebutFAT);

                        unsigned char * bufferFAT;
                        bufferFAT = lire_secteur(adrDebutPartition, cheminDisque);

                        printf("\t\t\t\t\t[+] Taille cluster = %4.2f Ko\n", (float)(bufferFAT[13] * TAILLE_SECTEUR) / 1024);
                        printf("\t\t\t\t\t[+] Adresse Debut FAT = %d", bufferFAT[14] + bufferFAT[15] * 16 * 16 + adresseDebutFAT);

                    }

                    k++;
                    if (bufferEBR[k * 16 + 0x1BE + 4] != 0) {
                        unsigned long newAdr = bufferEBR[k * 16 + 0x1BE + 11] * 16 * 16 * 16 * 16 * 16 * 16 + bufferEBR[k * 16 + 0x1BE + 10] * 16 * 16 * 16 * 16 + bufferEBR[k * 16 + 0x1BE + 9] * 16 * 16 + bufferEBR[k * 16 + 0x1BE + 8] +
                            adrDebutPartition; //adr debut de la partition étendue
                        bufferEBR = lire_secteur(newAdr, cheminDisque);
                        k = 0;
                    }
                }
            }

            printf("\n\n");

        }

    }

    //afficherSecteur(0,"/dev/sda");

}

int main(int argc, char
    const * argv[]) {

    printf("\n\n\t\t\t--------------------------------");
    printf("\n\t\t\t--------- TP SYSTEME 2 ---------\n");
    printf("\t\t\t--------------------------------\n\n");

    while (1) {
        char reponse[TAILLE_SECTEUR];

        printf("\n\tInfo : A tester sur des disques externes au cas où..\n");
        printf("\n\t[+] a : Afficher les disques et partitions.");
        printf("\n\t[+] b : Afficher le secteur d'un disque.");
        printf("\n\t[+] c : Recherche d'une chaine dans un secteur.");
        printf("\n\t[+] d : Information (MBR) d'un disque");
        printf("\n\t[+] CTRL+C Pour quitter.\n");
        printf("\nChoix : ");
        scanf("%s", reponse);

        switch (reponse[0]) {
        case 'a': {
            system("clear");
            printf("\n\tInfo : Répértoire par défaut : /dev/.\n");
            list_disques();
            printf("\n");

        };
        break;

        case 'b': {
            system("clear");
            printf("\n\t[?] Lancer le programme en mode ROOT pour permettre l'affichage. \n");
            char path[512];
            int numSecteur;
            printf("\n\t[+] Le chemin vers le disque : ");
            scanf("%s", path);
            printf("\n\t[+] Le numero de secteur : ");
            scanf("%d", & numSecteur);
            printf("\n");
            afficherSecteur((unsigned long) numSecteur, path);

        };
        break;

        case 'c': {
            system("clear");
            char path[512];
            char str[TAILLE_SECTEUR];
            int numSecteurDebut;
            int numSecteurFin;
            // printf("\n[-] Codiw la fonction existe :) :) \n\n");
            printf("\n\t[+] Le chemin vers le disque : ");
            scanf("%s", path);
            printf("\n\t[+] Le secteur début : ");
            scanf("%d", & numSecteurDebut);
            printf("\n\t[+] Le secteur fin : ");
            scanf("%d", & numSecteurFin);
            printf("\n\t[+] La chaine a rechercher <Max %d Caractères> : ", TAILLE_SECTEUR);
            scanf("%s", str);
            rechercher_secteur(str, (unsigned long) numSecteurDebut, (unsigned long) numSecteurFin, path);
            printf("\n");

        };
        break;

        case 'd': {
            system("clear");
            char path[512];
            printf("\n\t[+] Le chemin vers le disque : ");
            scanf("%s", path);
            caractDisque(path);

        };
        break;

        default: {
            system("clear");
            printf("\n\n\t[-] a, b ou c.\n\n\n");
        }

        }
    }

    return 0;
}
