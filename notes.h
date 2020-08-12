#ifndef NOTES_H
#define NOTES_H

#include <string>
#include <vector>
#include "note.h"

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
