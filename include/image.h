#ifndef IMAGE_H
#define IMAGE_H


#include <QString>
#include <QImage>
#include "transmittable.h"
#include "key.h"
#include "Steganifier.h"

class Image : public Transmittable{
public:
    Image() = default;
    Image(const Image&) = default;
    Image(Image&&) = default;
    Image(int, Key, QString, QString, int, int);
    //fake path, real path, pathToSaveTo, name
    Image(int Id, Key, QString, QString, QString, QString);
    ~Image() = default;

    Image& operator=(const Image&);

    char *serialize(unsigned int &) const;
    bool deserialize(const char * const, const unsigned int);

    QImage getImage(); // Local invocation. Return image for Display (Open the file and Decode Steganography).

    static Steganifier s;

    int ID; // Image identifier.
    Key owner_key; // Image owner.
    QString path; // Path to image.
    QString image_name; // Image name.

    int up_count; // number of times the image has been viewed.
    int view_limit; // the maximum number of times to view the image.
};

#endif // IMAGE_H
