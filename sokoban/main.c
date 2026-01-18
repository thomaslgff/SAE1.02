#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>

#define TAILLE 12
#define NB_D_MAX 1000
#define SOKOBAN '@'
#define SOKOBAN_SUR_CIBLE '+'
#define MUR '#'
#define CAISSE '$'
#define CAISSE_SUR_CIBLE '*'
#define CIBLE '.'
#define VIDE ' '

#define SOKOG 'g'
#define SOKOH 'h'
#define SOKOB 'b'
#define SOKOD 'd'
#define SOKOCG 'G'
#define SOKOCH 'H'
#define SOKOCB 'B'
#define SOKOCD 'D'

const int PAUSE = 400000;

typedef struct{
    int x;
    int y;
} position;

typedef char typePlateau[TAILLE][TAILLE];
typedef char typeDeplacements[NB_D_MAX];
typedef position typePosition[NB_D_MAX];

void charger_partie(typePlateau plateau, char fichier[50]);
void afficher_entete(char fichier[50], int nbDeplacements);
void afficher_plateau(typePlateau plateau);
char retirer_sokoban(char contenuCase);
char afficher_sokoban(char contenuCase);
char afficher_caisse(char contenuCase);
char retirer_caisse(char contenuCase);
void trouver_sokoban(typePlateau plateau, int *ligSoko, int *colSoko);
bool traiter_victoire(typePlateau plateau);
void afficher_resultat(typePlateau plateau, char fichier[50], char fichierDep[50], int nb, bool r, int nbDeplacementsTotal, typeDeplacements tOpti);
void deplacement(typePlateau plateau, char t, int *ligSoko, int *colSoko, int *signalCaisse);
void charger_deplacements(typeDeplacements t, char fichier[50], int * nb);
void sauvegarder_solution(char fichierDep[50], typeDeplacements t, int nb);
void optimiser_deplacements(typePlateau plateau, typeDeplacements t, typePosition historiquePos, position departPos, 
                           int *nbDeplacementsRestants, int *ligSoko, int *colSoko, char fichier[50]);

int main() {
    char fichier[50], fichierDep[50];
    typePlateau plateau;
    typeDeplacements t;
    typePosition historiquePos;

    int nbDeplacementsTotal = 0;
    int nbDeplacementsRestants = 0;
    int ligSoko = 0, colSoko = 0;
    
    position departPos;

    printf("nom fichier partie (ex: niveau.sok): ");
    scanf("%49s", fichier);
    printf("nom fichier deplacements (ex: solution.dep): ");
    scanf("%49s", fichierDep);

    charger_partie(plateau, fichier);
    trouver_sokoban(plateau, &ligSoko, &colSoko);
    departPos.x = ligSoko;
    departPos.y = colSoko;

    charger_deplacements(t, fichierDep, &nbDeplacementsTotal);
    nbDeplacementsRestants = nbDeplacementsTotal;

    optimiser_deplacements(plateau, t, historiquePos, departPos, &nbDeplacementsRestants, &ligSoko, &colSoko, fichier);

    system("clear");
    afficher_entete(fichier, nbDeplacementsRestants);
    afficher_plateau(plateau);

    bool res = traiter_victoire(plateau);
    afficher_resultat(plateau, fichier, fichierDep, nbDeplacementsRestants, res, nbDeplacementsTotal, t);

    return 0;
}

void optimiser_deplacements(typePlateau plateau, typeDeplacements dep,
                            typePosition historique, position posDepart,
                            int *nbDep, int *ligSoko, int *colSoko, char fichier[50])
{
    int indiceDerniereCaisse = -1;
    int deplacementCaisse;

    for (int i = 0; i < *nbDep; i++) {

        int ancienneLig = *ligSoko;
        int ancienneCol = *colSoko;

        system("clear");
        afficher_entete(fichier, i);
        afficher_plateau(plateau);
        deplacement(plateau, dep[i], ligSoko, colSoko, &deplacementCaisse);
        usleep(PAUSE);

        /* Suppression des déplacements impossibles */
        if (*ligSoko == ancienneLig && *colSoko == ancienneCol) {
            for (int k = i; k < *nbDep - 1; k++) {
                dep[k] = dep[k + 1];
            }
            (*nbDep)--;
            i--;
            continue;
        }

        /* Sauvegarde de la position courante */
        historique[i].x = *ligSoko;
        historique[i].y = *colSoko;

        /* Si une caisse est déplacée, on met à jour la référence */
        if (deplacementCaisse) {
            indiceDerniereCaisse = i;
            continue;
        }

        /* Recherche d’une boucle inutile sans déplacement de caisse */
        int debutRecherche;
        if (indiceDerniereCaisse == -1) {
            debutRecherche = 0;
        } else {
            debutRecherche = indiceDerniereCaisse;
        }

        for (int k = i - 1; k >= debutRecherche; k--) {
            if (historique[k].x == *ligSoko && historique[k].y == *colSoko) {

                int nbASupprimer = i - k;
                for (int m = i + 1; m < *nbDep; m++) {
                    dep[m - nbASupprimer] = dep[m];
                }

                *nbDep -= nbASupprimer;
                i = k;
                break;
            }
        }
    }
}



void afficher_entete(char fichier[50], int nbDeplacements) {
    printf("Partie %s \n", fichier);
    printf("Deplacement numero : %d\n", nbDeplacements);
}

void afficher_plateau(typePlateau plateau) {
    for (int lig = 0; lig < TAILLE; lig++) {
        printf("\n");
        for (int col = 0; col < TAILLE; col++) {
            char c = plateau[lig][col];
            if (c == SOKOBAN_SUR_CIBLE) c = SOKOBAN;
            if (c == CAISSE_SUR_CIBLE) c = CAISSE;
            printf("%c", c);
        }
    }
    printf("\n");
}

char retirer_sokoban(char contenuCase) {
    if (contenuCase == SOKOBAN_SUR_CIBLE) return CIBLE;
    if (contenuCase == SOKOBAN) return VIDE;
    return contenuCase;
}

char afficher_sokoban(char contenuCase) {
    if (contenuCase == CIBLE) return SOKOBAN_SUR_CIBLE;
    if (contenuCase == VIDE) return SOKOBAN;
    return contenuCase;
}

char retirer_caisse(char contenuCase) {
    if (contenuCase == CAISSE_SUR_CIBLE) return CIBLE;
    if (contenuCase == CAISSE) return VIDE;
    return contenuCase;
}

char afficher_caisse(char contenuCase) {
    if (contenuCase == CIBLE) return CAISSE_SUR_CIBLE;
    if (contenuCase == VIDE) return CAISSE;
    return contenuCase;
}

void deplacement(typePlateau plateau, char t, int *ligSoko, int *colSoko, int *signalCaisse) {
    int dlig = 0, dcol = 0;
    *signalCaisse = 0;

    switch (t) {
        case SOKOH:  dlig = -1; break;
        case SOKOB:  dlig = 1; break;
        case SOKOG:  dcol = -1; break;
        case SOKOD:  dcol = 1; break;
        case SOKOCH:  dlig = -1; *signalCaisse = 1; break;
        case SOKOCB:  dlig = 1; *signalCaisse = 1; break;
        case SOKOCG:  dcol = -1; *signalCaisse = 1; break;
        case SOKOCD:  dcol = 1; *signalCaisse = 1; break;
        default: return;
    }

    int ligSuivante  = *ligSoko + dlig;
    int colSuivante  = *colSoko + dcol;
    int ligDerriere  = ligSuivante + dlig;
    int colDerriere  = colSuivante + dcol;

    if (ligSuivante < 0 || ligSuivante >= TAILLE || colSuivante < 0 || colSuivante >= TAILLE) return;

    char caseSuivante = plateau[ligSuivante][colSuivante];

    if (caseSuivante == MUR && *signalCaisse == 0) return;

    if ((caseSuivante == VIDE || caseSuivante == CIBLE) && *signalCaisse == 0) {
        plateau[*ligSoko][*colSoko] = retirer_sokoban(plateau[*ligSoko][*colSoko]);
        plateau[ligSuivante][colSuivante] = afficher_sokoban(plateau[ligSuivante][colSuivante]);
        *ligSoko = ligSuivante;
        *colSoko = colSuivante;
        return;
    }

    if ((caseSuivante == CAISSE || caseSuivante == CAISSE_SUR_CIBLE) && *signalCaisse == 1) {
        if (ligDerriere < 0 || ligDerriere >= TAILLE || colDerriere < 0 || colDerriere >= TAILLE) return;

        char caseDerriere = plateau[ligDerriere][colDerriere];
        if (caseDerriere == MUR || caseDerriere == CAISSE || caseDerriere == CAISSE_SUR_CIBLE) return;

        plateau[ligDerriere][colDerriere] = afficher_caisse(caseDerriere);
        plateau[ligSuivante][colSuivante] = retirer_caisse(plateau[ligSuivante][colSuivante]);
        plateau[*ligSoko][*colSoko] = retirer_sokoban(plateau[*ligSoko][*colSoko]);
        plateau[ligSuivante][colSuivante] = afficher_sokoban(plateau[ligSuivante][colSuivante]);
        
        *ligSoko = ligSuivante; 
        *colSoko = colSuivante;
        return;
    }
}

void trouver_sokoban(typePlateau plateau, int *ligSoko, int *colSoko) {
    for (int lig = 0; lig < TAILLE; lig++) {
        for (int col = 0; col < TAILLE; col++) {
            if (plateau[lig][col] == SOKOBAN || plateau[lig][col] == SOKOBAN_SUR_CIBLE) {
                *ligSoko = lig;
                *colSoko = col;
            }
        }
    }
}

bool traiter_victoire(typePlateau plateau) {
    for (int lig = 0; lig < TAILLE; lig++) {
        for (int col = 0; col < TAILLE; col++) {
            if (plateau[lig][col] == CIBLE || plateau[lig][col] == SOKOBAN_SUR_CIBLE) {
                return false;
            }
        }
    }
    return true;
}

void afficher_resultat(typePlateau plateau, char fichier[50], char fichierDep[50], int nb, bool r, int nbDeplacementsTotal, typeDeplacements tOpti) {
    char rep;
    
    if(r){
        printf("La suite de deplacements %s est bien une solution pour la partie %s\n", fichierDep, fichier);
        printf("Elle contient initialement %d caracteres.\n", nbDeplacementsTotal);
        printf("Apres optimisation elle contient %d caracteres.\n", nb);
        
        printf("Souhaitez-vous l'enregistrer (O/N) ? ");
        scanf(" %c", &rep);

        if(rep == 'O' || rep == 'o') {
            sauvegarder_solution(fichierDep, tOpti, nb);
        }
    }
    else{
        printf("La suite de deplacements %s N'EST PAS une solution pour la partie %s\n", fichierDep, fichier);
    }
}

void charger_partie(typePlateau plateau, char fichier[50]){
    FILE * f;
    char finDeLigne;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("ERREUR SUR FICHIER PARTIE\n");
        exit(EXIT_FAILURE);
    } else {
        for (int ligne=0 ; ligne<TAILLE ; ligne++){
            for (int colonne=0 ; colonne<TAILLE ; colonne++){
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}

void charger_deplacements(typeDeplacements t, char fichier[50], int * nb){
    FILE * f;
    char dep;
    *nb = 0;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("FICHIER DEPLACEMENTS NON TROUVE\n");
    } else {
        fread(&dep, sizeof(char), 1, f);
        if (feof(f)){
            printf("FICHIER VIDE\n");
        } else {
            while (!feof(f) && *nb < NB_D_MAX){
                if(isalpha(dep)) { 
                    t[*nb] = dep;
                    (*nb)++;
                }
                fread(&dep, sizeof(char), 1, f);
            }
        }
        fclose(f);
    }
}

void sauvegarder_solution(char fichierDep[50], typeDeplacements t, int nb) {
    char nomFichier[60];
    printf("Entrez le nom du fichier de sauvegarde (ex: opti.dep) : ");
    scanf("%s", nomFichier);

    FILE *f = fopen(nomFichier, "w");
    if (f == NULL) {
        printf("Erreur lors de la creation du fichier.\n");
        return;
    }
    
    for(int i=0; i < nb; i++) {
        fprintf(f, "%c", t[i]);
    }
    fclose(f);
    printf("Sauvegarde terminee dans %s.\n", nomFichier);
}