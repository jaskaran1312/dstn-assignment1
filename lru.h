int getMMLRU(struct Hardware *hardware); //return index of the least recently accessed page. If multiple indexes are all 0, return first occurence
void shiftMMLRU(struct Hardware *hardware); //shift bits right. 
void updateMMLRU(struct Hardware *hardware, int frameNumber); //frame no (int) has just been accesed, make it's MSB 1.
