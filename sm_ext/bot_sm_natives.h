#pragma once

#include <IExtensionSys.h>

using namespace SourceMod;
using namespace SourcePawn;

cell_t sm_RCBotIsWaypointAvailable(IPluginContext *pContext, const cell_t *params);

cell_t sm_RCBotCreate(IPluginContext *pContext, const cell_t *params);
cell_t sm_RCBotIsClientBot(IPluginContext *pContext, const cell_t *params);

cell_t sm_RCBotSetProfileInt(IPluginContext *pContext, const cell_t *params);
cell_t sm_RCBotGetProfileInt(IPluginContext *pContext, const cell_t *params);

cell_t sm_RCBotSetProfileFloat(IPluginContext *pContext, const cell_t *params);
cell_t sm_RCBotGetProfileFloat(IPluginContext *pContext, const cell_t *params);

cell_t sm_botAim(IPluginContext *pContext, const cell_t *params);
cell_t sm_botsAim(IPluginContext *pContext, const cell_t *params);
cell_t sm_botFire(IPluginContext *pContext, const cell_t *params);
cell_t sm_botsFire(IPluginContext *pContext, const cell_t *params);
cell_t sm_botDuck(IPluginContext *pContext, const cell_t *params);
cell_t sm_botJump(IPluginContext *pContext, const cell_t *params);
cell_t sm_botMove(IPluginContext *pContext, const cell_t *params);

const sp_nativeinfo_t g_RCBotNatives[] = {
	{ "RCBot2_IsWaypointAvailable", sm_RCBotIsWaypointAvailable },
	
	{ "RCBot2_CreateBot", sm_RCBotCreate },
	{ "IsRCBot2Client", sm_RCBotIsClientBot },
	
	{ "RCBot2_SetProfileInt", sm_RCBotSetProfileInt },
	{ "RCBot2_GetProfileInt", sm_RCBotGetProfileInt },
	
	{ "RCBot2_SetProfileFloat", sm_RCBotSetProfileFloat },
	{ "RCBot2_GetProfileFloat", sm_RCBotGetProfileFloat },
	
	{ "sml_botAim", sm_botAim },
	{ "sml_botsAim", sm_botsAim },
	{ "sml_botFire", sm_botFire },
	{ "sml_botsFire", sm_botsFire },
	{ "sml_botDuck", sm_botDuck },
	{ "sml_botJump", sm_botJump },
	{ "sml_botMove", sm_botMove },

	{nullptr, nullptr},
};
