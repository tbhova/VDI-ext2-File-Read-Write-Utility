#ifndef VDIMAP_H
#define VDIMAP_H
#include <QVector>
#include <fstream>

#include <QObject>

class VdiMap : public QObject
{
    Q_OBJECT
public:
    explicit VdiMap(QObject *parent, long long startOffset, long long stopOffset, std::ifstream &file);
    int getMappedLocation (int location);
signals:

public slots:

private:
    QVector < unsigned int >*map; //dyanmic array of char for the map
};

#endif // VDIMAP_H
