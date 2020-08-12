#include "note.h"

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

