#!/data/data/com.termux/files/usr/bin/bash

set -e

SCRIPT_DIR=$(dirname "$(realpath "$0")")
HOME="/data/data/com.termux/files/home"
PREFIX="/data/data/com.termux/files/usr"

if [ "$(uname -o)" != "Android" ]; then
	echo "This script must be executed within Termux !"
	exit 1
fi

echo
echo "Installing configuration files and scripts will"
echo "remove existing ones. Directories like '~/.config'"
echo "or '~/.local' will be completely erased and replaced."
echo
read -re -p "Do you want to proceed ? (y/n): " CHOICE

if [[ "$CHOICE" != [Yy] ]]; then
	echo "Aborting !"
	exit 1
fi

echo
echo "[@] Installing script dependencies:"
pkg install -yq findutils grep unstable-repo x11-repo

if [ -f "$SCRIPT_DIR/packages.txt" ]; then
	echo
	echo "[@] Installing necessary packages:"
	pkg install -yq $(cat "$SCRIPT_DIR/packages.txt" | grep -vP '^(?:\s+)?#')
fi

# Just replace objects in $HOME. No matter directory or files.
if [ -d "$SCRIPT_DIR/home" ]; then
	echo
	echo "[@] Writing dotfiles for \$HOME:"
	while IFS= read -r -d '' file; do
		dst_file_path="${HOME}/.$(basename "$file")"

		if [ -d "$file" ]; then
			echo "Installing directory '\${HOME}/.$(basename "$file")'..."
		else
			echo "Installing file '\${HOME}/.$(basename "$file")'..."
		fi

		rm -rf "$dst_file_path"
		cp -a "$file" "$dst_file_path"
	done < <(find "$SCRIPT_DIR/home" -mindepth 1 -maxdepth 1 -print0)
	unset file dst_file_path
fi

# Files in $PREFIX should be handled carefully.
if [ -d "$SCRIPT_DIR/prefix" ]; then
	echo
	echo "[@] Writing dotfiles for \$PREFIX:"
	cd "$SCRIPT_DIR/prefix" && {
		while IFS= read -r -d '' file; do
			file=$(echo "$file" | sed 's/^\.\///')
			echo "Installing file '\${PREFIX}/${file}'..."
			mkdir -p "${PREFIX}/$(dirname "$file")"
			cp -f "$file" "${PREFIX}/${file}"
		done < <(find . -type f -print0)
		unset file dst_file_path
	}
fi

echo
echo "Done ! Now you need to restart Termux."
