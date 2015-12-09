#include "update.h"

Update::Update(Key key, unsigned int ImageKey, unsigned int NewLimit, bool NewStart)
    :owner_key(key), image_key(ImageKey), new_limit(NewLimit), new_start(NewStart){

}

char* Update::serialize(unsigned int & Size) const{
    Size = Key::PubKeySize + 4 + 4 + 1;
    char *serial = new char[Size];
    memcpy(serial, owner_key.getAsString().toStdString().c_str(), Key::PubKeySize);
    *(unsigned int*)(serial + Key::PubKeySize) = HostToNetwork(image_key);
    *(unsigned int*)(serial + Key::PubKeySize + 4) = HostToNetwork(new_limit);
    *(serial + Key::PubKeySize + 4 + 4) = new_start;
}

bool Update::deserialize(const char* const SerializedUpdate, const unsigned int Size){
    if(Size != Key::PubKeySize + 4 + 4 + 1) return false;
    owner_key = QString::fromStdString(std::string(SerializedUpdate, Key::PubKeySize));
    image_key = NetworkToHost(*(unsigned int*)(SerializedUpdate + Key::PubKeySize));
    new_limit = NetworkToHost(*(unsigned int*)(SerializedUpdate + Key::PubKeySize + 4));
    new_start = *(SerializedUpdate + Key::PubKeySize + 4 + 4);
}
