#!/bin/bash

exe="changelogger"


if [ "$1" == "clean" ]; then
    sudo rm -rf /usr/bin/$exe

    echo "[INFO] Application uninstalled successfully"
    exit 0
fi

install_exe() {
    sudo cp "./$1" "/usr/bin/$1"
    if [ $? -ne 0 ]; then
        echo "[ERRO] Failed to copy the executable to /usr/bin/"
        exit 1
    fi
    echo "[INFO] $exe installed successfully"
}

if [ -f "$exe" ]; then
    # Install the executable
    install_exe $exe

    echo "[INFO] Installation completed successfully."
else
    echo "$exe is not built. Building..."

    if make; then
        ./install.sh
    else
        echo "Failed to build $exe"
        exit 1
    fi
fi
