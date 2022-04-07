#include "notes.h"

Note::Note(std::string unNom, float uneFrequence)
{
    nomNote=unNom;
    frequenceNote=uneFrequence;
}

Note::Note(std::string unNom, float uneFrequence, bool estBemol, int hauteur)
{
    nomNote=unNom;
    frequenceNote=uneFrequence;
    this->estBemol=estBemol;
    this->hauteur=hauteur;
}

Note::Note(std::string unNom, float uneFrequence, QRect desDimensions, QString unCheminImage)
{
    nomNote=unNom;
    frequenceNote=uneFrequence;
    dimensionsNote=desDimensions;
    path=unCheminImage;
}

void Note::draw(QPainter *aPainter)
{

    imageNote.load(path);
    aPainter->drawImage(dimensionsNote,imageNote);
}
