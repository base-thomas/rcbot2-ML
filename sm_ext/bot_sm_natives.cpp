#include "bot_sm_natives.h"
#include "bot_sm_ext.h"

#include "bot.h"
#include "bot_profile.h"
#include "bot_waypoint.h"

enum RCBotProfileVar : std::uint8_t {
	RCBotProfile_iVisionTicks,
	RCBotProfile_iPathTicks,
	RCBotProfile_iVisionTicksClients,
	RCBotProfile_iSensitivity,
	RCBotProfile_fBraveness,
	RCBotProfile_fAimSkill,
	RCBotProfile_iClass,
};

int* GetIntProperty(CBotProfile* profile, RCBotProfileVar profileVar);
float* GetFloatProperty(CBotProfile* profile, RCBotProfileVar profileVar);

cell_t sm_RCBotIsWaypointAvailable(IPluginContext *pContext, const cell_t *params) {
	return CWaypoints::numWaypoints() > 0;
}

cell_t sm_RCBotCreate(IPluginContext *pContext, const cell_t *params) {
	char *name;
	pContext->LocalToString(params[1], &name);

	const int slot = CBots::createDefaultBot(name);
	
	// player slots are off-by-one (though this calculation is performed in the function)
	return (slot != -1)? slot + 1 : -1;
}

/* native void RCBot2_SetProfileInt(int client, RCBotProfileVar property, int value); */
cell_t sm_RCBotSetProfileInt(IPluginContext* pContext, const cell_t* params) {
	const int client = params[1];
	const RCBotProfileVar profileVar = static_cast<RCBotProfileVar>(params[2]);
	if (client < 1 || client > gpGlobals->maxClients) {
		pContext->ThrowNativeError("Invalid client index %d", client);
		return 0; // Ensure the function exits
	}

	const CBot* bot = CBots::getBot(client - 1);
	if (!bot) {
		pContext->ThrowNativeError("Client index %d is not a RCBot", client);
		return 0; // Ensure the function exits
	}

	CBotProfile* profile = bot->getProfile();
	int* value = GetIntProperty(profile, profileVar);
	if (!value) {
		pContext->ThrowNativeError("RCBot property %d is not an integer property", profileVar);
		return 0;
	}
	*value = params[3];

	return 0;
}

/* native void RCBot2_SetProfileFloat(int client, RCBotProfileVar property, float value); */
cell_t sm_RCBotSetProfileFloat(IPluginContext *pContext, const cell_t *params) {
	const int client = params[1];
	const RCBotProfileVar profileVar = static_cast<RCBotProfileVar>(params[2]);
	if (client < 1 || client > gpGlobals->maxClients) {
		pContext->ThrowNativeError("Invalid client index %d", client);
	}

	const CBot* bot = CBots::getBot(client - 1);
	if (!bot) {
		pContext->ThrowNativeError("Client index %d is not a RCBot", client);
	}
	
	CBotProfile* profile = bot->getProfile();
	float* value = GetFloatProperty(profile, profileVar);
	if (!value) {
		pContext->ThrowNativeError("RCBot property %d is not a float property", profileVar);
		return 0;
	}
	*value = sp_ctof(params[3]);
	
	return 0;
}

/* native int RCBot2_GetProfileInt(int client, RCBotProfileVar property); */
cell_t sm_RCBotGetProfileInt(IPluginContext *pContext, const cell_t *params) {
	const int client = params[1];
	const RCBotProfileVar profileVar = static_cast<RCBotProfileVar>(params[2]);
	if (client < 1 || client > gpGlobals->maxClients) {
		pContext->ThrowNativeError("Invalid client index %d", client);
	}

	const CBot* bot = CBots::getBot(client - 1);
	if (!bot) {
		pContext->ThrowNativeError("Client index %d is not a RCBot", client);
	}
	
	CBotProfile* profile = bot->getProfile();
	const int* value = GetIntProperty(profile, profileVar);
	if (!value) {
		pContext->ThrowNativeError("RCBot property %d is not an integer property", profileVar);
		return 0;
	}
	return *value;
}

/* native float RCBot2_GetProfileFloat(int client, RCBotProfileVar property); */
cell_t sm_RCBotGetProfileFloat(IPluginContext *pContext, const cell_t *params) {
	const int client = params[1];
	const RCBotProfileVar profileVar = static_cast<RCBotProfileVar>(params[2]);
	if (client < 1 || client > gpGlobals->maxClients) {
		pContext->ThrowNativeError("Invalid client index %d", client);
	}

	const CBot* bot = CBots::getBot(client - 1);
	if (!bot) {
		pContext->ThrowNativeError("Client index %d is not a RCBot", client);
	}
	
	CBotProfile* profile = bot->getProfile();
	const float* value = GetFloatProperty(profile, profileVar);
	if (!value) {
		pContext->ThrowNativeError("RCBot property %d is not a float property", profileVar);
		return 0;
	}
	return sp_ftoc(*value);
}

cell_t sm_RCBotIsClientBot(IPluginContext *pContext, const cell_t *params) {
	const int client = params[1];
	if (client < 1 || client > gpGlobals->maxClients) {
		pContext->ThrowNativeError("Invalid client index %d", client);
		return 0;
	}
	return CBots::getBot(client - 1) != nullptr;
}

int* GetIntProperty(CBotProfile* profile, const RCBotProfileVar profileVar) {
	switch (profileVar) {
		case RCBotProfile_iVisionTicks:
			return &profile->m_iVisionTicks;
		case RCBotProfile_iPathTicks:
			return &profile->m_iPathTicks;
		case RCBotProfile_iClass:
			return &profile->m_iClass;
		case RCBotProfile_iVisionTicksClients:
			return &profile->m_iVisionTicksClients;
		case RCBotProfile_iSensitivity:
			return &profile->m_iSensitivity;
	}
	return nullptr;
}

float* GetFloatProperty(CBotProfile* profile, const RCBotProfileVar profileVar) {
	switch (profileVar) {
		case RCBotProfile_fBraveness:
			return &profile->m_fBraveness;
		case RCBotProfile_fAimSkill:
			return &profile->m_fAimSkill;
	}
	return nullptr;
}


// ML control functions by base_thomas

cell_t sm_botAim(IPluginContext *pContext, const cell_t *params) {
	if (!params[1]) {
		logger->Log(LogLevel::ERROR, "No target specified in sm_botAim!");
		return 0;
	}
	char *name;
	pContext->LocalToString(params[1], &name);
	Vector tvec;
	//logger->Log(LogLevel::FATAL, "name: %s", name);

	//find bot by name
	//const char *name = "bot" + *id + "_"; //this doesnt work bc its not a std::string (it's a char *)
	edict_t *pEdict = CBotGlobals::findPlayerByTruncName(name);
	//logger->Log(LogLevel::FATAL, "edict: %i", pEdict);
	if (pEdict == nullptr) {
		logger->Log(LogLevel::ERROR, "Can't find player: %s", name);
		return 0;
	}
	int targeti = ENTINDEX(pEdict);
	//logger->Log(LogLevel::FATAL, "Target index: %i", targeti);
	CBot* bot = CBots::getBot(targeti - 1);
	if (!bot) {
		pContext->ThrowNativeError("Client index %d is not a RCBot!", targeti);
	}

	//logger->Log(LogLevel::FATAL, "botaim # of params: %d", sizeof(params));
	if (sizeof(params) == 4) {
		//logger->Log(LogLevel::FATAL, "sp_ctof(params[2]): %f and %s", sp_ctof(params[2]), sp_ctof(params[2]));
		tvec.x = sp_ctof(params[2]);
		tvec.y = sp_ctof(params[3]);
		tvec.z = sp_ctof(params[4]);
		//logger->Log(LogLevel::FATAL, "botaim params valid!: %s [id: %i] - (%f, %f, %f)", name, targeti, tvec.x, tvec.y, tvec.z);
	} else {
		//once base system is tested* assume 1 arg means aim at the target bot
		//tvec = bot->getEyePosition();
		//logger->Log(LogLevel::ERROR, "botaim params invalid!");//: %s - (%s, %s, %s)", params[1], params[2], params[3], params[4]);
		return 0;
	}

	bot->setLookVector(tvec); //rel dist from host to target(?)
	bot->setLookAtTask(LOOK_VECTOR);
	bot->setMoveLookPriority(MOVELOOK_OVERRIDE);
	//bot->setModLook(tvec);
	return 0;
}

cell_t sm_botsAim(IPluginContext *pContext, const cell_t *params) {

	//find target bot by name
	edict_t *pEdict = CBotGlobals::findPlayerByTruncName("bot0_");
	if (pEdict == nullptr) {
		logger->Log(LogLevel::ERROR, "Can't find player bot0_");
		return 0;
	}
	int targeti = ENTINDEX(pEdict);
	//logger->Log(LogLevel::FATAL, "Target index: %i", targeti);
	CBot* bot = CBots::getBot(targeti - 1);

	Vector tvec;
	if (sizeof(params) == 3) { // user-specified location to look at
		tvec.x = sp_ctof(params[1]);
		tvec.y = sp_ctof(params[2]);
		tvec.z = sp_ctof(params[3]);
		//logger->Log(LogLevel::FATAL, "botsaim params valid: (%s, %s, %s)", tvec.x, tvec.y, tvec.z);
	} else {
		tvec = bot->getEyePosition(); //save target pos
	}
	//logger->Log(LogLevel::FATAL, "tvec: (%f, %f, %f)", tvec.x, tvec.y, tvec.z);


	for (int i = 1; i < gpGlobals->maxClients; i++) {
		if (i == targeti - 1) {continue;} //do nothing (target bot)
		bot = CBots::getBot(i);
		if (!bot) {
			//pContext->ThrowNativeError("Client index %d is not a RCBot", i);
			//logger->Log(LogLevel::FATAL, "index %i is not a bot!", i);
			continue;
		}

		Vector d = tvec;// - bot->getEyePosition();
		//logger->Log(LogLevel::FATAL, "index %i: (%f, %f, %f)", i, d.x, d.y, d.z);
		bot->setLookVector(d); //rel dist from host to target
		bot->setLookAtTask(LOOK_VECTOR);
		bot->setMoveLookPriority(MOVELOOK_OVERRIDE);
	}
	return 0;
}

cell_t sm_botFire(IPluginContext *pContext, const cell_t *params) {
	char *name;
	pContext->LocalToString(params[1], &name);

	edict_t *pEdict = CBotGlobals::findPlayerByTruncName(name);
	if (pEdict == nullptr) {
		logger->Log(LogLevel::ERROR, "Can't find player");
		return 0;
	}

	int targeti = ENTINDEX(pEdict);
	CBot* bot = CBots::getBot(targeti - 1);
	if (!bot) {
		//pContext->ThrowNativeError("Client index %d is not a RCBot", i);
		logger->Log(LogLevel::FATAL, "(sm_botFire) index %i not a bot!", params[1]);
	}
	int psize = sizeof(params); //params[0] is equivalent to sizeof(params) somehow
	if (psize >= 2) { // extra parameters supplied
		float holdTime = 0.0;
		if (psize > 4) {
			logger->Log(LogLevel::FATAL, "ERROR: (sm_botFire) too many parameters (%i) supplied!", psize);
			return 0;
		}
		else if (params[4]) { // 'hold time' param supplied
			holdTime = sp_ctof(params[4]);
			if (holdTime == 0.0) {logger->Log(LogLevel::FATAL, "WARNING in botFire: shouldve checked float val");}
		}/* else {
			holdTime = nullptr;
		}*/
		if (params[2]) { // alt fire
			bot->secondaryAttack(bool(params[3]), holdTime); // TODO: bool()? to hide warning
		} else { // primary fire
			bot->primaryAttack(bool(params[3]), holdTime);
		}
	} else {
		bot->primaryAttack();
	}
	return 0;
}

cell_t sm_botsFire(IPluginContext *pContext, const cell_t *params) {
	//find target bot by name
	edict_t *pEdict = CBotGlobals::findPlayerByTruncName("bot0_");
	if (pEdict == nullptr) {
		logger->Log(LogLevel::ERROR, "Can't find player bot0_");
		return 0;
	}
	int targeti = ENTINDEX(pEdict);
	//logger->Log(LogLevel::FATAL, "Target index: %i", targeti);
	CBot* bot = CBots::getBot(targeti - 1);

	for (int i = 1; i < gpGlobals->maxClients; i++) {
		if (i == targeti - 1) {
			continue; //skip target bot (it shouldnt shoot)
		}
		CBot* bot = CBots::getBot(i);
		if (!bot) {
			//pContext->ThrowNativeError("Client index %d is not a RCBot", i);
			//logger->Log(LogLevel::FATAL, "index %i not a bot!", i);
			continue;
		}
		//bot->handleAttack(nullptr, nullptr); //bot->getCurrentWeapon()
		//logger->Log(LogLevel::FATAL, "bot %i shoot!", i);
		bot->primaryAttack();
	}
	return 0;
}

cell_t sm_botDuck(IPluginContext *pContext, const cell_t *params) {
	//find target bot by name
	char *name;
	pContext->LocalToString(params[1], &name);

	edict_t *pEdict = CBotGlobals::findPlayerByTruncName(name);
	if (pEdict == nullptr) {
		logger->Log(LogLevel::ERROR, "Can't find player");
		return 0;
	}
	int targeti = ENTINDEX(pEdict);
	CBot* bot = CBots::getBot(targeti - 1);
	if (!bot) {
		//pContext->ThrowNativeError("Client index %d is not a RCBot", i);
		logger->Log(LogLevel::ERROR, "index %i not a bot!", targeti);
		return 0;
	}
	bot->duck();
	return 0;
}

cell_t sm_botJump(IPluginContext *pContext, const cell_t *params) {
	//find target bot by name
	char *name;
	pContext->LocalToString(params[1], &name);

	edict_t *pEdict = CBotGlobals::findPlayerByTruncName(name);
	if (pEdict == nullptr) {
		logger->Log(LogLevel::ERROR, "Can't find player");
		return 0;
	}
	int targeti = ENTINDEX(pEdict);
	CBot* bot = CBots::getBot(targeti - 1);
	if (!bot) {
		//pContext->ThrowNativeError("Client index %d is not a RCBot", i);
		logger->Log(LogLevel::ERROR, "index %i not a bot!", targeti);
		return 0;
	}
	bot->jump();
	return 0;
}

cell_t sm_botMove(IPluginContext *pContext, const cell_t *params) {
	//find target bot by name
	char *name;
	pContext->LocalToString(params[1], &name);

	edict_t *pEdict = CBotGlobals::findPlayerByTruncName(name);
	if (pEdict == nullptr) {
		logger->Log(LogLevel::ERROR, "Can't find player");
		return 0;
	}
	int targeti = ENTINDEX(pEdict);
	CBot* bot = CBots::getBot(targeti - 1);
	if (!bot) {
		//pContext->ThrowNativeError("Client index %d is not a RCBot", i);
		logger->Log(LogLevel::ERROR, "index %i not a bot!", targeti);
		return 0;
	}

	int ws = params[2];
	int sd = params[3];
	bot->doModMove(ws, sd);
	return 0;
}
