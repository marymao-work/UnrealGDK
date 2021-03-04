// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using Microsoft.Win32;
using Newtonsoft.Json.Linq;
using LinuxScripts = Improbable.Unreal.Build.Common.LinuxScripts;

namespace Improbable
{
    public static class Build
    {
        public static void Main(string[] args)
        {
            var help = args.Count(arg => arg == "/?" || arg.ToLowerInvariant() == "--help") > 0;

            var exitCode = 0;
            if (args.Length < 4 && !help)
            {
                help = true;
                exitCode = 1;
                Console.Error.WriteLine("Path to uproject file is required.");
            }

            if (help)
            {
                Console.WriteLine("Usage: <GameName> <Platform> <Configuration> <game.uproject> [-nobuild] [-nocompile] <Additional UAT args>");

                Environment.Exit(exitCode);
            }

            var gameName = args[0];
            var platform = args[1];
            var configuration = args[2];
            var projectFile = Path.GetFullPath(args[3]);
            var noBuild = args.Count(arg => arg.ToLowerInvariant() == "-nobuild") > 0;
            var noCompile = args.Count(arg => arg.ToLowerInvariant() == "-nocompile") > 0;
            var noServer = args.Count(arg => arg.ToLowerInvariant() == "-noserver") > 0;
            var additionalUATArgs = string.Join(" ", args.Skip(4).Where(arg => (arg.ToLowerInvariant() != "-nobuild") && (arg.ToLowerInvariant() != "-nocompile")));

            var stagingDir = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(projectFile), "../spatial", "build", "unreal"));
            var outputDir = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(projectFile), "../spatial", "build", "assembly", "worker"));
            var baseGameName = Path.GetFileNameWithoutExtension(projectFile);

            // Locate the Unreal Engine.
            Console.WriteLine("Finding Unreal Engine build.");
            string uproject = File.ReadAllText(projectFile, Encoding.UTF8);

            dynamic projectJson = JObject.Parse(uproject);
            string engineAssociation = projectJson.EngineAssociation;

            Console.WriteLine("Engine Association: " + engineAssociation);

            string unrealEngine = "";

            // If the engine association is empty then climb the parent directories of the project looking for the Unreal Engine root directory.
            if (string.IsNullOrEmpty(engineAssociation))
            {
                DirectoryInfo currentDir = new DirectoryInfo(Directory.GetCurrentDirectory());
                while (currentDir.Parent != null)
                {
                    currentDir = currentDir.Parent;
                    // This is how Unreal asserts we have a valid root directory for the Unreal Engine. Must contain 'Engine/Binaries' and 'Engine/Build'. (FDesktopPlatformBase::IsValidRootDirectory)
                    if (Directory.Exists(Path.Combine(currentDir.FullName, "Engine", "Binaries")) && Directory.Exists(Path.Combine(currentDir.FullName, "Engine", "Build")))
                    {
                        unrealEngine = currentDir.FullName;
                        break;
                    }
                }
            }
            else if (Directory.Exists(Path.Combine(Path.GetDirectoryName(projectFile), engineAssociation))) // If the engine association is a path then use that.
            {
                unrealEngine = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(projectFile), engineAssociation));
            }
            else
            {
                // Finally check the registry for the path using the engine association as the key.
                string unrealEngineBuildKey = "HKEY_CURRENT_USER\\Software\\Epic Games\\Unreal Engine\\Builds";
                var unrealEngineValue = Registry.GetValue(unrealEngineBuildKey, engineAssociation, "");

                if (unrealEngineValue != null)
                {
                    unrealEngine = unrealEngineValue.ToString();
                }
                else
                {
                    Console.Error.WriteLine("Engine Association not found in the registry! Please run Setup.bat from within the UnrealEngine.");
                }
            }

            if (string.IsNullOrEmpty(unrealEngine))
            {
                Console.Error.WriteLine("Could not find the Unreal Engine. Please associate your '.uproject' with an engine version or ensure this game project is nested within an engine build.");
                Environment.Exit(1);
            }
            else
            {
                Console.WriteLine("Engine is at: " + unrealEngine);
            }

            string runUATBat = Path.Combine(unrealEngine, @"Engine\Build\BatchFiles\RunUAT.bat");
            string buildBat = Path.Combine(unrealEngine, @"Engine\Build\BatchFiles\Build.bat");

            if (gameName == baseGameName + "Editor")
            {
                if (noCompile)
                {
                    Common.WriteHeading(" > Using precompiled Editor for use as a managed worker.");
                }
                else
                {
                    Common.WriteHeading(" > Building Editor for use as a managed worker.");
                    
                    Common.RunRedirected(buildBat, new[]
                    {
                        gameName,
                        platform,
                        configuration,
                        Quote(projectFile)
                    });
                }

                var windowsEditorPath = Path.Combine(stagingDir, "WindowsEditor");
                if (!Directory.Exists(windowsEditorPath))
                {
                    Directory.CreateDirectory(windowsEditorPath);
                }

                var PathToUnrealEditor = Path.Combine(unrealEngine, "Engine\\Binaries\\Win64\\UE4Editor.exe");

                var StartEditorScript =
$@"setlocal ENABLEDELAYEDEXPANSION
{PathToUnrealEditor} {projectFile} %*
exit /b !ERRORLEVEL!";

                // Write a simple batch file to launch the Editor as a managed worker.
                File.WriteAllText(Path.Combine(windowsEditorPath, "StartEditor.bat"),
                    StartEditorScript, new UTF8Encoding(false));

                // The runtime currently requires all workers to be in zip files. Zip the batch file.
                Common.RunRedirected(runUATBat, new[]
                {
                    "ZipUtils",
                    "-add=" + Quote(windowsEditorPath),
                    "-archive=" + Quote(Path.Combine(outputDir, "UnrealEditor@Windows.zip")),
                });
            }
            else if (gameName == baseGameName)
            {
                Common.WriteHeading(" > Building client.");
                Common.RunRedirected(runUATBat, new[]
                {
                    "BuildCookRun",
                    noBuild ? "-nobuild" : "-build",
                    noCompile ? "-nocompile" : "-compile",
                    "-project=" + Quote(projectFile),
                    "-noP4",
                    "-clientconfig=" + configuration,
                    "-serverconfig=" + configuration,
                    "-utf8output",
                    "-cook",
                    "-stage",
                    "-package",
                    "-unversioned",
                    "-compressed",
                    "-stagingdirectory=" + Quote(stagingDir),
                    "-stdout",
                    "-FORCELOGFLUSH",
                    "-CrashForUAT",
                    "-unattended",
                    "-fileopenlog",
                    "-SkipCookingEditorContent",
                    "-platform=" + platform,
                    "-targetplatform=" + platform,
                    additionalUATArgs
                });

                var windowsTargetPath = Path.Combine(stagingDir, noServer ? "WindowsClient" : "WindowsNoEditor");

                ForceSpatialNetworkingUnlessPakSpecified(additionalUATArgs, windowsTargetPath, baseGameName);

                RenameExeForLauncher(windowsTargetPath, baseGameName);

                Common.RunRedirected(runUATBat, new[]
                {
                    "ZipUtils",
                    "-add=" + Quote(windowsTargetPath),
                    "-archive=" + Quote(Path.Combine(outputDir, "UnrealClient@Windows.zip")),
                });
            }
            else if (gameName == baseGameName + "SimulatedPlayer") // This is for internal use only. We do not support Linux clients.
            {
                Common.WriteHeading(" > Building simulated player.");
                Common.RunRedirected(runUATBat, new[]
                {
                    "BuildCookRun",
                    noBuild ? "-nobuild" : "-build",
                    noCompile ? "-nocompile" : "-compile",
                    "-project=" + Quote(projectFile),
                    "-noP4",
                    "-clientconfig=" + configuration,
                    "-serverconfig=" + configuration,
                    "-utf8output",
                    "-cook",
                    "-stage",
                    "-package",
                    "-unversioned",
                    "-compressed",
                    "-stagingdirectory=" + Quote(stagingDir),
                    "-stdout",
                    "-FORCELOGFLUSH",
                    "-CrashForUAT",
                    "-unattended",
                    "-fileopenlog",
                    "-SkipCookingEditorContent",
                    "-platform=" + platform,
                    "-targetplatform=" + platform,
                    "-nullrhi",
                    additionalUATArgs
                });

                var linuxSimulatedPlayerPath = Path.Combine(stagingDir, noServer ? "LinuxClient" : "LinuxNoEditor");

                ForceSpatialNetworkingUnlessPakSpecified(additionalUATArgs, linuxSimulatedPlayerPath, baseGameName);

                LinuxScripts.WriteWithLinuxLineEndings(LinuxScripts.GetSimulatedPlayerWorkerShellScript(baseGameName), Path.Combine(linuxSimulatedPlayerPath, "StartSimulatedClient.sh"));
                LinuxScripts.WriteWithLinuxLineEndings(LinuxScripts.GetStopSimulatedPlayerWorkerShellScript(baseGameName), Path.Combine(linuxSimulatedPlayerPath, "StopSimulatedClient.sh"));
                LinuxScripts.WriteWithLinuxLineEndings(LinuxScripts.GetSimulatedPlayerCoordinatorShellScript(baseGameName), Path.Combine(linuxSimulatedPlayerPath, "StartCoordinator.sh"));

                // Coordinator files are located in      ./UnrealGDK/SpatialGDK/Binaries/ThirdParty/Improbable/Programs/WorkerCoordinator/.
                // Executable of this build script is in ./UnrealGDK/SpatialGDK/Binaries/ThirdParty/Improbable/Programs/Build.exe
                // Assembly.GetEntryAssembly().Location gives the location of the Build.exe executable.
                var workerCoordinatorPath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location), "./WorkerCoordinator"));
                if (Directory.Exists(workerCoordinatorPath))
                {
                    Common.RunRedirected("xcopy", new[]
                    {
                        "/I",
                        "/Y",
                        Quote(workerCoordinatorPath),
                        Quote(linuxSimulatedPlayerPath)
                    });
                }
                else
                {
                    Common.WriteWarning($"Worker coordinator binary not found at {workerCoordinatorPath}. Please run Setup.bat to build the worker coordinator.");
                }

                var archiveFileName = "UnrealSimulatedPlayer@Linux.zip";
                Common.RunRedirected(runUATBat, new[]
                {
                    "ZipUtils",
                    "-add=" + Quote(linuxSimulatedPlayerPath),
                    "-archive=" + Quote(Path.Combine(outputDir, archiveFileName)),
                });
            }
            else if (gameName == baseGameName + "Server")
            {
                Common.WriteHeading(" > Building worker.");
                Common.RunRedirected(runUATBat, new[]
                {
                    "BuildCookRun",
                    noBuild ? "-nobuild" : "-build",
                    noCompile ? "-nocompile" : "-compile",
                    "-project=" + Quote(projectFile),
                    "-noP4",
                    "-clientconfig=" + configuration,
                    "-serverconfig=" + configuration,
                    "-utf8output",
                    "-cook",
                    "-stage",
                    "-package",
                    "-unversioned",
                    "-compressed",
                    "-stagingdirectory=" + Quote(stagingDir),
                    "-stdout",
                    "-FORCELOGFLUSH",
                    "-CrashForUAT",
                    "-unattended",
                    "-fileopenlog",
                    "-SkipCookingEditorContent",
                    "-server",
                    "-serverplatform=" + platform,
                    "-noclient",
                    additionalUATArgs
                });

                bool isLinux = platform == "Linux";
                var assemblyPlatform = isLinux ? "Linux" : "Windows";
                var serverPath = Path.Combine(stagingDir, assemblyPlatform + "Server");

                ForceSpatialNetworkingUnlessPakSpecified(additionalUATArgs, serverPath, baseGameName);

                if (isLinux)
                {
                    // Write out the wrapper shell script to work around issues between UnrealEngine and our cloud Linux environments.
                    // Also ensure script uses Linux line endings
                    LinuxScripts.WriteWithLinuxLineEndings(LinuxScripts.GetUnrealWorkerShellScript(baseGameName), Path.Combine(serverPath, "StartWorker.sh"));
                }

                Common.RunRedirected(runUATBat, new[]
                {
                    "ZipUtils",
                    "-add=" + Quote(serverPath),
                    "-archive=" + Quote(Path.Combine(outputDir, $"UnrealWorker@{assemblyPlatform}.zip"))
                });
            }
            else if (gameName == baseGameName + "Client")
            {
                Common.WriteHeading(" > Building client.");
                Common.RunRedirected(runUATBat, new[]
                {
                    "BuildCookRun",
                    noBuild ? "-nobuild" : "-build",
                    noCompile ? "-nocompile" : "-compile",
                    "-project=" + Quote(projectFile),
                    "-noP4",
                    "-clientconfig=" + configuration,
                    "-serverconfig=" + configuration,
                    "-utf8output",
                    "-cook",
                    "-stage",
                    "-package",
                    "-unversioned",
                    "-compressed",
                    "-stagingdirectory=" + Quote(stagingDir),
                    "-stdout",
                    "-FORCELOGFLUSH",
                    "-CrashForUAT",
                    "-unattended",
                    "-fileopenlog",
                    "-SkipCookingEditorContent",
                    "-client",
                    "-noserver",
                    "-platform=" + platform,
                    "-targetplatform=" + platform,
                    additionalUATArgs
                });

                var windowsClientPath = Path.Combine(stagingDir, "WindowsClient");

                ForceSpatialNetworkingUnlessPakSpecified(additionalUATArgs, windowsClientPath, baseGameName);

                RenameExeForLauncher(windowsClientPath, baseGameName + "Client");

                Common.RunRedirected(runUATBat, new[]
                {
                    "ZipUtils",
                    "-add=" + Quote(windowsClientPath),
                    "-archive=" + Quote(Path.Combine(outputDir, "UnrealClient@Windows.zip")),
                });
            }
            else
            {
                // Pass-through to Unreal's Build.bat.
                Common.WriteHeading($" > Building ${gameName}.");
                Common.RunRedirected(buildBat, new[]
                {
                    gameName,
                    platform,
                    configuration,
                    Quote(projectFile)
                });
            }
        }

        private static string Quote(string toQuote)
        {
            return $"\"{toQuote}\"";
        }

        private static void RenameExeForLauncher(string workerPath, string gameName)
        {
            // Add a _ to the start of the exe name, to ensure it is the exe selected by the launcher.
            // TO-DO: Remove this once LAUNCH-341 has been completed, and the _ is no longer necessary.
            var oldExe = Path.Combine(workerPath, $"{gameName}.exe");
            var renamedExe = Path.Combine(workerPath, $"_{gameName}.exe");
            if (File.Exists(renamedExe))
            {
                File.Delete(renamedExe);
            }
            if (File.Exists(oldExe))
            {
                File.Move(oldExe, renamedExe);
            }
            else
            {
                Console.WriteLine("Could not find the executable to rename.");
            }
        }

        private static void ForceSpatialNetworkingUnlessPakSpecified(string additionalUATArgs, string workerPath, string gameName)
        {
            if (additionalUATArgs.Contains("-pak"))
            {
                Console.WriteLine("Cannot force bSpatialNetworking with -pak argument.");
            }
            else
            {
                ForceSpatialNetworkingInConfig(workerPath, gameName);
            }
        }

        private static void ForceSpatialNetworkingInConfig(string workerPath, string gameName)
        {
            var defaultGameIniPath = Path.Combine(workerPath, gameName, "Config", "DefaultGame.ini");

            Console.WriteLine($"Forcing bSpatialNetworking to True in {defaultGameIniPath}");
            string defaultGameIniOverrideText =
@"
; Overridden by Spatial Build Tool:
[/Script/EngineSettings.GeneralProjectSettings]
bSpatialNetworking=True
";
            File.AppendAllText(defaultGameIniPath, defaultGameIniOverrideText);
        }
    }
}
