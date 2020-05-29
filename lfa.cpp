// Antonio-Alexandru Ganea // Grupa 135 // Tema LFA2

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <string.h>
#include <string>
#include <set>
#include <queue>
using namespace std;

#define DEBUG_PRINTS

struct Node{
    std::map< char, std::vector<int> > m;
    bool finalState;
    Node(){
        finalState = false;
    }
};

struct Automat{
    int initialState;
    Node* nodes;
    int node_number;
    char* characters;

    Automat(int states){
        nodes = new Node[states+1];
        node_number = states;

        characters = new char[256];
    }

    /*
    ~Automat() {
        //delete nodes;
        //delete characters;
    }
    */
};

bool vectorContains( std::vector<int> myvec, int number ){
    for (std::vector<int>::iterator it = myvec.begin() ; it != myvec.end(); ++it){
        if ( *it == number ){
            return true;
        }
    }
    return false;
}

bool checkIfStatesContainFinishingState( Node nodes[], std::vector<int> myvec ){
    for (std::vector<int>::iterator it = myvec.begin() ; it != myvec.end(); ++it) {
        if ( nodes[*it].finalState ) {
            return true;
        }
    }
    return false;
}

void printVector( std::vector<int> myvec ){
    for (std::vector<int>::iterator it = myvec.begin() ; it != myvec.end(); ++it)
        printf("%d ", *it);
    puts("");
}

char letters[] = "ab";

void passLetter( Automat & automat, std::vector<int> & currentStates, char currentChar ){
    std::vector<int> nextStates;

    for (std::vector<int>::iterator it = currentStates.begin() ; it != currentStates.end(); ++it){ // for all of the current states
        int currentState = *it;

        for( std::vector<int>::iterator it2 = automat.nodes[currentState].m[currentChar].begin(); it2 != automat.nodes[currentState].m[currentChar].end(); ++it2 ){
            // from currentState -> jumpingInto by letter 'currentChar'

            int jumpingInto = *it2;
            nextStates.push_back(jumpingInto);
        }
    }

    // currentStates becomes nextStates
    currentStates.clear();
    currentStates.assign(nextStates.begin(), nextStates.end());
}

void passLambdas( Automat & automat, std::vector<int> & currentStates ){
    char currentChar = 'X';
    std::vector<int> q;
    q.assign(currentStates.begin(),currentStates.end());

    while (!q.empty())
    {
        int currentState = q.back();
        q.pop_back();

        for( std::vector<int>::iterator it2 = automat.nodes[currentState].m[currentChar].begin(); it2 != automat.nodes[currentState].m[currentChar].end(); ++it2 ){
            // from currentState -> jumpingInto by letter 'currentChar'

            int jumpingInto = *it2;
            if ( !vectorContains(currentStates, jumpingInto) ){
                currentStates.push_back(jumpingInto);
                q.push_back(jumpingInto);
            }
        }
    }


}

Automat LambdaNFAtoNFA( Automat & automat ){

    Automat newAuto(automat.node_number);
    newAuto.initialState = automat.initialState;

    for ( int i = 0; i < automat.node_number; i++ ){
        newAuto.nodes[i].finalState = automat.nodes[i].finalState;
        for ( int C = 0; C < strlen(letters); C++ ) { // for currentLetter = letters[C], for every normal letter
            char currentChar = letters[C];

            std::vector<int> currentStates;
            currentStates.push_back(i);

            // for each state in states, for every letter ( except lambda ) do this:
            /// pass lambdas
            /// pass letters
            /// pass lambdas

            //cout << "Testing : ";
            //printVector(currentStates);

            //cout << "Passing lambdas " << endl;
            passLambdas(automat,currentStates);

            //cout << "Passing letter " << currentChar << endl;
            passLetter(automat,currentStates, currentChar);

            //cout << "Passing lambdas " << endl;
            passLambdas(automat,currentStates);

            //cout << "Resulted :";
            //printVector(currentStates);

            //cout << endl;

            // form new automat links
            for (std::vector<int>::iterator it = currentStates.begin() ; it != currentStates.end(); ++it){
                // din i in *it prin currentChar
                newAuto.nodes[i].m[currentChar].push_back(*it);
                printf("%d->%d by %c\n",i,*it,currentChar);
            }
        }

        cout << i << " is final state ? " << automat.nodes[i].finalState << endl;
    }


    return newAuto;
}

long long pow( long long b, int exp ){
    long long res = 1;
    for ( int i = 0; i < exp; i++ ){
        res *= b;
    }
    return res;
}

// Suppose you have at most 64 states
unsigned long long getStatesHash( std::vector<int> states ) {
    unsigned long long hash = 0;

    for (std::vector<int>::iterator it = states.begin() ; it != states.end(); ++it){
        hash |= 1 << *it;
    }

    return hash;
}

unsigned long long getSingleStateHash( int state ){
    unsigned long long hash = 1 << state;
    return hash;
}

void printBits( unsigned long long val ){
    for ( int i = 31; i >= 0; i-- ){
        unsigned long long mask = 1 << i;

        if ( mask & val ){
            cout << "1";
        } else {
            cout << "0";
        }
    }
}

void prettyPrintHash( unsigned long long val ){
    cout << "{ ";
    for ( int i = 0; i < 32; i++ ){
        unsigned long long mask = 1 << i;

        if ( mask & val ){
            cout << i << " ";
        }
    }
    cout << "}";
}


struct StateNumberAllocator{
private:
    int currentIndex;
public:
    //int states[1024];
    std::map< unsigned long long, int > m;
    StateNumberAllocator(){
        currentIndex = 0;
    }

    bool isHashTaken( unsigned long long h ){
        //cout << "Checking if " << h << " is taken" << endl;
        if ( m.find(h) != m.end() ){
            //cout << "Yep" << endl;
            return true;
        }
        else{
            //cout << "Nope" << endl;
            return false;
        }

    }

    int allocateHash(unsigned long long hash) {
        if ( m.find(hash) != m.end() ){
            //cout << "Already there" << endl;
            return m[hash];
        } else {
            m[hash] = currentIndex;
            currentIndex++;
            //cout << "Allocated!!!!!!!!!!!!!!!!!!!!!! for "; printBits(hash); cout << "  " << m[hash] << endl;
            return m[hash];
        }
    }

};

struct Link{
    int from;
    int to;
    char by;
    bool isFinal;
    Link( int _from, int _to, char _by, bool _isFinal){
        from = _from;
        to = _to;
        by = _by;
        isFinal = _isFinal;
    }
};

bool checkIfHashContainsFinalState( Automat & automat, unsigned long long hash ) {
    unsigned long long mask;
    for ( int i = 0; i < 32; i++ ){
        mask = 1 << i;

        if ( mask & hash ){
            if ( automat.nodes[i].finalState ){
                return true;
            }
        }
    }

    return false;
}

struct ModLink{
    unsigned long long from;
    unsigned long long to;
    char by;
    bool isFinal;
    ModLink( unsigned long long _from, unsigned long long _to, char _by, bool _isFinal){
        from = _from;
        to = _to;
        by = _by;
        isFinal = _isFinal;
    }
};

Automat NFAtoDFA( Automat & automat ){

    //Automat newAuto(automat.node_number);

    cout << endl << endl;

    StateNumberAllocator allocator;

    unsigned long long matrix [automat.node_number][strlen(letters)]; // states x letters matrix of hashes

    for ( int i = 0; i < automat.node_number; i++ ){
        for ( int C = 0; C < strlen(letters); C++ ) { // for currentLetter = letters[C], for every normal letter
            char currentChar = letters[C];

            std::vector<int> currentStates;
            currentStates.push_back(i);

            //cout << "Passing letter " << currentChar << " on state " << i << endl;
            passLetter(automat, currentStates, currentChar);
            //printVector(currentStates);
            //cout << endl;

            matrix[i][C] = getStatesHash(currentStates);
            cout << i << " " << letters[C] << " " ;
            printBits(matrix[i][C]);
            cout << endl;
            //<< matrix[i][C] << endl;
        }

        //cout << automat.nodes[i].finalState << " " << endl;
    }

    cout << endl;

    std::vector<unsigned long long> q;
    cout << automat.initialState << endl;
    q.push_back(getSingleStateHash(automat.initialState));
    allocator.allocateHash(getSingleStateHash(automat.initialState));

    StateNumberAllocator cleanAlloc;
    int maxStates = 0;

    std::vector<Link> links;
    std::vector<ModLink> modlinks;

    while (!q.empty())
    {
        unsigned long long currentHash = q.back();
        q.pop_back();

        for ( int C = 0; C < strlen(letters); C++ ) {
            char currentChar = letters[C];

            unsigned long long mask;
            unsigned long long composed = 0;

            #ifndef DEBUG_PRINTS
            cout << "Current hash "; printBits(currentHash); cout << " composes into :" << endl;
            #endif // DEBUG_PRINTS
            for ( int i = 0; i < 32; i++ ){
                mask = 1 << i;

                if ( mask & currentHash ){
                    #ifndef DEBUG_PRINTS
                    cout << "added " << i << " " << letters[C] << " "; printBits(matrix[i][C]); cout << endl;
                    #endif // DEBUG_PRINTS
                    composed |= matrix[i][C];
                }
            }
            #ifndef DEBUG_PRINTS
            cout << "composed : "; printBits(composed); cout << endl;
            #endif // DEBUG_PRINTS

            cout << ">> by letter '" << letters[C] << "' you can go from ";prettyPrintHash(currentHash);
            cout << " to "; prettyPrintHash(composed); cout << endl << endl;



            if ( composed != 0  ){
                    //cout << "entered" << endl;

                if ( !allocator.isHashTaken(composed) ){
                    q.push_back(composed);
                }

                int allocatedNumber = allocator.allocateHash(composed);

                /*
                if ( allocator.isHashTaken(composed)){
                    cout << "DEbug" << allocator.allocateHash(composed) << endl;
                }
                */
                //cout << "Pushed back " << composed << endl;

                bool isFinalState = checkIfHashContainsFinalState(automat, composed);
                //cout << "R::Current hash " << currentHash << " jumps -> " << composed << " by letter " << currentChar << " (isfinal? " << isFinalState << ")" << endl;

                int fromNode = cleanAlloc.allocateHash(currentHash);
                int toNode = cleanAlloc.allocateHash(composed);

                if ( fromNode > maxStates )
                    maxStates = fromNode;
                if ( toNode > maxStates )
                    maxStates = toNode;
                //cout << "T::Current hash " << fromNode << " jumps -> " << toNode << " by letter " << currentChar << endl;

                Link newLink(fromNode, toNode, currentChar, isFinalState);
                                                    //cout << "Pushed link " << fromNode << " " << toNode << " by " << currentChar << endl;
                links.push_back(newLink);

                ModLink newmodlink(currentHash,composed,currentChar,isFinalState);
                modlinks.push_back(newmodlink);
            }
        }
    }

    Automat newAuto(maxStates);

    newAuto.initialState = automat.initialState;
    //newAuto.nodes[i].finalState = automat.nodes[i].finalState;

    cout << "Constructing new automata with " << maxStates << " states" << endl;

    /*
    for (std::vector<Link>::iterator it = links.begin() ; it != links.end(); ++it){
        newAuto.nodes[it->from].m[it->by].push_back(it->to);
        //cout << it->from << " -> " << it->to << " by " << it->by << endl;

        //cout << it->to << " is a final state ? " << it->isFinal << endl;
        newAuto.nodes[it->to].finalState = it->isFinal;
    }
    */
    for (std::vector<ModLink>::iterator it = modlinks.begin() ; it != modlinks.end(); ++it){

        int from = allocator.allocateHash(it->from);
        int to = allocator.allocateHash(it->to);

        prettyPrintHash(it->from); cout << "(" << from << ") goes into "; prettyPrintHash(it->to); cout << "(" << to << ") by letter '" << it->by << "'" << endl;

        //newAuto.nodes[it->from].m[it->by].push_back(it->to);
        //cout << it->from << " -> " << it->to << " by " << it->by << endl;

        //cout << it->to << " is a final state ? " << it->isFinal << endl;
        //newAuto.nodes[it->to].finalState = it->isFinal;
    }

    return newAuto;
}

FILE * fin;
Automat readAutomat(){
    fin = fopen( "input.txt", "r" );

    int states, transitions;
    fscanf(fin,"%d%d",&states,&transitions);

    Automat automat(states);

    for ( int i = 0; i < transitions; i++ ){
        int a,b;
        char temp[100];
        fscanf(fin,"%d%d%s",&a,&b,&temp);
        char c = temp[0];

        // din a in b prin c
        automat.nodes[a].m[c].push_back(b);
    }

    int initialState;

    fscanf(fin,"%d",&initialState);
    automat.initialState = initialState;

    int finalStates;

    fscanf(fin,"%d", &finalStates);

    for ( int i = 0; i < finalStates; i++ ){
        int temp;
        fscanf(fin,"%d",&temp);
        automat.nodes[temp].finalState = true;
    }

    //fclose(fin);

    return automat;
}

int main() {
    Automat myAuto = readAutomat();

/*
    cout << myAuto.node_number << endl;
    for ( int i = 0; i < myAuto.node_number; i++ ){
        cout << myAuto.nodes[i].finalState << " " << endl;
    }
*/

    Automat NFAAuto = LambdaNFAtoNFA(myAuto);

    Automat DFAAuto = NFAtoDFA(NFAAuto);

/*
    std::vector<int> states;

    states.push_back(0);
    cout << getStateHash(states) << endl;
    states.push_back(2);
    cout << getStateHash(states) << endl;
    states.push_back(1);
    cout << getStateHash(states) << endl;
*/

    return 0;
}
