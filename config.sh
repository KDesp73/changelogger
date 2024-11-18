#!/bin/bash

exe="changelogger"

uninstall() {
    sudo rm -rf /usr/bin/$exe

    echo "[INFO] Application uninstalled successfully"
}

install_exe() {
    if ! sudo cp "$1" "/usr/bin/$2"; then
        echo "[ERRO] Failed to copy the executable to /usr/bin/"
        exit 1
    fi
    echo "[INFO] $2 installed successfully"
}

install() {
    echo "Make sure you have the sqlite3.h, cjson.h and yaml.h files in PATH (generally in /usr/include)"
    if [ -f "$exe" ]; then
        # Install the executable
        install_exe $exe $exe
        install_exe "./bin/clparse" "clparse"

        echo "[INFO] Installation completed successfully."
    else
        echo "$exe is not built. Building..."

        if make RELEASE=1; then
            install
        else
            echo "Failed to build $exe"
            exit 1
        fi
    fi

}

fetch() {
    git clone https://github.com/KDesp73/changelogger --depth=1
    cd "$HOME"/changelogger || exit 1
}

update() {
    cd "$HOME" || exit 1
    rm -rf "$HOME"/changelogger

}

help() {
    echo "USAGE"
    echo "  config.sh <command>"
    echo ""
    echo "COMMANDS"
    echo "  install      Installs the executable"
    echo "  uninstall    Uninstalls the executable"
    echo "  update       Updates to the latest version"
    echo "  help         Prints this message"
}

if [ $# == 0 ]; then
    echo "[ERRO] Specify a command" 1>&2; 
    help
    exit 1
fi

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
    esac
done

