/**
 * @file search.c
 * @author Nitzan Saar
 *
 * This program is a file search utility inspired by find(1). At its core, it
 * recursively traverses a directory structure and reports its contents with a
 * variety of filtering options.
 */

#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logger.h"

struct options {
    int max_depth;
    bool exact_match : 1; // these are bitfields (1 bit each)
    bool show_dirs : 1;
    bool show_files : 1;
    bool show_hidden : 1;
};
//-1 is default depth
struct options default_options = {-1, false, true, true, false};

/**
 * Prints help/program usage information.
 *
 * This output is displayed if there are issues with command line option parsing
 * or the user passes in the -h flag.
 */
void print_usage(char *prog_name)
{
    printf("Usage: %s [-defhH] [-l depth-limit] [directory] [search-pattern]\n" , prog_name);
    printf("\n");
    printf("Options:\n"
"    * -d    Only display directories (no files)\n"
"    * -e    Match search-pattern exactly; no partial matches reported.\n"
"    * -f    Only display files (no directories)\n"
"    * -l    Set a depth limit, e.g., recurse no more than 2 directories deep.\n"
"    * -h    Display hidden files.\n"
"    * -H    Display help/usage information\n");
    printf("\n");
}


int recursive_search(struct options *opts, char *directory, char *search_term, int depth) {
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        // stop recursing once we reach the max depth - if it is not specified then it won't stop
        if (depth == opts->max_depth) {
            return 0;
        }
        // allocate memory
        char *buf = malloc(strlen(directory) + strlen(entry->d_name) + strlen("/") + 1);
        if (buf == NULL) {
            perror("malloc");
            break;
        }
        sprintf(buf, "%s/%s", directory, entry->d_name);
        if (entry->d_type == DT_DIR) {
            if (strstr(entry->d_name, search_term) != NULL && opts->show_dirs == true) {
                printf("%s\n", buf);
            }
            // increase depth by 1 each time we make a recursive call
            recursive_search(opts, buf, search_term, depth + 1);
        // if d_type is a file and show_files is true
        } else if (entry->d_type == DT_REG && opts->show_files == true) {
            // don't print a hidden file if show_hidden is false
            if (opts->show_hidden == false && entry->d_name[0] == '.') {
                continue;
            }
            else if (strstr(entry->d_name, search_term) != NULL) {  
                if (opts->exact_match == true) {
                    if (strcmp(search_term, entry->d_name) == 0) {
                        printf("%s\n", buf);
                    }
                } else {
                    printf("%s\n", buf);
                }
            }
        }
        free(buf);
    }
    closedir(dir);
    return 0;
}



int main(int argc, char *argv[]) {

    struct options opts;
    opts = default_options;
    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, "defhHl:")) != -1) {
        switch (c) {
            case 'd':
                opts.show_files = false;
                break;
            case 'H':
                print_usage(argv[0]);
                return 1;
            case 'h':
                opts.show_hidden = true;
                break;
            case 'e':
                opts.exact_match = true;
                break;
            case 'f':
                opts.show_dirs = false;
                break;
            case 'l': {
                char *num_string = optarg;
                int depth_limit = atoi(num_string);
                    if (depth_limit <= 0) {
                        perror("Invalid limit");
                        print_usage(argv[0]);
                        return 1;
            }
                opts.max_depth = depth_limit;
            }
                break;
            case '?':
                if (optopt == 's') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                print_usage(argv[0]);
                return 1;
            default:
                abort();
        }
    }

    /* Default values. We search the current working directory (CWD) '.', and
     * provide an empty search string (no filtering applied). */
    char *dir = ".";
    char *search = "";

    /* Both of the following arguments are optional, so we have to check for
     * their presence first. */

    if (optind < argc) {
        // We have one extra argument after the options.
        // This should be the search directory.
        dir = argv[optind];
    }

    if (optind + 1 < argc) {
        // One more extra argument. Should be the search pattern.
        search = argv[optind + 1];
    }

    LOG("Starting search. Directory: %s; Search pattern: %s\n", dir, search);
    LOG("Depth limit: %d; Exact match %s; Show files %s; Show dirs %s; Show hidden %s\n",
            opts.max_depth,
            opts.exact_match ? "ON" : "OFF",
            opts.show_files ? "ON" : "OFF",
            opts.show_dirs ? "ON" : "OFF",
            opts.show_hidden ? "ON" : "OFF");

    // by default pass in 0 as the depth because if depth isn't specified it wont matter
    return recursive_search(&opts, dir, search, 0);
}
