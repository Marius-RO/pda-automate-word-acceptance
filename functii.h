#ifndef FUNCTII_H_INCLUDED
#define FUNCTII_H_INCLUDED

#define MOD "nDEBUG"


//Functii Automat

int Automat::indiceElementAlfabetSigma(char element){

    int indiceElement = 0;

    for (indiceElement = 0; indiceElement < alfabetSigma.size(); indiceElement++ )
                                         if ( alfabetSigma[indiceElement] == element )
                                                                         return indiceElement;

    return -1; //elementul nu exista in alfabet

}

int Automat::indiceElementAlfabetStiva(char element){

    int indiceElement = 0;

    for (indiceElement = 0; indiceElement < alfabetStiva.size(); indiceElement++ )
                                         if ( alfabetStiva[indiceElement] == element )
                                                                         return indiceElement;

    return -1; //elementul nu exista in alfabet

}

int Automat::indiceStare(int stare){

    int indiceStare = 0;

    for (indiceStare = 0; indiceStare < stari.size(); indiceStare++ )
                                         if ( stari[indiceStare] == stare )
                                                                         return indiceStare;

}

bool Automat::validareStareFinala(int stareDeValidat){

    for ( int i = 0; i < nrStariFinale; i++)
        if ( stareDeValidat == stariFinale[i] )
                        return true;

    return false;

}


//-------------------- Functii PDA ------------------

int PDA::turStari[DIM_TUR_LAMBDA] = {0};
int PDA::pozitieCurenta = 0;

PDA::PDA(ifstream &fin){ //citirea datelor din fisier

    reprezentarePDA = NULL;

    //citire stari
    fin >> nrStari;

    for ( int i = 0; i < nrStari; i++){
                              int x;  fin >> x;  stari.push_back(x);
    }


    //citire elemente alfabet Sigma
    fin >> nrElementeAlfabetSigma;

    nrElementeAlfabetSigma++; //pt LAMBDA

    for ( int i = 1; i <= nrElementeAlfabetSigma - 1; i++){
                         char x;  fin >> x;  alfabetSigma.push_back(x);
    }

    alfabetSigma.push_back(LAMBDA);

    //citire elemente alfabet Stiva
    fin >> nrElementeAlfabetStiva;

    nrElementeAlfabetStiva++; //pt LAMBDA

    for ( int i = 1; i <= nrElementeAlfabetStiva - 1; i++){
                         char x;  fin >> x;  alfabetStiva.push_back(x);
    }

    alfabetStiva.push_back(LAMBDA);
    nrElementeAlfabetStiva++; //pt Simbolul $
    alfabetStiva.push_back(SIMBOL_FINAL_STIVA);

    // stare intiala
    fin >> stareInitiala;

    for ( int i = 0; i < nrStari; i++)
            if ( stari[i] == stareInitiala ){
                                indiceStareInitiala = i;
                                break;
            }

    //stari finale
    fin >> nrStariFinale;

    for ( int i = 1; i <= nrStariFinale; i++){

                         int x;

                         fin >> x;

                         stariFinale.push_back(x);

    }

    //citire tranzitii
    fin >> nrTranzitii;

    reprezentarePDA = new StructuraTranzitii*[nrStari];

    if (!reprezentarePDA){
        cout << "\nNu a reusit alocarea dinamica pt. reprezentarePDA !\n";
        exit(-1);
    }

    for (int i = 0; i < nrStari; i++) {

        reprezentarePDA[i] = NULL;

        reprezentarePDA[i] = new StructuraTranzitii[nrElementeAlfabetSigma];

        if ( !reprezentarePDA[i] ){
                cout << "\nNu a reusit alocarea dinamica pt. reprezentarePDA[i] cu i = "<< i <<" !\n";
                exit(-1);
        }
    }

    //initializarea automatului ( initial automatul nu are nici o tranzitie )
    for ( int i = 0; i < nrStari; i++)
        for (int j = 0; j < nrElementeAlfabetSigma; j++)
               reprezentarePDA[i][j].setLipsaTranzitie();

    Tranzitii T;

    //crearea automatului pe baza tranzitiilor
    for ( int k = 1; k <= nrTranzitii; k++){

        int stareI, stareJ , indiceLiteraSigma = 0;
        char literaCitita, elementPopStiva;
        string pushStiva;
        char delimitator;

        fin >> stareI >> stareJ >> literaCitita >> delimitator >> elementPopStiva >> delimitator >> pushStiva;

        for (indiceLiteraSigma = 0; indiceLiteraSigma < alfabetSigma.size(); indiceLiteraSigma++ )
                                         if ( alfabetSigma[indiceLiteraSigma] == literaCitita )
                                                                         break;

        int indiceStareI = indiceStare(stareI), indiceStareJ = indiceStare(stareJ);

        T.setSimbolCitit(literaCitita);
        T.setElementPop(elementPopStiva);
        T.setElementPush(pushStiva);
        T.setStareFinala(indiceStareJ);

        if ( !reprezentarePDA[indiceStareI][indiceLiteraSigma].getExistaTranzitie() ){
                    reprezentarePDA[indiceStareI][indiceLiteraSigma].setExistaTranzitie();
                    reprezentarePDA[indiceStareI][indiceLiteraSigma].push(T);
        }

        else
            reprezentarePDA[indiceStareI][indiceLiteraSigma].push(T);

    }

    //intializare stiva
    stivaPDA.push(SIMBOL_FINAL_STIVA);

}

PDA::~PDA(){

    for ( int i = 0; i < nrStari; i++)
           delete[] reprezentarePDA[i];

    delete[] reprezentarePDA;

}

stack <char> PDA::prelucrareTranzitie(char elementPop, string pushStiva, stack <char> stivaPDA_Local, bool &prelucrareValida){
    //verfic validitatea unei tranzitii

    if ( MOD == "DEBUG" )
            cout << "\nelementPop = " << elementPop << "\nstring = " << pushStiva << "\n";

    int indiceElement = indiceElementAlfabetStiva(elementPop);

    if ( indiceElement == -1 ){ //elementul nu exista in alfabetul stivei
                    incompatibilitateCuvant = true;
                    prelucrareValida = false;
                    return stivaPDA_Local;
    }

    if ( !stivaPDA_Local.empty() ){ //daca am elemente pe stiva

                if ( elementPop == LAMBDA && pushStiva.back() == SIMBOL_FINAL_STIVA ){ //daca am deja un singur element in stiva acela nu poate fi decat $
                        prelucrareValida = false;                                  // adica simbolul pt finalul stivei --> asta inseamna ca nu mai pot
                        return stivaPDA_Local;                                     // adauga inca un astfel de simbol daca nu il scot pe cel curent
                }

                if ( elementPop == SIMBOL_FINAL_STIVA && pushStiva[0] == LAMBDA && stivaPDA_Local.size() == 1){
                                            prelucrareValida = true;                //daca trebuie sa scot elementul de final de stiva si sa nu mai adaug nimic
                                            stivaPDA_Local.pop();                   //( cazul particular pentru tranzitia  $/.  )           
                                            return stivaPDA_Local;

                }

                if ( elementPop == SIMBOL_FINAL_STIVA && stivaPDA_Local.size() > 1 ){//daca am doua sau mai multe elemente in stiva inseamna ca finalul stivei a fost marcat
                                        prelucrareValida = false;                   // deci nu pot elimina elementul care marcheaza finalul stivei
                                        return stivaPDA_Local;

                }

                if ( elementPop == LAMBDA && pushStiva[0] == LAMBDA ){ //daca am LAMBDA/LAMBDA , nu adaug nimic pe stiva si nu extrag nimic
                            prelucrareValida = true;
                            return stivaPDA_Local;
                }

                if ( elementPop == LAMBDA ){ //daca am LAMBDA/string -- nu extrag nimic dar adaug stringul de la dreapta -> stanga ( ex: abc --> adaug c , b , a)
                        for ( int i = pushStiva.size() - 1; i >= 0;  i--)
                                        stivaPDA_Local.push(pushStiva[i]);

                        prelucrareValida = true;
                        return stivaPDA_Local;

                }

                 if ( stivaPDA_Local.top() == elementPop ) //extrag elementulPop daca este acelasi cu stiva top
                                                stivaPDA_Local.pop();

                 else {
                    //daca ce e in stiva.top() nu corespunde cu ce trebuie scos nu execut tranzitia
                    prelucrareValida = false;
                    return stivaPDA_Local;
                 }

                 if ( pushStiva[0] == LAMBDA ){ //in caz ca stringul de adaugat este LAMBDA , nu mai adaug nimic pe stiva
                                    prelucrareValida = true;
                                    return stivaPDA_Local;
                }

                for ( int i = pushStiva.size() - 1; i >= 0; i--) //altfel adaug stringul;
                            stivaPDA_Local.push(pushStiva[i]);

                prelucrareValida = true;
                return stivaPDA_Local;

    }
    else { //daca stiva este goala

        if ( elementPop == LAMBDA && pushStiva.back() == SIMBOL_FINAL_STIVA ){ //adaug doar daca am tranzitie ./$ sau ./srtring $ ( se prespune este continut doar un singur element $ )
               for ( int i = pushStiva.size() - 1; i >= 0; i--) //adaug stringul
                           stivaPDA_Local.push(pushStiva[i]);

                prelucrareValida = true;
                return stivaPDA_Local;
        }

         prelucrareValida = false;
         return stivaPDA_Local;

    }

} //verific daca o tranzitie este valida

bool PDA::verificaCuvant(int indiceStareCurenta, int pozitie, stack <char> stivaPDA_Local){

    if ( !incompatibilitateCuvant && !cuvantValid) {

                if ( pozitie < cuvant.size() ) {
                    
                            char element = cuvant[pozitie];
                            bool validarePrelucrare = false;

                            //daca am cuvantul vid il accept doar daca starea intiala este si stare finala
                            if ( element == LAMBDA && cuvant.size() == 1 && validareStareFinala(stari[indiceStareCurenta]) ){ 
                                            cuvantValid = true;
                                            return true;
                            }

                            int indiceElement = indiceElementAlfabetSigma(element);

                            if (indiceElement == -1){ //elementul nu exista in alfabet

                                        incompatibilitateCuvant = true;

                                        return false;
                            }

                            stack <char> auxPDA = stivaPDA_Local;
          
                            //daca exista tranzitii lambda din starea curenta
                            if ( reprezentarePDA[indiceStareCurenta][nrElementeAlfabetSigma-1].getExistaTranzitie() ){

                                        //verific fiecare lambda tranzitie
                                        for (int j = 0; j < reprezentarePDA[indiceStareCurenta][nrElementeAlfabetSigma-1].getSize(); j++){

                                                    if ( MOD == "DEBUG" )
                                                            cout << "\nPrelucrare LAMBDA\n j = " << j << "\n\nInainte\n\nStiva Size: " <<stivaPDA_Local.size();

                                                    Tranzitii T = reprezentarePDA[indiceStareCurenta][nrElementeAlfabetSigma-1].getElement(j);

                                                    // daca am trazitie  x x ., ./.   din starea X in aceeasi stare X o sar pt ca nu are sens executia ei
                                                    if ( T.getStareFinala() == indiceStareCurenta && T.getElementString(0) == LAMBDA && T.getSimbolCitit() == LAMBDA && T.getElementPop() == LAMBDA )
                                                                                                    continue;

                                                    validarePrelucrare = false;
                                                    stivaPDA_Local = prelucrareTranzitie(T.getElementPop(),T.getElementPush(),stivaPDA_Local,validarePrelucrare);

                                                    //marchez trecerea prin aceeasi stare doar daca am tranzitie de tipul ., ./. ( LAMBDA peste tot )
                                                    turStari[T.getStareFinala()]++;

                                                    //daca nu trec a doua oara prin aceeasi stare in decursul unui ciclu
                                                    if ( turStari[T.getStareFinala()] < 2 ){ 

                                                                if ( validarePrelucrare ){ //si am o tranzitie valida

                                                                        if ( !stivaPDA_Local.empty() && MOD == "DEBUG" ){ 
                                                                                    cout <<"\n---------------------------\nDupa\n";
                                                                                    cout <<"\nStiva Size: " <<stivaPDA_Local.size();
                                                                                    cout <<"\nStiva top: " << stivaPDA_Local.top() << "\n";

                                                                        }
                                                                
                                                                        verificaCuvant(T.getStareFinala(),pozitie,stivaPDA_Local);
                                                                }

                                                    }

                                                    else { 
                                                            
                                                            if ( pozitie > pozitieCurenta){ //daca am citit un element

                                                                    if ( validarePrelucrare ){ //si am tranzitie valida
    
                                                                        resetareTurStari(); //resetez contoarele starilor

                                                                        pozitieCurenta = pozitie;

                                                                        //continui cu tranzitia
                                                                        verificaCuvant(T.getStareFinala(),pozitie,stivaPDA_Local);

                                                                    }
                                                            }
                                                            else{ 
                                                                //daca am un ciclu in care nu am citit nimic inchei apelul recursiv
                                                                return false;
                                                            }
                                                            
                                                    }
                                                            
                                                    if ( MOD == "DEBUG" )
                                                                cout << endl;

                                                    stivaPDA_Local = auxPDA;
                                        }
                                        
                                        resetareTurStari();

                                        //dupa ce verific fiecare lambda tranzitie verific si restul posibilitatilor care nu sunt tranzitii lambda
                                         if ( reprezentarePDA[indiceStareCurenta][indiceElement].getExistaTranzitie() ){
                                                for (int i = 0; i < reprezentarePDA[indiceStareCurenta][indiceElement].getSize(); i++){

                                                            if ( MOD == "DEBUG" ){
                                                                cout << "\n====================\nelement la prelucrat: " << element << "\n\npozitie litera: " << pozitie << "\n----------------------\n\n";
                                                                cout << "i = " << i << "\n\nInainte\n\nStiva Size: " <<stivaPDA_Local.size();
                                                            }

                                                            Tranzitii T = reprezentarePDA[indiceStareCurenta][indiceElement].getElement(i);

                                                            validarePrelucrare = false;
                                                            stivaPDA_Local = prelucrareTranzitie(T.getElementPop(),T.getElementPush(),stivaPDA_Local,validarePrelucrare);

                                                            if ( validarePrelucrare ){

                                                                    if ( !stivaPDA_Local.empty() && MOD == "DEBUG"){ 
                                                                        cout <<"\n---------------------------\nDupa\n";
                                                                        cout <<"\nStiva Size: " <<stivaPDA_Local.size();
                                                                        cout <<"\nStiva top: " << stivaPDA_Local.top() << "\n";

                                                                    }
                                                                

                                                                    verificaCuvant(T.getStareFinala(),pozitie+1,stivaPDA_Local);
                                                            }

                                                            if ( MOD == "DEBUG" )
                                                                        cout << endl;

                                                            stivaPDA_Local = auxPDA;
                                                 
                                                 }
                                         }

                                          pozitieCurenta = 0; //resetarea la revenirea din recursivitate pentru parcurgerea corecta a posibilitatilor de tranzitie

                             }

                             else { //nu exista lambda tranzitie

                                        if ( reprezentarePDA[indiceStareCurenta][indiceElement].getExistaTranzitie() )
                                            for (int i = 0; i < reprezentarePDA[indiceStareCurenta][indiceElement].getSize(); i++){

                                                    if ( MOD == "DEBUG" ){
                                                            cout << "\n====================\nelement la prelucrat: " << element << "\n\npozitie litera: " << pozitie << "\n----------------------\n\n";
                                                            cout << "i = " << i << "\n\nInainte\n\nStiva Size: " <<stivaPDA_Local.size();
                                                    }


                                                    Tranzitii T = reprezentarePDA[indiceStareCurenta][indiceElement].getElement(i);


                                                    validarePrelucrare = false;
                                                    stivaPDA_Local = prelucrareTranzitie(T.getElementPop(),T.getElementPush(),stivaPDA_Local,validarePrelucrare);


                                                    if ( validarePrelucrare ){

                                                        if ( !stivaPDA_Local.empty() && MOD == "DEBUG"){ 
                                                                            cout <<"\n---------------------------\nDupa\n";
                                                                            cout <<"\nStiva Size: " <<stivaPDA_Local.size();
                                                                            cout <<"\nStiva top: " << stivaPDA_Local.top() << "\n";

                                                            }
                                                      
                                                
                                                        verificaCuvant(T.getStareFinala(),pozitie+1,stivaPDA_Local);
                                                    }

                                                    if ( MOD == "DEBUG" )
                                                                cout << endl;

                                                    stivaPDA_Local = auxPDA;
                                            }
                                        }

                }

             else { 
                 
                     if ( MOD == "DEBUG" )
                            cout <<"\nCuvant citit !\n";

                    if ( validareStareFinala(stari[indiceStareCurenta]) || stivaPDA_Local.empty() ){ //daca am ajuns in stare finala sau stiva este goala
                                    
                                    if ( MOD == "DEBUG" )
                                                    cout <<"\nCuvant valid !\n";

                                        cuvantValid = true;

                                        //exit(-1);

                                        return true;
                    }

                    else { //verific daca exista tranzitie/tranzitii lambda din starea curenta catre starea finala

                              if ( reprezentarePDA[indiceStareCurenta][nrElementeAlfabetSigma-1].getExistaTranzitie() ){

                                        stack <char> auxPDA = stivaPDA_Local;

                                        //verific fiecare lambda tranzitie
                                        for (int j = 0; j < reprezentarePDA[indiceStareCurenta][nrElementeAlfabetSigma-1].getSize(); j++){

                                            Tranzitii T = reprezentarePDA[indiceStareCurenta][nrElementeAlfabetSigma-1].getElement(j);

                                            // daca am trazitie  x x ., ./.   din starea X in aceeasi stare X o sar pt ca nu are sens executia ei
                                            if ( T.getStareFinala() == indiceStareCurenta && T.getElementString(0) == LAMBDA && T.getSimbolCitit() == LAMBDA && T.getElementPop() == LAMBDA )
                                                                                                    continue;

                                            bool validarePrelucrare = false;
                                            stivaPDA_Local = prelucrareTranzitie(T.getElementPop(),T.getElementPush(),stivaPDA_Local,validarePrelucrare);

                                            //marchez trecerea prin aceeasi stare doar daca am tranzitie de tipul ., ./. ( LAMBDA peste tot )
                                            turStari[T.getStareFinala()]++;

                                            if ( turStari[T.getStareFinala()] < 2 ){

                                                        if ( validarePrelucrare ){

                                                            if ( !stivaPDA_Local.empty() && MOD == "DEBUG"){ 
                                                                        cout <<"\n------\nStari lambda spre starea finala\n";
                                                                        cout <<"\n---------------------------\nDupa\n";
                                                                        cout <<"\nStiva Size: " <<stivaPDA_Local.size();
                                                                        cout <<"\nStiva top: " << stivaPDA_Local.top() << "\n";

                                                            }

                                                            verificaCuvant(T.getStareFinala(),pozitie,stivaPDA_Local);
                                                        }
                                            }
                                            else{
                                                turStari[indiceStareCurenta] = 0;
                                                return false;
                                            }


                                             if ( MOD == "DEBUG" )
                                                        cout << endl;

                                            stivaPDA_Local = auxPDA;
                                     }
                              }

                              return false; //nu a existat stare lambda din starea curenta
                    }

            }
    }

    return false;

}


//------------------------- Functii friend -------------------------

ostream &operator<<(ostream &OUT, PDA &A){

    OUT << "\nnrStari = " << A.nrStari << "\nStari: ";

    for (int i = 0; i < A.stari.size(); i++)
        OUT << A.stari[i] << " ";

    OUT << "\n\nnrElementeAlfabetSigma = " << A.nrElementeAlfabetSigma << "\nAlfabet: ";

    for (int i = 0; i < A.alfabetSigma.size(); i++)
        OUT << A.alfabetSigma[i] << " ";

    OUT << "\n\nnrElementeAlfabetStiva = " << A.nrElementeAlfabetStiva << "\nAlfabet: ";

    for (int i = 0; i < A.alfabetStiva.size(); i++)
        OUT << A.alfabetStiva[i] << " ";


    OUT << "\n\nstareInitiala = " << A.stareInitiala;

    OUT << "\n\nIndice stareInitiala = " << A.indiceStareInitiala;

    OUT << "\n\nnrStariFinale = " << A.nrStariFinale << "\nStari Finale: ";

    for (int i = 0; i < A.stariFinale.size(); i++)
        OUT << A.stariFinale[i] << " ";

    OUT << "\n\nnrTranzitii = " << A.nrTranzitii << "\n\nReprezentare:\n\n";

    for (int i = 0; i < A.nrStari; i++){
        for ( int j = 0; j < A.nrElementeAlfabetSigma; j++ ){

            OUT << "size [" << i << "]["<<j<<"] = " << A.reprezentarePDA[i][j].getSize() << " cu elementele: ";

            for ( int k = 0; k < A.reprezentarePDA[i][j].getSize(); k++ ){
                         Tranzitii T = A.reprezentarePDA[i][j].getElement(k);
                         OUT << " { ";
                         OUT << "din [" << A.stari[i] << "]->";
                         OUT << "[" << A.stari[T.getStareFinala()] << "] cu:  ";
                         OUT << T.getSimbolCitit() << ", ";
                         OUT << T.getElementPop() << "/";
                         OUT << T.getElementPush() << " ";
                         OUT << " } -- ";
            }

            OUT << "\n";

        }
        OUT << "\n";
    }

    OUT << "\n";

    return OUT;

} //doar pt debug


#endif
