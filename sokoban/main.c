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

typedef char t_Plateau[TAILLE][TAILLE];
typedef char typeDeplacements[NB_D_MAX];

/* ==== DECLARATION FONCTIONS / PROCEDURES ==== */

void charger_partie(t_Plateau plateau, char fichier[50]);
void afficher_entete(char fichier[50], int nbDeplacements);
void afficher_plateau(t_Plateau plateau);
char retirer_sokoban(char contenuCase);
char afficher_sokoban(char contenuCase);
char afficher_caisse(char contenuCase);
char retirer_caisse(char contenuCase);
void trouver_sokoban(t_Plateau plateau, int *ligSoko, int *colSoko);
bool traiter_victoire(t_Plateau plateau);
void afficher_resultat(t_Plateau plateau, char fichier[50], int nb,   t_tabDeplacement t);
void conversion_retourTouche(char *retourTouche);
void undo_deplacement(t_Plateau plateau, int *ligSoko, int *colSoko, t_tabDeplacement tableauDep);
void chargerDeplacements(typeDeplacements t, char fichier[], int * nb);

/* ==== MAIN ==== */

int main() {
    char fichier[50], fichierDep[50], touche;
    t_Plateau plateau;
    typeDeplacements t;
    int nbDeplacementsTotal = 0,nbDeplacements = 0, ligSoko = 0, colSoko = 0;
    bool gagner = false;

    printf("nom fichier: ");
    scanf("%49s", fichier);
    printf("nom fichier deplacements: ");
    scanf("%49s", fichierDep);

    afficher_entete(fichier, nbDeplacements);
    charger_partie(plateau, fichier);
    copier_plateau(plateauInitial, plateau);
    trouver_sokoban(plateau, &ligSoko, &colSoko);
    chargerDeplacements(tableauDep, fichierDep[], &nbDeplacementsTotal)

    for(int i; i < nbDeplacementsTotal; i++)
        int ligAncienne = ligSoko, colAncienne = colSoko;
        system("clear");
        afficher_entete(fichier, nbDeplacements);
        afficher_plateau(plateau);
        deplacer_sokoban(plateau, t[i], &ligSoko, &colSoko, &retourTouche);
        if(ligAncienne != ligSoko || colAncienne != colSoko)
            nbDeplacements++;

    //     // DETERMINATION DES ACTIONS EN FONCTION DE LA TOUCHE PRESSEE
    //     if (touche == SOKOH || touche == SOKOB || touche == SOKOG || touche == SOKOD) {
    //         int ligAncienne = ligSoko, colAncienne = colSoko;
    //         deplacer_sokoban(plateau, touche, &ligSoko, &colSoko, &retourTouche); // GESTION DES DEPLACEMENTS

    //         if (ligAncienne != ligSoko || colAncienne != colSoko) { // ON AUNGMENTE LE NB DE DEPLACEMENT SI SOKOBAN CHANGE DE CASE
    //             nbDeplacements++;
    //             tableauDep[i++] = retourTouche;
    //             if (traiter_victoire(plateau)) { gagner = true; enCours = false; } // VERIFIE SI LA PARTIE EST GAGNEE
    //         }
        
    //     }
    // }
    //affichage de fin de partie
    bool res = traiter_victoire(t_Plateau plateau);



    return 0;
}



/* ==== FONCTIONS ==== */


/* affiche les infos de la partie (en-tête) */
void afficher_entete(char fichier[50], int nbDeplacements) {
    printf("Partie %s \n", fichier);
    printf("Nombre de déplacements : %d\n", nbDeplacements);
}

/* affiche le plateau de jeu */
void afficher_plateau(t_Plateau plateau) {
    for (int lig = 0; lig < TAILLE; lig++) {
        printf("\n");
        for (int col = 0; col < TAILLE; col++) {
            char c = plateau[lig][col];

            /* on remplace les états "sur cible" par les symboles normaux */
            if (c == SOKOBAN_SUR_CIBLE) c = SOKOBAN;
            if (c == CAISSE_SUR_CIBLE) c = CAISSE;

            printf("%c", c);
        }
    }
}

/* enlève le Sokoban d'une case */
char retirer_sokoban(char contenuCase) {
    if (contenuCase == SOKOBAN_SUR_CIBLE) return CIBLE;
    if (contenuCase == SOKOBAN) return VIDE;
    return contenuCase;
}

/* place le Sokoban sur une case */
char afficher_sokoban(char contenuCase) {
    if (contenuCase == CIBLE) return SOKOBAN_SUR_CIBLE;
    if (contenuCase == VIDE) return SOKOBAN;
    return contenuCase;
}

/* enlève une caisse d'une case */
char retirer_caisse(char contenuCase) {
    if (contenuCase == CAISSE_SUR_CIBLE) return CIBLE;
    if (contenuCase == CAISSE) return VIDE;
    return contenuCase;
}

/* place une caisse sur une case */
char afficher_caisse(char contenuCase) {
    if (contenuCase == CIBLE) return CAISSE_SUR_CIBLE;
    if (contenuCase == VIDE) return CAISSE;
    return contenuCase;
}

void deplacement(t_Plateau plateau, int *ligSoko, int *colSoko, char touche) {

    touche = tolower(touche)
    int dlig = 0, dcol = 0;

    switch (touche) {
        case SOKOH:  dlig = -1; break;
        case SOKOB:  dlig = 1; break;
        case SOKOG:  dcol = -1; break;
        case SOKOD:  dcol = 1; break;
        default: return;
    }

    int ligSuivante  = *lig_soko + dlig;
    int colSuivante  = *col_soko + dcol;
    int ligDerriere  = ligSuivante + dlig;
    int colDerriere  = colSuivante + dcol;

    if (ligSuivante < 0 || ligSuivante >= TAILLE || colSuivante < 0 || colSuivante >= TAILLE){
        return;
    }

    char caseSuivante = plateau[ligSuivante][colSuivante];

    if (caseSuivante == MUR){
        return;
    }
    if (caseSuivante == VIDE || caseSuivante == CIBLE) {
        plateau[*lig_soko][*col_soko]   = retirerSokoban(plateau[*lig_soko][*col_soko]);
        plateau[ligSuivante][colSuivante] = afficherSokoban(plateau[ligSuivante][colSuivante]);
        *lig_soko = ligSuivante;
        *col_soko = colSuivante;
        return;
    }
    if (caseSuivante == CAISSE || caseSuivante == CAISSE_SUR_CIBLE) {
        if (ligDerriere < 0 || ligDerriere >= TAILLE || colDerriere < 0 || colDerriere >= TAILLE){
            return;
        }

        char caseDerriere = plateau[ligDerriere][colDerriere];

        if (caseDerriere == MUR || caseDerriere == CAISSE || caseDerriere == CAISSE_SUR_CIBLE){
            return;
        }
        plateau[ligDerriere][colDerriere] = afficherCaisse(caseDerriere);
        plateau[ligSuivante][colSuivante] = retirerCaisse(plateau[ligSuivante][colSuivante]);

        plateau[*lig_soko][*col_soko]   = retirerSokoban(plateau[*lig_soko][*col_soko]);
        plateau[ligSuivante][colSuivante] = afficherSokoban(plateau[ligSuivante][colSuivante]);

        *lig_soko = ligSuivante;
        *col_soko = colSuivante;
        return;
    }
}

/* recherche la position du Sokoban sur le plateau */
void trouver_sokoban(t_Plateau plateau, int *ligSoko, int *colSoko) {
    for (int lig = 0; lig < TAILLE; lig++) {
        for (int col = 0; col < TAILLE; col++) {
            if (plateau[lig][col] == SOKOBAN || plateau[lig][col] == SOKOBAN_SUR_CIBLE) {
                *ligSoko = lig;
                *colSoko = col;
            }
        }
    }
}

/* teste si la partie est gagnée (toutes les cibles ont une caisse) */
bool traiter_victoire(t_Plateau plateau) {
    for (int lig = 0; lig < TAILLE; lig++) {
        for (int col = 0; col < TAILLE; col++) {
            if (plateau[lig][col] == CIBLE || plateau[lig][col] == SOKOBAN_SUR_CIBLE) {
                return false;
            }
        }
    }
    return true;
}


/* affiche l'écran final de victoire avec le plateau */
void afficher_resultat(t_Plateau plateau, char fichier[50], char fichierDep[], int nb, t_tabDeplacement t, bool r) {

    system("clear");
    afficher_entete(fichier, nb);
    afficher_plateau(plateau);

    if(r){
        printf("La suite de déplacements %s est bien une solution pour la partie %s.\n
            Elle contient %d déplacements.\n", fichierDep, fichier, nb);
    }
    else{
        La suite de déplacements <fichier.dep> N’EST PAS une solution pour
        la partie <fichier.sok>.
    }

}

void chargerPartie(typePlateau plateau, char fichier[]){
    FILE * f;
    char finDeLigne;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("ERREUR SUR FICHIER");
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

void chargerDeplacements(typeDeplacements t, char fichier[], int * nb){
    FILE * f;
    char dep;
    *nb = 0;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("FICHIER NON TROUVE\n");
    } else {
        fread(&dep, sizeof(char), 1, f);
        if (feof(f)){
            printf("FICHIER VIDE\n");
        } else {
            while (!feof(f)){
                t[*nb] = dep;
                (*nb)++;
                fread(&dep, sizeof(char), 1, f);
            }
        }
    }
    fclose(f);
}