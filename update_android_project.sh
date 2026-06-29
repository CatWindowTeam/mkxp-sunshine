#/bin/sh
TMP=$(mktemp)
find src > $TMP
python3 create-android-project.py cats.catwindowteam.sunshine --variant symlink --output ./android-project < $TMP
