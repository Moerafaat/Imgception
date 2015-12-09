#ifndef UPDATE_H
#define UPDATE_H


#include "transmittable.h"
#include "key.h"

class Update : public Transmittable{
public:
    Update() = default;
    Update(const Update&) = default;
    Update(Update&&) = default;
    Update(Key, unsigned int, unsigned int, bool);

    char *serialize(unsigned int &) const;
    bool deserialize(const char * const, const unsigned int);
private:
    Key owner_key; // notification sender.
    unsigned int image_key; // image to be updated.
    unsigned int new_limit; // limit update.
    bool new_start; // If true, reset up_count, otherwise keep it the same.
};

#endif // UPDATE_H
