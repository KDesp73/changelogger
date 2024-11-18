# changelogger

A cli tool to help you keep a changelog in all your projects

## Installation

```bash
bash <(curl -s https://raw.githubusercontent.com/KDesp73/changelogger/refs/heads/main/config.sh) fetch install
```

## Usage

```bash
# Initialize the changelogger
$ changelogger init

# Add an entry
$ changelogger add "Message" --status changed
$ changelogger add --commits

# List entries
$ changelogger list
$ changelogger list -V 0.0.1 --status added

# Edit entries
$ changelogger edit
$ changelogger edit -t "New title"
$ changelogger edit --all # To have all entries available

# Delete entries
$ changelogger delete
$ changelogger delete --all # To have all entries available

# Set variables
$ changelogger set --remote-repo https://github.com/KDesp73/changelogger

# Get variables
$ changelogger get version # Latest release
$ changelogger get --all

# Create a new release
$ changelogger release --new patch
$ changelogger release --yank 0.0.3
$ changelogger release --unyank 0.0.3
$ changelogger release --new patch --asset <path> --push -y

# Push an unpushed release on Github
$ changelogger push -V 0.0.3
$ changelogger push --asset <path>

# Export CHANGELOG.md
$ changelogger export
$ changelogger export --format json

# Import CHANGELOG.md
$ changelogger import -f ./CHANGELOG.md

# Generate various files
$ changelogger generate config

# Update changelogger
$ changelogger update
```

## Config file

The default location for the config file is `~/.config/.changelogger.yml`

### Example file

```yml
# ~/.config/.changelogger.yml

editor: nvim
always-export: true
always-push: false
release-warning-message: "Remember to update the version and commit everything important!"
# config-path: /path/to/another/config/to/be/used
```

Adding the `config-path` label will redirect changelogger to use that file as a config instead of the default one

## Help

Try `changelogger -h` or `changelogger <command> -h` 

## References

- [keepachangelog](https://keepachangelog.com/en/1.1.0/)
- [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
- [clparse](https://github.com/marcaddeo/clparse)
- [gh-cli](https://cli.github.com/manual/)

## LICENSE

[MIT](./LICENSE)

