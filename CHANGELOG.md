# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


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
- Added --commits option to add commits as entries
- Config editor variable added

### Changed

- Increased title length when listing entries
- Edited help messages according to the latest changes
- Updated version to 0.0.10

### Fixed

- fixed sorting in case of unreleased
- checking if tag is released
- Sorting entries before exporting

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

- Importing changelog file into the database
- generate command implemented (config only)
- Added config file parsing and loading

### Changed

- Using -y to skip warning message
- Included import command to help messages
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

- push command implemented
- Implemented set/get for always_push
- Added pushed field in Releases table
- list --releases added
- --all to have all entries available

### Changed

- Updated help messages
- Release is not pushed unless its specified
- Refactored command_list
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

- install.sh script
- get command done
- export command done
- release command done
- LICENSE added
- set command done
- Database creation query on init
- Parse options method
- Integrated SQLite library
- Implemented Version
- Implemented Date

### Removed

- Deleted unnecessary directories


[0.0.1]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.1
[0.0.10]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.10
[0.0.11]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.11
[0.0.12]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.12
[0.0.13]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.13
[0.0.2]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.2
[0.0.3]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.3
[0.0.4]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.4
[0.0.5]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.5
[0.0.6]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.6
[0.0.7]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.7
[0.0.8]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.8
[0.0.9]: https://github.com/KDesp73/changelogger/releases/tag/v0.0.9

