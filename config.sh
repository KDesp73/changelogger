#!/usr/bin/env bash

exe="changelogger"
install_dir="/usr/bin"

uninstall() {
    sudo rm -rf "$install_dir/$exe"
    echo "[INFO] Application uninstalled successfully."
}

install_exe() {
    if ! sudo cp -f "$1" "$install_dir/$2"; then
        echo "[ERRO] Failed to copy the executable to $install_dir"
        exit 1
    fi
    echo "[INFO] $2 installed successfully."
}

install() {
    echo "[WARN] Make sure you have the sqlite3.h, cjson.h, and yaml.h files in PATH (generally in /usr/include)."
    if [ -f "$exe" ]; then
        install_exe "$exe" "$exe"
        install_exe "./bin/clparse" "clparse"
        echo "[INFO] Installation completed successfully."
    else
        echo "[WARN] $exe is not built. Building..."
        if make RELEASE=1; then
            install_exe "$exe" "$exe"
            install_exe "./bin/clparse" "clparse"
            echo "[INFO] Installation completed successfully."
        else
            echo "[ERRO] Failed to build $exe"
            exit 1
        fi
    fi

    if [ -d "$HOME/changelogger" ]; then
        rm -rf "$HOME/changelogger"
    fi
}

fetch() {
    if [ -d "$HOME/changelogger" ]; then
        echo "[ERRO] Directory $HOME/changelogger already exists."
        echo "[INFO] Remove it or use 'update' to fetch the latest version."
        exit 1
    fi
    git clone https://github.com/KDesp73/changelogger --depth=1 "$HOME/changelogger"
    cd "$HOME/changelogger" || exit 1
}

update() {
    cd "$HOME" || exit 1
    rm -rf "$HOME/changelogger"
    fetch
    install
}

help() {
    cat <<EOF
USAGE
  config.sh <commands>...

COMMANDS
  install      Installs the executable
  uninstall    Uninstalls the executable
  update       Updates to the latest version
  fetch        Fetches the latest version
  help         Prints this message
EOF
}

if [ $# -eq 0 ]; then
    echo "[ERRO] Specify at least one command" 1>&2
    help
    exit 1
fi

# Process all commands passed as arguments
for arg in "$@"; do
    case "$arg" in
        "uninstall")
            uninstall
            ;;
        "update")
            update
            ;;
        "fetch")
            fetch
            ;;
        "install")
            install
            ;;
        "help")
            help
            ;;
        *)
            echo "[ERRO] Unknown command: $arg"
            help
            exit 1
            ;;
    esac
done
