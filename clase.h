#ifndef CLASE_H_INCLUDED
#define CLASE_H_INCLUDED

#define SIMBOL_FINAL_STIVA '$'
#define LAMBDA '.' //conventia pentru notatia cuvantului vid
#define DIM_TUR_LAMBDA 1000 //am luat un vector de elemente statice pt a verifica ciclurile infinite din Lambda NFA

using namespace std;

class Automat{

    protected:

        int nrStari, nrElementeAlfabetSigma, nrElementeAlfabetStiva, stareInitiala, nrStariFinale , nrTranzitii;
        int indiceStareInitiala;
        vector <int> stari, stariFinale;
        string alfabetSigma, alfabetStiva, cuvant;
        bool cuvantValid; //cuvantul este acceptat sau nu
        bool incompatibilitateCuvant; //elementul nu apartine alfabetului

    public:

        int indiceElementAlfabetSigma(char element);
        int indiceElementAlfabetStiva(char element);
        int indiceStare(int stare);
        bool validareStareFinala(int stareDeValidat);
        void setCuvant(string X) { cuvant = X; }
        void unsetCuvant() { cuvant.clear(); }
        void setCuvantValid(){ cuvantValid = false; incompatibilitateCuvant = false; }
        bool getCuvantValid(){ return cuvantValid; }
        int getIndiceStareInitiala(){ return indiceStareInitiala; }

};

class Tranzitii{ //componenta unei tranzitii

    char simbolCitit; //simbolul pentru elementul prelucrat
    char elementPop; //elementul care va fi eliminat din stiva
    string elementPush; //sirul care va fi adaugat pe stiva
    int stareFinala; //starea in care merg cu tranzitia

    public:
        void setSimbolCitit(char X){ simbolCitit = X; }
        void setElementPop(char X){ elementPop = X; }
        void setElementPush(string X){ elementPush = X; }
        void setStareFinala(int X){ stareFinala = X; }
        char getSimbolCitit(){return simbolCitit; }
        char getElementString(int poz){return elementPush[poz]; }
        char getElementPop(){ return elementPop; }
        int getSizeElementPush(){ return elementPush.size(); }
        string getElementPush(){ return elementPush; }
        int getStareFinala(){ return stareFinala; }

};

class StructuraTranzitii{ //listele de tranzitii pentru PDA

    vector <Tranzitii> listaPosibilitati;
    bool existaTranzitie;

    public:

        void push(Tranzitii element) { listaPosibilitati.push_back(element); }
        void setLipsaTranzitie() { existaTranzitie = false; }
        void setExistaTranzitie() { existaTranzitie = true; }
        bool getExistaTranzitie() { return existaTranzitie; }
        int getSize() { return listaPosibilitati.size(); }
        Tranzitii getElement(int i) { return listaPosibilitati[i]; }
        int getStareDestinatie(int i) { return listaPosibilitati[i].getStareFinala(); }

};

class PDA:public Automat{

    StructuraTranzitii **reprezentarePDA;
    stack <char> stivaPDA;
    static int turStari[DIM_TUR_LAMBDA];
    static int pozitieCurenta;

    public:

        PDA(ifstream &fin);
        ~PDA();

        stack <char> prelucrareTranzitie(char elementPop, string pushStiva, stack <char> stivaPDA_Local, bool &prelucrareValida);
        bool verificaCuvant(int indiceStareCurenta, int pozitie, stack <char> stivaPDA_Local);
        stack <char> getStivaPDA(){return stivaPDA; }
        void resetareTurStari(){ for ( int i = 0; i < DIM_TUR_LAMBDA; i++) turStari[i] = 0; }
        void resetarePozitieCurenta() { pozitieCurenta = 0; }
        void setStiva() { while ( !stivaPDA.empty() ) stivaPDA.pop();    stivaPDA.push(SIMBOL_FINAL_STIVA); }

        friend ostream &operator<<(ostream &OUT, PDA &A);

};

#endif
