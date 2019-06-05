#!/bin/bash
##
##  Compare local and remote versions of packages.
##  Similar to ./scripts/check-built-packages.py in termux-packages repository.
##

set -e

: "${TMPDIR:=/tmp}"

REPO_DIR=$(realpath "$(dirname "$0")/../")

if [ -e "${REPO_DIR}/scripts/properties.sh" ]; then
	echo "[*] Loading ./scripts/properties.sh"
	. "${REPO_DIR}/scripts/properties.sh"
fi

read_package_list() {
	local architecture

	for architecture in "$1"; do
		local package_list
		package_list=$(mktemp "${TMPDIR}/termux-packages.${architecture}.XXXXXX")

		if [ ! -s "${package_list}" ]; then
			echo "[*] Downloading package list for architecture '${architecture}'..."
			curl \
				--fail \
				--location \
				--output "${package_list}" \
				"https://dl.bintray.com/termux/termux-packages-24/dists/stable/main/binary-${architecture}/Packages"
			echo >> "${package_list}"
		fi

		echo "[*] Reading package list for '${architecture}'..."
		while read -r -d $'\xFF' package; do
			if [ -n "$package" ]; then
				local package_name
				package_name=$(echo "$package" | grep -i "^Package:" | awk '{ print $2 }')

				if [ -z "${PACKAGE_METADATA["$package_name"]}" ]; then
					PACKAGE_METADATA["$package_name"]="$package"
				else
					local prev_package_ver cur_package_ver
					cur_package_ver=$(echo "$package" | grep -i "^Version:" | awk '{ print $2 }')
					prev_package_ver=$(echo "${PACKAGE_METADATA["$package_name"]}" | grep -i "^Version:" | awk '{ print $2 }')

					# If package has multiple versions, make sure that our metadata
					# contains the latest one.
					if [ "$(echo -e "${prev_package_ver}\n${cur_package_ver}" | sort -rV | head -n1)" = "${cur_package_ver}" ]; then
						PACKAGE_METADATA["$package_name"]="$package"
					fi
				fi
			fi
		done < <(sed -e "s/^$/\xFF/g" "${package_list}")

		rm -f "${package_list}"
	done
}

declare -A PACKAGE_METADATA

for architecture in aarch64 arm i686 x86_64; do
	read_package_list "$architecture"

	for package_path in ${REPO_DIR}/packages/*; do
		package_name=$(basename "$package_path")

		# Remote version - a one in APT repository.
		remote_version=$(echo "${PACKAGE_METADATA[$(basename "$package_name")]}" | grep Version: | awk '{ print $2 }')

		# Local version - a one in package's build.sh script.
		# Note that build.sh script should be loaded inside function to prevent errors.
		local_version=$(
			fn_stub() {
				. "${package_path}/build.sh"
				if [ -n "$TERMUX_PKG_REVISION" ]; then
					echo "${TERMUX_PKG_VERSION}-${TERMUX_PKG_REVISION}"
				else
					echo "${TERMUX_PKG_VERSION}"
				fi
			}
			fn_stub
		)

		if [ -z "$remote_version" ]; then
			echo "${package_name}@${architecture}: package isn't available in repository"
		elif [ "$remote_version" != "$local_version" ]; then
			echo "${package_name}@${architecture}: latest version is '${local_version}' but in repository '${remote_version}'"
		fi
	done
done
