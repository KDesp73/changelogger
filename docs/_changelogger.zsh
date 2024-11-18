#compdef changelogger

_changelogger_cmd_0 () {
    _path_files
}

_changelogger () {
    local -a literals=("-s" "--yank" "major" "--commits" "export" "init" "--all" "add" "json" "-h" "-s" "changed" "fixed" "false" "-s" "minor" "--push" "-U" "-F" "--title" "push" "removed" "added" "delete" "patch" "-V" "--always-export" "get" "remote" "-h" "config" "export" "yaml" "markdown" "security" "json" "true" "major" "removed" "--version-full" "--unyank" "--yes" "-C" "markdown" "-y" "-V" "--help" "edit" "yml" "--file" "update" "--format" "-A" "--version" "deprecated" "--remote-repo" "yaml" "-f" "-V" "--help" "set" "editor" "added" "--status" "fixed" "--config-dir" "-V" "--editor" "minor" "--p" "-B" "changed" "patch" "--releases" "config" "version" "security" "--asset" "autocomplete" "-Y" "-A" "md" "list" "-r" "--always-push" "release" "man" "--yes" "push" "-t" "deprecated" "yml" "md" "generate" "-N" "-A" "--new" "import" "-v")

    local -A descriptions
    descriptions[1]="Specify the status of the entry (default is 'added')"
    descriptions[5]="Always export CHANGELOG.md"
    descriptions[6]="Initializes the changelog"
    descriptions[8]="Adds an entry"
    descriptions[10]="Prints the help message"
    descriptions[11]="Set the status to filter the list"
    descriptions[15]="Specify new status"
    descriptions[17]="Push the release on github"
    descriptions[18]="Set a release as not YANKED"
    descriptions[19]="Specify in which format to export"
    descriptions[21]="Always push the release on Github"
    descriptions[24]="Delete any of the unreleased entries"
    descriptions[26]="Specify new version"
    descriptions[27]="Specify whether to export the CHANGELOG.md file"
    descriptions[28]="Returns a variable"
    descriptions[29]="The remote git repo url"
    descriptions[30]="Prints this message"
    descriptions[31]="The path to the configuration file"
    descriptions[32]="Exports the CHANGELOG.md file"
    descriptions[43]="Add unreleased commit messages"
    descriptions[45]="Skip the confirmation message"
    descriptions[46]="Set the version of the project"
    descriptions[48]="Edit any of the unreleased entries"
    descriptions[51]="Update changelogger to the latest version"
    descriptions[53]="List all entries and have them available for deletion"
    descriptions[56]="Specify the url of the remote repo"
    descriptions[58]="File to import"
    descriptions[59]="Set the version to filter the list"
    descriptions[61]="Sets a variable"
    descriptions[62]="Editor to use when editing a file is needed"
    descriptions[66]="Specify the configuration file path"
    descriptions[67]="Specify the version of the release you want to push"
    descriptions[68]="Specify the editor to use"
    descriptions[71]="Include an asset in the release"
    descriptions[75]="A starting point for your config file (~/.config/.changelogger.yml)"
    descriptions[76]="Latest release"
    descriptions[79]="Autocomplete for the active shell"
    descriptions[80]="Set a release as YANKED"
    descriptions[81]="List all entries and have them available for editing"
    descriptions[83]="Lists entries"
    descriptions[84]="List only the releases"
    descriptions[85]="Specify whether to immediately push the release"
    descriptions[86]="Creates a new release"
    descriptions[87]="Man page for changelogger"
    descriptions[89]="Push an unpushed release to Github"
    descriptions[90]="Specify new title/message"
    descriptions[94]="Generate various files"
    descriptions[96]="Print all variables"
    descriptions[97]="Creates a new release"
    descriptions[98]="Import CHANGELOG.md file into the database"
    descriptions[99]="Prints the project version"

    local -A literal_transitions
    literal_transitions[1]="([60]=2 [48]=3 [24]=4 [6]=2 [28]=5 [83]=6 [86]=7 [8]=8 [54]=2 [32]=9 [51]=2 [61]=10 [89]=11 [10]=2 [94]=12 [98]=13 [99]=2)"
    literal_transitions[3]="([20]=24 [64]=21 [90]=24 [40]=17 [81]=2 [7]=2 [15]=20 [26]=17 [30]=22)"
    literal_transitions[4]="([30]=22 [7]=2 [53]=2)"
    literal_transitions[5]="([60]=2 [62]=2 [76]=2 [21]=2 [5]=2 [7]=2 [31]=2 [29]=2 [96]=2 [30]=2)"
    literal_transitions[6]="([60]=2 [59]=17 [74]=2 [64]=21 [11]=20 [40]=17 [84]=2 [30]=2)"
    literal_transitions[7]="([60]=2 [2]=17 [78]=18 [45]=15 [30]=2 [80]=17 [17]=2 [41]=17 [95]=19 [18]=17 [97]=16 [70]=2 [71]=18)"
    literal_transitions[8]="([60]=2 [1]=20 [4]=2 [43]=2 [64]=21 [30]=2)"
    literal_transitions[9]="([60]=2 [19]=23 [52]=26 [30]=2)"
    literal_transitions[10]="([60]=2 [46]=17 [66]=18 [40]=17 [68]=27 [27]=25 [56]=14 [85]=25 [30]=2)"
    literal_transitions[11]="([60]=2 [78]=18 [67]=17 [88]=2 [40]=17 [71]=18 [45]=2 [30]=2)"
    literal_transitions[12]="([60]=2 [75]=2 [79]=2 [87]=2 [30]=2)"
    literal_transitions[13]="([60]=2 [50]=18 [58]=18 [30]=2)"
    literal_transitions[15]="([88]=2)"
    literal_transitions[16]="([69]=2 [73]=2 [38]=2)"
    literal_transitions[19]="([38]=2 [69]=2 [73]=2)"
    literal_transitions[20]="([65]=2 [91]=2 [77]=2 [63]=2 [39]=2 [72]=2)"
    literal_transitions[21]="([39]=2 [63]=2 [77]=2 [65]=2 [91]=2 [72]=2)"
    literal_transitions[22]="([60]=2)"
    literal_transitions[23]="([57]=2 [93]=2 [44]=2 [36]=2 [92]=2)"
    literal_transitions[25]="([37]=2 [14]=2)"
    literal_transitions[26]="([92]=2 [36]=2 [93]=2 [44]=2 [57]=2)"

    local -A match_anything_transitions
    match_anything_transitions=([27]=2 [18]=2 [14]=2 [17]=2 [24]=2)

    declare -A subword_transitions

    local state=1
    local word_index=2
    while [[ $word_index -lt $CURRENT ]]; do
        if [[ -v "literal_transitions[$state]" ]]; then
            local -A state_transitions
            eval "state_transitions=${literal_transitions[$state]}"

            local word=${words[$word_index]}
            local word_matched=0
            for ((literal_id = 1; literal_id <= $#literals; literal_id++)); do
                if [[ ${literals[$literal_id]} = "$word" ]]; then
                    if [[ -v "state_transitions[$literal_id]" ]]; then
                        state=${state_transitions[$literal_id]}
                        word_index=$((word_index + 1))
                        word_matched=1
                        break
                    fi
                fi
            done
            if [[ $word_matched -ne 0 ]]; then
                continue
            fi
        fi

        if [[ -v "match_anything_transitions[$state]" ]]; then
            state=${match_anything_transitions[$state]}
            word_index=$((word_index + 1))
            continue
        fi

        return 1
    done
    declare -A literal_transitions_level_0=([25]="37 14" [1]="48 24 6 28 83 86 8 32 51 61 89 10 94 98 99" [20]="65 91 77 63 39 72" [7]="45 30 80 17 18 97 71" [16]="69 73 38" [6]="59 11 84 30" [10]="46 66 68 27 56 85 30" [11]="67 71 45 30" [4]="30 53" [9]="19 30" [15]="88" [13]="58 30" [8]="1 43 30" [12]="75 79 87 30" [22]="60" [3]="90 81 15 26 30" [5]="62 76 21 5 31 29 96 30" [23]="57 93 44 36 92")
    declare -A literal_transitions_level_1=([1]="60 54" [21]="39 63 77 65 91 72" [11]="60 78 88 40" [6]="60 74 64 40" [10]="60 40" [7]="60 2 78 41 95 70" [4]="7" [9]="60 52" [3]="20 64 40 7" [19]="38 69 73" [13]="60 50" [8]="60 4 64" [12]="60" [5]="60 7" [26]="92 36 93 44 57")
    declare -A subword_transitions_level_0=()
    declare -A subword_transitions_level_1=()
    declare -A commands_level_0=()
    declare -A commands_level_1=()
    declare -A specialized_commands_level_0=([18]="0")
    declare -A specialized_commands_level_1=()

     local max_fallback_level=1
     for (( fallback_level=0; fallback_level <= max_fallback_level; fallback_level++ )) {
         completions_no_description_trailing_space=()
         completions_no_description_no_trailing_space=()
         completions_trailing_space=()
         suffixes_trailing_space=()
         descriptions_trailing_space=()
         completions_no_trailing_space=()
         suffixes_no_trailing_space=()
         descriptions_no_trailing_space=()
         matches=()

         declare literal_transitions_name=literal_transitions_level_${fallback_level}
         eval "declare initializer=\${${literal_transitions_name}[$state]}"
         eval "declare -a transitions=($initializer)"
         for literal_id in "${transitions[@]}"; do
             if [[ -v "descriptions[$literal_id]" ]]; then
                 completions_trailing_space+=("${literals[$literal_id]}")
                 suffixes_trailing_space+=("${literals[$literal_id]}")
                 descriptions_trailing_space+=("${descriptions[$literal_id]}")
             else
                 completions_no_description_trailing_space+=("${literals[$literal_id]}")
             fi
         done

         declare subword_transitions_name=subword_transitions_level_${fallback_level}
         eval "declare initializer=\${${subword_transitions_name}[$state]}"
         eval "declare -a transitions=($initializer)"
         for subword_id in "${transitions[@]}"; do
             _changelogger_subword_${subword_id} complete "${words[$CURRENT]}"
             completions_no_description_trailing_space+=("${subword_completions_no_description_trailing_space[@]}")
             completions_trailing_space+=("${subword_completions_trailing_space[@]}")
             completions_no_trailing_space+=("${subword_completions_no_trailing_space[@]}")
             suffixes_no_trailing_space+=("${subword_suffixes_no_trailing_space[@]}")
             suffixes_trailing_space+=("${subword_suffixes_trailing_space[@]}")
             descriptions_trailing_space+=("${subword_descriptions_trailing_space[@]}")
             descriptions_no_trailing_space+=("${subword_descriptions_no_trailing_space[@]}")
         done

         declare commands_name=commands_level_${fallback_level}
         eval "declare initializer=\${${commands_name}[$state]}"
         eval "declare -a transitions=($initializer)"
         for command_id in "${transitions[@]}"; do
             local output=$(_changelogger_cmd_${command_id} "${words[$CURRENT]}")
             local -a command_completions=("${(@f)output}")
             for line in ${command_completions[@]}; do
                 local parts=(${(@s:	:)line})
                 if [[ -v "parts[2]" ]]; then
                     completions_trailing_space+=("${parts[1]}")
                     suffixes_trailing_space+=("${parts[1]}")
                     descriptions_trailing_space+=("${parts[2]}")
                 else
                     completions_no_description_trailing_space+=("${parts[1]}")
                 fi
             done
         done

         declare specialized_commands_name=specialized_commands_level_${fallback_level}
         eval "declare initializer=\${${specialized_commands_name}[$state]}"
         eval "declare -a transitions=($initializer)"
         for command_id in "${transitions[@]}"; do
             _changelogger_cmd_${command_id} ${words[$CURRENT]}
         done

         local maxlen=0
         for suffix in ${suffixes_trailing_space[@]}; do
             if [[ ${#suffix} -gt $maxlen ]]; then
                 maxlen=${#suffix}
             fi
         done
         for suffix in ${suffixes_no_trailing_space[@]}; do
             if [[ ${#suffix} -gt $maxlen ]]; then
                 maxlen=${#suffix}
             fi
         done

         for ((i = 1; i <= $#suffixes_trailing_space; i++)); do
             if [[ -z ${descriptions_trailing_space[$i]} ]]; then
                 descriptions_trailing_space[$i]="${(r($maxlen)( ))${suffixes_trailing_space[$i]}}"
             else
                 descriptions_trailing_space[$i]="${(r($maxlen)( ))${suffixes_trailing_space[$i]}} -- ${descriptions_trailing_space[$i]}"
             fi
         done

         for ((i = 1; i <= $#suffixes_no_trailing_space; i++)); do
             if [[ -z ${descriptions_no_trailing_space[$i]} ]]; then
                 descriptions_no_trailing_space[$i]="${(r($maxlen)( ))${suffixes_no_trailing_space[$i]}}"
             else
                 descriptions_no_trailing_space[$i]="${(r($maxlen)( ))${suffixes_no_trailing_space[$i]}} -- ${descriptions_no_trailing_space[$i]}"
             fi
         done

         compadd -O m -a completions_no_description_trailing_space; matches+=("${m[@]}")
         compadd -O m -a completions_no_description_no_trailing_space; matches+=("${m[@]}")
         compadd -O m -a completions_trailing_space; matches+=("${m[@]}")
         compadd -O m -a completions_no_trailing_space; matches+=("${m[@]}")

         if [[ ${#matches} -gt 0 ]]; then
             compadd -Q -a completions_no_description_trailing_space
             compadd -Q -S ' ' -a completions_no_description_no_trailing_space
             compadd -l -Q -a -d descriptions_trailing_space completions_trailing_space
             compadd -l -Q -S '' -a -d descriptions_no_trailing_space completions_no_trailing_space
             return 0
         fi
     }
}

if [[ $ZSH_EVAL_CONTEXT =~ :file$ ]]; then
    compdef _changelogger changelogger
else
    _changelogger
fi
