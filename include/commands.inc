/*
 * X-macro list of changelogger commands.
 *
 * Do not include this file directly. Include it where a COMMAND macro with
 * this signature is defined, then #undef it afterwards:
 *
 *   #define COMMAND(upper, lower, usage, description, help) ...
 *   #include "commands.inc"
 *   #undef COMMAND
 *
 * upper:       enum suffix        (COMMAND_ADD)
 * lower:       function suffix     (command_add, handle_add) and command name
 * usage:       left column of the COMMANDS help section
 * description: right column of the COMMANDS help section
 * help:        function printing that command's own help
 */

COMMAND(INIT,     init,     "init",                     "Initializes the changelog",                  help)
COMMAND(ADD,      add,      "add <msg> [<options>]",    "Adds an entry",                              add_help)
COMMAND(LIST,     list,     "list [<options>]",         "Lists all entries",                          list_help)
COMMAND(EDIT,     edit,     "edit <options>",           "Edit any of the unreleased entries",         edit_help)
COMMAND(DELETE,   delete,   "delete <options>",         "Delete any of the unreleased entries",       delete_help)
COMMAND(SET,      set,      "set <options>",            "Sets a variable",                            set_help)
COMMAND(GET,      get,      "get <options>",            "Returns a variable",                         get_help)
COMMAND(RELEASE,  release,  "release <options>",        "Creates a new release",                      release_help)
COMMAND(PUSH,     push,     "push <options>",           "Push an unpushed release to Github",         push_help)
COMMAND(EXPORT,   export,   "export [<options>]",       "Exports the CHANGELOG.md file",              export_help)
COMMAND(GENERATE, generate, "generate <value>",         "Generate various files",                     generate_help)
COMMAND(IMPORT,   import,   "import <options>",         "Import CHANGELOG.md file into the database", import_help)
COMMAND(UPDATE,   update,   "update",                   "Updates changelogger to the latest version", update_help)
