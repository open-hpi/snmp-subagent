#include <glib.h>
#include <SaHpi.h>
int string2entitypath(const gchar *epathstr, SaHpiEntityPathT *epathptr);
int entitypath2string(const SaHpiEntityPathT *epathptr, gchar *epathstr, gint strsize);
