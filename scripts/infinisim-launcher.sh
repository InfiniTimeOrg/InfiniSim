#!/usr/bin/env bash
set -euo pipefail

APP_NAME="InfiniSim"
INFINITIME_REPO="https://github.com/InfiniTimeOrg/InfiniTime.git"

if [[ -n "${APPDIR:-}" && -d "${APPDIR}/usr/share/infinisim/source" ]]; then
  export PATH="${APPDIR}/usr/bin:${PATH}"
  if [[ -d "${APPDIR}/usr/lib/git-core" ]]; then
    export GIT_EXEC_PATH="${APPDIR}/usr/lib/git-core"
  fi
  if [[ -d "${APPDIR}/usr/share/git-core/templates" ]]; then
    export GIT_TEMPLATE_DIR="${APPDIR}/usr/share/git-core/templates"
  fi
  PACKAGED_INFINISIM_SOURCE_DIR="${APPDIR}/usr/share/infinisim/source"
  INFINISIM_SOURCE_DIR="${PACKAGED_INFINISIM_SOURCE_DIR}"
else
  PACKAGED_INFINISIM_SOURCE_DIR=""
  script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
  INFINISIM_SOURCE_DIR="$(cd "${script_dir}/.." && pwd)"
fi

DATA_HOME="${XDG_DATA_HOME:-${HOME}/.local/share}/infinisim"
CACHE_HOME="${XDG_CACHE_HOME:-${HOME}/.cache}/infinisim"
CONFIG_HOME="${XDG_CONFIG_HOME:-${HOME}/.config}/infinisim"
CONFIG_FILE="${CONFIG_HOME}/launcher.conf"
BUILD_ROOT="${CACHE_HOME}/build"
DEPS_ROOT="${CACHE_HOME}/deps"
DEFAULT_INFINITIME_DIR="${DATA_HOME}/InfiniTime"
BUNDLED_INFINISIM="${APPDIR:-}/usr/bin/infinisim-official"
BUNDLED_LITTLEFS_DO="${APPDIR:-}/usr/bin/littlefs-do"
BUNDLED_RESOURCE_ZIP="${APPDIR:-}/usr/share/infinisim/resources/resource.zip"
BUNDLED_INFINITIME_REF_FILE="${APPDIR:-}/usr/share/infinisim/resources/infinitime-ref.txt"

mkdir -p "${DATA_HOME}" "${CACHE_HOME}" "${CONFIG_HOME}" "${BUILD_ROOT}" "${DEPS_ROOT}"

prepare_stable_appimage_source() {
  if [[ -z "${PACKAGED_INFINISIM_SOURCE_DIR}" ]]; then
    return
  fi

  local stable_source_dir="${CACHE_HOME}/source"
  local stamp_file="${stable_source_dir}/.infinisim-appimage-stamp"
  local appimage_stamp

  if [[ -n "${APPIMAGE:-}" && -f "${APPIMAGE}" ]]; then
    appimage_stamp="$(stat -c '%n:%Y:%s' "${APPIMAGE}")"
  else
    appimage_stamp="$(stat -c '%n:%Y:%s' "${APPDIR}/AppRun")"
  fi

  if [[ ! -f "${stable_source_dir}/CMakeLists.txt" ]] || [[ ! -f "${stamp_file}" ]] || [[ "$(cat "${stamp_file}")" != "${appimage_stamp}" ]]; then
    rm -rf "${stable_source_dir}"
    mkdir -p "${stable_source_dir}"
    cp -a "${PACKAGED_INFINISIM_SOURCE_DIR}/." "${stable_source_dir}/"
    printf '%s\n' "${appimage_stamp}" > "${stamp_file}"
  fi

  INFINISIM_SOURCE_DIR="${stable_source_dir}"
}

message() {
  local text="$1"
  if command -v zenity >/dev/null 2>&1; then
    zenity --info --title="${APP_NAME}" --text="${text}" || true
  elif command -v kdialog >/dev/null 2>&1; then
    kdialog --title "${APP_NAME}" --msgbox "${text}" || true
  else
    printf '%s\n' "${text}" >&2
  fi
}

load_config() {
  if [[ -f "${CONFIG_FILE}" ]]; then
    # shellcheck disable=SC1090
    source "${CONFIG_FILE}"
  fi
}

error() {
  local text="$1"
  if command -v zenity >/dev/null 2>&1; then
    zenity --error --title="${APP_NAME}" --text="${text}" || true
  elif command -v kdialog >/dev/null 2>&1; then
    kdialog --title "${APP_NAME}" --error "${text}" || true
  else
    printf 'Error: %s\n' "${text}" >&2
  fi
}

choose_action() {
  if command -v zenity >/dev/null 2>&1; then
    zenity --list --title="${APP_NAME}" \
      --width=760 --height=320 \
      --text="Elige como iniciar InfiniSim." \
      --column="accion" --column="opcion" --hide-column=1 \
      "run-compiled" "Iniciar un simulador ya compilado" \
      "build-local" "Compilar desde una carpeta local de InfiniTime" \
      "build-clone" "Descargar InfiniTime oficial y compilar" 2>/dev/null || true
  elif command -v kdialog >/dev/null 2>&1; then
    kdialog --title "${APP_NAME}" --menu "Elige como iniciar InfiniSim." \
      "run-compiled" "Iniciar un simulador ya compilado" \
      "build-local" "Compilar desde una carpeta local de InfiniTime" \
      "build-clone" "Descargar InfiniTime oficial y compilar" 2>/dev/null || true
  else
    printf '%s\n' "Selecciona una accion:" >&2
    printf '%s\n' "1) Iniciar simulador ya compilado" >&2
    printf '%s\n' "2) Compilar desde carpeta local de InfiniTime" >&2
    printf '%s\n' "3) Descargar InfiniTime oficial y compilar" >&2
    printf '> ' >&2
    read -r reply
    case "${reply}" in
      1) printf '%s\n' "run-compiled" ;;
      2) printf '%s\n' "build-local" ;;
      3) printf '%s\n' "build-clone" ;;
      *) return 1 ;;
    esac
  fi
}

choose_directory() {
  local initial_dir="${1:-${HOME}}"
  if command -v zenity >/dev/null 2>&1; then
    zenity --file-selection --directory --filename="${initial_dir}/" --title="Selecciona la carpeta de InfiniTime" 2>/dev/null || true
  elif command -v kdialog >/dev/null 2>&1; then
    kdialog --getexistingdirectory "${initial_dir}" "Selecciona la carpeta de InfiniTime" 2>/dev/null || true
  else
    printf '%s' "Ruta de InfiniTime: " >&2
    read -r reply
    printf '%s\n' "${reply}"
  fi
}

choose_binary_file() {
  local initial_file="${1:-}"
  if command -v zenity >/dev/null 2>&1; then
    if [[ -n "${initial_file}" ]]; then
      zenity --file-selection --filename="${initial_file}" --title="Selecciona un ejecutable de InfiniSim" 2>/dev/null || true
    else
      zenity --file-selection --title="Selecciona un ejecutable de InfiniSim" 2>/dev/null || true
    fi
  elif command -v kdialog >/dev/null 2>&1; then
    if [[ -n "${initial_file}" ]]; then
      kdialog --getopenfilename "${initial_file}" "*" "Selecciona un ejecutable de InfiniSim" 2>/dev/null || true
    else
      kdialog --getopenfilename "${HOME}" "*" "Selecciona un ejecutable de InfiniSim" 2>/dev/null || true
    fi
  else
    printf '%s' "Ruta del ejecutable de InfiniSim: " >&2
    read -r reply
    printf '%s\n' "${reply}"
  fi
}

load_last_dir() {
  load_config
  printf '%s\n' "${INFINITIME_DIR:-}"
}

load_last_binary() {
  load_config
  printf '%s\n' "${INFINISIM_BINARY:-}"
}

save_last_dir() {
  local dir="$1"
  local last_binary
  last_binary="$(load_last_binary)"
  {
    printf 'INFINITIME_DIR=%q\n' "${dir}"
    printf 'INFINISIM_BINARY=%q\n' "${last_binary}"
  } > "${CONFIG_FILE}"
}

save_last_binary() {
  local binary="$1"
  local last_dir
  last_dir="$(load_last_dir)"
  {
    printf 'INFINITIME_DIR=%q\n' "${last_dir}"
    printf 'INFINISIM_BINARY=%q\n' "${binary}"
  } > "${CONFIG_FILE}"
}

ensure_command() {
  local cmd="$1"
  local hint="$2"
  if ! command -v "${cmd}" >/dev/null 2>&1; then
    error "Falta '${cmd}'. ${hint}"
    exit 1
  fi
}

missing_build_requirements() {
  local missing=()

  if ! command -v cmake >/dev/null 2>&1; then
    missing+=("cmake")
  fi

  if ! command -v git >/dev/null 2>&1; then
    missing+=("git")
  fi

  if ! command -v c++ >/dev/null 2>&1 && ! command -v g++ >/dev/null 2>&1 && ! command -v clang++ >/dev/null 2>&1; then
    missing+=("c++")
  fi

  if command -v pkg-config >/dev/null 2>&1; then
    if ! pkg-config --exists sdl2; then
      missing+=("sdl2-dev")
    fi
  elif ! command -v sdl2-config >/dev/null 2>&1; then
    missing+=("sdl2-dev")
  fi

  printf '%s\n' "${missing[@]:-}"
}

build_requirements_hint() {
  if [[ -f /etc/os-release ]]; then
    # shellcheck disable=SC1091
    source /etc/os-release
    case "${ID:-}" in
      fedora)
        printf '%s\n' "sudo dnf install cmake git SDL2-devel gcc-c++ npm python3 python3-virtualenv"
        return
        ;;
      ubuntu|debian|linuxmint|pop)
        printf '%s\n' "sudo apt install -y cmake git libsdl2-dev g++ npm python3 python3-venv"
        return
        ;;
      arch|manjaro)
        printf '%s\n' "sudo pacman -S cmake git sdl2 gcc npm python python-virtualenv"
        return
        ;;
      opensuse*|sles)
        printf '%s\n' "sudo zypper install cmake git libSDL2-devel gcc-c++ npm python3 python3-virtualenv"
        return
        ;;
    esac
  fi

  printf '%s\n' "Instala: cmake, git, SDL2 devel, compilador C++, npm y python3"
}

ensure_build_tools() {
  local missing
  missing="$(missing_build_requirements)"
  if [[ -n "${missing}" ]]; then
    error "Faltan dependencias de compilacion:\n${missing}\n\nInstalacion sugerida:\n$(build_requirements_hint)"
    exit 1
  fi
}

run_bundled_official() {
  local infinitime_dir="$1"
  shift

  if [[ "${INFINISIM_FORCE_REBUILD:-0}" == "1" || ! -x "${BUNDLED_INFINISIM}" ]]; then
    return 1
  fi

  if [[ -d "${APPDIR:-}" && "${infinitime_dir}" == "${DEFAULT_INFINITIME_DIR}" ]]; then
    if [[ -f "${BUNDLED_RESOURCE_ZIP}" && -x "${BUNDLED_LITTLEFS_DO}" ]]; then
      "${BUNDLED_LITTLEFS_DO}" res load "${BUNDLED_RESOURCE_ZIP}" || true
    fi
    exec "${BUNDLED_INFINISIM}" "$@"
  fi

  return 1
}

ensure_infinisim_submodules() {
  if [[ ! -e "${INFINISIM_SOURCE_DIR}/lv_drivers/CMakeLists.txt" && -d "${INFINISIM_SOURCE_DIR}/.git" ]]; then
    message "Preparando submodulos de InfiniSim..."
    git -C "${INFINISIM_SOURCE_DIR}" submodule update --init --recursive lv_drivers
  fi
}

ensure_infinitime_tree() {
  local dir="$1"
  if [[ ! -f "${dir}/CMakeLists.txt" || ! -d "${dir}/src" ]]; then
    error "'${dir}' no parece una carpeta valida de InfiniTime."
    exit 1
  fi

  if [[ ! -d "${dir}/src/libs/lvgl/src" && -d "${dir}/.git" ]]; then
    message "Preparando submodulos de InfiniTime..."
    git -C "${dir}" submodule update --init --recursive
  fi

  if [[ ! -d "${dir}/src/libs/lvgl/src" ]]; then
    error "InfiniTime necesita el submodulo src/libs/lvgl. Ejecuta: git -C '${dir}' submodule update --init --recursive"
    exit 1
  fi
}

ensure_node_tools() {
  if command -v lv_font_conv >/dev/null 2>&1; then
    return
  fi
  ensure_command npm "Instala Node.js/npm o instala lv_font_conv manualmente."
  if [[ ! -x "${DEPS_ROOT}/node_modules/.bin/lv_font_conv" ]]; then
    message "Instalando lv_font_conv en la cache local de InfiniSim..."
    npm --prefix "${DEPS_ROOT}" install lv_font_conv@1.5.2
  fi
  export PATH="${DEPS_ROOT}/node_modules/.bin:${PATH}"
}

ensure_python_tools() {
  ensure_command python3 "Instala Python 3 para generar recursos."
  if python3 - <<'PY' >/dev/null 2>&1
import PIL
PY
  then
    return
  fi
  if [[ ! -x "${DEPS_ROOT}/venv/bin/python" ]]; then
    message "Instalando Pillow en la cache local de InfiniSim..."
    python3 -m venv "${DEPS_ROOT}/venv"
    "${DEPS_ROOT}/venv/bin/python" -m pip install --upgrade pip wheel Pillow
  fi
  export PATH="${DEPS_ROOT}/venv/bin:${PATH}"
}

clone_official_infinitime() {
  local desired_ref=""

  if [[ -f "${BUNDLED_INFINITIME_REF_FILE}" ]]; then
    desired_ref="$(head -n 1 "${BUNDLED_INFINITIME_REF_FILE}")"
  elif [[ -d "${INFINISIM_SOURCE_DIR}/InfiniTime/.git" ]]; then
    desired_ref="$(git -C "${INFINISIM_SOURCE_DIR}/InfiniTime" rev-parse HEAD 2>/dev/null || true)"
  fi

  if [[ -d "${DEFAULT_INFINITIME_DIR}/.git" ]]; then
    message "Actualizando InfiniTime oficial en ${DEFAULT_INFINITIME_DIR}..."
    git -C "${DEFAULT_INFINITIME_DIR}" fetch --all --tags --prune >&2
  else
    message "Clonando InfiniTime oficial en ${DEFAULT_INFINITIME_DIR}..."
    rm -rf "${DEFAULT_INFINITIME_DIR}"
    git clone --recursive "${INFINITIME_REPO}" "${DEFAULT_INFINITIME_DIR}" >&2
  fi

  if [[ -n "${desired_ref}" ]]; then
    message "Usando revision oficial probada con esta version de InfiniSim..."
    git -C "${DEFAULT_INFINITIME_DIR}" checkout "${desired_ref}" >&2
  else
    git -C "${DEFAULT_INFINITIME_DIR}" pull --ff-only >&2
  fi

  git -C "${DEFAULT_INFINITIME_DIR}" submodule update --init --recursive >&2
  printf '%s\n' "${DEFAULT_INFINITIME_DIR}"
}

resolve_compiled_binary() {
  local action="$1"
  local binary_path=""
  local last_binary
  last_binary="$(load_last_binary)"

  if [[ -n "${INFINISIM_BINARY:-}" ]]; then
    binary_path="${INFINISIM_BINARY}"
  elif [[ -x "${BUNDLED_INFINISIM}" ]]; then
    binary_path="${BUNDLED_INFINISIM}"
  else
    binary_path="$(choose_binary_file "${last_binary}")"
  fi

  if [[ -z "${binary_path}" ]]; then
    exit 0
  fi

  if [[ ! -f "${binary_path}" ]]; then
    error "No se encontro el archivo seleccionado."
    exit 1
  fi

  if [[ ! -x "${binary_path}" ]]; then
    error "El archivo seleccionado no es ejecutable."
    exit 1
  fi

  save_last_binary "${binary_path}"

  if [[ "${action}" == "run-compiled" && "${binary_path}" == "${BUNDLED_INFINISIM}" ]] && [[ -f "${BUNDLED_RESOURCE_ZIP}" && -x "${BUNDLED_LITTLEFS_DO}" ]]; then
    "${BUNDLED_LITTLEFS_DO}" res load "${BUNDLED_RESOURCE_ZIP}" || true
  fi

  printf '%s\n' "${binary_path}"
}

build_and_run() {
  local infinitime_dir="$1"
  local source_key build_dir jobs
  shift

  source_key="$(printf '%s\n%s' "${INFINISIM_SOURCE_DIR}" "${infinitime_dir}" | cksum | awk '{print $1}')"
  build_dir="${BUILD_ROOT}/${source_key}"
  jobs="${INFINISIM_BUILD_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf '2')}"

  cmake -S "${INFINISIM_SOURCE_DIR}" -B "${build_dir}" \
    -DInfiniTime_DIR="${infinitime_dir}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_PNG=OFF
  cmake --build "${build_dir}" --parallel "${jobs}"

  if [[ -f "${build_dir}/resources/resource.zip" && -x "${build_dir}/littlefs-do" ]]; then
    "${build_dir}/littlefs-do" res load "${build_dir}/resources/resource.zip" || true
  fi

  exec "${build_dir}/infinisim" "$@"
}

main() {
  local action infinitime_dir binary_path last_dir
  prepare_stable_appimage_source

  if [[ -n "${INFINITIME_DIR:-}" ]]; then
    action="build-local"
  elif [[ -n "${INFINISIM_BINARY:-}" ]]; then
    action="run-compiled"
  else
    action="$(choose_action)"
  fi

  case "${action}" in
    run-compiled)
      binary_path="$(resolve_compiled_binary "${action}")"
      exec "${binary_path}" "$@"
      ;;
    build-local)
      last_dir="$(load_last_dir)"
      if [[ -n "${INFINITIME_DIR:-}" ]]; then
        infinitime_dir="${INFINITIME_DIR}"
      else
        infinitime_dir="$(choose_directory "${last_dir:-$HOME}")"
      fi
      ;;
    build-clone)
      infinitime_dir="$(clone_official_infinitime)"
      ;;
    "")
      exit 0
      ;;
    *)
      error "Opcion no reconocida."
      exit 1
      ;;
  esac

  if [[ -z "${infinitime_dir:-}" ]]; then
    exit 0
  fi

  infinitime_dir="$(cd "${infinitime_dir}" && pwd)"
  ensure_infinitime_tree "${infinitime_dir}"
  save_last_dir "${infinitime_dir}"

  run_bundled_official "${infinitime_dir}" "$@" || true

  ensure_build_tools
  ensure_infinisim_submodules
  ensure_node_tools
  ensure_python_tools

  build_and_run "${infinitime_dir}" "$@"
}

main "$@"
