# Notes

### Basic Features
1. **Initialize Changelog**: Create a new changelog file (e.g., `CHANGELOG.md`) in the project directory.
2. **Add Entry**: Allow users to add new entries to the changelog with a specific format (e.g., version number, date, description).
3. **List Entries**: Display the current changelog entries in a readable format.
4. **Delete Entry**: Provide an option to remove an entry from the changelog.
5. **Versioning**: Support semantic versioning (e.g., `1.0.0`, `1.1.0`, etc.) and automatically increment versions based on the type of changes (major, minor, patch).

### Advanced Features
1. **Categorization**: Allow users to categorize changes (e.g., "Added", "Changed", "Deprecated", "Removed", "Fixed", "Security").
2. **Template Support**: Provide customizable templates for changelog entries.
3. **Export Options**: Allow exporting the changelog to different formats (e.g., JSON, XML).
4. **Integration with Git**: Automatically pull commit messages from Git to populate the changelog.
5. **Markdown Support**: Use Markdown formatting for better readability in the changelog file.

### Example Command Structure 

```bash
# Initialize a new changelog
$ changelogger init

# Add a new entry
$ changelogger add "Message" --status fixed

# Set variables
$ changelogger set --remote-repo https://github.com/KDesp73/changelogger

# Get variables
$ changelogger get {version | remote | config | export}


# List all entries
$ changelogger list # lists every entry
$ changelogger list -V "0.0.3"
$ changelogger list -s added

# Delete an entry
$ changelogger delete -V "1.0.0"
$ changelogger delete -I # list (possibly filtered) entries and ask for an index (maybe)

# Manage releases (Github CLI tool integration)
$ changelogger release --new {major | minor | patch}
```

