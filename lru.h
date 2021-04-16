int getLRU(struct Hardware *hardware); //return index of the least recently accessed page
//if multiple indexes are all 0, return first occurence??
int shiftLRU(struct Hardware *hardware); //shift bits right. Return value optional
int updateLRU(struct Hardware *hardware, int frameno); //frame no (int) has just been accesed, make it's MSB 1.

// inialise all LRU values to 0 ig.
