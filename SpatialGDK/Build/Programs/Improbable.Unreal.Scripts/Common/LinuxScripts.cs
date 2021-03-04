// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

using System.IO;
using System.Text;

namespace Improbable.Unreal.Build.Common
{
    public static class LinuxScripts
    {
        private const string UnrealWorkerShellScript =
@"#!/bin/bash
NEW_USER=unrealworker
WORKER_ID=$1
LOG_FILE=$2
shift 2

# 2>/dev/null silences errors by redirecting stderr to the null device. This is done to prevent errors when a machine attempts to add the same user more than once.
mkdir -p /improbable/logs/UnrealWorker/
useradd $NEW_USER -m -d /improbable/logs/UnrealWorker 2>/dev/null
chown -R $NEW_USER:$NEW_USER $(pwd) 2>/dev/null
chmod -R o+rw /improbable/logs 2>/dev/null

# Create log file in case it doesn't exist and redirect stdout and stderr to the file.
touch ""${{LOG_FILE}}""
exec 1>>""${{LOG_FILE}}""
exec 2>&1

SCRIPT=""$(pwd)/{0}Server.sh""

if [ ! -f $SCRIPT ]; then
    echo ""Expected to run ${{SCRIPT}} but file not found!""
    exit 1
fi

chmod +x $SCRIPT
echo ""Running ${{SCRIPT}} to start worker...""
gosu $NEW_USER ""${{SCRIPT}}"" ""$@""";

        // This is for internal use only. We do not support Linux clients.
        public const string SimulatedPlayerWorkerShellScript =
@"#!/bin/bash
NEW_USER=unrealworker
WORKER_ID=$1
shift 1

SCRIPT=""$(pwd)/{0}.sh""

echo ""Trying to launch worker {0} with id ${{WORKER_ID}}"" >> ""/improbable/logs/${{WORKER_ID}}.log""
gosu $NEW_USER ""${{SCRIPT}}"" ""$@"" >> ""/improbable/logs/${{WORKER_ID}}.log"" 2>&1";

        public const string SimulatedPlayerCoordinatorShellScript =
@"#!/bin/sh

# Some clients are quite large so in order to avoid running out of disk space on the node we attempt to delete the zip
WORKER_ZIP_DIR=""/tmp/runner_source/""
if [ -d ""$WORKER_ZIP_DIR"" ]; then
  rm -rf ""$WORKER_ZIP_DIR""
fi

sleep 5

chmod +x WorkerCoordinator.exe
chmod +x StartSimulatedClient.sh
chmod +x StopSimulatedClient.sh
chmod +x {0}.sh

NEW_USER=unrealworker
useradd $NEW_USER -m -d /improbable/logs/ 2> /improbable/logs/CoordinatorErrors.log
chown -R $NEW_USER:$NEW_USER $(pwd) 2> /improbable/logs/CoordinatorErrors.log
chmod -R o+rw /improbable/logs 2> /improbable/logs/CoordinatorErrors.log

mono WorkerCoordinator.exe $@ 2> /improbable/logs/CoordinatorErrors.log";

        public const string StopSimulatedPlayerWorkerShellScript =
@"#!/bin/bash
PLAYER_ID=$1
ps -ef | grep $PLAYER_ID | grep -v grep | grep -v .sh | awk '{print $2'} | xargs kill -9";

        // Returns a version of UnrealWorkerShellScript with baseGameName templated into the right places.
        // baseGameName should be the base name of your Unreal game.
        public static string GetUnrealWorkerShellScript(string baseGameName)
        {
            return string.Format(UnrealWorkerShellScript, baseGameName);
        }

        // Returns a version of SimulatedPlayerWorkerShellScript with baseGameName templated into the right places.
        // baseGameName should be the base name of your Unreal game.
        public static string GetSimulatedPlayerWorkerShellScript(string baseGameName)
        {
            return string.Format(SimulatedPlayerWorkerShellScript, baseGameName);
        }

        public static string GetStopSimulatedPlayerWorkerShellScript(string baseGameName)
        {
            return StopSimulatedPlayerWorkerShellScript;
        }

        // Returns a version of SimulatedPlayerCoordinatorShellScript with baseGameName templated into the right places.
        // baseGameName should be the base name of your Unreal game.
        public static string GetSimulatedPlayerCoordinatorShellScript(string baseGameName)
        {
            return string.Format(SimulatedPlayerCoordinatorShellScript, baseGameName);
        }

        // Writes out fileContents to fileName, ensuring that the resulting file has Linux line endings.
        public static void WriteWithLinuxLineEndings(string fileContents, string fileName)
        {
            File.WriteAllText(fileName, fileContents.Replace("\r\n", "\n"), new UTF8Encoding(false));
        }
    }
}
