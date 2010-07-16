/* Functions written for the Linux port */
/* See linux_config.c for documentation */

char* linux_find_ext_p(char* filename);
char* linux_find_path_p(char* filename);
char* linux_find_basename_p(char* filename);

void linux_find_ext(char* filename, char* ext);
void linux_find_path(char* filename, char* path);
void linux_find_basename(char* filename, char* path);

void linux_change_ext(char* new_filename, char* old_filename, const char* ext);
void linux_change_basename(char* new_filename, char* old_filename, const char* ext);