#include "parser.h"
#include "status.h"
#include <stdlib.h>
#include <string.h>
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include <cjson/cJSON.h>

char* get_repository_url(const char* url) {
    char* repo_url = malloc(strlen(url) + 1); // Allocate memory for the result
    if (repo_url == NULL) {
        return NULL; // Failed to allocate memory
    }

    // Find the index of the third '/' character
    int fifth_slash_index = -1;
    int slash_count = 0;
    for (int i = 0; url[i]; i++) {
        if (url[i] == '/') {
            slash_count++;
            if (slash_count == 5) {
                fifth_slash_index = i;
                break;
            }
        }
    }

    // Copy the first three parts of the URL
    strncpy(repo_url, url, fifth_slash_index + 1);
    repo_url[fifth_slash_index + 1] = '\0'; // Terminate the string

    return repo_url;
}

Changelog parse(const char* file)
{
    char* command = clib_format_text("/usr/bin/clparse -f json %s", file);
    char* json_string = clib_execute_command(command);

    Changelog changelog = {0};
    cJSON *json = cJSON_Parse(json_string);
    if (json == NULL) {
        printf("Error parsing JSON\n");
        return changelog;
    }

    cJSON* title = cJSON_GetObjectItem(json, "title");
    cJSON* description = cJSON_GetObjectItem(json, "description");
    cJSON* releases = cJSON_GetObjectItem(json, "releases");

    if(cJSON_IsString(title) && title->valuestring != NULL)
        changelog.title = strdup(title->valuestring);
    if(cJSON_IsString(description) && description->valuestring != NULL)
        changelog.description = strdup(description->valuestring);

    changelog.releases = malloc(sizeof(ParserRelease) * 100);
    changelog.release_count = 0;

   if (cJSON_IsArray(releases)) {
        cJSON *release;
        cJSON_ArrayForEach(release, releases) {
            cJSON *version = cJSON_GetObjectItem(release, "version");
            cJSON *link = cJSON_GetObjectItem(release, "link");
            cJSON *date = cJSON_GetObjectItem(release, "date");
            cJSON *changes = cJSON_GetObjectItem(release, "changes");
            cJSON *yanked = cJSON_GetObjectItem(release, "yanked");
            
            if(cJSON_IsString(version) && version->valuestring != NULL)
                changelog.releases[changelog.release_count].version = strdup(version->valuestring);
            else if(version->valuestring == NULL)
                changelog.releases[changelog.release_count].version = NULL;

            if(cJSON_IsString(link) && link->valuestring != NULL){
                changelog.releases[changelog.release_count].link = strdup(link->valuestring);
                if(changelog.release_count == 1){
                    changelog.remote_repo = get_repository_url(link->valuestring);
                }
            } else if(link->valuestring == NULL)
                changelog.releases[changelog.release_count].link = NULL;

            if(cJSON_IsString(date) && date->valuestring != NULL)
                changelog.releases[changelog.release_count].date = strdup(date->valuestring);
            else if(date->valuestring == NULL)
                changelog.releases[changelog.release_count].date = NULL;

            changelog.releases[changelog.release_count].yanked = yanked->valueint;
            changelog.releases[changelog.release_count].changes = malloc(sizeof(Change) * 100);
            
            if (cJSON_IsArray(changes)) {
                cJSON *change;
                cJSON_ArrayForEach(change, changes) {
                    cJSON *added = cJSON_GetObjectItem(change, "added");
                    cJSON *changed = cJSON_GetObjectItem(change, "changed");
                    cJSON *fixed = cJSON_GetObjectItem(change, "fixed");
                    cJSON *removed = cJSON_GetObjectItem(change, "removed");
                    cJSON *deprecated = cJSON_GetObjectItem(change, "deprecated");
                    cJSON *security = cJSON_GetObjectItem(change, "security");


                    if (cJSON_IsString(added) && (added->valuestring != NULL)) {
                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .status = "added";

                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .message = strdup(added->valuestring);
                    }
                    if (cJSON_IsString(changed) && (changed->valuestring != NULL)) {
                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .status = "changed";

                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .message = strdup(changed->valuestring);
                    }
                    if (cJSON_IsString(fixed) && (fixed->valuestring != NULL)) {
                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .status = "fixed";

                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .message = strdup(fixed->valuestring);
                    }
                    if (cJSON_IsString(removed) && (removed->valuestring != NULL)) {
                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .status = "removed";

                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .message = strdup(removed->valuestring);
                    }
                    if (cJSON_IsString(deprecated) && (deprecated->valuestring != NULL)) {
                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .status = "deprecated";

                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .message = strdup(deprecated->valuestring);
                    }
                    if (cJSON_IsString(security) && (security->valuestring != NULL)) {
                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .status = "security";

                        changelog
                            .releases[changelog.release_count]
                            .changes[changelog.releases[changelog.release_count].change_count]
                            .message = strdup(security->valuestring);
                    }

                    changelog.releases[changelog.release_count].change_count++;
                }
            }

            changelog.release_count++;
        }
    }

    // Clean up
    cJSON_Delete(json); 
    return changelog;
}

void free_changelog(Changelog* changelog)
{
    for(size_t i = 0; i < changelog->release_count; ++i){
        free((char*) changelog->releases[i].date);
        free((char*) changelog->releases[i].link);
        free((char*) changelog->releases[i].version);

        for(size_t j = 0; j < changelog->releases[i].change_count; ++j){
            free((char*) changelog->releases[i].changes[j].message);
        }
        free(changelog->releases[i].changes);
    }
    free(changelog->releases);
}

void log_changelog(Changelog changelog)
{
    for(size_t i = 0; i < changelog.release_count; ++i){
        ParserRelease r = changelog.releases[i];
        printf("Release v%s\n", (r.version == NULL) ? "Unreleased" : r.version);
        printf("  Link: %s\n", (r.link == NULL) ? "NULL" : r.link);
        printf("  Yanked: %s\n", BOOL(r.yanked));
        printf("  Date: %s\n", (r.date == NULL) ? "NULL" : r.date);
        printf("  Changes:\n");
        for(size_t j = 0; j < r.change_count; ++j){
            Change c = r.changes[j];
            printf("    Message: %s\n", c.message);
            printf("    Status: %s\n\n", c.status);
        }
        printf("\n");
    }
}
