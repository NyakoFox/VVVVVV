#ifndef INPUT_H
#define INPUT_H

extern bool fadetomode;

void titleinput(void);

void gameinput(void);

void mapinput(void);

void teleporterinput(void);

void gamecompleteinput(void);

void gamecompleteinput2(void);

void startmode(const enum StartMode mode);
void handlefadetomode(void);

#endif /* INPUT_H */
