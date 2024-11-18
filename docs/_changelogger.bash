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

    declare -a literals=(-s --yank major --commits export init --all add json -h -s changed fixed false -s minor --push -U -F --title push removed added delete patch -V --always-export get remote -h config export yaml markdown security json true major removed --version-full --unyank --yes -C markdown -y -V --help edit yml --file update --format -A --version deprecated --remote-repo yaml -f -V --help set editor added --status fixed --config-dir -V --editor minor --p -B changed patch --releases config version security --asset autocomplete -Y -A md list -r --always-push release man --yes push -t deprecated yml md generate -N -A --new import -v)
    declare -A literal_transitions=()
    literal_transitions[0]="([59]=1 [47]=2 [23]=3 [5]=1 [27]=4 [82]=5 [85]=6 [7]=7 [53]=1 [31]=8 [50]=1 [60]=9 [88]=10 [9]=1 [93]=11 [97]=12 [98]=1)"
    literal_transitions[2]="([19]=23 [63]=20 [89]=23 [39]=16 [80]=1 [6]=1 [14]=19 [25]=16 [29]=21)"
    literal_transitions[3]="([29]=21 [6]=1 [52]=1)"
    literal_transitions[4]="([59]=1 [61]=1 [75]=1 [20]=1 [4]=1 [6]=1 [30]=1 [28]=1 [95]=1 [29]=1)"
    literal_transitions[5]="([59]=1 [58]=16 [73]=1 [63]=20 [10]=19 [39]=16 [83]=1 [29]=1)"
    literal_transitions[6]="([59]=1 [1]=16 [77]=17 [44]=14 [29]=1 [79]=16 [16]=1 [40]=16 [94]=18 [17]=16 [96]=15 [69]=1 [70]=17)"
    literal_transitions[7]="([59]=1 [0]=19 [3]=1 [42]=1 [63]=20 [29]=1)"
    literal_transitions[8]="([59]=1 [18]=22 [51]=25 [29]=1)"
    literal_transitions[9]="([59]=1 [45]=16 [65]=17 [39]=16 [67]=26 [26]=24 [55]=13 [84]=24 [29]=1)"
    literal_transitions[10]="([59]=1 [77]=17 [66]=16 [87]=1 [39]=16 [70]=17 [44]=1 [29]=1)"
    literal_transitions[11]="([59]=1 [74]=1 [78]=1 [86]=1 [29]=1)"
    literal_transitions[12]="([59]=1 [49]=17 [57]=17 [29]=1)"
    literal_transitions[14]="([87]=1)"
    literal_transitions[15]="([68]=1 [72]=1 [37]=1)"
    literal_transitions[18]="([37]=1 [68]=1 [72]=1)"
    literal_transitions[19]="([64]=1 [90]=1 [76]=1 [62]=1 [38]=1 [71]=1)"
    literal_transitions[20]="([38]=1 [62]=1 [76]=1 [64]=1 [90]=1 [71]=1)"
    literal_transitions[21]="([59]=1)"
    literal_transitions[22]="([56]=1 [92]=1 [43]=1 [35]=1 [91]=1)"
    literal_transitions[24]="([36]=1 [13]=1)"
    literal_transitions[25]="([91]=1 [35]=1 [92]=1 [43]=1 [56]=1)"
    declare -A match_anything_transitions=([26]=1 [17]=1 [13]=1 [16]=1 [23]=1)
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

    declare -A literal_transitions_level_0=([24]="36 13" [0]="47 23 5 27 82 85 7 31 50 60 88 9 93 97 98" [19]="64 90 76 62 38 71" [6]="44 29 79 16 17 96 70" [15]="68 72 37" [5]="58 10 83 29" [9]="45 65 67 26 55 84 29" [10]="66 70 44 29" [3]="29 52" [8]="18 29" [14]="87" [12]="57 29" [7]="0 42 29" [11]="74 78 86 29" [21]="59" [2]="89 80 14 25 29" [4]="61 75 20 4 30 28 95 29" [22]="56 92 43 35 91")
    declare -A literal_transitions_level_1=([0]="59 53" [20]="38 62 76 64 90 71" [10]="59 77 87 39" [5]="59 73 63 39" [9]="59 39" [6]="59 1 77 40 94 69" [3]="6" [8]="59 51" [2]="19 63 39 6" [18]="37 68 72" [12]="59 49" [7]="59 3 63" [11]="59" [4]="59 6" [25]="91 35 92 43 56")
    declare -A subword_transitions_level_0=()
    declare -A subword_transitions_level_1=()
    declare -A commands_level_0=()
    declare -A commands_level_1=()
    declare -A specialized_commands_level_0=([17]="0")
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
