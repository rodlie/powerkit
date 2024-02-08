#!/bin/sh
CWD=`pwd`

if [ ! -f "${CWD}/CMakeLists.txt" ]; then
    echo "Run this script from the powerkit root directory"
    exit 1
fi

PANDOC=${PANDOC:-pandoc}
MONTH=`echo $(LANG=en_us_88591; date "+%B")`
YEAR=`echo $(LANG=en_us_88591; date "+%Y")`
VERSION=`cat ${CWD}/CMakeLists.txt | sed '/powerkit VERSION/!d;s/)//' | awk '{print $3}'`
echo "% POWERKIT(1) Version ${VERSION} | PowerKit Documentation" > ${CWD}/tmp.md
echo "% Ole-André Rodlie" >> ${CWD}/tmp.md
echo "% ${MONTH} ${YEAR}" >> ${CWD}/tmp.md
cat ${CWD}/docs/README.md >> ${CWD}/tmp.md
${PANDOC} ${CWD}/tmp.md -s -t man > ${CWD}/docs/powerkit.1
rm ${CWD}/tmp.md
