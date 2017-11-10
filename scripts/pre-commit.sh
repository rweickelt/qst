#!/bin/bash
# Various sanity checks, quick & dirty.

for f in $(git diff --cached --name-only | grep -e "\.c\$\|\.cpp\$\|\.h\$\|\.qml\$\|\.rst\$\|\.cfg\$\|\.md\$"); do
    # No Windows line endings.
    if file "$f" | grep "CRLF" 2>&1 1>/dev/null; then
        wrong_line_ending_files="$wrong_line_ending_files $f"
        # Don't run other tests on this file.
    fi

    # Check for trailing spaces
    if grep -i -e "[[:blank:]]\$" $f 2>&1 1>/dev/null; then
        trailing_space_files="$trailing_space_files $f"
    fi
done

year=`date +"%Y"`
copyright_line="Copyright[[:space:]](C)[[:space:]].*${year}.*The[[:space:]]Qst[[:space:]]Project\."
for f in $(git diff --cached --name-only | grep -e "\.c\$\|\.cpp\$\|\.h\$"); do
    # Ensure correct copyright line
    if ! grep -i -e "$copyright_line" $f 2>&1 1>/dev/null; then
        missing_copyright_files="$missing_copyright_files $f"
    fi
done

if [ -n "$missing_copyright_files" ] || [ -n "$wrong_line_ending_files" ] || [ -n "$trailing_space_files" ]; then
    echo "Please fix the following issues before committing."
    error=1
else
    error=0
fi

if [ -n "$missing_copyright_files" ]; then
    echo "Files with incorrect copyright note:"
    for f in $missing_copyright_files; do
        echo "    $f"
    done
fi
if [ -n "$wrong_line_ending_files" ]; then
    echo 'Files with incorrect (\\r\\n) line endings:'
    for f in $wrong_line_ending_files; do
        echo "    $f"
    done
fi
if [ -n "$trailing_space_files" ]; then
    echo "Files with trailing white spaces:"
    for f in $trailing_space_files; do
        echo "    $f"
    done
fi

if [ "$error" -eq 1 ]; then
   exit 1
fi