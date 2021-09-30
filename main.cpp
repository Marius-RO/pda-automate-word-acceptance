#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <stack>
#include "clase.h"
#include "functii.h"

/*

    Mod de citire din fisier:

        nr stari
        starile

        nr elemente alfabet sigma
        elementele

        nr elemente alfabet stiva
        elementele

        starea initiala

        nr stari finale
        starile finale

        nr tranzitii
        tranzitiile care sunt de forma ( s d i, j/X ) -- unde:
                                                             s = starea din care se pleaca
                                                             d = starea in care se pleaca
                                                             i = simbolul care se citeste
                                                             j = stimbolul la care se da pop de pe stiva
                                                             X - stringul la care se da push pe stiva
                                                             , si / sunt delimitatoare

        nr cuvinte
        cuvintele


    Obs: Pentru citirea tranzitiilor cu lambda se foloseste . ca reprezentant pt cuvantul vid
                    ==> 1 2 ., $/$ ( exista tranzitie lambda de 1 la 2 , unde $ reprezinta finalul stivei )

*/


int main(){

    ifstream fin;

    fin.open("date.in");


    if ( !fin ){
        cout << "\nEroare la deschiderea fisierului !\n";
        exit(-1);
    }

    PDA P(fin);
    //cout << P;

    int nrCuvinte = 0;
    string cuvant;

    fin >> nrCuvinte;
    fin.get(); //citirea elementului de rand nou de dupa nrCuvinte pentru a putea trece efectiv la citirea cuvintelor

    for (int i = 1; i <= nrCuvinte; i++ ){

            fin >> cuvant;

            cout << "\nCuvant: " << cuvant << "\nRezultat: ";

            P.setCuvant(cuvant);

            P.setCuvantValid();

            P.setStiva();

            P.resetarePozitieCurenta();

            P.resetareTurStari();

            P.verificaCuvant(P.getIndiceStareInitiala(),0,P.getStivaPDA());

            if ( P.getCuvantValid() )
                        cout << "DA\n";
            else
                        cout << "NU\n";

            P.unsetCuvant();

    }

    return 0;
}
