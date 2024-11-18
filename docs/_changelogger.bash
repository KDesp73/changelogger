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

    declare -a literals=(-s --yank major --commits export init --all add json -h -s fixed changed false -s minor --push -U -F --title push removed added delete patch -V --always-export get remote config export yaml markdown --file security json true major --version-full removed --unyank --yes -C markdown -y -V --help edit yml update --version --format -A deprecated --remote-repo yaml -f -V --help set editor added --status fixed --config-dir -V --editor minor --p -B changed patch --releases config version security --asset autocomplete -Y -A md list -r --always-push release man --yes push -t deprecated yml md generate -N -A --new import -v)
    declare -A literal_transitions=()
    literal_transitions[0]="([58]=1 [47]=2 [23]=3 [5]=1 [27]=4 [81]=5 [84]=6 [7]=7 [50]=1 [30]=8 [49]=9 [59]=10 [87]=11 [9]=1 [92]=12 [96]=13 [97]=1)"
    literal_transitions[2]="([9]=22 [19]=16 [62]=18 [88]=16 [38]=17 [79]=1 [6]=1 [14]=19 [25]=17)"
    literal_transitions[3]="([9]=22 [6]=1 [52]=1)"
    literal_transitions[4]="([58]=1 [60]=1 [74]=1 [9]=1 [20]=1 [4]=1 [6]=1 [29]=1 [28]=1 [94]=1)"
    literal_transitions[5]="([58]=1 [57]=17 [9]=1 [62]=18 [10]=19 [38]=17 [82]=1 [72]=1)"
    literal_transitions[6]="([58]=1 [1]=17 [76]=21 [93]=27 [9]=1 [78]=17 [16]=1 [40]=17 [17]=17 [68]=1 [95]=14 [69]=21 [44]=20)"
    literal_transitions[7]="([58]=1 [0]=19 [3]=1 [9]=1 [42]=1 [62]=18)"
    literal_transitions[8]="([58]=1 [18]=23 [9]=1 [51]=15)"
    literal_transitions[9]="([9]=1 [58]=1)"
    literal_transitions[10]="([58]=1 [9]=1 [64]=21 [38]=17 [66]=26 [26]=24 [54]=25 [83]=24 [45]=17)"
    literal_transitions[11]="([58]=1 [76]=21 [65]=17 [9]=1 [86]=1 [38]=17 [69]=21 [44]=1)"
    literal_transitions[12]="([58]=1 [73]=1 [77]=1 [9]=1 [85]=1)"
    literal_transitions[13]="([58]=1 [9]=1 [33]=21 [56]=21)"
    literal_transitions[14]="([67]=1 [71]=1 [37]=1)"
    literal_transitions[15]="([90]=1 [35]=1 [91]=1 [43]=1 [55]=1)"
    literal_transitions[18]="([39]=1 [61]=1 [75]=1 [63]=1 [89]=1 [70]=1)"
    literal_transitions[19]="([63]=1 [89]=1 [75]=1 [61]=1 [39]=1 [70]=1)"
    literal_transitions[20]="([86]=1)"
    literal_transitions[22]="([58]=1)"
    literal_transitions[23]="([55]=1 [91]=1 [43]=1 [35]=1 [90]=1)"
    literal_transitions[24]="([36]=1 [13]=1)"
    literal_transitions[27]="([37]=1 [67]=1 [71]=1)"
    declare -A match_anything_transitions=([17]=1 [16]=1 [21]=1 [26]=1 [25]=1)
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

    declare -A literal_transitions_level_0=([24]="36 13" [19]="63 89 75 61 39 70" [0]="47 23 5 27 81 84 7 30 49 59 87 9 92 96 97" [20]="86" [10]="9 64 66 26 54 83 45" [5]="57 9 10 82" [6]="9 78 16 17 95 69 44" [9]="9" [3]="9 52" [8]="18 9" [14]="67 71 37" [12]="73 77 9 85" [7]="0 9 42" [11]="65 9 69 44" [2]="9 88 79 14 25" [13]="9 56" [4]="60 74 9 20 4 29 28 94" [22]="58" [23]="55 91 43 35 90")
    declare -A literal_transitions_level_1=([0]="58 50" [10]="58 38" [15]="90 35 91 43 55" [5]="58 62 38 72" [6]="58 1 76 93 40 68" [9]="58" [3]="6" [8]="58 51" [2]="19 62 38 6" [18]="39 61 75 63 89 70" [12]="58" [11]="58 76 86 38" [7]="58 3 62" [13]="58 33" [4]="58 6" [27]="37 67 71")
    declare -A subword_transitions_level_0=()
    declare -A subword_transitions_level_1=()
    declare -A commands_level_0=()
    declare -A commands_level_1=()
    declare -A specialized_commands_level_0=([21]="0")
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
