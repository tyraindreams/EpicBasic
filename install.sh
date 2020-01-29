#!/bin/bash
INSTALLPREFIX=${INSTALLPREFIX:-"/usr/local"}

KDELOCALPREFIX=`kde4-config --localprefix`
if [[ -z "$KDELOCALPREFIX" ]]; then
	KDELOCALPREFIX="$HOME/.kde"
fi

set -e

# Sudo program - set to " " or something if you dont want to sudo install
SUDOPROG=${SUDOPROG:-sudo}

cd "$(dirname "$0")"

echo "Welcome to the EpicBasic interactive installer."
echo "Current install prefix is : ${INSTALLPREFIX}"
echo ""
echo "For this script to succeed and for you to have a working version of the EpicBasic we need the following packages on your system."
echo ""
echo "build-essential"
echo "clang"
echo "libpcre3-dev"
echo ""
echo "If you are running Ubuntu or Debian we can install these packages automatically."
read -p "Are you running Ubuntu or Debian?[no] " INPUT
INPUT=${INPUT:-no}
echo ""

set +e

if [ "yes" == "$INPUT" ] || [ "y" == "$INPUT" ]; then
   
   
   PKG_OK=$(dpkg-query -W --showformat='${Status}\n' build-essential|grep "install ok installed")
   
   echo "Checking for build-essential: $PKG_OK"
   
   if [ "" == "$PKG_OK" ]; then
      
      echo "build-essential does not appear to be installed. Installing build-essential."
      
      ${SUDOPROG} apt-get --force-yes --yes install build-essential
      
   fi
   
   PKG_OK=$(dpkg-query -W --showformat='${Status}\n' clang|grep "install ok installed")
   
   echo "Checking for clang: $PKG_OK"
   
   if [ "" == "$PKG_OK" ]; then
      
      echo "clang does not appear to be installed. Installing clang."
      
      ${SUDOPROG} apt-get --force-yes --yes install clang
      
   fi
   
   PKG_OK=$(dpkg-query -W --showformat='${Status}\n' libpcre3-dev|grep "install ok installed")
   
   echo "Checking for libpcre3-dev: $PKG_OK"
   
   if [ "" == "$PKG_OK" ]; then
      
      echo "libpcre3-dev does not appear to be installed. Installing libpcre3-dev."
      
      ${SUDOPROG} apt-get --force-yes --yes install libpcre3-dev
      
   fi
   
fi

set -e

echo ""
echo "Building progenitor..."
echo ""

clang++ ./src/progenitor/Main.eb.cpp -o /tmp/progenitor.bin -std=c++11 -lpcre -v > /dev/null

echo ""
echo "Building ebcompiler..."
echo ""

export EPICBASIC_HEADER_PATH=$(pwd)/modules/
export EPICBASIC_MODULE_PATH=$(pwd)/modules/

/tmp/progenitor.bin ./src/ebcompiler/Main.eb -o /tmp/ebcompiler -coff > /dev/null
/tmp/ebcompiler ./src/ebcompiler/Main.eb -o /tmp/ebcompiler -O3 -coff -cd

unset EPICBASIC_HEADER_PATH
unset EPICBASIC_MODULE_PATH

echo ""
echo "Installing EpicBasic"
echo ""

${SUDOPROG} mkdir -p ${INSTALLPREFIX}/bin/
${SUDOPROG} cp -f /tmp/ebcompiler ${INSTALLPREFIX}/bin/

if [ "$(pwd)" != ${INSTALLPREFIX}/share/epicbasic ]; then
   
   ${SUDOPROG} mkdir -p ${INSTALLPREFIX}/share/epicbasic
   ${SUDOPROG} cp -rf ./ ${INSTALLPREFIX}/share/epicbasic/
   
fi

export EPICBASIC_HEADER_PATH="${INSTALLPREFIX}/share/epicbasic/modules/"
export EPICBASIC_MODULE_PATH="${INSTALLPREFIX}/share/epicbasic/modules/"

${INSTALLPREFIX}/bin/ebcompiler ./src/ebsci/ebsci.eb -o /tmp/ebsci -O3 -coff > /dev/null

unset EPICBASIC_HEADER_PATH
unset EPICBASIC_MODULE_PATH

echo ""
echo "Installing EpicBasic Tools"
echo ""

${SUDOPROG} cp -f /tmp/ebsci ${INSTALLPREFIX}/bin/

echo ""
read -p "Would you like to install the KDevelop/Kate syntax highlighting file?[no] " INPUT
INPUT=${INPUT:-no}
echo ""

if [ "yes" == "$INPUT" ] || [ "y" == "$INPUT" ]; then
   
   echo "Installing KDevelop/Kate hightlighting file"
   mkdir -p ${KDELOCALPREFIX}/share/apps/katepart/syntax/
   cp -f ./extra/Kdevelop-Kate/epicbasic.xml ${KDELOCALPREFIX}/share/apps/katepart/syntax/
   
fi

echo ""
read -p "Would you like to install the KDevelop project templates?[no] " INPUT
INPUT=${INPUT:-no}
echo ""

if [ "yes" == "$INPUT" ] || [ "y" == "$INPUT" ]; then
   
   echo "Installing KDevelop templates"
   mkdir -p ${KDELOCALPREFIX}/share/apps/kdevappwizard/templates/
   cp -f ./extra/Kdevelop-Kate/epicbasic.tar.bz2 ${KDELOCALPREFIX}/share/apps/kdevappwizard/templates/
   cp -f ./extra/Kdevelop-Kate/epicbasic-cmake.tar.bz2 ${KDELOCALPREFIX}/share/apps/kdevappwizard/templates/
fi

