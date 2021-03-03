// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "Improbable/SpatialEngineConstants.h"
#include "Schema/UnrealObjectRef.h"
#include "SpatialCommonTypes.h"
#include "UObject/Script.h"

#include <WorkerSDK/improbable/c_schema.h>
#include <WorkerSDK/improbable/c_worker.h>

#include "SpatialConstants.generated.h"

#define LOCTEXT_NAMESPACE "SpatialConstants"

UENUM()
enum class ERPCType : uint8
{
	Invalid,
	ClientReliable,
	ClientUnreliable,
	ServerReliable,
	ServerUnreliable,
	ServerAlwaysWrite,
	NetMulticast,
	CrossServer,

	// Helpers to iterate RPC types with ring buffers
	RingBufferTypeBegin = ClientReliable,
	RingBufferTypeEnd = NetMulticast
};

enum ESchemaComponentType : int32
{
	SCHEMA_Invalid = -1,

	// Properties
	SCHEMA_Data, // Represents properties being replicated to all workers
	SCHEMA_OwnerOnly,
	SCHEMA_Handover,

	SCHEMA_Count,

	// Iteration helpers
	SCHEMA_Begin = SCHEMA_Data,
};

namespace SpatialConstants
{
inline FString RPCTypeToString(ERPCType RPCType)
{
	switch (RPCType)
	{
	case ERPCType::ClientReliable:
		return TEXT("Client, Reliable");
	case ERPCType::ClientUnreliable:
		return TEXT("Client, Unreliable");
	case ERPCType::ServerReliable:
		return TEXT("Server, Reliable");
	case ERPCType::ServerUnreliable:
		return TEXT("Server, Unreliable");
	case ERPCType::ServerAlwaysWrite:
		return TEXT("Server, AlwaysWrite");
	case ERPCType::NetMulticast:
		return TEXT("Multicast");
	case ERPCType::CrossServer:
		return TEXT("CrossServer");
	}

	checkNoEntry();
	return FString();
}

enum EntityIds
{
	INITIAL_SPAWNER_ENTITY_ID = 1,
	INITIAL_GLOBAL_STATE_MANAGER_ENTITY_ID = 2,
	INITIAL_VIRTUAL_WORKER_TRANSLATOR_ENTITY_ID = 3,
	INITIAL_SNAPSHOT_PARTITION_ENTITY_ID = 4,
	FIRST_AVAILABLE_ENTITY_ID = 5,
};

const Worker_PartitionId INVALID_PARTITION_ID = INVALID_ENTITY_ID;

const Worker_ComponentId INVALID_COMPONENT_ID = 0;

const Worker_ComponentId INVALID_QUERY_TAG = 0;

const Worker_ComponentId METADATA_COMPONENT_ID = 53;
const Worker_ComponentId POSITION_COMPONENT_ID = 54;
const Worker_ComponentId PERSISTENCE_COMPONENT_ID = 55;
const Worker_ComponentId INTEREST_COMPONENT_ID = 58;

// This is a marker component used by the Runtime to define which entities are system entities.
const Worker_ComponentId SYSTEM_COMPONENT_ID = 59;

// This is a component on per-worker system entities.
const Worker_ComponentId WORKER_COMPONENT_ID = 60;
const Worker_ComponentId PLAYERIDENTITY_COMPONENT_ID = 61;
const Worker_ComponentId AUTHORITY_DELEGATION_COMPONENT_ID = 65;
const Worker_ComponentId PARTITION_COMPONENT_ID = 66;

const Worker_ComponentId MAX_RESERVED_SPATIAL_SYSTEM_COMPONENT_ID = 100;

const Worker_ComponentId SPAWN_DATA_COMPONENT_ID = 9999;
const Worker_ComponentId PLAYER_SPAWNER_COMPONENT_ID = 9998;
const Worker_ComponentId UNREAL_METADATA_COMPONENT_ID = 9996;
const Worker_ComponentId GDK_DEBUG_COMPONENT_ID = 9995;
const Worker_ComponentId DEPLOYMENT_MAP_COMPONENT_ID = 9994;
const Worker_ComponentId STARTUP_ACTOR_MANAGER_COMPONENT_ID = 9993;
const Worker_ComponentId GSM_SHUTDOWN_COMPONENT_ID = 9992;
const Worker_ComponentId PLAYER_CONTROLLER_COMPONENT_ID = 9991;

const Worker_ComponentId SERVER_AUTH_COMPONENT_SET_ID = 9900;
const Worker_ComponentId CLIENT_AUTH_COMPONENT_SET_ID = 9901;
const Worker_ComponentId DATA_COMPONENT_SET_ID = 9902;
const Worker_ComponentId OWNER_ONLY_COMPONENT_SET_ID = 9903;
const Worker_ComponentId HANDOVER_COMPONENT_SET_ID = 9904;
const Worker_ComponentId GDK_KNOWN_ENTITY_AUTH_COMPONENT_SET_ID = 9905;
const Worker_ComponentId ROUTING_WORKER_AUTH_COMPONENT_SET_ID = 9906;

const FString SERVER_AUTH_COMPONENT_SET_NAME = TEXT("ServerAuthoritativeComponentSet");
const FString CLIENT_AUTH_COMPONENT_SET_NAME = TEXT("ClientAuthoritativeComponentSet");
const FString DATA_COMPONENT_SET_NAME = TEXT("DataComponentSet");
const FString OWNER_ONLY_COMPONENT_SET_NAME = TEXT("OwnerOnlyComponentSet");
const FString HANDOVER_COMPONENT_SET_NAME = TEXT("HandoverComponentSet");
const FString ROUTING_WORKER_COMPONENT_SET_NAME = TEXT("RoutingWorkerComponentSet");

const Worker_ComponentId NOT_STREAMED_COMPONENT_ID = 9986;
const Worker_ComponentId DEBUG_METRICS_COMPONENT_ID = 9984;
const Worker_ComponentId ALWAYS_RELEVANT_COMPONENT_ID = 9983;
const Worker_ComponentId TOMBSTONE_COMPONENT_ID = 9982;
const Worker_ComponentId DORMANT_COMPONENT_ID = 9981;
const Worker_ComponentId AUTHORITY_INTENT_COMPONENT_ID = 9980;
const Worker_ComponentId VIRTUAL_WORKER_TRANSLATION_COMPONENT_ID = 9979;
const Worker_ComponentId VISIBLE_COMPONENT_ID = 9970;
const Worker_ComponentId SERVER_ONLY_ALWAYS_RELEVANT_COMPONENT_ID = 9968;

const Worker_ComponentId CROSSSERVER_SENDER_ENDPOINT_COMPONENT_ID = 9960;
const Worker_ComponentId CROSSSERVER_SENDER_ACK_ENDPOINT_COMPONENT_ID = 9961;
const Worker_ComponentId CROSSSERVER_RECEIVER_ENDPOINT_COMPONENT_ID = 9962;
const Worker_ComponentId CROSSSERVER_RECEIVER_ACK_ENDPOINT_COMPONENT_ID = 9963;

const Worker_ComponentId CLIENT_ENDPOINT_COMPONENT_ID = 9978;
const Worker_ComponentId SERVER_ENDPOINT_COMPONENT_ID = 9977;
const Worker_ComponentId MULTICAST_RPCS_COMPONENT_ID = 9976;
const Worker_ComponentId SPATIAL_DEBUGGING_COMPONENT_ID = 9975;
const Worker_ComponentId SERVER_WORKER_COMPONENT_ID = 9974;
const Worker_ComponentId SERVER_TO_SERVER_COMMAND_ENDPOINT_COMPONENT_ID = 9973;
const Worker_ComponentId NET_OWNING_CLIENT_WORKER_COMPONENT_ID = 9971;
const Worker_ComponentId MIGRATION_DIAGNOSTIC_COMPONENT_ID = 9969;
const Worker_ComponentId PARTITION_SHADOW_COMPONENT_ID = 9967;

const Worker_ComponentId STARTING_GENERATED_COMPONENT_ID = 10000;

// System query tags for entity completeness
const Worker_ComponentId FIRST_EC_COMPONENT_ID = 2001;
const Worker_ComponentId ACTOR_AUTH_TAG_COMPONENT_ID = 2001;
const Worker_ComponentId ACTOR_TAG_COMPONENT_ID = 2002;
const Worker_ComponentId LB_TAG_COMPONENT_ID = 2005;

const Worker_ComponentId GDK_KNOWN_ENTITY_TAG_COMPONENT_ID = 2007;
const Worker_ComponentId TOMBSTONE_TAG_COMPONENT_ID = 2008;
const Worker_ComponentId ROUTINGWORKER_TAG_COMPONENT_ID = 2009;
const Worker_ComponentId LAST_EC_COMPONENT_ID = 2010;

const Schema_FieldId DEPLOYMENT_MAP_MAP_URL_ID = 1;
const Schema_FieldId DEPLOYMENT_MAP_ACCEPTING_PLAYERS_ID = 2;
const Schema_FieldId DEPLOYMENT_MAP_SESSION_ID = 3;
const Schema_FieldId DEPLOYMENT_MAP_SCHEMA_HASH = 4;

const Schema_FieldId STARTUP_ACTOR_MANAGER_CAN_BEGIN_PLAY_ID = 1;

const Schema_FieldId ACTOR_COMPONENT_REPLICATES_ID = 1;
const Schema_FieldId ACTOR_TEAROFF_ID = 3;

const Schema_FieldId SHUTDOWN_MULTI_PROCESS_REQUEST_ID = 1;
const Schema_FieldId SHUTDOWN_ADDITIONAL_SERVERS_EVENT_ID = 1;

// DebugMetrics command IDs
const Schema_FieldId DEBUG_METRICS_START_RPC_METRICS_ID = 1;
const Schema_FieldId DEBUG_METRICS_STOP_RPC_METRICS_ID = 2;
const Schema_FieldId DEBUG_METRICS_MODIFY_SETTINGS_ID = 3;

// ModifySettingPayload Field IDs
const Schema_FieldId MODIFY_SETTING_PAYLOAD_NAME_ID = 1;
const Schema_FieldId MODIFY_SETTING_PAYLOAD_VALUE_ID = 2;

// UnrealObjectRef Field IDs
const Schema_FieldId UNREAL_OBJECT_REF_ENTITY_ID = 1;
const Schema_FieldId UNREAL_OBJECT_REF_OFFSET_ID = 2;
const Schema_FieldId UNREAL_OBJECT_REF_PATH_ID = 3;
const Schema_FieldId UNREAL_OBJECT_REF_NO_LOAD_ON_CLIENT_ID = 4;
const Schema_FieldId UNREAL_OBJECT_REF_OUTER_ID = 5;
const Schema_FieldId UNREAL_OBJECT_REF_USE_CLASS_PATH_TO_LOAD_ID = 6;

// UnrealRPCPayload Field IDs
const Schema_FieldId UNREAL_RPC_PAYLOAD_OFFSET_ID = 1;
const Schema_FieldId UNREAL_RPC_PAYLOAD_RPC_INDEX_ID = 2;
const Schema_FieldId UNREAL_RPC_PAYLOAD_RPC_PAYLOAD_ID = 3;
const Schema_FieldId UNREAL_RPC_PAYLOAD_TRACE_ID = 4;
const Schema_FieldId UNREAL_RPC_PAYLOAD_RPC_ID = 5;

const Schema_FieldId UNREAL_RPC_TRACE_ID = 1;
const Schema_FieldId UNREAL_RPC_SPAN_ID = 2;

// Unreal(Client|Server|Multicast)RPCEndpoint Field IDs
const Schema_FieldId UNREAL_RPC_ENDPOINT_READY_ID = 1;
const Schema_FieldId UNREAL_RPC_ENDPOINT_EVENT_ID = 1;
const Schema_FieldId UNREAL_RPC_ENDPOINT_COMMAND_ID = 1;

const Schema_FieldId PLAYER_SPAWNER_SPAWN_PLAYER_COMMAND_ID = 1;

// AuthorityIntent codes and Field IDs.
const Schema_FieldId AUTHORITY_INTENT_VIRTUAL_WORKER_ID = 1;

// VirtualWorkerTranslation Field IDs.
const Schema_FieldId VIRTUAL_WORKER_TRANSLATION_MAPPING_ID = 1;
const Schema_FieldId MAPPING_VIRTUAL_WORKER_ID = 1;
const Schema_FieldId MAPPING_PHYSICAL_WORKER_NAME_ID = 2;
const Schema_FieldId MAPPING_SERVER_WORKER_ENTITY_ID = 3;
const Schema_FieldId MAPPING_PARTITION_ID = 4;
const PhysicalWorkerName TRANSLATOR_UNSET_PHYSICAL_NAME = FString("UnsetWorkerName");

// WorkerEntity Field IDs.
const Schema_FieldId WORKER_ID_ID = 1;
const Schema_FieldId WORKER_TYPE_ID = 2;

// WorkerEntity command IDs
const Schema_FieldId WORKER_DISCONNECT_COMMAND_ID = 1;
const Schema_FieldId WORKER_CLAIM_PARTITION_COMMAND_ID = 2;

// SpatialDebugger Field IDs.
const Schema_FieldId SPATIAL_DEBUGGING_AUTHORITATIVE_VIRTUAL_WORKER_ID = 1;
const Schema_FieldId SPATIAL_DEBUGGING_AUTHORITATIVE_COLOR = 2;
const Schema_FieldId SPATIAL_DEBUGGING_INTENT_VIRTUAL_WORKER_ID = 3;
const Schema_FieldId SPATIAL_DEBUGGING_INTENT_COLOR = 4;
const Schema_FieldId SPATIAL_DEBUGGING_IS_LOCKED = 5;

// ServerWorker Field IDs.
const Schema_FieldId SERVER_WORKER_NAME_ID = 1;
const Schema_FieldId SERVER_WORKER_READY_TO_BEGIN_PLAY_ID = 2;
const Schema_FieldId SERVER_WORKER_SYSTEM_ENTITY_ID = 3;
const Schema_FieldId SERVER_WORKER_FORWARD_SPAWN_REQUEST_COMMAND_ID = 1;

// SpawnPlayerRequest type IDs.
const Schema_FieldId SPAWN_PLAYER_URL_ID = 1;
const Schema_FieldId SPAWN_PLAYER_UNIQUE_ID = 2;
const Schema_FieldId SPAWN_PLAYER_PLATFORM_NAME_ID = 3;
const Schema_FieldId SPAWN_PLAYER_IS_SIMULATED_ID = 4;
const Schema_FieldId SPAWN_PLAYER_CLIENT_SYSTEM_ENTITY_ID = 5;

// ForwardSpawnPlayerRequest type IDs.
const Schema_FieldId FORWARD_SPAWN_PLAYER_DATA_ID = 1;
const Schema_FieldId FORWARD_SPAWN_PLAYER_START_ACTOR_ID = 2;
const Schema_FieldId FORWARD_SPAWN_PLAYER_CLIENT_SYSTEM_ENTITY_ID = 3;
const Schema_FieldId FORWARD_SPAWN_PLAYER_RESPONSE_SUCCESS_ID = 1;

// NetOwningClientWorker Field IDs.
const Schema_FieldId NET_OWNING_CLIENT_PARTITION_ENTITY_FIELD_ID = 1;

// UnrealMetadata Field IDs.
const Schema_FieldId UNREAL_METADATA_STABLY_NAMED_REF_ID = 1;
const Schema_FieldId UNREAL_METADATA_CLASS_PATH_ID = 2;
const Schema_FieldId UNREAL_METADATA_NET_STARTUP_ID = 3;

// Migration diagnostic Field IDs
const Schema_FieldId MIGRATION_DIAGNOSTIC_COMMAND_ID = 1;

// MigrationDiagnosticRequest type IDs.
const Schema_FieldId MIGRATION_DIAGNOSTIC_AUTHORITY_WORKER_ID = 1;
const Schema_FieldId MIGRATION_DIAGNOSTIC_ENTITY_ID = 2;
const Schema_FieldId MIGRATION_DIAGNOSTIC_REPLICATES_ID = 3;
const Schema_FieldId MIGRATION_DIAGNOSTIC_HAS_AUTHORITY_ID = 4;
const Schema_FieldId MIGRATION_DIAGNOSTIC_LOCKED_ID = 5;
const Schema_FieldId MIGRATION_DIAGNOSTIC_EVALUATION_ID = 6;
const Schema_FieldId MIGRATION_DIAGNOSTIC_DESTINATION_WORKER_ID = 7;
const Schema_FieldId MIGRATION_DIAGNOSTIC_OWNER_ID = 8;

// Worker component field IDs
const Schema_FieldId WORKER_COMPONENT_WORKER_ID_ID = 1;
const Schema_FieldId WORKER_COMPONENT_WORKER_TYPE_ID = 2;

// Partition component field IDs
const Schema_FieldId PARTITION_COMPONENT_WORKER_ID = 1;

// Reserved entity IDs expire in 5 minutes, we will refresh them every 3 minutes to be safe.
const float ENTITY_RANGE_EXPIRATION_INTERVAL_SECONDS = 180.0f;

const float FIRST_COMMAND_RETRY_WAIT_SECONDS = 0.2f;
const uint32 MAX_NUMBER_COMMAND_ATTEMPTS = 5u;
const float FORWARD_PLAYER_SPAWN_COMMAND_WAIT_SECONDS = 0.2f;

const VirtualWorkerId INVALID_VIRTUAL_WORKER_ID = 0;
const ActorLockToken INVALID_ACTOR_LOCK_TOKEN = 0;
const FString INVALID_WORKER_NAME = TEXT("");

static const FName DefaultLayer = FName(TEXT("DefaultLayer"));

const FName RoutingWorkerType(TEXT("RoutingWorker"));

const FString ClientsStayConnectedURLOption = TEXT("clientsStayConnected");
const FString SpatialSessionIdURLOption = TEXT("spatialSessionId=");

const FString LOCATOR_HOST = TEXT("locator.improbable.io");
const FString LOCATOR_HOST_CN = TEXT("locator.spatialoschina.com");
const uint16 LOCATOR_PORT = 443;

const FString CONSOLE_HOST = TEXT("console.improbable.io");
const FString CONSOLE_HOST_CN = TEXT("console.spatialoschina.com");

const FString AssemblyPattern = TEXT("^[a-zA-Z0-9_.-]{5,64}$");
const FText AssemblyPatternHint =
	LOCTEXT("AssemblyPatternHint",
			"Assembly name may only contain alphanumeric characters, '_', '.', or '-', and must be between 5 and 64 characters long.");
const FString ProjectPattern = TEXT("^[a-z0-9_]{3,32}$");
const FText ProjectPatternHint =
	LOCTEXT("ProjectPatternHint",
			"Project name may only contain lowercase alphanumeric characters or '_', and must be between 3 and 32 characters long.");
const FString DeploymentPattern = TEXT("^[a-z0-9_]{2,32}$");
const FText DeploymentPatternHint =
	LOCTEXT("DeploymentPatternHint",
			"Deployment name may only contain lowercase alphanumeric characters or '_', and must be between 2 and 32 characters long.");
const FString Ipv4Pattern = TEXT("^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$");

inline float GetCommandRetryWaitTimeSeconds(uint32 NumAttempts)
{
	// Double the time to wait on each failure.
	uint32 WaitTimeExponentialFactor = 1u << (NumAttempts - 1);
	return FIRST_COMMAND_RETRY_WAIT_SECONDS * WaitTimeExponentialFactor;
}

const FString LOCAL_HOST = TEXT("127.0.0.1");
const uint16 DEFAULT_PORT = 7777;

const uint16 DEFAULT_SERVER_RECEPTIONIST_PROXY_PORT = 7777;

const float ENTITY_QUERY_RETRY_WAIT_SECONDS = 3.0f;

const Worker_ComponentId MIN_EXTERNAL_SCHEMA_ID = 1000;
const Worker_ComponentId MAX_EXTERNAL_SCHEMA_ID = 2000;

const FString SPATIALOS_METRICS_DYNAMIC_FPS = TEXT("Dynamic.FPS");

// URL that can be used to reconnect using the command line arguments.
const FString RECONNECT_USING_COMMANDLINE_ARGUMENTS = TEXT("0.0.0.0");
const FString URL_LOGIN_OPTION = TEXT("login=");
const FString URL_PLAYER_IDENTITY_OPTION = TEXT("playeridentity=");
const FString URL_DEV_AUTH_TOKEN_OPTION = TEXT("devauthtoken=");
const FString URL_TARGET_DEPLOYMENT_OPTION = TEXT("deployment=");
const FString URL_PLAYER_ID_OPTION = TEXT("playerid=");
const FString URL_DISPLAY_NAME_OPTION = TEXT("displayname=");
const FString URL_METADATA_OPTION = TEXT("metadata=");
const FString URL_USE_EXTERNAL_IP_FOR_BRIDGE_OPTION = TEXT("useExternalIpForBridge");

const FString SHUTDOWN_PREPARATION_WORKER_FLAG = TEXT("PrepareShutdown");

const FString DEVELOPMENT_AUTH_PLAYER_ID = TEXT("Player Id");

const FString SCHEMA_DATABASE_FILE_PATH = TEXT("Spatial/SchemaDatabase");
const FString SCHEMA_DATABASE_ASSET_PATH = TEXT("/Game/Spatial/SchemaDatabase");

// An empty map with the game mode override set to GameModeBase.
const FString EMPTY_TEST_MAP_PATH = TEXT("/SpatialGDK/Maps/Empty");

const FString DEV_LOGIN_TAG = TEXT("dev_login");

// A list of components clients require on top of any generated data components in order to handle non-authoritative actors correctly.
const TArray<Worker_ComponentId> REQUIRED_COMPONENTS_FOR_NON_AUTH_CLIENT_INTEREST = TArray<Worker_ComponentId>{
	// Actor components
	UNREAL_METADATA_COMPONENT_ID, SPAWN_DATA_COMPONENT_ID, TOMBSTONE_COMPONENT_ID, TOMBSTONE_TAG_COMPONENT_ID, DORMANT_COMPONENT_ID,

	// Multicast RPCs
	MULTICAST_RPCS_COMPONENT_ID,

	// Global state components
	DEPLOYMENT_MAP_COMPONENT_ID, STARTUP_ACTOR_MANAGER_COMPONENT_ID, GSM_SHUTDOWN_COMPONENT_ID,

	// Debugging information
	DEBUG_METRICS_COMPONENT_ID, SPATIAL_DEBUGGING_COMPONENT_ID,

	// Actor tag
	ACTOR_TAG_COMPONENT_ID
};

// A list of components clients require on entities they are authoritative over on top of the components already checked out by the interest
// query.
const TArray<Worker_ComponentId> REQUIRED_COMPONENTS_FOR_AUTH_CLIENT_INTEREST =
	TArray<Worker_ComponentId>{ // RPCs from the server
								SERVER_ENDPOINT_COMPONENT_ID,

								// Actor tags
								ACTOR_TAG_COMPONENT_ID, ACTOR_AUTH_TAG_COMPONENT_ID
	};

// A list of components servers require on top of any generated data and handover components in order to handle non-authoritative actors
// correctly.
const TArray<Worker_ComponentId> REQUIRED_COMPONENTS_FOR_NON_AUTH_SERVER_INTEREST =
	TArray<Worker_ComponentId>{ // Actor components
								UNREAL_METADATA_COMPONENT_ID, SPAWN_DATA_COMPONENT_ID, TOMBSTONE_COMPONENT_ID, DORMANT_COMPONENT_ID,
								NET_OWNING_CLIENT_WORKER_COMPONENT_ID,

								// Multicast RPCs
								MULTICAST_RPCS_COMPONENT_ID,

								// Global state components
								DEPLOYMENT_MAP_COMPONENT_ID, STARTUP_ACTOR_MANAGER_COMPONENT_ID, GSM_SHUTDOWN_COMPONENT_ID,

								// Unreal load balancing components
								VIRTUAL_WORKER_TRANSLATION_COMPONENT_ID,

								// Authority intent component to handle scattered hierarchies
								AUTHORITY_INTENT_COMPONENT_ID,

								// Tags: Well known entities, non-auth actors, and tombstone tags
								GDK_KNOWN_ENTITY_TAG_COMPONENT_ID, ACTOR_TAG_COMPONENT_ID, TOMBSTONE_TAG_COMPONENT_ID,

								PLAYER_CONTROLLER_COMPONENT_ID, PARTITION_COMPONENT_ID
	};

// A list of components servers require on entities they are authoritative over on top of the components already checked out by the interest
// query.
const TArray<Worker_ComponentId> REQUIRED_COMPONENTS_FOR_AUTH_SERVER_INTEREST =
	TArray<Worker_ComponentId>{ // RPCs from clients
								CLIENT_ENDPOINT_COMPONENT_ID,

								// Player controller
								PLAYER_CONTROLLER_COMPONENT_ID,

								// Cross server endpoint
								CROSSSERVER_SENDER_ACK_ENDPOINT_COMPONENT_ID, CROSSSERVER_RECEIVER_ENDPOINT_COMPONENT_ID,

								// Actor tags
								ACTOR_TAG_COMPONENT_ID, ACTOR_AUTH_TAG_COMPONENT_ID,

								PARTITION_COMPONENT_ID
	};

inline bool IsEntityCompletenessComponent(Worker_ComponentId ComponentId)
{
	return ComponentId >= SpatialConstants::FIRST_EC_COMPONENT_ID && ComponentId <= SpatialConstants::LAST_EC_COMPONENT_ID;
}

// TODO: These containers should be cleaned up when we move to reading component set data directly from schema bundle - UNR-4666
const TArray<FString> ServerAuthorityWellKnownSchemaImports = {
	"improbable/standard_library.schema",
	"unreal/gdk/authority_intent.schema",
	"unreal/gdk/debug_component.schema",
	"unreal/gdk/debug_metrics.schema",
	"unreal/gdk/net_owning_client_worker.schema",
	"unreal/gdk/not_streamed.schema",
	"unreal/gdk/query_tags.schema",
	"unreal/gdk/relevant.schema",
	"unreal/gdk/rpc_components.schema",
	"unreal/gdk/spatial_debugging.schema",
	"unreal/gdk/spawndata.schema",
	"unreal/gdk/tombstone.schema",
	"unreal/gdk/unreal_metadata.schema",
	"unreal/generated/rpc_endpoints.schema",
	"unreal/generated/NetCullDistance/ncdcomponents.schema",
};

const TMap<Worker_ComponentId, FString> ServerAuthorityWellKnownComponents = {
	{ POSITION_COMPONENT_ID, "improbable.Position" },
	{ INTEREST_COMPONENT_ID, "improbable.Interest" },
	{ AUTHORITY_DELEGATION_COMPONENT_ID, "improbable.AuthorityDelegation" },
	{ AUTHORITY_INTENT_COMPONENT_ID, "unreal.AuthorityIntent" },
	{ GDK_DEBUG_COMPONENT_ID, "unreal.DebugComponent" },
	{ DEBUG_METRICS_COMPONENT_ID, "unreal.DebugMetrics" },
	{ NET_OWNING_CLIENT_WORKER_COMPONENT_ID, "unreal.NetOwningClientWorker" },
	{ NOT_STREAMED_COMPONENT_ID, "unreal.NotStreamed" },
	{ ALWAYS_RELEVANT_COMPONENT_ID, "unreal.AlwaysRelevant" },
	{ DORMANT_COMPONENT_ID, "unreal.Dormant" },
	{ VISIBLE_COMPONENT_ID, "unreal.Visible" },
	{ SERVER_TO_SERVER_COMMAND_ENDPOINT_COMPONENT_ID, "unreal.UnrealServerToServerCommandEndpoint" },
	{ SPATIAL_DEBUGGING_COMPONENT_ID, "unreal.SpatialDebugging" },
	{ SPAWN_DATA_COMPONENT_ID, "unreal.SpawnData" },
	{ TOMBSTONE_COMPONENT_ID, "unreal.Tombstone" },
	{ UNREAL_METADATA_COMPONENT_ID, "unreal.UnrealMetadata" },
	{ SERVER_ENDPOINT_COMPONENT_ID, "unreal.generated.UnrealServerEndpoint" },
	{ MULTICAST_RPCS_COMPONENT_ID, "unreal.generated.UnrealMulticastRPCs" },
	{ SERVER_ENDPOINT_COMPONENT_ID, "unreal.generated.UnrealServerEndpoint" },
	{ CROSSSERVER_SENDER_ENDPOINT_COMPONENT_ID, "unreal.generated.UnrealCrossServerSenderRPCs" },
	{ CROSSSERVER_RECEIVER_ACK_ENDPOINT_COMPONENT_ID, "unreal.generated.UnrealCrossServerReceiverACKRPCs" },
};

const TArray<FString> ClientAuthorityWellKnownSchemaImports = { "unreal/gdk/player_controller.schema", "unreal/gdk/rpc_components.schema",
																"unreal/generated/rpc_endpoints.schema" };

const TMap<Worker_ComponentId, FString> ClientAuthorityWellKnownComponents = {
	{ PLAYER_CONTROLLER_COMPONENT_ID, "unreal.PlayerController" },
	{ CLIENT_ENDPOINT_COMPONENT_ID, "unreal.generated.UnrealClientEndpoint" },
};

const TMap<Worker_ComponentId, FString> RoutingWorkerComponents = {
	{ CROSSSERVER_SENDER_ACK_ENDPOINT_COMPONENT_ID, "unreal.generated.UnrealCrossServerSenderACKRPCs" },
	{ CROSSSERVER_RECEIVER_ENDPOINT_COMPONENT_ID, "unreal.generated.UnrealCrossServerReceiverRPCs" },
};

const TArray<FString> RoutingWorkerSchemaImports = { "unreal/gdk/rpc_components.schema", "unreal/generated/rpc_endpoints.schema" };

const TArray<Worker_ComponentId> KnownEntityAuthorityComponents = { POSITION_COMPONENT_ID,		 METADATA_COMPONENT_ID,
																	INTEREST_COMPONENT_ID,		 PLAYER_SPAWNER_COMPONENT_ID,
																	DEPLOYMENT_MAP_COMPONENT_ID, STARTUP_ACTOR_MANAGER_COMPONENT_ID,
																	GSM_SHUTDOWN_COMPONENT_ID,	 VIRTUAL_WORKER_TRANSLATION_COMPONENT_ID,
																	SERVER_WORKER_COMPONENT_ID };

} // namespace SpatialConstants

DECLARE_STATS_GROUP(TEXT("SpatialNet"), STATGROUP_SpatialNet, STATCAT_Advanced);

#undef LOCTEXT_NAMESPACE
