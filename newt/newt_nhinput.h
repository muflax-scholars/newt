#ifndef NEWT_NHINPUT_H
#define NEWT_NHINPUT_H

int newt_nhgetch (void);
int newt_nh_poskey (int *x, int *y, int *mod);
void newt_getlin (const char *ques, char *input);
int newt_get_ext_cmd (void);

#endif
