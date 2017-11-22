FROM rweickelt/qbs as downloader

ENV SDK=simplelink_cc13x0_sdk_1_50_00_08 \
    XDC=xdctools_3_50_03_33_core

RUN apt-get install -y bzip2 zip wget

RUN wget -nv --continue --tries=20 --waitretry=10 --retry-connrefused \
       --no-dns-cache --timeout 300 \
       https://developer.arm.com/-/media/Files/downloads/gnu-rm/6-2017q2/gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2

RUN tar xjf gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2 -C /opt/


RUN wget -nv --continue --tries=20 --waitretry=10 --retry-connrefused \
        --no-dns-cache --timeout 300 \
        "http://downloads.ti.com/ccs/esd/tirex/zips/${SDK}/${SDK}__linux.zip"

RUN unzip -q -o -d /opt/ti/ \
        "${SDK}__linux.zip" \
        -x "${SDK}/docs/*" \
           "${SDK}/examples/*" \
           "${SDK}/tools/*" \
           "${SDK}/source/ti/blestack/*"


RUN wget -nv --continue --tries=20 --waitretry=10 --retry-connrefused \
        --no-dns-cache --timeout 300 \
        "http://downloads.ti.com/ccs/esd/tirex/zips/${XDC}/${XDC}__linux.zip"

RUN unzip -q -o -d /opt/ti/ \
        "${XDC}__linux.zip"


FROM rweickelt/qbs

ENV SDK=simplelink_cc13x0_sdk_1_50_00_08 \
    XDC=xdctools_3_50_03_33_core \
    TOOLCHAIN=gcc-arm-none-eabi-6-2017-q2-update

RUN apt-get install -y gcc-multilib

COPY qbs /usr/share/
COPY --from=downloader /opt/ti /opt/ti
COPY --from=downloader /opt/${TOOLCHAIN} /opt/${TOOLCHAIN}

RUN qbs setup-toolchains --type gcc /opt/${TOOLCHAIN}/bin/arm-none-eabi-g++ gcc_arm_6 \
    && qbs config profiles.gcc_arm_6.qbs.architecture arm \
    && qbs config profiles.gcc_arm_6.cpp.compilerName g++ \
    && qbs config profiles.gcc_arm_6.cpp.cxxCompilerName g++

RUN qbs config profiles.simplelink_cc13x0.baseProfile gcc_arm_6 \
    && qbs config profiles.simplelink_cc13x0.simplelink.core.deviceFamily cc13x0 \
    && qbs config profiles.simplelink_cc13x0.simplelink.core.installPath /opt/ti/${SDK} \
    && qbs config profiles.simplelink_cc13x0.simplelink.tirtos.xdcInstallPath /opt/ti/${XDC} \
    && qbs config profiles.simplelink_cc13x0.cpp.platformCommonCompilerFlags "[\"-mcpu=cortex-m3\", \"-mthumb\", \"-mfloat-abi=soft\"]"
