_changelogger_cmd_0 () {
    compgen -A file "$1"
}

_changelogger () {
    if [[ $(type -t _get_comp_words_by_ref) != function ]]; then
        echo _get_comp_words_by_ref: function not defined.  Make sure the bash-completions system package is installed
        return 1
    fi

    local words cword
    _get_comp_words_by_ref -n "$COMP_WORDBREAKS" words cword

    declare -a literals=(-s --yank major --commits export init --all add json -h -s changed fixed false -s minor --push -U -F --title push removed added delete patch -V --always-export get remote -h config export yaml markdown security json true major removed --version-full --unyank --yes -C markdown -y -V --help edit yml --file --version --format -A deprecated --remote-repo yaml -f -V --help set editor added --status fixed --config-dir -V --editor minor --p -B changed patch --releases config version security --asset autocomplete -Y -A md list -r --always-push release man --yes push -t deprecated yml md generate -N -A --new import -v)
    declare -A literal_transitions=()
    literal_transitions[0]="([58]=1 [47]=2 [23]=3 [5]=1 [27]=4 [81]=5 [84]=6 [7]=7 [50]=1 [31]=8 [59]=9 [97]=1 [87]=10 [9]=1 [92]=11 [96]=12)"
    literal_transitions[2]="([19]=14 [62]=20 [88]=14 [39]=15 [79]=1 [6]=1 [14]=19 [25]=15 [29]=21)"
    literal_transitions[3]="([29]=21 [6]=1 [52]=1)"
    literal_transitions[4]="([58]=1 [60]=1 [74]=1 [20]=1 [4]=1 [6]=1 [30]=1 [28]=1 [94]=1 [29]=1)"
    literal_transitions[5]="([58]=1 [57]=15 [72]=1 [62]=20 [10]=19 [39]=15 [82]=1 [29]=1)"
    literal_transitions[6]="([58]=1 [1]=15 [76]=16 [44]=18 [29]=1 [78]=15 [16]=1 [40]=15 [93]=17 [17]=15 [95]=13 [68]=1 [69]=16)"
    literal_transitions[7]="([58]=1 [0]=19 [3]=1 [42]=1 [62]=20 [29]=1)"
    literal_transitions[8]="([58]=1 [18]=22 [51]=25 [29]=1)"
    literal_transitions[9]="([58]=1 [45]=15 [64]=16 [39]=15 [66]=26 [26]=23 [54]=24 [83]=23 [29]=1)"
    literal_transitions[10]="([58]=1 [76]=16 [65]=15 [86]=1 [39]=15 [69]=16 [44]=1 [29]=1)"
    literal_transitions[11]="([58]=1 [73]=1 [77]=1 [85]=1 [29]=1)"
    literal_transitions[12]="([58]=1 [49]=16 [56]=16 [29]=1)"
    literal_transitions[13]="([67]=1 [71]=1 [37]=1)"
    literal_transitions[17]="([37]=1 [67]=1 [71]=1)"
    literal_transitions[18]="([86]=1)"
    literal_transitions[19]="([63]=1 [89]=1 [75]=1 [61]=1 [38]=1 [70]=1)"
    literal_transitions[20]="([38]=1 [61]=1 [75]=1 [63]=1 [89]=1 [70]=1)"
    literal_transitions[21]="([58]=1)"
    literal_transitions[22]="([55]=1 [91]=1 [43]=1 [35]=1 [90]=1)"
    literal_transitions[23]="([36]=1 [13]=1)"
    literal_transitions[25]="([90]=1 [35]=1 [91]=1 [43]=1 [55]=1)"
    declare -A match_anything_transitions=([24]=1 [26]=1 [15]=1 [14]=1 [16]=1)
    declare -A subword_transitions

    local state=0
    local word_index=1
    while [[ $word_index -lt $cword ]]; do
        local word=${words[$word_index]}

        if [[ -v "literal_transitions[$state]" ]]; then
            declare -A state_transitions
            eval "state_transitions=${literal_transitions[$state]}"

            local word_matched=0
            for literal_id in $(seq 0 $((${#literals[@]} - 1))); do
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

    declare -A literal_transitions_level_0=([0]="47 23 5 27 81 84 7 31 59 97 87 9 92 96" [19]="63 89 75 61 38 70" [6]="44 29 78 16 17 95 69" [10]="65 69 44 29" [5]="57 10 82 29" [9]="45 64 66 26 54 83 29" [3]="29 52" [8]="18 29" [18]="86" [12]="56 29" [7]="0 42 29" [13]="67 71 37" [21]="58" [11]="73 77 85 29" [4]="60 74 20 4 30 28 94 29" [2]="88 79 14 25 29" [22]="55 91 43 35 90" [23]="36 13")
    declare -A literal_transitions_level_1=([0]="58 50" [20]="38 61 75 63 89 70" [17]="37 67 71" [5]="58 72 62 39" [9]="58 39" [6]="58 1 76 40 93 68" [10]="58 76 86 39" [3]="6" [8]="58 51" [2]="19 62 39 6" [12]="58 49" [7]="58 3 62" [11]="58" [4]="58 6" [25]="90 35 91 43 55")
    declare -A subword_transitions_level_0=()
    declare -A subword_transitions_level_1=()
    declare -A commands_level_0=()
    declare -A commands_level_1=()
    declare -A specialized_commands_level_0=([16]="0")
    declare -A specialized_commands_level_1=()

    local -a matches=()
    local max_fallback_level=1
    local prefix="${words[$cword]}"
    for (( fallback_level=0; fallback_level <= max_fallback_level; fallback_level++ )) {
       eval "declare literal_transitions_name=literal_transitions_level_${fallback_level}"
       eval "declare -a transitions=(\${$literal_transitions_name[$state]})"
       for literal_id in "${transitions[@]}"; do
           local literal="${literals[$literal_id]}"
           if [[ $literal = "${prefix}"* ]]; then
               matches+=("$literal ")
           fi
       done

       eval "declare subword_transitions_name=subword_transitions_level_${fallback_level}"
       eval "declare -a transitions=(\${$subword_transitions_name[$state]})"
       for subword_id in "${transitions[@]}"; do
           readarray -t -O "${#matches[@]}" matches < <(_changelogger_subword_$subword_id complete "$prefix")
       done

       eval "declare commands_name=commands_level_${fallback_level}"
       eval "declare -a transitions=(\${$commands_name[$state]})"
       for command_id in "${transitions[@]}"; do
           local completions=()
           readarray -t completions < <(_changelogger_cmd_$command_id "$prefix" | cut -f1)
           for item in "${completions[@]}"; do
               if [[ $item = "${prefix}"* ]]; then
                   matches+=("$item")
               fi
           done
       done

       eval "declare specialized_commands_name=specialized_commands_level_${fallback_level}"
       eval "declare -a transitions=(\${$specialized_commands_name[$state]})"
       for command_id in "${transitions[@]}"; do
           local completions=()
           readarray -t completions < <(_changelogger_cmd_"${command_id}" "$prefix" | cut -f1)
           for item in "${completions[@]}"; do
               if [[ $item = "${prefix}"* ]]; then
                   matches+=("$item")
               fi
           done
       done

       if [[ ${#matches[@]} -gt 0 ]]; then
           local shortest_suffix="$prefix"
           for ((i=0; i < ${#COMP_WORDBREAKS}; i++)); do
               local char="${COMP_WORDBREAKS:$i:1}"
               local candidate=${prefix##*$char}
               if [[ ${#candidate} -lt ${#shortest_suffix} ]]; then
                   shortest_suffix=$candidate
               fi
           done
           local superfluous_prefix=""
           if [[ "$shortest_suffix" != "$prefix" ]]; then
               local superfluous_prefix=${prefix%$shortest_suffix}
           fi
           COMPREPLY=("${matches[@]#$superfluous_prefix}")
           break
       fi
    }

    return 0
}

complete -o nospace -F _changelogger changelogger
