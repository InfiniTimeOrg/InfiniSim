#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
APPDIR="${ROOT_DIR}/build/AppDir"
LINUXDEPLOY="${ROOT_DIR}/build/linuxdeploy-x86_64.AppImage"
OFFICIAL_BUILD_DIR="${ROOT_DIR}/build/appimage-official"

rm -rf "${APPDIR}"

mkdir -p "${APPDIR}/usr/bin" \
  "${APPDIR}/usr/share/applications" \
  "${APPDIR}/usr/share/icons/hicolor/scalable/apps" \
  "${APPDIR}/usr/share/infinisim/resources"

if [[ -d "${ROOT_DIR}/InfiniTime/src/libs/lvgl/src" ]]; then
  official_ref="$(git -C "${ROOT_DIR}/InfiniTime" rev-parse HEAD)"

  cmake -S "${ROOT_DIR}" -B "${OFFICIAL_BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_PNG=OFF
  cmake --build "${OFFICIAL_BUILD_DIR}" --parallel "$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf '2')"

  install -m 0755 "${OFFICIAL_BUILD_DIR}/infinisim" "${APPDIR}/usr/bin/infinisim-official"
  install -m 0755 "${OFFICIAL_BUILD_DIR}/littlefs-do" "${APPDIR}/usr/bin/littlefs-do"
  if [[ -f "${OFFICIAL_BUILD_DIR}/resources/resource.zip" ]]; then
    install -m 0644 "${OFFICIAL_BUILD_DIR}/resources/resource.zip" "${APPDIR}/usr/share/infinisim/resources/resource.zip"
  fi
  printf '%s\n' "${official_ref}" > "${APPDIR}/usr/share/infinisim/resources/infinitime-ref.txt"
else
  printf '%s\n' "InfiniTime submodule is missing; AppImage will not include the official prebuilt binary." >&2
fi

install -m 0755 "${ROOT_DIR}/scripts/infinisim-launcher.sh" "${APPDIR}/usr/bin/infinisim-launcher"
install -m 0755 "${ROOT_DIR}/scripts/infinisim-launcher-ui.py" "${APPDIR}/usr/bin/infinisim-launcher-ui.py"
install -m 0755 "${ROOT_DIR}/scripts/infinisim_launcher_i18n.py" "${APPDIR}/usr/bin/infinisim_launcher_i18n.py"

# Install i18n translation files
mkdir -p "${APPDIR}/usr/share/locale/en/LC_MESSAGES" \
         "${APPDIR}/usr/share/locale/es/LC_MESSAGES"
install -m 0644 "${ROOT_DIR}/scripts/infinisim-launcher-en.mo" "${APPDIR}/usr/share/locale/en/LC_MESSAGES/infinisim-launcher.mo"
install -m 0644 "${ROOT_DIR}/scripts/infinisim-launcher-es.mo" "${APPDIR}/usr/share/locale/es/LC_MESSAGES/infinisim-launcher.mo"
install -m 0755 "${ROOT_DIR}/packaging/appimage/AppRun" "${APPDIR}/AppRun"
install -m 0644 "${ROOT_DIR}/packaging/appimage/infinisim.desktop" "${APPDIR}/usr/share/applications/infinisim.desktop"
install -m 0644 "${ROOT_DIR}/packaging/appimage/infinisim.svg" "${APPDIR}/usr/share/icons/hicolor/scalable/apps/infinisim.svg"

if command -v git >/dev/null 2>&1; then
  install -m 0755 "$(command -v git)" "${APPDIR}/usr/bin/git"
  if [[ -d /usr/lib/git-core ]]; then
    mkdir -p "${APPDIR}/usr/lib"
    rsync -a /usr/lib/git-core "${APPDIR}/usr/lib/"
  fi
  if [[ -d /usr/share/git-core ]]; then
    mkdir -p "${APPDIR}/usr/share"
    rsync -a /usr/share/git-core "${APPDIR}/usr/share/"
  fi
fi

rsync -a --delete \
  --exclude '/.git' \
  --exclude '/build' \
  --exclude '/InfiniTime' \
  --exclude '/node_modules' \
  --exclude '/.venv' \
  "${ROOT_DIR}/" "${APPDIR}/usr/share/infinisim/source/"

if [[ ! -x "${LINUXDEPLOY}" ]]; then
  curl -L \
    https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage \
    -o "${LINUXDEPLOY}"
  chmod +x "${LINUXDEPLOY}"
fi

linuxdeploy_args=(
  --appdir "${APPDIR}"
  --desktop-file "${APPDIR}/usr/share/applications/infinisim.desktop"
  --icon-file "${APPDIR}/usr/share/icons/hicolor/scalable/apps/infinisim.svg"
)

if [[ -x "${APPDIR}/usr/bin/infinisim-official" ]]; then
  linuxdeploy_args+=(--executable "${APPDIR}/usr/bin/infinisim-official")
fi

if [[ -x "${APPDIR}/usr/bin/littlefs-do" ]]; then
  linuxdeploy_args+=(--executable "${APPDIR}/usr/bin/littlefs-do")
fi

if [[ -x "${APPDIR}/usr/bin/git" ]]; then
  linuxdeploy_args+=(--executable "${APPDIR}/usr/bin/git")
  while IFS= read -r helper; do
    if readelf -h "${helper}" >/dev/null 2>&1; then
      linuxdeploy_args+=(--executable "${helper}")
    fi
  done < <(find "${APPDIR}/usr/lib/git-core" -maxdepth 1 -type f -executable 2>/dev/null)
fi

NO_STRIP=1 ARCH=x86_64 "${LINUXDEPLOY}" "${linuxdeploy_args[@]}" --output appimage
