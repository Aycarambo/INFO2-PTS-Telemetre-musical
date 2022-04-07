#include <iostream>
#include "Gamme.h"
#include "variables.h"
#include <cstdarg>

using namespace std;

Gamme::Gamme(string unNom){
	this->nomGamme=unNom;
}

void Gamme::ajouteNotes(Note note1, Note note2, Note note3, Note note4, Note note5, Note note6, Note note7, Note note8, Note note9, Note note10, Note note11, Note note12){
	this->vecteurDeNotes.push_back(note1);
	this->vecteurDeNotes.push_back(note2);
	this->vecteurDeNotes.push_back(note3);
	this->vecteurDeNotes.push_back(note4);
	this->vecteurDeNotes.push_back(note5);
	this->vecteurDeNotes.push_back(note6);
	this->vecteurDeNotes.push_back(note7);
	this->vecteurDeNotes.push_back(note8);
	this->vecteurDeNotes.push_back(note9);
	this->vecteurDeNotes.push_back(note10);
	this->vecteurDeNotes.push_back(note11);
	this->vecteurDeNotes.push_back(note12);
}

Note Gamme::trouveNoteSelonDistance(double distance){
    double distanceTotal=DISTANCE_MAX - DISTANCE_MIN;
    double lePas = distanceTotal/12;
    Note noteParDefaut = this->vecteurDeNotes.at(0);
    for (int i = 1 ; i <=12 ; i++)
    {
        if (distance <= (DISTANCE_MIN + (lePas*i)))
        {
            return this->vecteurDeNotes.at(i-1);
        }

    }
    return noteParDefaut;
}
