#!/bin/sh
PANDOC=${PANDOC:-pandoc}
MONTH=`echo $(LANG=en_us_88591; date "+%B")`
YEAR=`echo $(LANG=en_us_88591; date "+%Y")`
VERSION=`cat CMakeLists.txt | sed '/powerkit VERSION/!d;s/)//' | awk '{print $3}'`
echo "% POWERKIT(1) Version ${VERSION} | PowerKit Documentation" > tmp.md
echo "% Ole-André Rodlie" >> tmp.md
echo "% ${MONTH} ${YEAR}" >> tmp.md
cat docs/README.md >> tmp.md
$PANDOC tmp.md -s -t man > share/powerkit.1
rm tmp.md
