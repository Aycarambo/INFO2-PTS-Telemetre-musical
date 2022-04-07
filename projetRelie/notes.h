#ifndef NOTE
#define NOTE_H

#include <iostream>
#include <QRect>
#include <QImage>
#include <QPainter>

class Note{
	public:
    float frequenceNote;
	std::string nomNote;
    QRect dimensionsNote;
    QImage imageNote;
    QString path;
    bool estBemol;
    int hauteur; //3 - 18

    Note() {}
    Note(std::string unNom, float uneFrequence);
    Note(std::string unNom, float uneFrequence, bool estBemol, int hauteur);
    Note(std::string unNom, float uneFrequence, QRect desDimensions, QString unCheminImage);

    void draw(QPainter* aPainter);
};

#endif
