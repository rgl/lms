/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (C) 2018-2020 Intel Corporation
 */
#include "global.h"
#include "WlanDefs.h"
#include "WlanBL.h"
#include "Tools.h"
#include <string>
#include <map>
const std::string IntelInstanceIDUser = "Intel(r) AMT:WiFi Endpoint User Settings ";

typedef std::map<std::wstring, int> str_int_map_t;

wlanps::authenticationSet_t supportedAuthentication = { L"open", L"WPAPSK", L"WPA2PSK", L"WPA3SAE", L"OWE" };
wlanps::encriptionSet_t supportedEncription = { L"WEP", L"TKIP", L"AES", L"none" };

// User Profiles DB maximum size
#define MAX_USER_PROFILES 16

wlanps::WlanBL::WlanBL() : m_osProfiles()
{
	int index;
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]\n");

	// Set m_wlanOsProfiles to nullptr pointers
	for (index = 0; index < MAX_OS_USER_PROFILES; index++)
	{
		m_wlanOsProfiles[index] = nullptr;
	}

	m_numOsUserProfiles = 0;
}

wlanps::WlanBL::~WlanBL()
{
	int index;

	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]\n");

	for (index = 0; index < MAX_OS_USER_PROFILES; index++)
	{
		if (m_wlanOsProfiles[index] != nullptr)
		{
			UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": Free the allocated memory of Profile [%W] index %d\n",
				m_wlanOsProfiles[index]->profile, index);

			free(m_wlanOsProfiles[index]);
			m_wlanOsProfiles[index] = nullptr;
		}
		else
		{
			UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": index %d has no Profile\n", index);
		}
	}
}

wlanps::WlanBL& wlanps::WlanBL::getInstance()
{
	static WlanBL wlanBL;
	return wlanBL;
}

void wlanps::WlanBL::CleanOsProfileList()
{
	int index;

	for (index = 0; index < MAX_OS_USER_PROFILES; index++)
	{
		if (m_wlanOsProfiles[index] != nullptr)
		{
			UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": Free the allocated memory of Profile [%W] index %d\n",
				m_wlanOsProfiles[index]->profile, index);

			free(m_wlanOsProfiles[index]);
			m_wlanOsProfiles[index] = nullptr;
		}
		else
		{
			UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": index %d has no Profile\n", index);
		}
	}
}

bool wlanps::WlanBL::Init(HANDLE hwlan)
{
	std::lock_guard<std::mutex> lock(_updateMutex);

	bool ret = m_osProfiles.Init(hwlan);
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: m_osProfiles.Init ret = %d\n", ret);

	if (ret)
	{
		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: Get Initial Porfile List form the OS\n");
		ret = FetchOsProfiles();
	}

	return ret;
}

bool wlanps::WlanBL::FetchOsProfiles()
{
	bool ret;

	ret = m_osProfiles.GetProfiles(m_wlanOsProfiles, &m_numOsUserProfiles, supportedAuthentication, supportedEncription);
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: %d \n", m_numOsUserProfiles);

	if (ret)
	{
		PrintInternalOsUserProfileList();
	}
	return ret;
}

void wlanps::WlanBL::PrintInternalOsUserProfileList()
{
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: ------- GetProfiles: - OS Profiles List Start (length [%02d]) ------\n",
		m_numOsUserProfiles);
	for (int i = 0; i < m_numOsUserProfiles; i++)
	{
		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]:[%02d] Profile= %-25W SSID=  %-25W aut = %-10sW, enc= %W\n",
			i, m_wlanOsProfiles[i]->profile, m_wlanOsProfiles[i]->SSID, m_wlanOsProfiles[i]->auth, m_wlanOsProfiles[i]->encr);
	}
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: ------- GetProfiles - OS Profiles List End (length [%02d]) ------\n",
		m_numOsUserProfiles);
}

void wlanps::WlanBL::SyncProfiles()
{
	std::lock_guard<std::mutex> lock(_updateMutex);

	bool wsmanStatus;
	MeProfileList MeProfileList;
	WlanWSManClient wsmanClient;

	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: Enumerating ME (CSME FW) Profiles, Wait for it...\n");
	wsmanStatus = wsmanClient.Enumerate(MeProfileList);

	if (!wsmanStatus)
	{
		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: Enumerate failed -> Stop sync operation\n");
		return;
	}

	int numMeProfiles = MeProfileList.size();
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: ------- ME Profiles List Start (length [%02d]) ------\n", numMeProfiles);

	for (int i = 0; i < numMeProfiles; i++)
	{
		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: [%02d] Profile  = %C \n", i, MeProfileList[i]->ElementName().c_str());
	}
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: -------       ME Profiles List End            ------\n");

	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: Delete Profiles which are not in the top16 use profiles \n");
	wsmanStatus = CleanupProfilesInMe(wsmanClient, MeProfileList);
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: CleanupProfilesInMe completed %C\n", wsmanStatus == true ? "successfully" : "with failure");

	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: Add Missing profiles \n");
	wsmanStatus = AddMissingProfilesToMe(wsmanClient, MeProfileList);
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: AddMissingProfilesToMe completed %C\n", wsmanStatus == true ? "successfully" : "with failure");
}

bool wlanps::WlanBL::CleanupProfilesInMe(WlanWSManClient &wsmanClient, MeProfileList &MeProfileList)
{
	int numOsProfiles;
	int numMEPRofiles;
	int meProfilesIndex;
	int osProfilesIndex;
	bool bFoundMatch;
	bool status;

	try
	{
		numOsProfiles = m_numOsUserProfiles > MAX_USER_PROFILES ? MAX_USER_PROFILES : m_numOsUserProfiles;
		numMEPRofiles = MeProfileList.size();

		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: numOsProfiles %d, numMEPRofiles %d\n",  numOsProfiles, numMEPRofiles);

		//---- Go over ME profile list in loop. Any profile that doesn't exist in the 1st 16 profiles from the OS - delete
		for (meProfilesIndex = 0; meProfilesIndex < numMEPRofiles; meProfilesIndex++)
		{
			bFoundMatch = false;
			for (osProfilesIndex = 0; osProfilesIndex < numOsProfiles; osProfilesIndex++)
			{

				std::string currentOsProfile = WStringToString(m_wlanOsProfiles[osProfilesIndex]->profile);

				if (MeProfileList[meProfilesIndex]->ElementName().compare(currentOsProfile) == 0)
				{
					UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: [%02d] %-25C  Profile matching with one of top 16 OS User Profile at osIndex [%d] \n",
						meProfilesIndex, currentOsProfile.c_str(), osProfilesIndex);
					bFoundMatch = true;
					break;
				}
			}

			if (bFoundMatch == false)
			{
				//If I got here, it means I didn't find a match in the OS list for this profile
				std::string name = MeProfileList[meProfilesIndex]->ElementName();
				UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: [%02d] %-25C  Profile is not at the top 16 - Deleting Profile From ME !\n",
					meProfilesIndex, name.c_str());
				status = wsmanClient.DeleteProfile(*MeProfileList[meProfilesIndex]);
				UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: [%02d] %-25C  DeleteProfile completed %C\n",
					meProfilesIndex, name.c_str(), status == true ? "successfully" : "with Exception");
			}
		}
	}
	catch (std::exception* e)
	{
		UNS_ERROR(L"[ProfileSync] " __FUNCTIONW__"[%03l]:  Exception %C\n", e->what());
		return false;
	}

	return true;
}

bool wlanps::WlanBL::AddMissingProfilesToMe(WlanWSManClient &wsmanClient, MeProfileList &MeProfileList)
{
	bool ret = true;
	int osProfilesIndex;
	int numOsProfiles;
	int numMEPRofiles;
	int meProfilesIndex;
	bool isThisAMissingProfile;
	bool wsmanStatus;

	try
	{
		numOsProfiles = m_numOsUserProfiles > MAX_USER_PROFILES ? MAX_USER_PROFILES : m_numOsUserProfiles;
		numMEPRofiles = MeProfileList.size();
		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: numOsProfiles %d, numMEPRofiles %d\n", numOsProfiles, numMEPRofiles);

		// Go over the top 16 profiles in the OS profile list. Any profile that doesn't exist in the ME list - Add it
		for (osProfilesIndex = 0; osProfilesIndex < numOsProfiles; osProfilesIndex++)
		{
			isThisAMissingProfile = true;
			std::string currentOsProfile = WStringToString(m_wlanOsProfiles[osProfilesIndex]->profile);

			numMEPRofiles = MeProfileList.size();
			for (meProfilesIndex = 0; meProfilesIndex < numMEPRofiles; meProfilesIndex++)
			{
				if (MeProfileList[meProfilesIndex]->ElementName().compare(currentOsProfile) == 0)
				{
					UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: [%02d] %-25C OS Profile matches existing ME Profile at meProfilesIndex [%d] \n",
						osProfilesIndex, currentOsProfile.c_str(), meProfilesIndex);
					isThisAMissingProfile = false;
					break;
				}
			}

			if (isThisAMissingProfile == true)
			{
				//If I got here, it means I didn't find a match in the OS list for this profile
				UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: [%02d] %-25C OS Profile has no match - Add Profile To ME !\n",
					osProfilesIndex, currentOsProfile.c_str());
				using namespace Intel::Manageability::Cim::Typed;
				CIM_WiFiEndpointSettings			wifiSettings = { 0 };
				PINTEL_PROFILE_DATA	pIntelProfileData = m_wlanOsProfiles[osProfilesIndex];
				ret = trans2CIM(pIntelProfileData, wifiSettings);
				if (ret)
				{
					wsmanStatus = wsmanClient.AddProfile(wifiSettings);
					UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: [%02d] %-25C AddProfile completed %C [ret = %d]\n",
						osProfilesIndex, currentOsProfile.c_str(), wsmanStatus == true ? "successfully" : "with Error", ret);
				}
				else
				{
					UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: trans2CIM failed -> Don't add this profile\n");
				}
			}
		}
	}
	catch (std::exception* e)
	{
		UNS_ERROR(L"[ProfileSync] " __FUNCTIONW__"[%03l]:  Exception %C\n", e->what());
		return false;
	}

	return ret;
}

void wlanps::WlanBL::onConnectionComplete(PINTEL_PROFILE_DATA profileData)
{
	std::lock_guard<std::mutex> lock(_updateMutex);
	bool retVal;
	bool bFound = false;
	bool bFoundMatch = false;
	bool wsmanStatus;
	unsigned long profileFlags = 0;
	int numMEPRofiles;
	int numOsProfiles;
	int meProfilesIndex;
	int osProfilesIndex;
	WlanWSManClient wsmanClient;
	MeProfileList MeProfileList;


	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: Got connected to profile: [%W]\n", profileData->profile);

	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": Calling m_osProfiles.GetProfileData...\n");
	retVal = m_osProfiles.GetProfileData(profileData, &profileFlags);

	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": Calling m_osProfiles.GetProfileData returned %d\n", retVal);
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": Profile details: SSID = %W aut = %W, enc = %W profileFlags 0x%08X\n",
		profileData->SSID, profileData->auth, profileData->encr, profileFlags);

	if (!retVal)
	{
		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": GetProfileData failed! Stop add Profile process\n");
		return;
	}

	// Check if this profile is an IT (Group Policy) profile
	if (profileFlags == WLAN_PROFILE_GROUP_POLICY)
	{
		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": This is an IT Profile --> Do nothing\n");
		return;
	}

	Intel::Manageability::Cim::Typed::CIM_WiFiEndpointSettings wifiSettings;
	retVal = trans2CIM(profileData, wifiSettings);
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": trans2CIM returned %d\n", retVal);

	if (!retVal)
	{
		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: trans2CIM failed -> Don't add this profile\n");
		return;
	}

	// Check if the connected Profile exists in ME Profile List
	try
	{
		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: Getting ME Profiles list\n");

		// Get Profile list from ME
		wsmanStatus = wsmanClient.Enumerate(MeProfileList);

		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: m_wsmanClient.Enumerate completed %C\n",
			wsmanStatus == true ? "successfully" : "with failure");

		if (!wsmanStatus)
		{
			UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": Enumerate failed -> Stop operation\n");

			return;
		}

		numMEPRofiles = MeProfileList.size();

		// Before getting the updated OS Profile list, need to clean the current list
		CleanOsProfileList();

		retVal = m_osProfiles.GetProfiles(m_wlanOsProfiles, &m_numOsUserProfiles, supportedAuthentication, supportedEncription);

		numOsProfiles = m_numOsUserProfiles > MAX_USER_PROFILES ? MAX_USER_PROFILES : m_numOsUserProfiles;

		UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: Num of ME profiles %d Profiles - Num of OS Profiles %d\n",
			numMEPRofiles, numOsProfiles);

		// Search the Profile in ME Profile list
		for (int i = 0; i < numMEPRofiles; i++)
		{
			UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: [%02d] m_MeProfileList->ElementName: [%C]\n",
				i, MeProfileList[i]->ElementName().c_str());

			if (MeProfileList[i]->ElementName().compare(wifiSettings.ElementName()) == 0)
			{
				bFound = true;
				break;
			}
		}

		if ((MAX_USER_PROFILES == numMEPRofiles) && (bFound == false))
		{
			// TODO FW DB is full, delete the appropriate user profile
			for (meProfilesIndex = 0; meProfilesIndex < numMEPRofiles; meProfilesIndex++)
			{
				bFoundMatch = false;
				UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__": Going to search ME Profile %C in OS profile list...\n",
					MeProfileList[meProfilesIndex]->ElementName().c_str());
				for (osProfilesIndex = 0; osProfilesIndex < numOsProfiles; osProfilesIndex++)
				{
					std::string currentOsProfile = WStringToString(m_wlanOsProfiles[osProfilesIndex]->profile);

					if (MeProfileList[meProfilesIndex]->ElementName().compare(currentOsProfile) == 0)
					{
						UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: [%02d] %-25C  Profile matching with one of top 16 OS User Profile at osIndex [%d] \n",
							meProfilesIndex, currentOsProfile.c_str(), osProfilesIndex);
						bFoundMatch = true;
						break;
					}
				}

				if (bFoundMatch == false)
				{
					std::string name = MeProfileList[meProfilesIndex]->ElementName();
					UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: [%02d] %-25C  Profile is not at the top 16 - Deleting Profile From ME!\n",
						meProfilesIndex, name.c_str());

					wsmanStatus = wsmanClient.DeleteProfile(*MeProfileList[meProfilesIndex]);

					UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: [%02d] %-25C  DeleteProfile completed\n",
						meProfilesIndex, name.c_str());
					break;
				}
			}
		}

		if (bFound == true)
		{
			// Profile exists in ME Profile list -> Update Profile
			wsmanStatus = wsmanClient.UpdateProfile(wifiSettings);
			UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: m_wsmanClient.UpdateProfile [%W] completed %C\n",
				profileData->profile, wsmanStatus == true ? "successfully" : "with Exception");
		}
		else
		{
			// Profile not exists in ME Profile list -> Add Profile
			wsmanStatus = wsmanClient.AddProfile(wifiSettings);
			UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: m_wsmanClient.AddProfile [%W] completed %C\n",
				profileData->profile, wsmanStatus == true ? "successfully" : "with Exception");
		}
	}
	catch (std::exception* e)
	{
		UNS_ERROR(L"[ProfileSync] " __FUNCTIONW__"[%03l]: Exception %C\n", e->what());
		return;
	}
}

bool wlanps::WlanBL::trans2CIM(PINTEL_PROFILE_DATA profileData, SingleMeProfile& wifiSettings)
{
	static const unsigned short TKIPPriority = 4;
	static const unsigned short Priority802_11x_AES	= 6;
	static const unsigned short WEPPriority = 7;
	static const unsigned short NoneEncPriority = 8;

	int auth = 1;
	int encr = 1;
	int priority = 8;

	// translate to CIM_WiFiEndpointSettings
	static const str_int_map_t authMap = {
		{ L"open",    AuthenticationMethodOpenSystem },
		{ L"WPAPSK",  AuthenticationMethodWPAPSK },
		{ L"WPA2PSK", AuthenticationMethodWPA2PSK },
		{ L"WPA3SAE", AuthenticationMethodWPA3SAE },
		{ L"OWE", AuthenticationMethodWPA3OWE }
	};
	static const str_int_map_t encrMap = {
		{ L"WEP",  EncryptionMethodWEP },
		{ L"TKIP", EncryptionMethodTKIP },
		{ L"AES",  EncryptionMethodCCMP }, // CCMP
		{ L"none", EncryptionMethodNone }
	};

	static const str_int_map_t priorityMap = {
		{ L"WEP",  WEPPriority },
		{ L"TKIP", TKIPPriority },
		{ L"AES",  Priority802_11x_AES }, // CCMP
		{ L"none", NoneEncPriority }
	};

	wifiSettings.ElementName(WStringToString(profileData->profile));
	wifiSettings.SSID(WStringToString(profileData->SSID));

	try
	{
		auth = authMap.at(profileData->auth);
		encr = encrMap.at(profileData->encr);
		priority = priorityMap.at(profileData->encr);
	}
	catch (...)
	{
		UNS_ERROR(L"[ProfileSync] Fix the auth or encr map\n");
		return false;
	}

	wifiSettings.Priority(priority);
	wifiSettings.AuthenticationMethod(auth);
	wifiSettings.EncryptionMethod(encr);

	// PSKPassPhrase should be NULL if AuthenticationMethod does not contain
	// 4 ("WPA PSK") or 6 ("WPA2 PSK") or 32768 ("WPA3 SAE").
	if ((auth == AuthenticationMethodWPAPSK || auth == AuthenticationMethodWPA2PSK || auth == AuthenticationMethodWPA3SAE) &&
		encr != EncryptionMethodNone)
	{
		wifiSettings.PSKPassPhrase(WStringToString(profileData->keyMaterial));
	}

	PrintWifiSetting(auth, encr, priority, profileData->profile, profileData->SSID);

	return true;
}

void wlanps::WlanBL::PrintWifiSetting(int auth, int enc, int prio, wchar_t* elementName, wchar_t* ssid)
{
	UNS_DEBUG(L"[ProfileSync] " __FUNCTIONW__"[%03l]: wifiSettings Profile=%-25W, ssid=%-25W prio=%d auth=%d enc=%d\n",
		elementName, ssid, prio, auth, enc);
}
