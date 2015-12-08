#include "image.h"

Image::Image(int Id, int OwnerKey, QString Path, QString Name, int UpCount, int ViewLimit){

}

Image& Image::operator=(const Image& image){
    ID = image.ID;
    owner_key = image.owner_key;
    path = image.path;
    image_name = image.image_name;
    up_count = image.up_count;
    view_limit = image.view_limit;
}

char* Image::serialize(unsigned int & Size) const{
    return nullptr;
}

bool Image::deserialize(const char* const SerializedImage, const unsigned int Size){
    return false;
}

// Stub entry.
QImage Image::getImage(){ // Function that returns the image for display.

}
