#include "notes.h"

/*********************
 * NOTES DEFINITIONS *
 *********************/

notes::~notes(){
    wipeMemory();
}

notes::notes(const notes& original){
    init();
    const std::vector<note*>& og = original.getNotes();
    for(int i = 0; i < og.size(); i++){
        noteVector->push_back(new note(*(og[i])));
    }
}

void notes::init(){
    if(noteVector == nullptr){
        noteVector = new std::vector<note*> ();
    } else {
        wipeMemory();
    }
}

void notes::wipeMemory(){
    if(noteVector != nullptr){
        for(int i = 0; i < noteVector->size(); i++){
            delete noteVector->at(i);
        }
        delete noteVector;
        noteVector = nullptr;
    }
}

void notes::clearAll(){
    wipeMemory();
    init();
}

note* notes::newNote(const std::string& data){
    note* n = new note(data);
    noteVector->push_back(n);
    return n;
}

void notes::insertNote(note* notePtr){
    if(notePtr != nullptr){
        noteVector->push_back(notePtr);
    }
}

void notes::deleteNote(int index){
    note* n = *(noteVector->begin()+index);
    delete n;
    noteVector->erase(noteVector->begin()+index);
}

note* notes::removeNote(const int& index){
    note* n = *(noteVector->begin()+index);
    noteVector->erase(noteVector->begin()+index);
    return n;
}

const std::vector<note*>& notes::getNotes() const{
    return *noteVector;
}

int notes::getCount() const{
    return noteVector->size();
}

int notes::getIncompleteCount() const{
    int count = 0;
    for(auto it = noteVector->begin(); it != noteVector->end(); it++){
        if(!(*it)->isCompleted()){
            count++;
        }
    }
    return count;
}

note* notes::getNote(int index) const{
    return noteVector->at(index);
}

int notes::getImportantCount() const{
    int impCount = 0;
    for(int i = 0; i < getCount(); i++){
        if(getNote(i)->isPriority() && !getNote(i)->isCompleted()){
            impCount++;
        }
    }
    return impCount;
}

/*************************
 * NOTE NODE DEFINITIONS *
 *************************/

Color::Modifier redl(Color::FG_RED);
Color::Modifier defl(Color::FG_DEFAULT);
Color::Modifier greenl(Color::FG_GREEN);
Color::Modifier bluel(Color::FG_BLUE);

bool note::isCompleted() const {
    return completed;
}

std::string note::getText() const {
    return data;
}

void note::setText(const std::string& data){
    this->data = data;
}

void note::markComplete(){
    completed = true;
}

void note::markIncomplete(){
    completed = false;
}

std::string note::toString(int formatWidth){
    int buffSize = formatWidth - data.size();
    std::string toReturn = data;
    //return data + " [" + (completed ? "COMPLETED" : "INCOMPLETE") + "]";
    for(int i = 0; i < buffSize; i++){
        toReturn += " ";
    }
    toReturn += std::string(" | [") 
        + (archived ? (bluel + std::string("ARCHIVED")) 
                : (completed ? (greenl + std::string("COMPLETED")) 
                    : (redl + std::string("INCOMPLETE")))) + (defl + "]") 
        + (completed ? " " : "") + std::string(isPriority() 
                ? (" | [" + (bluel + "*IMP*") + (defl + "]")) : "");
    return toReturn;
}

void note::prioritize(){
    priority = true;
}

void note::deprioritize(){
    priority = false;
}

bool note::isPriority(){
    return priority;
}

bool note::isArchived(){
    return archived;
}

void note::archive(){
    archived = true;
}

void note::unarchive(){
    archived = false;
}
