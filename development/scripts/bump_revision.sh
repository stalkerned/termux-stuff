#!/usr/bin/env bash

if [ "$#" = "0" ]; then
	echo
	echo "Usage: bump_revision.sh [path/to/build.sh] ..."
	echo
	echo "Add or increment TERMUX_PKG_REVISION in build.sh files."
	echo
	exit 1
fi

for pkg_buildsh in "$@"; do
	if [ -f "$pkg_buildsh" ]; then
		if grep -qP '^TERMUX_PKG_REVISION=(\d+)$' "${pkg_buildsh}"; then
			# Increment revision.
			current_rev=$(. "${pkg_buildsh}"; echo $TERMUX_PKG_REVISION)
			incremented_rev=$((current_rev + 1))

			echo "$(dirname "${pkg_buildsh}"): bumping to ${incremented_rev}"
			sed -i -E "s/^(TERMUX_PKG_REVISION=)(.*)$/\1${incremented_rev}/g" "$pkg_buildsh"
		else
			# Add base (1) revision.
			echo "$(dirname "${pkg_buildsh}"): setting revision to 1"
			sed -i -E "/TERMUX_PKG_VERSION=(.*)/a TERMUX_PKG_REVISION=1" "$pkg_buildsh"
		fi
	else
		echo "[!] File '$pkg_buildsh' is not found."
	fi
done
