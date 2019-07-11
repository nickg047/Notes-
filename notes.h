#ifndef NOTES_H
#define NOTES_H

#include <string>
#include <vector>
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


class notes{
private:
    /****************
     * PRIVATE DATA *
     ****************/
    std::vector<note*>* noteVector;

    /*********************
     * PRIVATE FUNCTIONS *
     *********************/
    void wipeMemory();
    void init();

public:
    /**************************
     * PUBLIC CLASS FUNCTIONS *
     **************************/
    inline notes() : noteVector(nullptr) {init();} // inline constructor
    ~notes(); // destructor
    notes(const notes&); // copy constructor

    /********************
     * PUBLIC FUNCTIONS *
     ********************/
    note* getNote(int) const;
    void clearAll();
    note* newNote(const std::string&);
    void insertNote(note* notePtr);
    note* removeNote(const int&);
    void deleteNote(int);
    const std::vector<note*>& getNotes() const;
    int getCount() const;
    int getIncompleteCount() const;
    int getImportantCount() const;
};

#endif
