_changelogger_autocomplete() {
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts="init add list edit delete set get release push export generate import --help --version"

    if [[ ${cur} == -* ]]; then
        COMPREPLY=( "$(compgen -W "${opts}" -- "${cur}")" )
        return 0
    fi

    case "${prev}" in
        add)
            # Autocomplete options for 'add' command
            COMPREPLY=( "$(compgen -W "-h --help -C --commits -s --status" -- "${cur}")" )
            return 0
            ;;
        set)
            # Autocomplete options for 'set' command
            COMPREPLY=( "$(compgen -W "--config-dir --remote-repo --editor --always-push --always-export" -- "${cur}")" )
            return 0
            ;;
        list)
            # Autocomplete options for 'list' command
            COMPREPLY=( "$(compgen -W "-h --help -V --version-full -s --status -r --releases" -- "${cur}")" )
            return 0
            ;;
        delete)
            # Autocomplete options for 'delete' command
            COMPREPLY=( "$(compgen -W "-h --help -A --all" -- "${cur}")" )
            return 0
            ;;
        get)
            # Autocomplete options for 'get' command
            COMPREPLY=( "$(compgen -W "version export remote config push editor -h --help -A --all" -- "${cur}")" )
            return 0
            ;;
        export)
            # Autocomplete options for 'export' command
            COMPREPLY=( "$(compgen -W "-h --help -F --format" -- "${cur}")" )
            return 0
            ;;
        push)
            # Autocomplete options for 'push' command
            COMPREPLY=( "$(compgen -W "-h --help -V --version-full -y --yes" -- "${cur}")" )
            return 0
            ;;
        release)
            # Autocomplete options for 'release' command
            COMPREPLY=( "$(compgen -W "-h --help -N --new -p --push -y --yes -Y --yank -U --unyank" -- "${cur}")" )
            return 0
            ;;
        edit)
            # Autocomplete options for 'edit' command
            COMPREPLY=( "$(compgen -W "-h --help -s --status -t --title -V --version-full -A --all" -- "${cur}")" )
            return 0
            ;;
        generate)
            # Autocomplete options for 'generate' command
            COMPREPLY=( "$(compgen -W "config autocomplete man" -- "${cur}")" )
            return 0
            ;;
        import)
            # Autocomplete options for 'import' command
            COMPREPLY=( "$(compgen -W "-h --help -f --file" -- "${cur}")" )
            return 0
            ;;
        *)
            COMPREPLY=( "$(compgen -W "${opts}" -- "${cur}")" )
            return 0
            ;;
    esac
}

complete -F _changelogger_autocomplete changelogger
