#compdef changelogger

_changelogger() {
    local -a commands
    commands=(
        'init:Initialize a new changelog'
        'add:Add a new entry'
        'list:List entries in the changelog'
        'edit:Edit an entry'
        'delete:Delete an entry'
        'set:Set configuration options'
        'get:Get configuration options'
        'release:Manage releases'
        'push:Push changes to the remote repository'
        'export:Export the changelog'
        'import:Import a changelog'
        'generate:Generate files (config, autocomplete, man page)'
        '-h:Show help'
        '-v:Show version information'
    )

    _arguments \
        '1:command:->command' \
        '*::options:->args'

    case $state in
        command)
            _describe 'command' commands
            ;;
        args)
            case $words[1] in
                add)
                    _arguments \
                        '-h[Show help for add command]' \
                        '-C[Add unreleased commit messages]' \
                        '-s[Specify the status of the entry]'
                    ;;
                set)
                    _arguments \
                        '-h[Show help for set command]' \
                        '--config-dir[Specify the configuration file path]' \
                        '--remote-repo[Specify the URL of the remote repository]' \
                        '--editor[Specify the editor to use]' \
                        '--always-push[Push releases automatically]' \
                        '--always-export[Export the changelog automatically]' \
                        '--version-ful[Set the current project version]'
                    ;;
                list)
                    _arguments \
                        '-h[Show help for list command]' \
                        '-V[Filter list by version]' \
                        '-s[Filter list by status]' \
                        '-r[List only releases]'
                    ;;
                delete)
                    _arguments \
                        '-h[Show help for delete command]' \
                        '-A[List all entries for deletion]'
                    ;;
                get)
                    _values 'variable' \
                        'version[Latest release]' \
                        'export[Always export the changelog]' \
                        'remote[Remote repository URL]' \
                        'config[Configuration file path]' \
                        'push[Always push the release]' \
                        'editor[Editor to use for editing]'
                    _arguments \
                        '-h[Show help for get command]' \
                        '-A[Print all configuration variables]'
                    ;;
                export)
                    _arguments \
                        '-h[Show help for export command]' \
                        '-F[Specify the export format]'
                    ;;
                push)
                    _arguments \
                        '-h[Show help for push command]' \
                        '-V[Specify release version to push]' \
                        '-y[Skip confirmation prompt]' \
                        '-B[Include an asset with the release]'
                    ;;
                release)
                    _arguments \
                        '-h[Show help for release command]' \
                        '-N[Create a new release]' \
                        '-p[Push the release to GitHub]' \
                        '-y[Skip confirmation prompt]' \
                        '-Y[Mark release as YANKED]' \
                        '-U[Unmark release as YANKED]' \
                        '-B[Include an asset with the release]'
                    ;;
                edit)
                    _arguments \
                        '-h[Show help for edit command]' \
                        '-s[Specify new status]' \
                        '-t[Specify new title/message]' \
                        '-V[Specify new version]' \
                        '-A[List all entries for editing]'
                    ;;
                generate)
                    _arguments \
                        '-h[Show help for generate command]' \
                        '--config[Generate a configuration file]' \
                        '--autocomplete[Generate shell autocomplete script]' \
                        '--man[Generate man page]'
                    ;;
                import)
                    _arguments \
                        '-h[Show help for import command]' \
                        '-f[Specify file to import]'
                    ;;
                *)
                    _message "Unknown command or option"
                    ;;
            esac
            ;;
    esac
}

compdef _changelogger changelogger
