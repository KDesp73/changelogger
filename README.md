# changelogger

A cli tool to help you keep a changelog in all your projects

## Installation

```bash
# 1. Get the source code
$ git clone https://github.com/KDesp73/changelogger --depth=1

# 2. Checkout to the latest release
$ git checkout <latest>

# 3. Build and Install
$ ./install.sh
```

## Usage

```bash
# Initialize the changelogger
$ changelogger init

# Add an entry
$ changelogger add "Message" --status changed

# List entries
$ changelogger list
$ changelogger list -V 0.0.1 --status added

# Edit entries
$ changelogger edit -t "New title"

# Delete entries
$ changelogger delete
$ changelogger delete --all # To have all entries available

# Set variables
$ changelogger set --remote-repo https://github.com/KDesp73/changelogger

# Get variables
$ changelogger get version # Latest release

# Create a new release
$ changelogger release --new patch

# Push an unpushed release on Github
$ changelogger push -V 0.0.3

# Export CHANGELOG.md
$ changelogger export
```

## Config file

The default location for the config file is `~/.changelogger.yml`

### Example file

```yml
# ~/.changelogger.yml

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

## LICENSE

[MIT](./LICENSE)
