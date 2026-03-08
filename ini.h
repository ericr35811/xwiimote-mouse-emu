#ifndef _INI_H
#define _INI_H

#include "list.h"   // included here because list and node are in parameters

#define INI_COMMENT '#'
#define INI_SECTION_BEGIN '['
#define INI_SECTION_END ']'
#define INI_SEPARATOR '='
#define INI_ROOT ""

#define INI_ERR_PREFIX "INI ERROR: "

#define MAX_LINE_LENGTH 1024
#define MAX_KEYVAL_LENGTH 256

void *ini_get_value(struct list *store, char *sec_key, char *val_key) ;
#define ini_get_str(store, sec_key, val_key) ((char *)ini_get_value(store, sec_key, val_key))
int ini_get_int(struct list *store, char *sec_key, char *val_key);
struct list *ini_new_section(struct list *store, char *keybuf);
struct node *ini_new_value(struct list *section, char *keybuf, void *valbuf);
struct list *ini_parse(char *path);

#endif