#include <iostream>
#include <string>
#include <locale>
#include <fstream>
#include <cstring>

#include "notes.h"
#include "signals.h"
#include "color.h"

#define DB_VERSION 2
#define APP_VERSION 0.52

using namespace std;
using namespace Color;

// forward declarations
void handleInput(const string&); // process user input
string prompt(); // prompt the user without a header
string prompt(const string&); // prompt the user with a specified header
string toLower(const string&); // convert a string to lower case
signal strToSignal(const string&); // convert a string to its signal enum
bool areEqual(const string&, const string&); // compare two strings and return boolean
bool areEqual(const char*, const char*); // compare two cstrings - TODO refactor prior calls to use this !! TODO
void printHelp(); // print the help menu to stdout
void saveDB(bool); // save the db to a file - bool(true) indicates silent mode
void loadDB(bool); // load the db from a file - bool(true) indicates silent mode
void printCliHelp(); // print the cli help menu

notes* n; // main notes object
notes* a; // archived notes object

// setup default db location string
string dbFile = string(getenv("HOME")) + "/._note++.db";

// indicator for determining whether or not to write to db file
bool changesMade = false;

// console color flags - send to stdout to change all text following
Modifier red(FG_RED); // red
Modifier blue(FG_BLUE); // blue
Modifier green(FG_GREEN); // green
Modifier def(FG_DEFAULT); // reset to default console color

int main(int argc, char** argv){
    bool goSilent = false;
    if(argc > 1){
        if(strcmp(argv[1], "-db") == 0){
            dbFile = string(argv[2]);
            if(argc > 3){
                goSilent = true;
            }
        } else {
            goSilent = true;
        }
    }

    if(!goSilent) {
        cout << "Notes++ " << APP_VERSION << endl;
        cout << "By: nickg047@sdf.org" << endl;
        cout << "====================" << endl << endl;
    }
    
    n = new notes();
    a = new notes();
    loadDB(goSilent);
    
    if(argc > 1){
        for(int i = 1; i < argc; i++){
            if(strcmp(argv[i],"-p") == 0){
                cout << "NPP: You have " << red << n->getIncompleteCount() << def << " pending tasks" << endl; 
            } else if (strcmp(argv[i], "-n") == 0){
                changesMade = true;
                i++;
                if(i == argc){
                    handleInput("new");
                } else {
                    string newNoteStr = "";
                    for(int j = i; j < argc; j++){
                        newNoteStr += string(argv[j]);
                        if(j+1 < argc){
                            newNoteStr += " ";
                        }
                    }
                    note* newNote = n->newNote(newNoteStr);
                    cout << blue << "Added" << def << ": " << n->getCount() << ". " << newNote->getText() << endl;
                }
                break;
            } else if (strcmp(argv[i], "-ci") == 0){
                cout << "NPP: You have " << blue << n->getImportantCount() << def << " pending important tasks" << endl;
            } else if (strcmp(argv[i], "-bulk") == 0){
                string line = "";
                changesMade = true;
                while(true) {
                    line = prompt();
                    if(line.compare("") == 0){
                        break;
                    } else {
                        n->newNote(line);
                    }
                }
            } else if (strcmp(argv[i], "-v") == 0){
                cout << "Notes++ Version " << APP_VERSION << endl;
                cout << "By: nickg047@sdf.org" << endl;
            } else if (strcmp(argv[i], "-l") == 0){
                handleInput("list");
            } else if (strcmp(argv[i], "-h") == 0){
                printCliHelp();
            } else if (strcmp(argv[i], "-c") == 0){
                i++;
                changesMade = true;
                int target = stoi(string(argv[i]));
                n->getNote(target-1)->markComplete();
                //n->getNotes()[target-1]->markComplete();
                cout << green << "Completed" << def << ": " << target << ". " << n->getNote(target-1)->getText() << endl;
            } else if (areEqual(argv[i], "-i")) {
                i++;
                changesMade = true;
                int target = stoi(string(argv[i]));
                n->getNote(target-1)->isPriority() ? n->getNote(target-1)->deprioritize() : n->getNote(target-1)->prioritize();
                cout << blue << "Important" << def << ": " << target << ". " << n->getNote(target-1)->getText() << endl;
            } else if (strcmp(argv[i], "-la") == 0){
                handleInput("la");
            } else if (strcmp(argv[i], "-ln") == 0){
                handleInput("ls -a");
            } else if (strcmp(argv[i], "-a") == 0){
                handleInput("archive");
            }
        }
    }

    if(!goSilent) {
        string userInput = "";
        while (userInput != "quit"){
            userInput = toLower(prompt());
            handleInput(userInput);
        }
    }

    if(changesMade) saveDB(goSilent);
    if(!goSilent) cout << "Goodbye..." << endl;

    delete n;
    delete a;
    return 0;
}

void printCliHelp(){
    cout << "Notes++ " << APP_VERSION << endl;
    cout << "By: nickg047@sdf.org" << endl;
    cout << "====================" << endl;
    cout << endl;
    cout << "-h         : print help" << endl;
    cout << "-n [note]  : new note" << endl;
    cout << "-c [index] : mark the note at [index] as complete" << endl;
    cout << "-i [index] : mark the note at [index] as impotant" << endl;
    cout << "-p         : print number of pending tasks" << endl;
    cout << "-l         : list notes" << endl;
    cout << "-la        : list archived notes" << endl;
    cout << "-ln        : list all notes [including archived notes]" << endl;
    cout << "-db        : specify the database location" << endl;
    cout << "-bulk      : create multiple entries" << endl;
    cout << "-a         : archive completed tasks" << endl;
    cout << "-ci        : print the number of important tasks" << endl; 
    cout << endl;
}

void saveDB(bool silent){
    
    ofstream db(dbFile);
    if(db.is_open()){
        // header stuff
        // first preamble
        db << "NPP-DB-HEAD" << endl;
        // save version used to write db
        db << "V:" << DB_VERSION << endl;
        // save count
        db << "C:" << n->getCount() + a->getCount() << endl;
        // end head
        db << "BEGIN" << endl;

        // save each note as [STATUS_ID]:[NOTE TEXT]\n
        vector<note*> noteVec = n->getNotes();
        vector<note*> arcVec = a->getNotes();
        noteVec.insert(noteVec.end(), arcVec.begin(), arcVec.end());
        int count = 0;
        for(auto it = noteVec.begin(); it != noteVec.end(); it++){
            note* currentNote = *it;
            bool imp = currentNote->isPriority();
            db << (imp ? "I:" : "N:") << (currentNote->isArchived() ? 2 : (currentNote->isCompleted() ? 1 : 0)) 
                << ":" << currentNote->getText() << endl;
            count++;
        }

        db << "EOF" << endl;
        if(!silent){
            cout << "Saved " << count << " database entries..." << endl;
        }
        db.close();
    } else {
        cout << "Error saving database..." << endl;
    }
}

void loadDB(bool silent){
    ifstream db(dbFile);
    if(db.is_open()){
        // lets collect header data
        string currentLine;
        int count = 0;
        getline(db, currentLine);
        if(currentLine.compare("NPP-DB-HEAD") != 0){
            cout << "Failed to load n++ database: 01_BAD_HEADER" << endl;
            return;
        }

        getline(db, currentLine);
        int db_ver = stoi(currentLine.substr(2, currentLine.length()-2));
        //if(currentLine.substr(2,1).compare(to_string(DB_VERSION)) != 0){
        if(db_ver > DB_VERSION){
            cout << "Failed to load n++ database: 02_DB_VERSION_MISMATCH" << endl;
            cout << "An unsupported database version was detected" << endl;
            return;
        }

        if(db_ver < DB_VERSION){
            cout << "Older database version detected. Current: " << db_ver << " Latest: " << DB_VERSION << endl;
            cout << "Converting to latest version..." << endl;
            changesMade = true;
        }

        getline(db, currentLine);
        int toRead = stoi(currentLine.substr(2, currentLine.length()-2));

        // now get notes until EOF

        getline(db, currentLine);

        if(currentLine.compare("BEGIN") != 0){
            currentLine = "EOF";
            cout << "Error: 04_BAD_DATABASE_HEADER" << endl;
        }

        while(currentLine.compare("EOF") != 0){
            getline(db, currentLine);
            // get status, get note, insert to notes object
            // 1 = complete, 0 = incomplete
            bool imp = false;
            bool arch = false;
            if(currentLine.substr(0,1).compare("N") != 0 && currentLine.substr(0,1).compare("I") != 0){
                break;
            } else {
                if(currentLine.substr(0,1).compare("I") == 0){
                    imp = true;
                }
            }

            int compStat = stoi(currentLine.substr(2,1));
            string noteData = currentLine.substr(4, currentLine.length()-4);
            note* newNote = nullptr;
            if(compStat == 2){
                newNote = a->newNote(noteData);
                newNote->archive();
                newNote->markComplete();
            } else {
                newNote = n->newNote(noteData);
                if(compStat == 1){
                    newNote->markComplete();
                }
            }

            if(imp){
                newNote->prioritize();
            }

            count++;
        }
        if(count != toRead){
            cout << "Error: 03_DATABASE_MISCOUNT" << endl;
        }
        if(!silent) cout << "Loaded " << count << " database entries..." << endl;
        db.close();
    }
   
}

signal strToSignal(const string& str){
    if(areEqual(str, "quit")){
        return QUIT;
    } else if (areEqual(str, "new note") || areEqual(str, "new")){
        return NEW_NOTE;
    } else if (areEqual( str, "delete note") || areEqual(str, "delete")){
        return DELETE_NOTE;
    } else if (areEqual(str, "list") || areEqual(str, "list notes") || areEqual(str, "ls")){
        return LIST_NOTES;
    } else if (areEqual(str, "mark complete") || areEqual(str, "complete")){
        return MARK_COMPLETE;
    } else if (areEqual(str, "mark incomplete") || areEqual(str, "incomplete")){
        return MARK_INCOMPLETE;
    } else if (areEqual(str, "clear list") || areEqual(str, "clear")){
        return CLEAR_LIST;
    } else if (areEqual(str, "clear completed") || areEqual(str, "clean")){
        return CLEAR_COMPLETED;
    } else if (areEqual(str, "help") || areEqual(str, "h")){
        return HELP;
    } else if (areEqual(str, "high priority") || areEqual(str, "high")){
        return MARK_PRIORITY;
    } else if (areEqual(str, "low priority") || areEqual(str, "low")){
        return UNMARK_PRIORITY;
    } else if (areEqual(str, "archive")){
        return ARCHIVE;
    } else if (areEqual(str, "unarchive")){
        return UNARCHIVE;
    } else if (areEqual(str, "list archive") || areEqual(str, "la")){
        return PRINT_ARCHIVE;
    } else if (areEqual(str, "list all") || areEqual(str, "ls -a") || areEqual(str, "ln")){
        return LIST_ALL;
    } else {
        return INVALID_SIGNAL;
    }
}

void handleInput(const string& input){
    switch (strToSignal(input)){
        case MARK_PRIORITY: {
            changesMade = true;
            int num = stoi(prompt("Which note? (Number)"));

            // do a boundary check
            if(num-1 > n->getCount()){
                cout << "Invalid index specified" << endl;
            } else {
                n->getNote(num-1)->prioritize();
            }
            break;
        }
        case UNMARK_PRIORITY: {
            changesMade = true;
            int num = stoi(prompt("Which note? (Number)"));

            // do a boundary check
            if(num-1 > n->getCount()){
                cout << "Invalid index specified" << endl;
            } else {
                n->getNote(num-1)->deprioritize();
            }
            break;
        }
        case ARCHIVE:{
            //int count = 0;
            for(int i = 0; i < n->getCount(); i++){
                note* temp = n->getNote(i);
                if(temp->isCompleted()){
                    a->insertNote(temp);
                    n->removeNote(i)->archive();
                    changesMade = true;
                    i--;
                    cout << blue << "Archiving: " << def << temp->getText() << endl;
                    //count++;
                }
            }
            //cout << "Archived " << blue << count << def << " tasks" << endl;
            break;
        }
        case LIST_ALL:{
            handleInput("ls");
            cout << endl;
            handleInput("la");
            break;
        }
        case PRINT_ARCHIVE:{
            cout << "===============" << endl;
            cout << "----Archive----" << endl;
            cout << "===============" << endl << endl;
            int count = 1;
            vector<note*> aVec = a->getNotes();
            int longest = 0;
            for(auto it = aVec.begin(); it != aVec.end(); it++){
                string current = (*it)->getText();
                longest = current.size() > longest ? current.size() : longest;
            }

            for(auto it = aVec.begin(); it != aVec.end(); it++){
                cout << count << (count < 10 ? ".  " : ". ") << (*it)->toString(longest) << endl;
                count++;
            }
            break;
        }
        case NEW_NOTE: {
            changesMade = true;
            note* newNote = n->newNote(prompt("New Note"));
            cout << green << "Added" << def << ": " << n->getCount() << ". " << newNote->getText() << endl;
            break;
        }
        case DELETE_NOTE: {
            changesMade = true;
            int num = stoi(prompt("Which note? (Number)"));
            
            // do a boundary check
            if(num-1 > n->getCount()){
                cout << "Invalid index specified" << endl;
            } else {
                n->deleteNote(num-1);
            }
            
            break;
        }
        case LIST_NOTES: {
            cout << "=============" << endl;
            cout << "----Notes----" << endl;
            cout << "=============" << endl << endl;
            int count = 1;
            vector<note*> nVec = n->getNotes();
            int longest = 0;
            for(auto it = nVec.begin(); it != nVec.end(); it++){
                string current = (*it)->getText();
                longest = current.size() > longest ? current.size() : longest;
            }

            for(auto it = nVec.begin(); it != nVec.end(); it++){
                cout << count << (count < 10 ? ".  " : ". ") << (*it)->toString(longest) << endl;
                count++;
            }
            break;
        }
        case MARK_COMPLETE: {
            changesMade = true;
            int num = stoi(prompt("Which note? (Number)"));
            
            // do a boundary check
            if(num-1 > n->getCount()){
                cout << "Invalid index specified" << endl;    
            } else {
                vector<note*> nVec = n->getNotes();
                nVec.at(num-1)->markComplete();
            }
                
            break;
        }
        case MARK_INCOMPLETE: {
            changesMade = true;
            int num = stoi(prompt("Which note? (Number)"));
            if(num-1 > n->getCount()){
                cout << "Invalid index specified" << endl;
            } else {
                vector<note*> nVec = n->getNotes();
                nVec.at(num-1)->markIncomplete();
            }
            break;
        }
        case CLEAR_LIST: {
            string decision = prompt("Are you sure? (y/n)");
            if(areEqual(decision, "y")){
                changesMade = true;
                n->clearAll();
            }
            break;
        }
        case CLEAR_COMPLETED: {
            string decision = prompt("Are you sure? You can archive completed tasks instead.. (y/n)");
            if(areEqual(decision, "y")){
                changesMade = true;
                vector<note*> nVec = n->getNotes();
                for(int i = 0; i != nVec.size(); i++){
                    if(nVec[i]->isCompleted()){
                        // delete this note
                        n->deleteNote(i);
                        nVec = n->getNotes();
                        i--;
                    }
                }
            }
            break;
        }
        case QUIT: {
            // handled in caller - do nothing here
            break;
        }
        case HELP: {
            printHelp();
            break;
        }
        default:
        case INVALID_SIGNAL:
            cout << "Unrecognized command \'" << input << "\'" << endl;
            cout << "Type \'help\' for a list of valid commands" << endl;
            break;
    }
}

void printHelp(){
    cout << endl;
    cout << "Notes++ Help/Command Info" << endl;
    cout << "=========================" << endl << endl;
    cout << "\'new note\'         - Create a new note" << endl;
    cout << "\'delete note\'      - Delete a note" << endl;
    cout << "\'list\'             - List all current notes" << endl;
    cout << "\'list archive\'     - List all archived notes" << endl;
    cout << "\'list all\'         - List all notes" << endl;
    cout << "\'mark complete\'    - Change a note to complete" << endl;
    cout << "\'mark incomplete\'  - Change a note to incomplete" << endl;
    cout << "\'high priority\'    - Mark a note as important" << endl;
    cout << "\'low priority\'     - Remove priority from a note" << endl;
    cout << "\'clear list\'       - Delete all notes in the list" << endl;
    cout << "\'clear completed\'       - Delete all completed notes in the list" << endl;
    cout << "\'archive\'          - Archive all completed items" << endl;
    cout << "\'help\'             - Print this info" << endl;
    cout << "\'quit\'             - Exit this program" << endl;
}

string toLower(const string& original){
    locale loc;
    string toReturn = "";
    for(string::size_type i = 0; i < original.length(); i++){
        toReturn += tolower(original[i],loc);
    }
    return toReturn;
}

string prompt(){
    return prompt("");
}

string prompt(const string& header){
    //cout << endl << header << "> ";
    cout << header << "> ";
    string input = "";
    getline(cin, input);
    return input;
}

bool areEqual(const string& str1, const string& str2){
    return str1.compare(str2) == 0;    
}

bool areEqual(const char* a, const char* b){ // compare two cstrings
    return strcmp(a,b) == 0;
}
