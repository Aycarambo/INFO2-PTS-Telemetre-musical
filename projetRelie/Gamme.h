#ifndef GAMME_H
#define GAMME_H

#include "notes.h"
#include <vector>

class Gamme{
	public:
	std::vector <Note> vecteurDeNotes;
	std::string nomGamme;
	
    Gamme(std::string unNom);
	void ajouteNotes(
		Note note1, 
		Note note2, 
		Note note3, 
		Note note4, 
		Note note5, 
		Note note6, 
		Note note7,
		Note note8,
		Note note9,
		Note note10,
		Note note11,
		Note note12
	);

    Note trouveNoteSelonDistance(double distance);
};

#endif // DSFFH_H
