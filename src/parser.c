#include "parser.h"
#include "status.h"
#include <stdlib.h>
#include <string.h>
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include <cjson/cJSON.h>

#define INITIAL_CAPACITY 16

char* get_repository_url(const char* url) {
    if (url == NULL) {
        return NULL;
    }

    char* repo_url = malloc(strlen(url) + 1);
    if (repo_url == NULL) {
        return NULL;
    }

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

    if (fifth_slash_index < 0) {
        free(repo_url);
        return strdup(url);
    }

    strncpy(repo_url, url, (size_t)fifth_slash_index + 1);
    repo_url[fifth_slash_index + 1] = '\0';

    return repo_url;
}

static void push_change(ParserRelease* release, size_t* change_capacity, const char* status, const char* message)
{
    if (release->change_count >= *change_capacity) {
        *change_capacity *= 2;
        release->changes = realloc(release->changes, sizeof(Change) * *change_capacity);
    }

    size_t index = release->change_count++;
    release->changes[index].status = status;
    release->changes[index].message = strdup(message);
}

static void parse_change_fields(ParserRelease* release, size_t* change_capacity, cJSON* change)
{
    cJSON *added = cJSON_GetObjectItem(change, "added");
    cJSON *changed = cJSON_GetObjectItem(change, "changed");
    cJSON *fixed = cJSON_GetObjectItem(change, "fixed");
    cJSON *removed = cJSON_GetObjectItem(change, "removed");
    cJSON *deprecated = cJSON_GetObjectItem(change, "deprecated");
    cJSON *security = cJSON_GetObjectItem(change, "security");

    if (cJSON_IsString(added) && added->valuestring != NULL) {
        push_change(release, change_capacity, "added", added->valuestring);
    }
    if (cJSON_IsString(changed) && changed->valuestring != NULL) {
        push_change(release, change_capacity, "changed", changed->valuestring);
    }
    if (cJSON_IsString(fixed) && fixed->valuestring != NULL) {
        push_change(release, change_capacity, "fixed", fixed->valuestring);
    }
    if (cJSON_IsString(removed) && removed->valuestring != NULL) {
        push_change(release, change_capacity, "removed", removed->valuestring);
    }
    if (cJSON_IsString(deprecated) && deprecated->valuestring != NULL) {
        push_change(release, change_capacity, "deprecated", deprecated->valuestring);
    }
    if (cJSON_IsString(security) && security->valuestring != NULL) {
        push_change(release, change_capacity, "security", security->valuestring);
    }
}

Changelog parse(const char* file)
{
    Changelog changelog = {0};

    char* command = clib_format_text("/usr/bin/clparse -f json %s", file);
    char* json_string = clib_execute_command(command);
    free(command);

    if (json_string == NULL) {
        ERRO("Failed to parse %s", file);
        return changelog;
    }

    cJSON *json = cJSON_Parse(json_string);
    free(json_string);
    if (json == NULL) {
        ERRO("Failed to parse JSON from %s", file);
        return changelog;
    }

    cJSON* title = cJSON_GetObjectItem(json, "title");
    cJSON* description = cJSON_GetObjectItem(json, "description");
    cJSON* releases = cJSON_GetObjectItem(json, "releases");

    if (!cJSON_IsArray(releases)) {
        ERRO("Invalid changelog format: missing releases array");
        cJSON_Delete(json);
        return changelog;
    }

    if (cJSON_IsString(title) && title->valuestring != NULL) {
        changelog.title = strdup(title->valuestring);
    }
    if (cJSON_IsString(description) && description->valuestring != NULL) {
        changelog.description = strdup(description->valuestring);
    }

    size_t release_capacity = INITIAL_CAPACITY;
    changelog.releases = calloc(release_capacity, sizeof(ParserRelease));
    if (changelog.releases == NULL) {
        cJSON_Delete(json);
        return changelog;
    }

    cJSON *release;
    cJSON_ArrayForEach(release, releases) {
        if (changelog.release_count >= release_capacity) {
            release_capacity *= 2;
            changelog.releases = realloc(changelog.releases, sizeof(ParserRelease) * release_capacity);
        }

        ParserRelease* current = &changelog.releases[changelog.release_count];
        memset(current, 0, sizeof(ParserRelease));

        cJSON *version = cJSON_GetObjectItem(release, "version");
        cJSON *link = cJSON_GetObjectItem(release, "link");
        cJSON *date = cJSON_GetObjectItem(release, "date");
        cJSON *changes = cJSON_GetObjectItem(release, "changes");
        cJSON *yanked = cJSON_GetObjectItem(release, "yanked");

        if (version != NULL && cJSON_IsString(version) && version->valuestring != NULL) {
            current->version = strdup(version->valuestring);
        }

        if (link != NULL && cJSON_IsString(link) && link->valuestring != NULL) {
            current->link = strdup(link->valuestring);
            if (changelog.remote_repo == NULL) {
                changelog.remote_repo = get_repository_url(link->valuestring);
            }
        }

        if (date != NULL && cJSON_IsString(date) && date->valuestring != NULL) {
            current->date = strdup(date->valuestring);
        }

        if (yanked != NULL && cJSON_IsBool(yanked)) {
            current->yanked = cJSON_IsTrue(yanked);
        }

        size_t change_capacity = INITIAL_CAPACITY;
        current->changes = malloc(sizeof(Change) * change_capacity);
        current->change_count = 0;

        if (cJSON_IsArray(changes)) {
            cJSON *change;
            cJSON_ArrayForEach(change, changes) {
                parse_change_fields(current, &change_capacity, change);
            }
        }

        changelog.release_count++;
    }

    changelog.valid = true;
    cJSON_Delete(json);
    return changelog;
}

void free_changelog(Changelog* changelog)
{
    free((char*) changelog->title);
    free((char*) changelog->description);
    free((char*) changelog->remote_repo);

    for (size_t i = 0; i < changelog->release_count; ++i) {
        free((char*) changelog->releases[i].date);
        free((char*) changelog->releases[i].link);
        free((char*) changelog->releases[i].version);

        for (size_t j = 0; j < changelog->releases[i].change_count; ++j) {
            free((char*) changelog->releases[i].changes[j].message);
        }
        free(changelog->releases[i].changes);
    }
    free(changelog->releases);
    changelog->releases = NULL;
    changelog->release_count = 0;
}

void log_changelog(Changelog changelog)
{
    for (size_t i = 0; i < changelog.release_count; ++i) {
        ParserRelease r = changelog.releases[i];
        printf("Release v%s\n", (r.version == NULL) ? "Unreleased" : r.version);
        printf("  Link: %s\n", (r.link == NULL) ? "NULL" : r.link);
        printf("  Yanked: %s\n", BOOL(r.yanked));
        printf("  Date: %s\n", (r.date == NULL) ? "NULL" : r.date);
        printf("  Changes:\n");
        for (size_t j = 0; j < r.change_count; ++j) {
            Change c = r.changes[j];
            printf("    Message: %s\n", c.message);
            printf("    Status: %s\n\n", c.status);
        }
        printf("\n");
    }
}
