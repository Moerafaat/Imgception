#ifndef UPDATE_H
#define UPDATE_H


#include "transmittable.h"

class Update : public Transmittable{
public:
    Update();

    char *serialize(unsigned int &) const;
    bool deserialize(const char * const, const unsigned int);
private:
    int owner_key; // notification sender.
    int image_key; // image to be updated.
    int new_limit; // limit update.
    bool new_start; // If true, reset up_count, otherwise keep it the same.
};

#endif // UPDATE_H
