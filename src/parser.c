#include "parser.h"
#include "status.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "extern/fs.h"
#include "extern/logging.h"
#include "extern/str.h"
#include <stdbool.h>
#include <cjson/cJSON.h>
#include "utils.h"

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

/*
 * clparse only understands "## 1.0.0 - date" release headers while
 * changelogger writes the Keep a Changelog style "## [1.0.0] - date".
 * Write a copy of `file` without the brackets to a temporary file and
 * return its path. The caller must unlink() and free() it.
 */
char* clparse_compatible_copy(const char* file)
{
    char* content = fs_read_file(file, "r");
    if (content == NULL) {
        return NULL;
    }

    size_t len = strlen(content);
    char* out = malloc(len + 1);
    if (out == NULL) {
        free(content);
        return NULL;
    }

    /* Every transformation only removes characters, so `out` always fits */
    size_t out_len = 0;
    const char* line = content;
    while (*line != '\0') {
        const char* newline = strchr(line, '\n');
        size_t line_len = (newline != NULL) ? (size_t)(newline - line) : strlen(line);

        if (line_len > 4 && strncmp(line, "## [", 4) == 0) {
            const char* close = memchr(line + 4, ']', line_len - 4);
            if (close != NULL) {
                out_len += (size_t)sprintf(out + out_len, "## %.*s",
                                           (int)(close - (line + 4)), line + 4);
                line = close + 1;
                continue;
            }
        }

        memcpy(out + out_len, line, line_len);
        out_len += line_len;
        line += line_len;

        if (newline != NULL) {
            out[out_len++] = '\n';
            line++;
        }
    }
    out[out_len] = '\0';
    free(content);

    const char* tmpdir = getenv("TMPDIR");
    if (tmpdir == NULL || tmpdir[0] == '\0') {
        tmpdir = "/tmp";
    }
    char* path = str_format("%s/changelogger-XXXXXX", tmpdir);
    if (path == NULL) {
        free(out);
        return NULL;
    }

    int fd = mkstemp(path);
    if (fd < 0) {
        free(path);
        free(out);
        return NULL;
    }

    FILE* fp = fdopen(fd, "w");
    if (fp == NULL) {
        close(fd);
        unlink(path);
        free(path);
        free(out);
        return NULL;
    }

    size_t written = fwrite(out, 1, out_len, fp);
    int close_status = fclose(fp);
    free(out);

    if (written != out_len || close_status != 0) {
        unlink(path);
        free(path);
        return NULL;
    }

    return path;
}

Changelog parse(const char* file)
{
    Changelog changelog = {0};

    char* clparse = find_executable("clparse");
    if (clparse == NULL) {
        ERRO("clparse not found in PATH. Install it from https://github.com/marcaddeo/clparse");
        return changelog;
    }

    char* compatible = clparse_compatible_copy(file);
    if (compatible == NULL) {
        ERRO("Failed to read %s", file);
        free(clparse);
        return changelog;
    }

    char* quoted_file = shell_quote(compatible);
    char* command = str_format("%s -f json %s", clparse, quoted_file);
    free(clparse);
    free(quoted_file);

    char* json_string = NULL;
    int status = execute_command_capture(command, &json_string);
    free(command);

    unlink(compatible);
    free(compatible);

    if (status != 0 || json_string == NULL) {
        free(json_string);
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
        printf("  Yanked: %s\n", r.yanked ? "true" : "false");
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
