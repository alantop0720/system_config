#!/bin/bash

# 检查命令是否存在并打印软件包管理器名称
check_command() {
    if command -v $1 &> /dev/null; then
        echo "$2 is available"
        exit 0
    fi
}

# Check for common package managers
check_command apt "APT (Debian, Ubuntu)"
check_command apt-get "APT (Debian, Ubuntu)"
check_command yum "YUM (CentOS, RHEL)"
check_command dnf "DNF (Fedora, RHEL 8+)"
check_command rpm "RPM (CentOS, RHEL, Fedora, SUSE)"
check_command pacman "Pacman (Arch Linux)"
check_command zypper "Zypper (openSUSE, SLES)"
check_command eopkg "EOPKG (Solus)"
check_command snap "Snap (Cross-distribution)"
check_command flatpak "Flatpak (Cross-distribution)"
check_command opkg "OPKG (OpenWrt, embedded systems)"
check_command pkg "PKG (FreeBSD)"
check_command apk "APK (Alpine Linux)"
check_command xbps-install "XBPS (Void Linux)"
check_command swupd "swupd (Clear Linux)"
check_command pkgutil "pkgutil (Solaris, Illumos)"
check_command pkg_add "pkg_add (OpenBSD)"
check_command yast "YaST (SUSE Linux Enterprise)"
check_command equo "Equo (Sabayon Linux)"
check_command emerge "Portage (Gentoo)"
check_command nix-env "Nix (NixOS)"
check_command guix "Guix (GuixSD)"
check_command ups "UPS (Unified Package System, Oracle Solaris)"

# 如果没有找到公共包管理器
echo "No common package manager found"
exit 1

