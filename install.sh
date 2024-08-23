#!/bin/bash

exe="changelogger"


if [ "$1" == "clean" ]; then
    sudo rm -rf /usr/bin/$exe

    echo "[INFO] Application uninstalled successfully"
    exit 0
fi

install_exe() {
    if ! sudo cp "./$1" "/usr/bin/$2"; then
        echo "[ERRO] Failed to copy the executable to /usr/bin/"
        exit 1
    fi
    echo "[INFO] $2 installed successfully"
}

if [ -f "$exe" ]; then
    # Install the executable
    install_exe $exe $exe
    install_exe "./bin/clparse" "clparse"

    echo "[INFO] Installation completed successfully."
else
    echo "$exe is not built. Building..."

    if make RELEASE=1; then
        ./install.sh
    else
        echo "Failed to build $exe"
        exit 1
    fi
fi
