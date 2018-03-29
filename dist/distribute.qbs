// Content taken from https://github.com/bjorn/tiled/blob/master/dist/distribute.qbs
import qbs
import qbs.File
import qbs.FileInfo

Product {
    name: "distribute"
    type: "installable"
    condition : { return qbs.architecture.startsWith("x86") }
    builtByDefault: qbs.buildVariant === "release"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.qml" }

    Group {
        name: "Qt QML files"
        prefix: Qt.qml.qmlPath + "/"
        files: [
            "QtQml/**"
        ]
        qbs.installSourceBase: Qt.qml.qmlPath + "/QtQml"
        qbs.install: true
        qbs.installDir: "share/qst/imports/QtQml"
    }

    Group {
        name: "Qt DLLs"
        prefix: {
            if (qbs.targetOS.contains("windows")) {
                return Qt.core.binPath + "/"
            } else {
                return Qt.core.libPath + "/lib"
            }
        }
        property string postfix: {
            var suffix = "";
            if (qbs.targetOS.contains("windows") && qbs.debugInformation)
                suffix += "d";
            return suffix + cpp.dynamicLibrarySuffix;
        }
        files: {
            function addQtVersions(libs) {
                var result = [];
                for (i = 0; i < libs.length; ++i) {
                    var major = libs[i] + "." + Qt.core.versionMajor;
                    var minor = major + "." + Qt.core.versionMinor;
                    var patch = minor + "." + Qt.core.versionPatch;
                    result.push(libs[i], major, minor, patch);
                }
                return result;
            }

            var list = [];

            if (!Qt.core.frameworkBuild) {
                list.push(
                    "Qt5Concurrent" + postfix,
                    "Qt5Core" + postfix,
                    "Qt5Network" + postfix,
                    "Qt5Qml" + postfix,
                    "Qt5SerialPort" + postfix
                );
            }

            if (qbs.targetOS.contains("windows")) {
                if (Qt.core.versionMinor < 7) {
                    list.push("icuin54.dll",
                              "icuuc54.dll",
                              "icudt54.dll");
                }
            } else if (qbs.targetOS.contains("linux")) {
                list = addQtVersions(list);
                if (File.exists(prefix + "icudata.so.56")) {
                    list.push("icudata.so.56", "icudata.so.56.1");
                    list.push("icui18n.so.56", "icui18n.so.56.1");
                    list.push("icuuc.so.56", "icuuc.so.56.1");
                }
            }

            return list;
        }
        qbs.install: true
        qbs.installDir: qbs.targetOS.contains("windows") ? "bin" : "lib"
    }

    Group {
        name: "Runtime DLLs"
        condition: qbs.targetOS.contains("windows")

        prefix: {
            if (qbs.toolchain.contains("mingw"))
                return "/usr/lib/gcc/x86_64-w64-mingw32/5.3-win32/"
            else if (qbs.architecture === "x86_64")
                return "C:/windows/system32/"
            else
                return "C:/windows/SysWOW64/"
        }
        files: {
            if (qbs.toolchain.contains("mingw")) {
                return [
//                    "libgcc_s_dw2-1.dll",
                    "libgcc_s_seh-1.dll",
                    "libstdc++-6.dll",
//                    "libwinpthread-1.dll",
                ]
            } else {
                return [
                    "MSVCP120.DLL",
                    "MSVCR120.DLL",
                ]
            }
        }
        qbs.install: true
        qbs.installDir: "bin"
    }

    Group {
        name: "Misc Files"
        prefix: "../"
        files: {
            var list = [
                "COPYING",
                "README.md",
            ];

            return list;
        }
        qbs.install: true
        qbs.installDir: ""
    }
}
