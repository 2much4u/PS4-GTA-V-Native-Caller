#pragma once

#include "type.h"
#include "gta.h"

static u64 *args = (u64*)FreeSpaceAddress;
static u64 *rets = (u64*)FreeSpaceAddress + 0x100;

static void(*invokeNative)(u64 hash) = (void*)PayloadAddress;

// Find more natives at http://dev-c.com/nativedb/

Inline int GET_FRAME_COUNT() {
	invokeNative(0xFC8202EFC642E6F2);
	return rets[0];
}

Inline Player PLAYER_ID() {
	invokeNative(0x4F8644AF03D0E0D6);
	return rets[0];
}

Inline Ped PLAYER_PED_ID() {
	invokeNative(0xD80958FC74E988A6);
	return rets[0];
}

Inline void SET_SUPER_JUMP_THIS_FRAME(Player player) {
	args[0] = player;
	invokeNative(0x57FFF03E423A4C0B);
}

Inline void SET_PLAYER_INVINCIBLE(Player player, BOOL toggle) {
	args[0] = player;
	args[1] = toggle;
	invokeNative(0x239528EACDC3E7DE);
}

Inline Blip GET_FIRST_BLIP_INFO_ID(int blipSprite) {
	args[0] = blipSprite;
	invokeNative(0x1BEDE233E6CD2A1F);
	return rets[0];
}

Inline BOOL DOES_BLIP_EXIST(Blip blip) {
	args[0] = blip;
	invokeNative(0xA6DB27D19ECBB7DA);
	return rets[0];
}

Inline Vector3 GET_BLIP_COORDS(Blip blip) {
	args[0] = blip;
	invokeNative(0x586AFE3FF72D996E);
	Vector3 vec3 = { *(float*)&rets[0], *(float*)&rets[1], *(float*)&rets[2] };
	return vec3;
}

Inline Vector3 GET_ENTITY_COORDS(Entity entity, BOOL alive) {
	args[0] = entity;
	args[1] = alive;
	invokeNative(0x3FEF770D40960D5A);
	Vector3 vec3 = { *(float*)&rets[0], *(float*)&rets[1], *(float*)&rets[2] };
	return vec3;
}

Inline void SET_ENTITY_COORDS(Entity entity, float xPos, float yPos, float zPos, BOOL xAxis, BOOL yAxis, BOOL zAxis, BOOL clearArea) {
	args[0] = entity;
	*(float*)&args[1] = xPos;
	*(float*)&args[2] = yPos;
	*(float*)&args[3] = zPos;
	args[4] = xAxis;
	args[5] = yAxis;
	args[6] = zAxis;
	args[7] = clearArea;
	invokeNative(0x06843DA7060A026B);
}

Inline BOOL IS_CONTROL_JUST_PRESSED(int inputGroup, int control) {
	args[0] = inputGroup;
	args[1] = control;
	invokeNative(0x580417101DDB492F);
	return rets[0];
}

Inline BOOL IS_DISABLED_CONTROL_JUST_PRESSED(int inputGroup, int control) {
	args[0] = inputGroup;
	args[1] = control;
	invokeNative(0x91AEF906BCA88877);
	return rets[0];
}

Inline void DISABLE_CONTROL_ACTION(int inputGroup, int control, BOOL disable) {
	args[0] = inputGroup;
	args[1] = control;
	args[2] = disable;
	invokeNative(0xFE99B66D079CF6BC);
}

Inline void SET_INPUT_EXCLUSIVE(int inputGroup, int control) {
	args[0] = inputGroup;
	args[1] = control;
	invokeNative(0xEDE476E5EE29EDB1);
}

Inline void PLAY_SOUND_FRONTEND(int soundID, char *audioName, char *audioRef, BOOL p3) {
	args[0] = soundID;
	args[1] = (u64)audioName;
	args[2] = (u64)audioRef;
	args[3] = p3;
	invokeNative(0x67C540AA08E4A6F5);
}

Inline void DISPLAY_ONSCREEN_KEYBOARD(int p0, char *windowTitle, char *p2, char *defaultText, char *defaultConcat1, char *defaultConcat2, char *defaultConcat3, int maxInputLength) {
	args[0] = p0;
	args[1] = (u64)windowTitle;
	args[2] = (u64)p2;
	args[3] = (u64)defaultText;
	args[4] = (u64)defaultConcat1;
	args[5] = (u64)defaultConcat2;
	args[6] = (u64)defaultConcat3;
	args[7] = maxInputLength;
	invokeNative(0x00DC833F2568DBF6);
}

Inline int UPDATE_ONSCREEN_KEYBOARD() {
	invokeNative(0x0CF2B696BBF945AE);
	return rets[0];
}

Inline char* GET_ONSCREEN_KEYBOARD_RESULT() {
	invokeNative(0x8362B09B91893647);
	return (char*)rets[0];
}

Inline Hash GET_HASH_KEY(char* value) {
	args[0] = (u64)value;
	invokeNative(0xD24D37CC275948CC);
	return rets[0];
}

Inline BOOL IS_MODEL_VALID(Hash model) {
	args[0] = model;
	invokeNative(0xC0296A2EDF545E92);
	return rets[0];
}

Inline BOOL HAS_MODEL_LOADED(Hash model) {
	args[0] = model;
	invokeNative(0x98A4EB5D89A0C952);
	return rets[0];
}

Inline void SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(Ped ped, int state) {
	args[0] = ped;
	args[1] = state;
	invokeNative(0x7A6535691B477C48);
}

Inline void SET_ENTITY_INVINCIBLE(Entity entity, BOOL toggle) {
	args[0] = entity;
	args[1] = toggle;
	invokeNative(0x3882114BDE571AD4);
}

Inline void SET_PED_CAN_RAGDOLL(Ped ped, BOOL toggle) {
	args[0] = ped;
	args[1] = toggle;
	invokeNative(0xB128377056A54E2A);
}

Inline void CLEAR_PLAYER_WANTED_LEVEL(Player player) {
	args[0] = player;
	invokeNative(0xB302540597885499);
}

Inline void SET_EXPLOSIVE_AMMO_THIS_FRAME(Player player) {
	args[0] = player;
	invokeNative(0xA66C71C98D5F2CFB);
}

Inline void SET_PLAYER_MODEL(Player player, Hash model) {
	args[0] = player;
	args[1] = model;
	invokeNative(0x00A1CADD00108836);
}

Inline void SET_PED_DEFAULT_COMPONENT_VARIATION(Ped ped) {
	args[0] = ped;
	invokeNative(0x45EEE61580806D63);
}

Inline void SET_PED_RANDOM_COMPONENT_VARIATION(Ped ped, BOOL p1) {
	args[0] = ped;
	args[1] = p1;
	invokeNative(0xC8A9481A01E63C28);
}

Inline void SET_MODEL_AS_NO_LONGER_NEEDED(Hash model) {
	args[0] = model;
	invokeNative(0xE532F5D78798DAAB);
}

Inline void REQUEST_MODEL(Hash model) {
	args[0] = model;
	invokeNative(0x963D27A58DF860AC);
}

Inline float GET_ENTITY_HEADING(Entity entity) {
	args[0] = entity;
	invokeNative(0xE83D4F9BA2A38914);
	return *(float*)&rets[0];
}

Inline Vehicle CREATE_VEHICLE(Hash modelHash, float x, float y, float z, float heading, BOOL isNetwork, BOOL p6) {
	args[0] = modelHash;
	*(float*)&args[1] = x;
	*(float*)&args[2] = y;
	*(float*)&args[3] = z;
	*(float*)&args[4] = heading;
	args[5] = isNetwork;
	args[6] = p6;
	invokeNative(0xAF35D0D2583051B0);
	return rets[0];
}

Inline BOOL DOES_ENTITY_EXIST(Entity entity) {
	args[0] = entity;
	invokeNative(0x7239B21A38F536BA);
	return rets[0];
}

Inline BOOL IS_PED_IN_ANY_VEHICLE(Ped ped, BOOL atGetIn) {
	args[0] = ped;
	args[1] = atGetIn;
	invokeNative(0x997ABD671D25CA0B);
	return rets[0];
}

Inline Vehicle GET_VEHICLE_PED_IS_USING(Ped ped) {
	args[0] = ped;
	invokeNative(0x6094AD011A2EA87D);
	return rets[0];
}

Inline void SET_ENTITY_AS_MISSION_ENTITY(Entity entity, BOOL p1, BOOL byThisScript) {
	args[0] = entity;
	args[1] = p1;
	args[2] = byThisScript;
	invokeNative(0xAD738C3085FE7E11);
}

Inline void DELETE_VEHICLE(Vehicle *vehicle) {
	args[0] = (u64)vehicle;
	invokeNative(0xEA386986E786A54F);
}

Inline void SET_PED_INTO_VEHICLE(Ped ped, Vehicle vehicle, int seatIndex) {
	args[0] = ped;
	args[1] = vehicle;
	args[2] = seatIndex;
	invokeNative(0xF75B0D629E1C063D);
}

Inline int GET_RANDOM_INT_IN_RANGE(int startRange, int endRange) {
	args[0] = startRange;
	args[1] = endRange;
	invokeNative(0xD53343AA4FB7DD28);
	return rets[0];
}

Inline void SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Vehicle vehicle, int r, int g, int b) {
	args[0] = vehicle;
	args[1] = r;
	args[2] = g;
	args[3] = b;
	invokeNative(0x7141766F91D15BEA);
}

Inline void SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Vehicle vehicle, int r, int g, int b) {
	args[0] = vehicle;
	args[1] = r;
	args[2] = g;
	args[3] = b;
	invokeNative(0x36CED73BFED89754);
}

Inline void SET_VEHICLE_FIXED(Vehicle vehicle) {
	args[0] = vehicle;
	invokeNative(0x115722B1B9C14C1C);
}

Inline void SET_VEHICLE_FORWARD_SPEED(Vehicle vehicle, float speed) {
	args[0] = vehicle;
	*(float*)&args[1] = speed;
	invokeNative(0xAB54A438726D25D5);
}

Inline void DRAW_RECT(float x, float y, float width, float height, int r, int g, int b, int a) {
	*(float*)&args[0] = x;
	*(float*)&args[1] = y;
	*(float*)&args[2] = width;
	*(float*)&args[3] = height;
	args[4] = r;
	args[5] = g;
	args[6] = b;
	args[7] = a;
	invokeNative(0x3A618A217E5154F0);
}

Inline void SET_TEXT_COLOUR(int red, int green, int blue, int alpha) {
	args[0] = red;
	args[1] = green;
	args[2] = blue;
	args[3] = alpha;
	invokeNative(0xBE6B23FFA53FB442);
}

Inline void SET_TEXT_WRAP(float start, float end) {
	*(float*)&args[0] = start;
	*(float*)&args[1] = end;
	invokeNative(0x63145D9C883A1A70);
}

Inline void SET_TEXT_DROPSHADOW(int distance, int r, int g, int b, int a) {
	args[0] = distance;
	args[1] = r;
	args[2] = g;
	args[3] = b;
	args[4] = a;
	invokeNative(0x465C84BC39F1C351);
}

Inline void SET_TEXT_OUTLINE() {
	invokeNative(0x2513DFB0FB8400FE);
}

Inline void SET_TEXT_FONT(int fontType) {
	args[0] = fontType;
	invokeNative(0x66E0276CC5F6B9DA);
}

Inline void SET_TEXT_CENTRE(BOOL align) {
	args[0] = align;
	invokeNative(0xC02F4DBFB51D988B);
}

Inline void SET_TEXT_SCALE(float p0, float size) {
	*(float*)&args[0] = p0;
	*(float*)&args[1] = size;
	invokeNative(0x07C837F9A01C34C9);
}

Inline void BEGIN_TEXT_COMMAND_DISPLAY_TEXT(char *text) {
	args[0] = (u64)text;
	invokeNative(0x25FBB336DF1804CB);
}

Inline void ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(char *text) {
	args[0] = (u64)text;
	invokeNative(0x6C188BE134E074AA);
}

Inline void END_TEXT_COMMAND_DISPLAY_TEXT(float x, float y) {
	*(float*)&args[0] = x;
	*(float*)&args[1] = y;
	invokeNative(0xCD015E5BB0D96A57);
}

Inline void SET_ENTITY_VISIBLE(Entity entity, BOOL toggle, BOOL unk) {
	args[0] = entity;
	args[1] = toggle;
	args[2] = unk;
	invokeNative(0xEA1C610A04DB6BBB);
}

Inline void SET_MAX_WANTED_LEVEL(int maxWantedLevel) {
	args[0] = maxWantedLevel;
	invokeNative(0xAA5F02DB48D704B9);
}

Inline void SET_PLAYER_WANTED_LEVEL(Player player, int wantedLevel, BOOL disableNoMission) {
	args[0] = player;
	args[1] = wantedLevel;
	args[2] = disableNoMission;
	invokeNative(0x39FF19C64EF7DA5B);
}

Inline void SET_PLAYER_WANTED_LEVEL_NOW(Player player, BOOL p1) {
	args[0] = player;
	args[1] = p1;
	invokeNative(0xE0A7D1E497FFCD6F);
}

Inline void SET_PED_INFINITE_AMMO_CLIP(Ped ped, BOOL toggle) {
	args[0] = ped;
	args[1] = toggle;
	invokeNative(0x183DADC6AA953186);
}

Inline void GIVE_WEAPON_TO_PED(Ped ped, Hash weaponHash, int ammoCount, int isHidden, BOOL equipNow) {
	args[0] = ped;
	args[1] = weaponHash;
	args[2] = ammoCount;
	args[3] = isHidden;
	args[4] = equipNow;
	invokeNative(0xBF0FD6E56C964FCB);
}