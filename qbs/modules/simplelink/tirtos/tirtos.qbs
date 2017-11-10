import qbs
import qbs.File
import qbs.FileInfo
import qbs.ModUtils
import qbs.Utilities
import qbs.TextFile
import "../SimpleLinkModule.qbs" as SimpleLinkModule

SimpleLinkModule {
    name : "tirtos"

    property string xdcInstallPath
    property string buildDirectoryName : "tirtos_kernel"

    Depends { name : "cpp" }

    cpp.includePaths: [
        simplelink.core.installPath + "/kernel/tirtos/packages/",
        simplelink.core.installPath + "/kernel/tirtos/packages/sysbios/posix/",
        simplelink.core.installPath + "/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano",
        simplelink.core.installPath + "/packages/",
        xdcInstallPath + "/packages"
    ]

    cpp.libraryPaths: [
        simplelink.core.installPath + "/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7-m",
        simplelink.core.installPath + "/kernel/tirtos/packages/ti/dpl/lib",
        // cpp.toolchaininstallPath + "/../arm-none-eabi/lib/thumb/v7-m/",
        // sdkInstallPath + "/kernel/nortos/lib",
    ]

    cpp.staticLibraries: [
        // "--entry",
        // "resetISR",
        ":dpl_" + simplelink.core.deviceFamily + ".am3g",
        // ":nortos_" + device + ".am3g",
    ]

    FileTagger {
        patterns: ["*.cfg"]
        fileTags: ["cfg"]
    }

    /* Build .cfg file */
    Rule {
        inputs: ["cfg"]

        Artifact {
            fileTags: "kernel_linkerscript"
            filePath: product.buildDirectory
                      + "/" + product.moduleProperty("simplelink.tirtos", "buildDirectoryName")
                      + "/linker.cmd"
        }

        Artifact {
            fileTags: "toolchainOptions"
            filepath: product.buildDirectory
                      + "/" + product.moduleProperty("simplelink.tirtos", "buildDirectoryName")
                      + "/compiler.opt"
        }

        prepare : {
            var defines = product.moduleProperty("cpp", "defines");
            var compilerOptions = Array();
            compilerOptions = compilerOptions.concat(defines.map( function(item) { return "-D" + item; } ));

            var args = [
                "--xdcpath="
                        + product.moduleProperty("simplelink.core", "installPath") + "/source;"
                        + product.moduleProperty("simplelink.core", "installPath") + "/kernel/tirtos/packages;"
                        + "/opt/ti/ccsv7/ccs_base",
                "xdc.tools.configuro",
                "-o",
                product.moduleProperty("simplelink.tirtos", "buildDirectoryName"),
                "-t",
                "gnu.targets.arm.M3",
                "-p",
                "ti.platforms.simplelink:" + "CC1310F128",
                "-r",
                "release",
                "-c",
                product.moduleProperty("cpp", "toolchainInstallPath") + "/../",
                "--compileOptions",
                compilerOptions.join(" ")
            ];
            args.push(input.filePath)

            var cmd = new Command(product.moduleProperty("simplelink.tirtos", "xdcInstallPath") + "/xs", args);
            cmd.description = "building tirtos kernel from " + input.fileName;
            cmd.highlight = "compiler";
            cmd.workingDirectory = product.buildDirectory;
            return cmd;
        }
    }

    /* Combine linker scripts in correct order */
    Rule {
        inputs: [ "board_linkerscript", "kernel_linkerscript" ]
        multiplex: true

        Artifact {
            fileTags: "linkerscript"
            filePath: product.buildDirectory + "/combined-linkerscript.lds"
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "Combinging linker scripts";
            cmd.sourceCode = function() {
                var boardLinkerFile = new TextFile(inputs.board_linkerscript[0].filePath, TextFile.ReadOnly);
                var boardLinkerScript = boardLinkerFile.readAll();
                boardLinkerFile.close();

                var kernelLinkerFile = new TextFile(inputs.kernel_linkerscript[0].filePath, TextFile.ReadOnly);
                var kernelLinkerScript = kernelLinkerFile.readAll();
                kernelLinkerFile.close();

                var outFile = new TextFile(output.filePath, TextFile.WriteOnly);
                outFile.write(boardLinkerScript);
                outFile.write("\n");
                outFile.write(kernelLinkerScript);
                outFile.close();
            };
            return cmd;
        }
    }
}
