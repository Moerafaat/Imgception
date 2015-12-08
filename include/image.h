#ifndef IMAGE_H
#define IMAGE_H


#include <QString>
#include <QImage>
#include "transmittable.h"
#include "key.h"

class Image : public Transmittable{
public:
    Image() = default;
    Image(const Image&) = default;
    Image(Image&&) = default;
    Image(int, int, QString, QString, int, int);
    ~Image() = default;

    Image& operator=(const Image&);

    char *serialize(unsigned int &) const;
    bool deserialize(const char * const, const unsigned int);

    QImage getImage(); // Local invocation. Return image for Display (Open the file and Decode Steganography).

    int ID; // Image identifier.
    Key owner_key; // Image owner.
    QString path; // Path to image.
    QString image_name; // Image name.

    int up_count; // number of times the image has been viewed.
    int view_limit; // the maximum number of times to view the image.
};

#endif // IMAGE_H
