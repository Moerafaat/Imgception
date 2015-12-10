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
    //fake path, real path, name
    Image(int Id, Key, QString, QString, QString);
    ~Image() = default;

    Image& operator=(const Image&);

    char *serialize(unsigned int &) const;
    bool deserialize(const char * const, const unsigned int);

    QImage getImage(); // Local invocation. Return image for Display (Open the file and Decode Steganography).

    static Steganifier s;

    int ID; // Image identifier.
    int up_count; // number of times the image has been viewed.
    int view_limit; // the maximum number of times to view the image.
    QString image_name; // Image name.
    QString path; // Path to image.
    Key owner_key; // Image owner.
};

#endif // IMAGE_H
