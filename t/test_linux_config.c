#include <stdio.h>
#include <string.h>

#include "tap.h"

#include "../linux_config.h"
#include "../linux_config.c"

int main() {
    plan_no_plan();
    char filename[] = "/tmp/test.txt";
    
    /* Point to the period of the extension */
    char* ext_p = linux_find_ext_p(filename);
    ok(
        ext_p == filename + strlen("/tmp/test"),
        "Point to first character of extension"
    );
    
    /* Point to the last forward-slash of the path */
    char* path_p = linux_find_path_p(filename);
    ok(
        path_p == filename + strlen("/tmp"),
        "Point to last character of path"
    );
    
    /* Point to the first character of the filename */
    char* basename_p = linux_find_basename_p(filename);
    ok(
        basename_p = filename + strlen("/tmp/"),
        "Point to the first character of basename"
    );
    
    /* Find the extension of a file */
    char ext[FILENAME_MAX];
    linux_find_ext(filename, ext);
    ok(
        !strcmp(ext, "txt"),
        "Find the extension of a file"
    );
    
    /* Find the full path of a file */
    char path[FILENAME_MAX];
    linux_find_path(filename, path);
    ok(
        !strcmp(path, "/tmp/"),
        "Find the path of a file"
    );
    
    /* Find the basename of a file from the full path */
    char basename[FILENAME_MAX];
    linux_find_basename(filename, basename);
    ok(
        !strcmp(basename, "test"),
        "Find the basename of a file"
    );
    
    /* Change the extension of a file */
    char new_filename[FILENAME_MAX];
    linux_change_ext(new_filename, filename, ".text");
    ok(
        !strcmp(new_filename, "/tmp/test.text"),
        "Change the extension of a file"
    );
    
    /* Change the basename of a file */
    linux_change_basename(new_filename, filename, "testing");
    ok(
        !strcmp(new_filename, "/tmp/testing.txt"),
        "Change the basename of a file"
    );
    
    return exit_status();
}
