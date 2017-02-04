/* vim: set ts=4 sw=4 expandtab: */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define TRUE 1
#define FALSE 0

static char zpm_init[PATH_MAX];
static char zpm_list[PATH_MAX];

char* generate_plugin_path(char* plugin_name) {
    if (plugin_name[0] == '/') {
        return strdup(plugin_name);
    }

    char* plugin_path = malloc(PATH_MAX);
    strcpy(plugin_path, getenv("HOME"));
    strcat(plugin_path, "/.zpm/plugins/");
    if (strncmp(plugin_name, "github.com/", 11)) {
        strcat(plugin_path, plugin_name);
    } else {
        strcat(plugin_path, plugin_name + 11);
    }
    return plugin_path;
}

char* get_plugin_entry_point(char* plugin_name) {
    DIR *dir;
    struct dirent *ent;
    char* plugin_path = generate_plugin_path(plugin_name);
    char* plugin_entry_point = malloc(PATH_MAX);
    strcpy(plugin_entry_point, "");

    if ((dir = opendir(plugin_path)) != NULL) {
      while (strlen(plugin_entry_point) == 0
                && (ent = readdir (dir)) != NULL) {
        if (strstr(ent->d_name, ".plugin.zsh")) {
            // x.plugin.zsh entry point
            strcpy(plugin_entry_point, ent->d_name);
        } else if (strstr(ent->d_name, "init.zsh")) {
            // prezto like
            strcpy(plugin_entry_point, ent->d_name);
        } else if (strstr(ent->d_name, ".zsh")) {
            strcpy(plugin_entry_point, ent->d_name);
        } else if (strstr(ent->d_name, ".sh")) {
            strcpy(plugin_entry_point, ent->d_name);
        }
      }

      closedir (dir);
    }

    free(plugin_path);
    return plugin_entry_point;
}

char* get_plugin_entry(char* plugin_name) {
    char* plugin_entry = malloc(PATH_MAX);

    char* plugin_path = generate_plugin_path(plugin_name);

    char* plugin_entry_point = get_plugin_entry_point(plugin_name);

    strcpy(plugin_entry, "source ");
    strcat(plugin_entry, plugin_path);
    strcat(plugin_entry, "/");
    strcat(plugin_entry, plugin_entry_point);
    strcat(plugin_entry, "\n");

    free(plugin_path);
    free(plugin_entry_point);
    return plugin_entry;
}

int generate_plugin_entry(char* plugin_name) {
    FILE* store = fopen(zpm_init,"ab+");

    char* plugin_entry = get_plugin_entry(plugin_name);
    char  plugin_entry_list[PATH_MAX];
    fread(plugin_entry_list, 1, PATH_MAX, store);
    fclose(store);
    if (strstr(plugin_entry_list, plugin_entry)) {
        free(plugin_entry);
        return 0;
    }
    FILE *init = fopen(zpm_init, "r+");
    fseek(init, -54, SEEK_END);
    strcat(plugin_entry, "fpath+=");
    if (plugin_name[0] == '/') {
        strcat(plugin_entry, plugin_name);
    } else {
        char* plugin_path = generate_plugin_path(plugin_name);
        strcat(plugin_entry, plugin_path);
        strcat(plugin_entry, "/");
        free(plugin_path);
    }
    strcat(plugin_entry, "\nPATH=");
    if (plugin_name[0] == '/') {
        strcat(plugin_entry, plugin_name);
    } else {
        char* plugin_path = generate_plugin_path(plugin_name);
        strcat(plugin_entry, plugin_path);
        free(plugin_path);
    }
    strcat(plugin_entry, ":$PATH");
    strcat(plugin_entry, "\nautoload -Uz compinit; compinit -iCd $HOME/.zcompdump\n");

    int status = fwrite(plugin_entry, strlen(plugin_entry), 1, init);
    fclose(init);
    free(plugin_entry);
    return status;
}

char* get_plugin_list_path() {
    char* plugin_list = malloc(PATH_MAX);

    strcpy(plugin_list, getenv("HOME"));
    strcat(plugin_list, "/.zpm/.plugin_list");

    return plugin_list;
}

int plugin_list_add_item(char* plugin_name) {
    int ret;
    char plugin_item[PATH_MAX];
    char plugin_item_list[PATH_MAX];

    if (!strncmp(plugin_name, "github.com", 11)) {
        strcpy(plugin_item, plugin_name + 11);
    } else {
        strcpy(plugin_item, plugin_name);
    }
    strcat(plugin_item, "\n");

    char* plugin_list = get_plugin_list_path();
    FILE* store = fopen(plugin_list,"ab+");

    if (!store) {
        return -1;
    }



    fread(plugin_item_list, 1, PATH_MAX, store);
    if (strstr(plugin_item_list, plugin_item)) {
        return 0;
    }

    ret = fwrite(plugin_item, strlen(plugin_item), 1, store);
    free(plugin_list);
    fclose(store);
    return ret;
}

/* https://gist.github.com/JonathonReinhart/8c0d90191c38af2dcadb102c4e202950 */
int mkdir_p(const char *path) {
    /* Adapted from http://stackoverflow.com/a/2336245/119527 */
    const size_t len = strlen(path);
    char _path[PATH_MAX];
    char *p;

    errno = 0;

    /* Copy string so its mutable */
    if (len > sizeof(_path)-1) {
        errno = ENAMETOOLONG;
        return -1;
    }
    strcpy(_path, path);

    /* Iterate the string */
    for (p = _path + 1; *p; p++) {
        if (*p == '/') {
            /* Temporarily truncate */
            *p = '\0';

            if (mkdir(_path, S_IRWXU) != 0) {
                if (errno != EEXIST)
                    return -1;
            }

            *p = '/';
        }
    }

    if (mkdir(_path, S_IRWXU) != 0) {
        if (errno != EEXIST)
            return -1;
    }

    return 0;
}

int rmdir_r(const char *path) {
    char c[PATH_MAX];
    strcpy(c, "rm -rf ");
    strcat(c, path);
    system(c);

    char* base = strrchr(path, '/');
    strcpy(c, "rmdir ");
    strncat(c, path, strlen(path) - strlen(base));
    strcat(c , " 2> /dev/null");
    system(c);
    return 0;
}

int local_clone_exists(char* plugin_name) {
    if (plugin_name[0] == '/') {
        return 0;
    }

    char* plugin_path = generate_plugin_path(plugin_name);

    if (!plugin_path) {
        return -1;
    }

    DIR* plugin_directory = opendir(plugin_path);

    if (plugin_directory != NULL) {
        closedir(plugin_directory);
        free(plugin_path);
        return 0;
    }
    /*int status = */mkdir_p(plugin_path);
    free(plugin_path);

    return 1;
}

int plugin_entry_exists(char* plugin_name) {
    int ret = 0;
    char* list = get_plugin_list_path();

    FILE *file = fopen(list, "r");
    if (!file) {
        return 0;
    }

    char entry[PATH_MAX];
    while (fgets(entry, PATH_MAX, file)) {
        if(!strncmp(entry, plugin_name, strlen(plugin_name))) {
            ret = 1;
            break;
        }
    }

    fclose(file);
    free(list);
    return ret;
}

char* generate_repository_url(char* plugin_name) {
    char* url = malloc(PATH_MAX);
    char* tmp = strstr(plugin_name, "/");

    if (!tmp) {
      return NULL;
      free(url);
    }
    tmp = strstr(tmp + 1, "/");
    if (!tmp) { 
        strcpy(url, "https://github.com/");
    } else {
        strcpy(url, "https://");
    }
    strcat(url, plugin_name);

    return url;
}

int locally_clone_plugin(char* plugin_name) {
    int ret;
    char* repository_url = generate_repository_url(plugin_name);
    char* clone_destination = generate_plugin_path(plugin_name);
    char  command[PATH_MAX];
    if (!repository_url) {
      return -1;
    }
    strcpy(command, "git clone --recursive --depth=1 ");
    strcat(command, repository_url);
    strcat(command, " ");
    strcat(command, clone_destination);
    strcat(command, " > /dev/null 2>&1");

    free(repository_url);
    free(clone_destination);
    ret = system(command);
    return ret;
}

char* get_zpm_plugin_list() {
    char* plugin_list_path = get_plugin_list_path();
    char* listing = malloc(PATH_MAX);
    FILE* list;

    list = fopen(plugin_list_path, "rb");
    if (list != NULL) {
        fread(listing, 1, PATH_MAX, list);
        fclose(list);
    }
    if (!list || !strcmp(listing, "")) {
        strcpy(listing, "Nothing to show.");
    }

    free(plugin_list_path);

    return listing;
}

int plugins_update_local_clone() {
    int ret;
    char  command[PATH_MAX];
    char* listing = get_zpm_plugin_list();
    char* plugin_name = strtok(listing, "\n");

    if (!strcmp(listing, "Nothing to show.")) {
      printf("Nothing to update.");
      return -1;
    }
    printf("Updating plugins ...\n");
    while (plugin_name) {
        if (plugin_name[0] == '/') {
            plugin_name = strtok(NULL, "\n");
            continue;
        }
        strcpy(command, "cd ~/.zpm/plugins/");
        strcat(command, plugin_name);
        strcat(command, "; git pull");
        printf("Updating %s...\n", plugin_name);
        ret = system(command);
        plugin_name = strtok(NULL, "\n");
    }
    free(listing);
    return ret;
}

void usage() {
    printf("%s\n", "Usage:\n\tzpm \"zsh-users/zsh-syntax-highlighting\"");
    printf("%s\n", "\tzpm disable \"zsh-users/zsh-syntax-highlighting\"");
    printf("%s\n", "\tzpm remove \"zsh-users/zsh-syntax-highlighting\"");
    printf("%s\n", "\nAvailable commands:\n\tzpm reset\n\tzpm list");
    printf("%s\n", "\tzpm update\n\tzpm help\n\tzpm save");
}

char* plugin_get_hash(char* plugin_name) {
    FILE *fp;
    struct stat s;
    char* plugin_path = generate_plugin_path(plugin_name);
    char plugin_hash[PATH_MAX];
    char command[PATH_MAX];

    strcat(plugin_path, "/.git");
    if (stat(plugin_path, &s) == -1 || !S_ISDIR(s.st_mode)) {
        free(plugin_path);
        return NULL;
    }
    memset(command, 0, PATH_MAX);
    strcat(command, "git --git-dir=");
    strcat(command, plugin_path);
    strcat(command, " rev-parse --short HEAD");
    fp = popen(command, "r");
    if (!fp) {
        free(plugin_path);
        printf("Failed to run command\n" );
        return NULL;
    }
    fgets(plugin_hash, sizeof(plugin_hash), fp);
    pclose(fp);
    free(plugin_path);
    return strdup(plugin_hash);
}

int plugin_print_list() {
    char* listing = get_zpm_plugin_list();
    char* plugin_name = strtok(listing, "\n");

    if (!strcmp(listing, "Nothing to show.")) {
        printf("%s\n", listing);
        return 1;
    }
    while (plugin_name) {
       char* hash = plugin_get_hash(plugin_name);
       printf("%s", plugin_name);
       if (hash) {
           printf(" %s", hash);
           free(hash);
       } else {
          printf("\n");
       }
       plugin_name = strtok(NULL, "\n");
    }
    free(listing);
    return 0;
}

int plugin_print_script() {
    char entry[PATH_MAX];
    FILE* store = fopen(zpm_init, "r");

    if (!store) {
        printf("Could not open \"%s\". Check the file exists and can be read.\n", zpm_init);
        return 1;
    }

    memset(entry, 0, PATH_MAX);
    while (fgets(entry, PATH_MAX, store)) {
        char plugin[PATH_MAX];
        strncpy(plugin, entry, strlen(entry) -1);
        printf("zpm \"%s\"\n", plugin);
    }
    fclose(store);
    return 0;
}

int plugin_remove_entry(char* plugin_name, char* file_name) {
    if (!plugin_name) {
        printf("remove/uninstall command needs argument.\n");
        return 1;
    }
    FILE* store = fopen(file_name, "r");
    if (!store) {
        printf("Could not open \"%s\". Check the file exists and can be read.\n", file_name);
        return 1;
    }

    FILE* tmp = fopen("/tmp/.zpm_tmp", "w");
    if (!tmp) {
        printf("Could not open \"%s\". Check the file can be written.\n", "/tmp/.zpm_tmp");
        return 1;
    }

    char entry[PATH_MAX];

    while (fgets(entry, PATH_MAX, store)) {
        if (!strstr(entry, plugin_name)) {
            fwrite(entry, strlen(entry), 1, tmp);
        }
    }

    fclose(store);
    fclose(tmp);

    tmp = fopen("/tmp/.zpm_tmp", "r");
    store =fopen(file_name, "w");
    while (fgets(entry, PATH_MAX, tmp)) {
        fwrite(entry, strlen(entry), 1, store);
    }
    fclose(store);
    fclose(tmp);
    unlink("/tmp/.zpm_tmp");
    return 0;
}

int plugin_remove(char* plugin_name, int uninstall) {
    if (!plugin_name) {
        printf("remove needs an argument.\n");
        return 1;
    }
    if (!plugin_entry_exists(plugin_name)) {
        printf("Plugin \"%s\" is not installed.\n", plugin_name);
        return 1;
    }
    plugin_remove_entry(plugin_name, zpm_init);
    plugin_remove_entry(plugin_name, get_plugin_list_path());
    if (uninstall && plugin_name[0] != '/') {
        char plugin_path[PATH_MAX];

        strcpy(plugin_path, getenv("HOME"));
        strcat(plugin_path, "/.zpm/plugins/");
        strcat(plugin_path, plugin_name);
        rmdir_r(plugin_path);
    }
    return 0;
}

void zpm_config_init() {
    char* home = getenv("HOME");
    char zpm_conf[PATH_MAX];

    strcpy(zpm_conf, getenv("HOME"));
    strcat(zpm_conf, "/.zpm");
    mkdir(zpm_conf, S_IRWXU);

    strcpy(zpm_init, home);
    strcat(zpm_init, "/.zpm-init.zsh");

    strcpy(zpm_list, home);
    strcat(zpm_list, "/.zpm/plugin_list");
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        usage();
        return 1;
    }

    char* plugin_name_or_command = argv[1];
    char* plugin_name = NULL;

    zpm_config_init();

    if (strstr(plugin_name_or_command, "reset")) {
        char* plugin_list = get_plugin_list_path();
        unlink(plugin_list);
        unlink(zpm_init);
        free(plugin_list);
        return 0;

    } else if (strstr(plugin_name_or_command, "update")) {
        plugins_update_local_clone();
        return 0;

    } else if (strstr(plugin_name_or_command, "list")) {
        return plugin_print_list();
    } else if (strstr(plugin_name_or_command, "save")) {
        return plugin_print_script();
    } else if (strstr(plugin_name_or_command, "disable")) {
        return plugin_remove(argv[2], 0);
    } else if (strstr(plugin_name_or_command, "remove")) {
        return plugin_remove(argv[2], 1);
    } else if (strstr(plugin_name_or_command, "help")) {
        usage();
        return 0;

    } else {
        if (plugin_entry_exists(plugin_name_or_command)) {
            printf("Plugin \"%s\" already installed.\n", plugin_name_or_command);
            return 1;
        }
        plugin_name = malloc(PATH_MAX);
        strcpy(plugin_name, plugin_name_or_command);
    }

    int status = strstr(plugin_name, "/") ? 0 : -1;
    char install[PATH_MAX];
    strcpy(install, "Installing ");
    strcat(install, plugin_name);
    strcat(install, "... ");
    printf("%s", install);
    fflush(stdout);

    if (local_clone_exists(plugin_name) == 1) {
        status = locally_clone_plugin(plugin_name);
    }

    if (status == 0) {
        printf("%s\n", "Done.");
        generate_plugin_entry(plugin_name);
        plugin_list_add_item(plugin_name);
    } else {
        printf("%s\n", "Error!");
    }

    free(plugin_name);
    return status;
}

