function _changelogger_spec_1
set 1 $argv[1]
__fish_complete_path "$1"
end

function _changelogger
    set COMP_LINE (commandline --cut-at-cursor)

    set COMP_WORDS
    echo $COMP_LINE | read --tokenize --array COMP_WORDS
    if string match --quiet --regex '.*\s$' $COMP_LINE
        set COMP_CWORD (math (count $COMP_WORDS) + 1)
    else
        set COMP_CWORD (count $COMP_WORDS)
    end

    set literals "-s" "--yank" "major" "--commits" "export" "init" "--all" "add" "json" "-h" "-s" "changed" "fixed" "false" "-s" "minor" "--push" "-U" "-F" "--title" "push" "removed" "added" "delete" "patch" "-V" "--always-export" "get" "remote" "-h" "config" "export" "yaml" "markdown" "security" "json" "true" "major" "removed" "--version-full" "--unyank" "--yes" "-C" "markdown" "-y" "-V" "--help" "edit" "yml" "--file" "--version" "--format" "-A" "deprecated" "--remote-repo" "yaml" "-f" "-V" "--help" "set" "editor" "added" "--status" "fixed" "--config-dir" "-V" "--editor" "minor" "--p" "-B" "changed" "patch" "--releases" "config" "version" "security" "--asset" "autocomplete" "-Y" "-A" "md" "list" "-r" "--always-push" "release" "man" "--yes" "push" "-t" "deprecated" "yml" "md" "generate" "-N" "-A" "--new" "import" "-v"

    set descriptions
    set descriptions[1] "Specify the status of the entry (default is 'added')"
    set descriptions[5] "Always export CHANGELOG.md"
    set descriptions[6] "Initializes the changelog"
    set descriptions[8] "Adds an entry"
    set descriptions[10] "Prints the help message"
    set descriptions[11] "Set the status to filter the list"
    set descriptions[15] "Specify new status"
    set descriptions[17] "Push the release on github"
    set descriptions[18] "Set a release as not YANKED"
    set descriptions[19] "Specify in which format to export"
    set descriptions[21] "Always push the release on Github"
    set descriptions[24] "Delete any of the unreleased entries"
    set descriptions[26] "Specify new version"
    set descriptions[27] "Specify whether to export the CHANGELOG.md file"
    set descriptions[28] "Returns a variable"
    set descriptions[29] "The remote git repo url"
    set descriptions[30] "Prints this message"
    set descriptions[31] "The path to the configuration file"
    set descriptions[32] "Exports the CHANGELOG.md file"
    set descriptions[43] "Add unreleased commit messages"
    set descriptions[45] "Skip the confirmation message"
    set descriptions[46] "Set the version of the project"
    set descriptions[48] "Edit any of the unreleased entries"
    set descriptions[53] "List all entries and have them available for deletion"
    set descriptions[55] "Specify the url of the remote repo"
    set descriptions[57] "File to import"
    set descriptions[58] "Set the version to filter the list"
    set descriptions[60] "Sets a variable"
    set descriptions[61] "Editor to use when editing a file is needed"
    set descriptions[65] "Specify the configuration file path"
    set descriptions[66] "Specify the version of the release you want to push"
    set descriptions[67] "Specify the editor to use"
    set descriptions[70] "Include an asset in the release"
    set descriptions[74] "A starting point for your config file (~/.config/.changelogger.yml)"
    set descriptions[75] "Latest release"
    set descriptions[78] "Autocomplete for the active shell"
    set descriptions[79] "Set a release as YANKED"
    set descriptions[80] "List all entries and have them available for editing"
    set descriptions[82] "Lists entries"
    set descriptions[83] "List only the releases"
    set descriptions[84] "Specify whether to immediately push the release"
    set descriptions[85] "Creates a new release"
    set descriptions[86] "Man page for changelogger"
    set descriptions[88] "Push an unpushed release to Github"
    set descriptions[89] "Specify new title/message"
    set descriptions[93] "Generate various files"
    set descriptions[95] "Print all variables"
    set descriptions[96] "Creates a new release"
    set descriptions[97] "Import CHANGELOG.md file into the database"
    set descriptions[98] "Prints the project version"

    set literal_transitions
    set literal_transitions[1] "set inputs 59 48 24 6 28 82 85 8 51 32 60 98 88 10 93 97; set tos 2 3 4 2 5 6 7 8 2 9 10 2 11 2 12 13"
    set literal_transitions[3] "set inputs 20 63 89 40 80 7 15 26 30; set tos 15 21 15 16 2 2 20 16 22"
    set literal_transitions[4] "set inputs 30 7 53; set tos 22 2 2"
    set literal_transitions[5] "set inputs 59 61 75 21 5 7 31 29 95 30; set tos 2 2 2 2 2 2 2 2 2 2"
    set literal_transitions[6] "set inputs 59 58 73 63 11 40 83 30; set tos 2 16 2 21 20 16 2 2"
    set literal_transitions[7] "set inputs 59 2 77 45 30 79 17 41 94 18 96 69 70; set tos 2 16 17 19 2 16 2 16 18 16 14 2 17"
    set literal_transitions[8] "set inputs 59 1 4 43 63 30; set tos 2 20 2 2 21 2"
    set literal_transitions[9] "set inputs 59 19 52 30; set tos 2 23 26 2"
    set literal_transitions[10] "set inputs 59 46 65 40 67 27 55 84 30; set tos 2 16 17 16 27 24 25 24 2"
    set literal_transitions[11] "set inputs 59 77 66 87 40 70 45 30; set tos 2 17 16 2 16 17 2 2"
    set literal_transitions[12] "set inputs 59 74 78 86 30; set tos 2 2 2 2 2"
    set literal_transitions[13] "set inputs 59 50 57 30; set tos 2 17 17 2"
    set literal_transitions[14] "set inputs 68 72 38; set tos 2 2 2"
    set literal_transitions[18] "set inputs 38 68 72; set tos 2 2 2"
    set literal_transitions[19] "set inputs 87; set tos 2"
    set literal_transitions[20] "set inputs 64 90 76 62 39 71; set tos 2 2 2 2 2 2"
    set literal_transitions[21] "set inputs 39 62 76 64 90 71; set tos 2 2 2 2 2 2"
    set literal_transitions[22] "set inputs 59; set tos 2"
    set literal_transitions[23] "set inputs 56 92 44 36 91; set tos 2 2 2 2 2"
    set literal_transitions[24] "set inputs 37 14; set tos 2 2"
    set literal_transitions[26] "set inputs 91 36 92 44 56; set tos 2 2 2 2 2"

    set match_anything_transitions_from 25 27 16 15 17
    set match_anything_transitions_to 2 2 2 2 2

    set state 1
    set word_index 2
    while test $word_index -lt $COMP_CWORD
        set -- word $COMP_WORDS[$word_index]

        if set --query literal_transitions[$state] && test -n $literal_transitions[$state]
            set --erase inputs
            set --erase tos
            eval $literal_transitions[$state]

            if contains -- $word $literals
                set literal_matched 0
                for literal_id in (seq 1 (count $literals))
                    if test $literals[$literal_id] = $word
                        set index (contains --index -- $literal_id $inputs)
                        set state $tos[$index]
                        set word_index (math $word_index + 1)
                        set literal_matched 1
                        break
                    end
                end
                if test $literal_matched -ne 0
                    continue
                end
            end
        end

        if set --query match_anything_transitions_from[$state] && test -n $match_anything_transitions_from[$state]
            set index (contains --index -- $state $match_anything_transitions_from)
            set state $match_anything_transitions_to[$index]
            set word_index (math $word_index + 1)
            continue
        end

        return 1
    end

    if set --query literal_transitions[$state] && test -n $literal_transitions[$state]
        set --erase inputs
        set --erase tos
        eval $literal_transitions[$state]
        for literal_id in $inputs
            if test -n $descriptions[$literal_id]
                printf '%s\t%s\n' $literals[$literal_id] $descriptions[$literal_id]
            else
                printf '%s\n' $literals[$literal_id]
            end
        end
    end

    set specialized_command_states 17
    set specialized_command_ids 1
    if contains $state $specialized_command_states
        set index (contains --index $state $specialized_command_states)
        set function_id $specialized_command_ids[$index]
        set function_name _changelogger_spec_$function_id
        set --erase inputs
        set --erase tos
        set lines (eval $function_name $COMP_WORDS[$COMP_CWORD])
        for line in $lines
            printf '%s\n' $line
        end
    end

    return 0
end

complete --command changelogger --no-files --arguments "(_changelogger)"
