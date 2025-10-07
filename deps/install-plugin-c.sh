#!/usr/bin/env bash
# ================================================================
# Build and install Protobuf C Compiler plugin from GitHub source releases.
#
# Options:
#   --tag=       Git tag to download (default: $TAG)
#   --prefix=    Installation prefix (default: $PREFIX)
#   --dpfx=      Same as --prefix
#   --noclean    Do not remove build directory after install
#   --getmd5     Download archive and print its MD5 sum; do not install
#   --md5=       Expected MD5 sum for archive verification
#   --skipcheck  Skip 'make check' tests
#
# Examples:
#   ./install-plugin-c.sh --noclean
#   ./install-plugin-c.sh --prefix=/opt/xcompute
#   ./install-plugin-c.sh CXX=clang++
#   ./install-plugin-c.sh --dpfx=$HOME/xcdeps --skipcheck --tag=3.20.3 \
#       --md5=a1e8f594f998576180ff1efa49007f54
#
# Requires Bash ≥ 3.0 and tools: curl tar, make
# ================================================================

set -euo pipefail

# Colors
RED='\033[1;31m'
YLW='\033[1;33m'
NC='\033[0m'
YLW_WARNING="${RED}[${YLW}WARNING${RED}]${NC}"

# Defaults
GH_PROTOCC_RLSEDL="https://github.com/protobuf-c/protobuf-c/releases/download"
TAG="v1.5.2"
MD5="0612ee47cccaaf4ad1c4f0c8bdc13abf"  # for v1.5.2

PREFIX="/usr/local"
noclean=0
getmd5=0
skipcheck=0
md5="$MD5"
tag="$TAG"
vers="${TAG#v}"
prefix="$PREFIX"

# Helpers
info() { echo -e "${YLW}[INFO]${NC} $*"; }
error() { echo -e "${RED}[ERROR]${NC} $*" >&2; }
warn() { echo -e "$YLW_WARNING $*" >&2; }

usage() {
    echo "Usage: $0 [--tag=TAG] [--prefix=DIR] [--dpfx=DIR] [--md5=SUM]"
    echo "          [--getmd5] [--noclean] [--skipcheck]"
    exit 1
}

download_file() {
    local url="$1"
    if command -v curl >/dev/null 2>&1; then
        info "Downloading with curl: $url"
        curl -L --fail -O "$url"
    elif command -v wget >/dev/null 2>&1; then
        info "Downloading with wget: $url"
        wget "$url"
    else
        error "Neither curl nor wget found"
        exit 1
    fi
}

compute_md5() {
    local file="$1"
    if command -v md5sum >/dev/null 2>&1; then
        md5sum "$file" | awk '{print $1}'
    elif command -v md5 >/dev/null 2>&1; then
        md5 -q "$file"
    else
        error "No MD5 utility found (need md5sum or md5)"
        exit 1
    fi
}

# Parse command-line args
for arg in "$@"; do
    case "$arg" in
        --tag=*)      tag="${arg#*=}"; vers="${tag#v}" ;;
        --prefix=*)   prefix="${arg#*=}" ;;
        --dpfx=*)     prefix="${arg#*=}" ;;
        --noclean)    noclean=1 ;;
        --getmd5)     getmd5=1 ;;
        --skipcheck)  skipcheck=1 ;;
        --md5=*)      md5="${arg#*=}" ;;
        -h|--help)    usage ;;
        *)            export "$arg" ;;  # allow passing make vars like CXX=clang++
    esac
done

# Pre-flight checks
for cmd in tar make; do
    command -v "$cmd" >/dev/null 2>&1 || { error "$cmd not found"; exit 1; }
done

# Download setup
archive="protobuf-c-${vers}.tar.gz"
url="${GH_PROTOCC_RLSEDL}/${tag}/${archive}"

# Download
download_file "$url"

# If only computing MD5
if [[ "$getmd5" -eq 1 ]]; then
    md5sum_out=$(compute_md5 "$archive")
    echo "MD5 for ${archive}: ${md5sum_out}"
    exit 0
fi

# Verify MD5 if provided
if [[ -z "$md5" ]]; then
    error "No --md5 provided; run with --getmd5 first to obtain it."
    exit 1
fi

actual_md5=$(compute_md5 "$archive")
if [[ "$actual_md5" != "$md5" ]]; then
    error "MD5 mismatch! expected=$md5 actual=$actual_md5"
    exit 1
fi
info "MD5 verified successfully."

# Extract
info "Extracting $archive ..."
tar -xzf "$archive"

src_dir=$(find . -maxdepth 1 -type d -name "protobuf*" ! -name ".*" | head -n1)
cd "$src_dir"
info "Building protobuf-c-${vers} in $PWD"

# Configure and Build
info "Configuring build ..."
./configure  --prefix="$prefix"

info "Building ..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

# Run tests if desired
if [[ "$skipcheck" -eq 0 ]]; then
    info "Running tests ..."
    make check
else
    info "Skipping tests (--skipcheck)."
fi

#--- Install
info "Installing to $prefix ..."
make install

#--- Done
echo
info "Protobuf ${tag} installed successfully at ${prefix}"
[[ "$noclean" -eq 1 ]] && info "Build directory preserved: ${src_dir}"
