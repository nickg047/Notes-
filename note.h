#ifndef NOTE_H
#define NOTE_H

#include <string>
#include "color.h"

class note{
private:
    /****************
     * PRIVATE DATA *
     ****************/
    std::string data;
    bool completed;
    bool priority;
    bool archived;

public:
    /**************************
     * PUBLIC CLASS FUNCTIONS *
     **************************/
    inline note() : data(""), completed(false), priority(false), archived(false) {}
    inline note(const std::string& data) : data(data), completed(false), priority(false), archived(false) {}

    /********************
     * PUBLIC FUNCTIONS *
     ********************/
    bool isCompleted() const;
    std::string getText() const;
    void setText(const std::string&);
    void markComplete();
    void markIncomplete();
    void prioritize();
    void deprioritize();
    bool isPriority();
    std::string toString(int);
    bool isArchived();
    void archive();
    void unarchive();
};


#endif
