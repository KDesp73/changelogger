# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [0.1.0] - 2026-10-20

### Added

- Portable `configure` installer with `--prefix`/`--bindir`, clparse and shell-completion setup

### Changed

- Replaced `config.sh` with `configure`
- Command dispatch now uses `cli.h`
- Updated version to 0.1.0

### Fixed

- Exported release dates no longer have a trailing space
- Export no longer crashes when an entry's release is missing
- `edit -V` rejects versions that have no release
- Incomplete databases are reported and repaired by `init`
- Imports and release creation run in a single transaction
- Push state is recorded from gh's actual result
- clparse is resolved from PATH
- CHANGELOG.md is no longer truncated when the entry query fails
- Database write and GitHub push failures now abort instead of being ignored

### Security

- Every SQL value is quoted and the query builder overflow is fixed


## [0.0.19] - 2024-11-18

### Added

- Changing result table when not enough horizontal space
- different results when piping is detected (list command)
- Added message length check

### Changed

- Updated version to 0.0.19


## [0.0.18] - 2024-11-18

### Added

- Added a limit when listing changes (15)

### Changed

- Updated version to 0.0.18


## [0.0.17] - 2024-11-18

### Added

- Added update command

### Changed

- Updated version to 0.0.17


## [0.0.16] - 2024-11-15

### Added

- Added Fish autocomplete script

### Changed

- Changed version to 0.0.16

### Fixed

- Correct autocomplete scripts (Zsh, Bash)


## [0.0.15] - 2024-11-15

### Changed

- updated table drawing when using `list`


## [0.0.14] - 2024-10-31

### Added

- Added assets option for release and push

### Changed

- Updated version to 0.0.14

### Fixed

- Updated the zsh autocomplete


## [0.0.13] - 2024-10-22

### Added

- -V to set 

### Changed

- Updated version to 0.0.13


## [0.0.12] - 2024-10-20

### Added

- Added release option to Makefile
- Added generate autocomplete command
- Added --all option to get command

### Changed

- ~/.changelogger.yml -> ~/.config/.changelogger.yml
- Updated version to 0.0.12


## [0.0.11] - 2024-08-22

### Changed

- Updated version to 0.0.11

### Fixed

- Fixed commit message addition
- fixed git commands
- checking if no commits


## [0.0.10] - 2024-08-22 [YANKED]

### Added

- Ability to edit version
- added make install option
- Config editor variable added
- Added --commits option to add commits as entries

### Changed

- Increased title length when listing entries
- Updated version to 0.0.10
- Edited help messages according to the latest changes

### Fixed

- checking if tag is released
- Sorting entries before exporting
- fixed sorting in case of unreleased

### Security

- Implemented sql injection checker


## [0.0.9] - 2024-08-17

### Added

- Ability to set release as YANKED

### Changed

- Updated version to 0.0.9

### Fixed

- Fixed set help message


## [0.0.8] - 2024-08-17

### Added

- Added different export formats (json, yml)

### Changed

- Updated version to 0.0.8


## [0.0.7] - 2024-08-17

### Added

- generate command implemented (config only)
- Importing changelog file into the database
- Added config file parsing and loading

### Changed

- Included import command to help messages
- Using -y to skip warning message
- Updated version to 0.0.7

### Fixed

- not loading file config if init was not executed


## [0.0.6] - 2024-08-15

### Added

- Making sure user wants to release or push

### Changed

- Updated version to 0.0.6

### Fixed

- Updating the pushed field when pushing


## [0.0.5] - 2024-08-15

### Added

- Added pushed field in Releases table
- Implemented set/get for always_push
- push command implemented
- --all to have all entries available
- list --releases added

### Changed

- Refactored command_list
- Updated help messages
- Release is not pushed unless its specified
- Updated version to 0.0.5


## [0.0.4] - 2024-08-15

### Added

- delete command implemented
- edit command implemented

### Changed

- Updated version to 0.0.4


## [0.0.3] - 2024-08-15

### Added

- list command implemented

### Changed

- Following bash conventions on install.sh

### Removed

- Removed the ability to set the version directly

### Fixed

- Better way to handle help messages


## [0.0.2] - 2024-08-13

### Changed

- Updated version to 0.0.2

### Fixed

- using the correct file on releases


## [0.0.1] - 2024-08-13

### Added

- Implemented Version
- Implemented Date
- get command done
- install.sh script
- export command done
- release command done
- LICENSE added
- set command done
- Database creation query on init
- Parse options method
- Integrated SQLite library

### Removed

- Deleted unnecessary directories


[0.0.1]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.1
[0.0.10]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.10
[0.0.11]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.11
[0.0.12]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.12
[0.0.13]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.13
[0.0.14]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.14
[0.0.15]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.15
[0.0.16]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.16
[0.0.17]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.17
[0.0.18]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.18
[0.0.19]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.19
[0.0.2]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.2
[0.0.3]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.3
[0.0.4]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.4
[0.0.5]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.5
[0.0.6]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.6
[0.0.7]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.7
[0.0.8]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.8
[0.0.9]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.9
[0.1.0]: https://github.com/KDesp73/changelogger/releases/tag/v0.1.0

