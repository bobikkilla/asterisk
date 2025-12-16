#!/bin/bash

script="./main.sh"

if [[ ! -x "$script" ]]; then
    echo "error: '$script' not found or not executable"
    exit 1
fi

test_dir="test_archives_$(date +%s)"
mkdir -p "$test_dir"
cd "$test_dir" || exit 1

# create test content
cat > testfile.txt <<'EOF'
this is a test file for decompression verification.
line 1
line 2
EOF

echo "creating test archives..."

# generate archives
gzip    < testfile.txt > testfile.txt.gz
bzip2   < testfile.txt > testfile.txt.bz2
xz      < testfile.txt > testfile.txt.xz
zip     -q testfile.txt.zip testfile.txt
tar -cf testfile.tar testfile.txt
tar -czf testfile.tar.gz testfile.txt
tar -cjf testfile.tar.bz2 testfile.txt
tar -cJf testfile.tar.xz testfile.txt

# optional: 7z and rar
has_7z=false
has_rar=false
if command -v 7z >/dev/null 2>&1; then
    7z a -y testfile.7z testfile.txt >/dev/null && has_7z=true
fi
if command -v unrar >/dev/null 2>&1 || command -v rar >/dev/null 2>&1; then
    if command -v rar >/dev/null 2>&1; then
        rar a -ep -inul testfile.rar testfile.txt && has_rar=true
    elif command -v unrar >/dev/null 2>&1 && [[ -f testfile.rar ]]; then
        has_rar=true
    fi
fi

cp testfile.txt not_an_archive.dat

echo
ls -lh
echo

echo "running decompression script..."
# pass existing files only (avoid unexpanded globs)
files=()
for f in *.gz *.bz2 *.xz *.zip *.tar *.7z *.rar; do
    [[ -f "$f" ]] && files+=("$f")
done

if [[ ${#files[@]} -gt 0 ]]; then
    "$script" "${files[@]}" 2>/dev/null || true
fi

echo
echo "checking results..."

passed=0
total=0

check() {
    local file="$1"
    local desc="$2"
    ((total++))
    if [[ -f "$file" ]] && cmp -s "$file" testfile.txt; then
        echo "ok:     $desc"
        ((passed++))
    else
        echo "failed: $desc"
    fi
}

check testfile.txt "gzip"
check testfile.txt "bzip2"
check testfile.txt "xz"
check testfile.txt "zip"
check testfile.txt "tar"
check testfile.txt "tar.gz"
check testfile.txt "tar.bz2"
check testfile.txt "tar.xz"

if [[ "$has_7z" == true ]]; then
    check testfile.txt "7z"
else
    echo "skip:   7z (p7zip not installed)"
fi

if [[ "$has_rar" == true ]] && [[ -f testfile.rar ]]; then
    check testfile.txt "rar"
else
    echo "skip:   rar (unrar/rar not installed)"
fi

# verify non-archive file is untouched
((total++))
if [[ -f not_an_archive.dat ]] && cmp -s not_an_archive.dat testfile.txt; then
    echo "ok:     non-archive file unchanged"
    ((passed++))
else
    echo "failed: non-archive file altered or missing"
fi

echo
echo "summary: $passed / $total tests passed"

cd ..
m -rf "$test_dir"

if [[ $passed -eq $total ]]; then
    echo "all tests passed."
    exit 0
else
    echo "some tests failed."
    exit 1
fi