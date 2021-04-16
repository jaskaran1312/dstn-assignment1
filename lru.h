int getMMLRU(struct Hardware *hardware); //return index of the least recently accessed page
//if multiple indexes are all 0, return first occurence??
void shiftMMLRU(struct Hardware *hardware); //shift bits right. Return value optional
void updateMMLRU(struct Hardware *hardware, int frameNumber); //frame no (int) has just been accesed, make it's MSB 1.

// inialise all LRU values to 0 ig.
