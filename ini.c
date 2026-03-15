#include <stdio.h>
#include <string.h>
#include <stdlib.h>  
#include <errno.h>
#include <ctype.h> // tolower()
#include "ini.h"
#include "list.h"

int line_no = 0;

// error function
void err_exit(char *msg, char include_line_no) {
    fprintf(stderr, INI_ERR_PREFIX "%s", msg);
    if (include_line_no)
        fprintf(stderr, " on line %d", line_no);
    putc('\n', stderr);
    exit(1);
}

/* copy characters to an output buffer until a delimiter is reached, and
   return a pointer to the source string 1 character after the separator */
char *copy_until(char *line, char *out, char c) {
    while (*line && *line != c)     // while current character is not separator or terminator...
        *(out++) = *(line++);       // copy character
    *out = '\0';                    // terminate output string
    
    if (*line == '\0' && c != '\0') // if EOL reached (and EOL was not the character we were checking for)
        err_exit("Unexpected EOL", 1);

    return line+1;
}

/* determine whether a character is whitespace */
int test_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

/* trim whitespace from the beginning of a buffer by returning a pointer
   to the first non-whitespace character */
char *trim_start_ptr(char *line) {
    while (test_whitespace(*line)) *(line++);
    return line;
}

/* trim whitespace from the beginning of a buffer by copying the content
   to the beginning of the same buffer */
void trim_start(char *line) {
    char *new = line;
    line = trim_start_ptr(line);
    while (*(new++) = *(line++));   // this also copies the null terminator
}

/* trim whitespace from the end of a string */
void trim_end(char *line) {
    while (*line) *line++;
    while (test_whitespace(*(--line)));
    *(++line) = '\0';
}

/* trim the comment character and any characters that follow it in the entire string.
   call this before any other trim functions */
void trim_comment(char *line) {
    while (*line && *line != INI_COMMENT) *(line++);
    *line = '\0';
}

/* combined start+end trim */
void trim(char *line) {
    trim_end(line);
    trim_start(line);
}


// create a new section, erroring if it already exists, returning the list representing it
struct list *ini_new_section(struct list *store, char *keybuf) {
    if (list_find(store, keybuf) == NULL) 
        // strdup() copies the key to a new location so it is not overwritten when keybuf is modified later
        return list_addval(store, strdup(keybuf), list_new())->value;
    else
        err_exit("Duplicate section header", 1);
}

// create a new value in a section, erroring if it already exists, returning the node representing it
struct node *ini_new_value(struct list *section, char *keybuf, void *valbuf) {
    if (list_find(section, keybuf) == NULL)
        return list_addval(section, strdup(keybuf), strdup(valbuf));
    else
        err_exit("Duplicate key within section", 1);
}

void *ini_get_value(struct list *store, char *sec_key, char *val_key) {
    struct list *section;
    struct node *value;
    char err[256];

    if ((section = list_findval(store, sec_key)) != NULL) {
        if ((value = list_findval(section, val_key)) != NULL) {
            return value;
        } else {
            sprintf(err, "Unable to find key \"%s\" in section \"%s\"", val_key, sec_key);
            err_exit(err, 0);
        }
    } else {
        sprintf(err, "Unable to find section \"%s\"", sec_key);
        err_exit(err, 0);
    }
}

int ini_get_int(struct list *store, char *sec_key, char *val_key) {
    int x, n, rc;
    char err[256];
    char *value;

    value = ini_get_str(store, sec_key, val_key);
    rc = sscanf(value, "%d%n", &x, &n);

    // reject input if it could not be converted or if there were extra 
    // unread characters due to garbage at the end of the string
    if (rc < 1 || (int)strlen(value) != n) {
        sprintf(err, "Could not convert \"%s\" to an integer", value);
        err_exit(err, 0);
    } else {
        return x;
    }
}

int ini_get_bool(struct list *store, char *sec_key, char *val_key) {
    char *value, err[256], *p;

    value = ini_get_str(store, sec_key, val_key);
    p = value;
    do {
        *p = tolower(*p);
    } while (*(++p));

    if (!strcmp(value, "true")) {
        return 1;
    } else if (!strcmp(value, "false")) {
        return 0;
    } else {
        sprintf(err, "Could not convert \"%s\" to a bool", value);
        err_exit(err, 0);
    }

}

struct list *ini_parse(char *path) {
    FILE *f;
    char linebuf[MAX_LINE_LENGTH];
    char secbuf[MAX_KEYVAL_LENGTH] = INI_ROOT;  // use the root section by default until another section is read
    char keybuf[MAX_KEYVAL_LENGTH];
    char valbuf[MAX_KEYVAL_LENGTH];
    char *c;    // pointer to current character within line
    struct list *store, *section;
    struct node *node;
    char *str;

    store = list_new();
    section = ini_new_section(store, INI_ROOT);

    if ((f = fopen(path, "r")) == NULL) 
        err_exit(strerror(errno), 0);

    while (fgets(linebuf, MAX_LINE_LENGTH, f) != NULL) {
        line_no++;      // update line counter for error purposes

        trim_comment(linebuf);  
        c = trim_start_ptr(linebuf); // only need to trim beginning whitespace so we can check the first character of the line

        if (*c == INI_SECTION_BEGIN) {
            // only section lines begin with a special character
            copy_until(++c, secbuf, INI_SECTION_END);
            trim(secbuf);
            
            // secbuf will stay set until a new section begins
            section = ini_new_section(store, secbuf);
            
        } else if (*c == '\0') {    
            // if the line was blank, all whitespace, or a comment, then trimming reduced
            // it to an empty string; ignore
            continue;
        } else {    
            // anything else must be a key and value
            c = copy_until(c, keybuf, INI_SEPARATOR);
            trim(keybuf);

            /* error if a key is not found; there is no such check for an empty value
               because an empty value may be an error in the main program but is
               not an INI parsing error */
            if (keybuf[0] == '\0') 
                err_exit("No key name before '='", 1);

            copy_until(c, valbuf, '\0');
            trim(valbuf);
            ini_new_value(section, keybuf, valbuf);
        }
    }
    
    fclose(f);

    return store;
}
