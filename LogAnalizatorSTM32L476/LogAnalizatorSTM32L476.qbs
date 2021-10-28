import qbs
import qbs.File
import qbs.FileInfo
import qbs.ModUtils

Project{

    //# cpu
    property string CPU: '-mcpu=cortex-m4'

    //# fpu
    property string FPU: '-mfpu=fpv4-sp-d16'

    //# float-abi
    property string FLOAT_ABI: '-mfloat-abi=hard'

    //# mcu
    // property string   MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

    CppApplication {
        condition: {
            return (qbs.toolchain.contains("gcc") && !qbs.toolchain.contains("xcode"))
        }

        type: ["application", "bin", "hex", "size"]

        //name: "stm32g4xx"
        cpp.cLanguageVersion: "c17"
        cpp.cxxLanguageVersion: "c++20"

        cpp.debugInformation: true//qbs.buildVariant !== "release"
        cpp.enableExceptions : false
        cpp.enableRtti : false
        cpp.optimization: qbs.buildVariant === "release" ? "fast" : "none"
        cpp.positionIndependentCode: false

        Group{
            name: "asm"
            fileTags:  "asm"
            prefix: path
            files:["/**/*.s"]
        }

        Group{
            name: "c"
            fileTags:  "c"
            prefix: path
            files:["/**/*.c"]
            excludeFiles: ["/**/main.c"]
        }

        Group{
            name: "doc"
            prefix: path
            files:["/**/*.pdf"]
        }

        Group{
            name: "hpp"
            fileTags:  "hpp"
            prefix: path
            files:["/**/*.h","/**/*.hpp"]
            excludeFiles: ["/Ada*/*"]
        }

        Group{
            name: "linkerscript"
            fileTags:  "linkerscript"
            prefix: path
            files:["/**/*.ld"]
            excludeFiles: ["/**/*M.ld"]
        }

        Group{
            name: "cpp"
            fileTags:  "cpp"
            prefix: path
            files:["/**/*.cpp"]
            excludeFiles: ["/Ada*/*"]
        }

        cpp.includePaths:{
            var includePaths = []
            if(0){
                var file = TextFile(FileInfo.joinPaths(path, 'Makefile'))
                var regex = /-I(\S+)\s*.?/
                while(!file.atEof()){
                    var result = regex.exec(file.readLine());
                    if(result !== null)
                        includePaths.push(result[1])
                }
                file.close()
            }else{
                var exclude = [
                            'DSP',
                            'RTOS2',
                            'RTOS',
                            'NN',
                            'Template',
                            'Core_A',
                        ]
                var func = function(path){
                    var filePpaths = File.directoryEntries(path, File.Files)
                    var size = includePaths.length
                    var fl = true
                    for(var i in exclude)
                        if(path.endsWith(exclude[i]))
                            fl = false
                    for(var i in filePpaths)
                        if(fl && size === includePaths.length && (filePpaths[i].endsWith('.h')||filePpaths[i].endsWith('.hpp')))
                            includePaths.push(path)
                    var dirPaths = File.directoryEntries(path, File.Dirs | File.NoDotAndDotDot)
                    for(var i in dirPaths)
                        func(FileInfo.joinPaths(path, dirPaths[i]))
                }
                func(path)
            }
            return includePaths
        }

        cpp.defines:[
            'USE_FULL_LL_DRIVER',
            //            'USE_HAL_DRIVER',
            'STM32L476xx',
        ]

        cpp.assemblerFlags: [
            "-mcpu=cortex-m4",
            "-mthumb",
            "-mfloat-abi=hard",
            "-mfpu=fpv4-sp-d16",
        ]

        cpp.driverFlags: [
            "-mcpu=cortex-m4",
            "-mthumb",
            "-mfloat-abi=hard",
            "-mfpu=fpv4-sp-d16",
            "-Wall",
            "-Wno-unused-parameter",
            "-fdata-sections",
            "-ffunction-sections",
//            "-finline-functions",
            "-specs=nano.specs",
            "-specs=nosys.specs",
            "-fno-inline-small-functions",//?
        ]

        cpp.cxxFlags: [
            "-Wno-volatile"
        ]

        cpp.linkerMode : 'manual'
        cpp.linkerName : 'g++'
        cpp.linkerFlags : [
            "-mcpu=cortex-m4",
            "-mthumb",
            "-mfloat-abi=hard",
            "-mfpu=fpv4-sp-d16",
            "-specs=nano.specs",
            "-specs=nosys.specs",
            '-u _printf_float -u _scanf_float',
            '-flto',
            '-Wl,--start-group',
            '-lc',
            '-lm',
            '-lstdc++',
            '-lsupc++',
            '-Wl,--end-group',
//            '-lc',
//            '-lm',
//            '-lstdc++',
//            '-lsupc++',
//            '-lgcc',
        ]
//        cpp.linkerMapSuffix : string

//        cpp.linkerPath : string
//        cpp.linkerVariant : string
//        cpp.linkerWrapper : stringList


        // Создать .bin файл
        Rule {
            id: binFrmw
            inputs: ["application"]

            Artifact {
                fileTags: ["bin"]
                filePath: input.baseDir + "/" + input.baseName + ".bin"
            }

            prepare: {
                var objCopyPath = "arm-none-eabi-objcopy"
                var argsConv = ["-O", "binary", input.filePath, output.filePath]
                var cmd = new Command(objCopyPath, argsConv)
                cmd.description = "converting to BIN: " + FileInfo.fileName(input.filePath) + " -> " + input.baseName + ".bin"

                //            // Запись в nor память по qspi
                //            // Write to the nor memory by qspi
                //            var argsFlashingQspi =
                //                    [           "-f", "board/stm32f746g-disco.cfg",
                //                     "-c", "init",
                //                     "-c", "reset init",
                //                     "-c", "flash write_bank 1 " + output.filePath + " 0",
                //                     "-c", "reset",
                //                     "-c", "shutdown"
                //                    ]

                //            var cmdFlashQspi = new Command("openocd", argsFlashingQspi);
                //            cmdFlashQspi.description = "Wrtie to the NOR QSPI"

                //            // Запись во внутреннюю память
                //            // Write to the internal memory
                //            var argsFlashingInternalFlash =
                //                    [           "-f", "board/stm32f746g-disco.cfg",
                //                     "-c", "init",
                //                     "-c", "reset init",
                //                     "-c", "flash write_image erase " + input.filePath,
                //                     "-c", "reset",
                //                     "-c", "shutdown"
                //                    ]

                //            var cmdFlashInternalFlash = new Command("openocd", argsFlashingInternalFlash);
                //            cmdFlashInternalFlash.description = "Wrtie to the internal flash"

                return [
                            cmd,
                            //cmdFlashQspi,
                            //cmdFlashInternalFlash
                        ]
            }
        }

        // Создать .hex файл
        Rule {
            id: hexFrmw
            inputs: ["application"]

            Artifact {
                fileTags: ["hex"]
                filePath: input.baseDir + "/" + input.baseName + ".hex"
            }

            prepare: {
                var objCopyPath = "arm-none-eabi-objcopy"
                var argsConv = ["-O", "ihex", input.filePath, output.filePath]
                var cmd = new Command(objCopyPath, argsConv)
                cmd.description = "converting to HEX: " + FileInfo.fileName(input.filePath) + " -> " + input.baseName + ".hex"

                return [cmd]
            }
        }

        // Создать .hex файл
        Rule {
            id: objSizeId
            inputs: ["application"]
            alwaysRun: true

            Artifact {
                fileTags: ["size"]
            }

            prepare: {
                var objSize = "arm-none-eabi-size"
                var args = [input.filePath]
                var cmd = new Command(objSize, args)
                cmd.description = "Size: " + FileInfo.fileName(input.filePath)
                cmd.stdoutFilterFunction = function(stdout){
                    var sizeOfRam = (96 + 32) * 1024 // 96 + 32K
                    var sizeOfFlash = 1024 * 1024 // 1024K
                    var regex = /(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+.+/g
                    var result = regex.exec(stdout);F
                    stdout += "\nSize Of Ram  : " + ((result[2] / sizeOfRam) * 100).toFixed(2) + " %"
                    stdout += "\nSize Of Flash: " + (((result[4] - result[2]) / sizeOfFlash) * 100).toFixed(2) + " %"
                    return stdout
                };
                return [cmd]
            }
        }
        //        LIBS = -lc -lm -lnosys
        //        LIBDIR =
        //        LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections


        /*
    allowUnresolvedSymbols : bool
    alwaysUseLipo : bool
    architecture : string
    archiverName : string
    archiverPath : string
    assemblerFlags : stringList
    assemblerListingSuffix : string
    assemblerName : string
    assemblerPath : string
    automaticReferenceCounting : bool
    cFlags : stringList
    cLanguageVersion : stringList
    combineCSources : bool
    combineCxxSources : bool
    combineObjcSources : bool
    combineObjcxxSources : bool
    commonCompilerFlags : stringList
    compilerDefinesByLanguage : var
    compilerFrameworkPaths : pathList
    compilerIncludePaths : pathList
    compilerLibraryPaths : pathList
    compilerListingSuffix : string
    compilerName : string
    compilerPath : string
    compilerPathByLanguage : var
    compilerVersion : string
    compilerVersionMajor : int
    compilerVersionMinor : int
    compilerVersionPatch : int
    compilerWrapper : stringList
    cppFlags : stringList
    createSymlinks : bool
    cxxFlags : stringList
    cxxLanguageVersion : stringList
    cxxStandardLibrary : string
    debugInfoBundleSuffix : string
    debugInfoSuffix : string
    debugInformation : bool
    defines : stringList
    discardUnusedData : bool
    distributionFrameworkPaths : pathList
    distributionIncludePaths : pathList
    distributionLibraryPaths : pathList
    driverFlags : stringList
    driverLinkerFlags : stringList
    dsymutilFlags : stringList
    dsymutilPath : string
    dynamicLibraries : stringList
    dynamicLibraryImportSuffix : string
    dynamicLibraryPrefix : string
    dynamicLibrarySuffix : string
    enableCompilerDefinesByLanguage : stringList
    enableCxxLanguageMacro : string
    enableExceptions : bool
    enableReproducibleBuilds : bool
    enableRtti : bool
    enableSuspiciousLinkerFlagWarnings : bool
    endianness : string
    entryPoint : string
    exceptionHandlingModel : string
    executablePrefix : string
    executableSuffix : string
    exportedSymbolsCheckMode : string
    frameworkPaths : pathList
    frameworks : stringList
    generateAssemblerListingFiles : bool
    generateCompilerListingFiles : bool
    generateLinkerMapFile : bool
    generateManifestFile : bool
    includePaths : pathList
    libraryPaths : pathList
    linkerFlags : stringList
    linkerMapSuffix : string
    linkerMode : string
    linkerName : string
    linkerPath : string
    linkerVariant : string
    linkerWrapper : stringList
    lipoPath : string
    loadableModulePrefix : string
    loadableModuleSuffix : string
    minimumIosVersion : string
    minimumMacosVersion : string
    minimumTvosVersion : string
    minimumWatchosVersion : string
    minimumWindowsVersion : string
    nmName : string
    nmPath : string
    objcFlags : stringList
    objcopyName : string
    objcopyPath : string
    objcxxFlags : stringList
    objectSuffix : string
    optimization : string
    platformDefines : stringList
    positionIndependentCode : bool
    prefixHeaders : pathList
    removeDuplicateLibraries : bool
    requireAppContainer : bool
    requireAppExtensionSafeApi : bool
    rpathLinkFlag : string
    rpathOrigin : string
    rpaths : stringList
    runtimeLibrary : string
    separateDebugInformation : bool
    soVersion : string
    sonamePrefix : string
    staticLibraries : stringList
    staticLibraryPrefix : string
    staticLibrarySuffix : string
    stripName : string
    stripPath : string
    systemFrameworkPaths : pathList
    systemIncludePaths : pathList
    systemRunPaths : stringList
    toolchainInstallPath : string
    treatSystemHeadersAsDependencies : bool
    treatWarningsAsErrors : bool
    useCPrecompiledHeader : bool
    useCxxPrecompiledHeader : bool
    useLanguageVersionFallback : bool
    useObjcPrecompiledHeader : bool
    useObjcxxPrecompiledHeader : bool
    useRPathLink : bool
    useRPaths : bool
    variantSuffix : string
    visibility : string
    warningLevel : string
    weakFrameworks : stringList
    windowsApiAdditionalPartitions : stringList
    windowsApiCharacterSet : string
    windowsApiFamily : string
    windowsSdkVersion : string



14:22:12 **** Build of configuration Debug for project L476 ****
make -j6 all
arm-none-eabi-gcc "../grbl/alarms.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/alarms.d" -MT"grbl/alarms.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/alarms.o"
arm-none-eabi-gcc "../grbl/coolant_control.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/coolant_control.d" -MT"grbl/coolant_control.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/coolant_control.o"
arm-none-eabi-gcc "../grbl/corexy.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/corexy.d" -MT"grbl/corexy.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/corexy.o"
arm-none-eabi-gcc "../grbl/errors.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/errors.d" -MT"grbl/errors.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/errors.o"
arm-none-eabi-gcc "../grbl/gcode.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/gcode.d" -MT"grbl/gcode.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/gcode.o"
arm-none-eabi-gcc "../grbl/grbllib.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/grbllib.d" -MT"grbl/grbllib.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/grbllib.o"
arm-none-eabi-gcc "../grbl/limits.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/limits.d" -MT"grbl/limits.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/limits.o"
arm-none-eabi-gcc "../grbl/maslow.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/maslow.d" -MT"grbl/maslow.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/maslow.o"
arm-none-eabi-gcc "../grbl/motion_control.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/motion_control.d" -MT"grbl/motion_control.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/motion_control.o"
arm-none-eabi-gcc "../grbl/my_plugin.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/my_plugin.d" -MT"grbl/my_plugin.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/my_plugin.o"
arm-none-eabi-gcc "../grbl/nuts_bolts.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/nuts_bolts.d" -MT"grbl/nuts_bolts.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/nuts_bolts.o"
arm-none-eabi-gcc "../grbl/nvs_buffer.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/nvs_buffer.d" -MT"grbl/nvs_buffer.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/nvs_buffer.o"
arm-none-eabi-gcc "../grbl/override.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/override.d" -MT"grbl/override.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/override.o"
arm-none-eabi-gcc "../grbl/pid.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/pid.d" -MT"grbl/pid.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/pid.o"
arm-none-eabi-gcc "../grbl/planner.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/planner.d" -MT"grbl/planner.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/planner.o"
arm-none-eabi-gcc "../grbl/protocol.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/protocol.d" -MT"grbl/protocol.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/protocol.o"
arm-none-eabi-gcc "../grbl/report.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/report.d" -MT"grbl/report.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/report.o"
arm-none-eabi-gcc "../grbl/settings.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/settings.d" -MT"grbl/settings.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/settings.o"
arm-none-eabi-gcc "../grbl/sleep.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/sleep.d" -MT"grbl/sleep.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/sleep.o"
arm-none-eabi-gcc "../grbl/spindle_control.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/spindle_control.d" -MT"grbl/spindle_control.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/spindle_control.o"
arm-none-eabi-gcc "../grbl/state_machine.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/state_machine.d" -MT"grbl/state_machine.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/state_machine.o"
arm-none-eabi-gcc "../grbl/stepper.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/stepper.d" -MT"grbl/stepper.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/stepper.o"
arm-none-eabi-gcc "../grbl/stream.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/stream.d" -MT"grbl/stream.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/stream.o"
arm-none-eabi-gcc "../grbl/system.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/system.d" -MT"grbl/system.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/system.o"
arm-none-eabi-gcc "../grbl/tool_change.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/tool_change.d" -MT"grbl/tool_change.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/tool_change.o"
arm-none-eabi-gcc "../grbl/wall_plotter.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"grbl/wall_plotter.d" -MT"grbl/wall_plotter.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "grbl/wall_plotter.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_dma.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_exti.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_gpio.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_rcc.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usart.o"
arm-none-eabi-gcc "../Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.d" -MT"Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_utils.o"
arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"Core/Startup/startup_stm32l476rgtx.d" -MT"Core/Startup/startup_stm32l476rgtx.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Startup/startup_stm32l476rgtx.o" "../Core/Startup/startup_stm32l476rgtx.s"
arm-none-eabi-gcc "../Core/Src/driver.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/driver.d" -MT"Core/Src/driver.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Src/driver.o"
arm-none-eabi-gcc "../Core/Src/flash.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/flash.d" -MT"Core/Src/flash.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Src/flash.o"
arm-none-eabi-gcc "../Core/Src/main.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/main.d" -MT"Core/Src/main.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Src/main.o"
arm-none-eabi-gcc "../Core/Src/serial.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/serial.d" -MT"Core/Src/serial.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Src/serial.o"
arm-none-eabi-gcc "../Core/Src/st_morpho.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/st_morpho.d" -MT"Core/Src/st_morpho.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Src/st_morpho.o"
arm-none-eabi-gcc "../Core/Src/stm32l4xx_hal_msp.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/stm32l4xx_hal_msp.d" -MT"Core/Src/stm32l4xx_hal_msp.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Src/stm32l4xx_hal_msp.o"
arm-none-eabi-gcc "../Core/Src/stm32l4xx_it.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/stm32l4xx_it.d" -MT"Core/Src/stm32l4xx_it.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Src/stm32l4xx_it.o"
arm-none-eabi-gcc "../Core/Src/syscalls.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/syscalls.d" -MT"Core/Src/syscalls.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Src/syscalls.o"
arm-none-eabi-gcc "../Core/Src/sysmem.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/sysmem.d" -MT"Core/Src/sysmem.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Src/sysmem.o"
arm-none-eabi-gcc "../Core/Src/system_stm32l4xx.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/system_stm32l4xx.d" -MT"Core/Src/system_stm32l4xx.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Src/system_stm32l4xx.o"
arm-none-eabi-gcc "../Core/Src/usb_serial.c" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DBOARD_GENERIC_UNO -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/ARM/L476" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/usb_serial.d" -MT"Core/Src/usb_serial.o" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "Core/Src/usb_serial.o"
arm-none-eabi-g++ -o "L476.elf" @"objects.list"   -mcpu=cortex-m4 -T"D:\ARM\L476\STM32L476RGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="L476.map" -Wl,--gc-sections -static --verbose --stats --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -u _scanf_float -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
Using built-in specs.
Reading specs from c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/../../../../arm-none-eabi/lib/nosys.specs
rename spec link_gcc_c_sequence to nosys_link_gcc_c_sequence
Reading specs from c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/../../../../arm-none-eabi/lib/nano.specs
rename spec link to nano_link
rename spec link_gcc_c_sequence to nano_link_gcc_c_sequence
rename spec cpp_unique_options to nano_cpp_unique_options
COLLECT_GCC=arm-none-eabi-g++
COLLECT_LTO_WRAPPER=c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/lto-wrapper.exe
Target: arm-none-eabi
Configured with: /build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/src/gcc/configure --build=x86_64-linux-gnu --host=x86_64-w64-mingw32 --target=arm-none-eabi --prefix=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/install-mingw --libexecdir=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/install-mingw/lib --infodir=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/install-mingw/share/doc/gcc-arm-none-eabi/info --mandir=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/install-mingw/share/doc/gcc-arm-none-eabi/man --htmldir=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/install-mingw/share/doc/gcc-arm-none-eabi/html --pdfdir=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/install-mingw/share/doc/gcc-arm-none-eabi/pdf --enable-languages=c,c++ --enable-mingw-wildcard --disable-decimal-float --disable-libffi --disable-libgomp --disable-libmudflap --disable-libquadmath --disable-libssp --disable-libstdcxx-pch --disable-nls --disable-shared --disable-threads --disable-tls --with-gnu-as --with-gnu-ld --with-headers=yes --with-newlib --with-python-dir=share/gcc-arm-none-eabi --with-sysroot=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/install-mingw/arm-none-eabi --with-libiconv-prefix=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/build-mingw/host-libs/usr --with-gmp=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/build-mingw/host-libs/usr --with-mpfr=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/build-mingw/host-libs/usr --with-mpc=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/build-mingw/host-libs/usr --with-isl=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/build-mingw/host-libs/usr --with-libelf=/build/gnu-tools-for-stm32_9-2020-q2-update.20201001-1621/build-mingw/host-libs/usr --with-host-libstdcxx='-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm' --with-pkgversion='GNU Tools for STM32 9-2020-q2-update.20201001-1621' --with-multilib-list=rmprofile,aprofile
Thread model: single
gcc version 9.3.1 20200408 (release) (GNU Tools for STM32 9-2020-q2-update.20201001-1621)
COMPILER_PATH=c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/;c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/;c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/../../../../arm-none-eabi/bin/
LIBRARY_PATH=c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/thumb/v7e-m+fp/hard/;c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/../../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/;c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../arm-none-eabi/lib/thumb/v7e-m+fp/hard/;c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/;c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/;c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/../../../../arm-none-eabi/lib/;c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../arm-none-eabi/lib/
COLLECT_GCC_OPTIONS='-o' 'L476.elf' '-mcpu=cortex-m4' '-T' 'D:\ARM\L476\STM32L476RGTX_FLASH.ld' '-specs=nosys.specs' '-static' '-v' '-fstats' '-specs=nano.specs' '-mfpu=fpv4-sp-d16' '-mfloat-abi=hard' '-mthumb' '-u' '_printf_float' '-u' '_scanf_float' '-march=armv7e-m+fp'
 c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/collect2.exe -plugin c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/liblto_plugin-0.dll -plugin-opt=c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/lto-wrapper.exe -plugin-opt=-fresolution=C:\Users\X-Ray\AppData\Local\Temp\cclQkMA6.res -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lc_nano -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lc_nano -plugin-opt=-pass-through=-lnosys -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lc_nano -plugin-opt=-pass-through=-lnosys --sysroot=c:\st\stm32cubeide_1.7.0\stm32cubeide\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346\tools\bin\../arm-none-eabi -Bstatic -X -o L476.elf -u _printf_float -u _scanf_float c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/thumb/v7e-m+fp/hard/crti.o c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/thumb/v7e-m+fp/hard/crtbegin.o c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/../../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/crt0.o -Lc:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/thumb/v7e-m+fp/hard -Lc:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/../../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard -Lc:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../arm-none-eabi/lib/thumb/v7e-m+fp/hard -Lc:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1 -Lc:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc -Lc:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/../../../../arm-none-eabi/lib -Lc:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../arm-none-eabi/lib @C:\Users\X-Ray\AppData\Local\Temp\ccOvO652 --start-group -lgcc -lc_nano --end-group --start-group -lgcc -lc_nano -lnosys --end-group --start-group -lgcc -lc_nano -lnosys --end-group c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/thumb/v7e-m+fp/hard/crtend.o c:/st/stm32cubeide_1.7.0/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools/bin/../lib/gcc/arm-none-eabi/9.3.1/thumb/v7e-m+fp/hard/crtn.o -T D:\ARM\L476\STM32L476RGTX_FLASH.ld
COLLECT_GCC_OPTIONS='-o' 'L476.elf' '-mcpu=cortex-m4' '-T' 'D:\ARM\L476\STM32L476RGTX_FLASH.ld' '-specs=nosys.specs' '-static' '-v' '-fstats' '-specs=nano.specs' '-mfpu=fpv4-sp-d16' '-mfloat-abi=hard' '-mthumb' '-u' '_printf_float' '-u' '_scanf_float' '-march=armv7e-m+fp'
Finished building target: L476.elf

arm-none-eabi-size   L476.elf
arm-none-eabi-objdump -h -S  L476.elf  > "L476.list"
arm-none-eabi-objcopy  -O binary  L476.elf  "L476.bin"
   text	   data	    bss	    dec	    hex	filename
 181812	   1336	  11568	 194716	  2f89c	L476.elf
Finished building: default.size.stdout

Finished building: L476.bin

Finished building: L476.list


14:22:21 Build Finished. 0 errors, 0 warnings. (took 8s.65ms)



    */

        //        name: "myapp"
        //        Group {
        //            name: "Runtime resources"
        //            files: "*.qml"
        //            qbs.install: true
        //            qbs.installDir: condition: qbs.targetOS.contains("unix")
        //                            ? "share/myapp" : "resources"
        //        }
        //        Group {
        //            name: "The App itself"
        //            fileTagsFilter: "application"
        //            qbs.install: true
        //            qbs.installDir: "bin"
        //        }
        //        qbs.installPrefix: condition: qbs.targetOS.contains("unix")
        //                           ? "usr/local" : "MyApp"

    }
}
