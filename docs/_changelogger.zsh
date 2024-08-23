#compdef changelogger

_values() {
    local desc
    local -a values
    local -a descriptions

    # Parse the arguments
    while [[ "$1" != "" ]]; do
        if [[ "$1" == *\[* ]]; then
            # Extract the value and description
            value="${1%%[*}"
            desc="${1#*[}"
            desc="${desc%]}"
            values+=("$value")
            descriptions+=("$desc")
        else
            values+=("$1")
            descriptions+=("")
        fi
        shift
    done

    # Display the values with descriptions
    _describe -t values "values" values descriptions
}

_changelogger() {
    local -a commands
    commands=(
        'init'
        'add'
        'list'
        'edit'
        'delete'
        'set'
        'get'
        'release'
        'push'
        'export'
        'import'
        'generate'
        '-h'
        '-v'
    )

    _arguments \
        '1: :($commands)' \
        '*:: :->args'

    case $state in
        args)
            case $words[1] in
                add)
                    _arguments \
                        '-h[Prints the help message]' \
                        '-C[Add unreleased commit messages]' \
                        '-s[Specify the status of the entry]'
                    ;;
                set)
                    _arguments \
                        '-h[Prints the help message]' \
                        '--config-dir[Specify the configuration file path]' \
                        '--remote-repo[Specify the URL of the remote repo]' \
                        '--editor[Specify the editor to use]' \
                        '--always-push[Specify whether to immediately push the release]' \
                        '--always-export[Specify whether to export the CHANGELOG.md file]'
                    ;;
                list)
                    _arguments \
                        '-h[Prints the help message]' \
                        '-V[Set the version to filter the list]' \
                        '-s[Set the status to filter the list]' \
                        '-r[List only the releases]'
                    ;;
                delete)
                    _arguments \
                        '-h[Prints the help message]' \
                        '-A[List all entries and have them available for deletion]'
                    ;;
                get)
                    _values \
                        'version[Latest release]' \
                        'export[Always export CHANGELOG.md]' \
                        'remote[The remote git repo URL]' \
                        'config[The path to the configuration file]' \
                        'push[Always push the release on GitHub]' \
                        'editor[Editor to use when editing a file is needed]'
                    _arguments \
                        '-h[Prints the help message]' \
                        '-A[Print all variables]'
                    ;;
                export)
                    _arguments \
                        '-h[Prints the help message]' \
                        '-F[Specify in which format to export]'
                    ;;
                push)
                    _arguments \
                        '-h[Prints the help message]' \
                        '-V[Specify the version of the release you want to push]' \
                        '-y[Skip the confirmation message]'
                    ;;
                release)
                    _arguments \
                        '-h[Prints the help message]' \
                        '-N[Creates a new release]' \
                        '-p[Push the release on GitHub]' \
                        '-y[Skip the confirmation message]' \
                        '-Y[Set a release as YANKED]' \
                        '-U[Set a release as not YANKED]'
                    ;;
                edit)
                    _arguments \
                        '-h[Prints the help message]' \
                        '-s[Specify new status]' \
                        '-t[Specify new title/message]' \
                        '-V[Specify new version]' \
                        '-A[List all entries and have them available for editing]'
                    ;;
                generate)
                    _arguments \
                        '-h[Prints the help message]'
                    _values \
                        'config[A starting point for your config file]' \
                        'autocomplete[Autocomplete for the active shell]' \
                        'man[Man page for changelogger]' 
                    ;;
                import)
                    _arguments \
                        '-h[Prints the help message]' \
                        '-f[File to import]'
                    ;;
            esac
            ;;
    esac
}

compdef _changelogger changelogger

