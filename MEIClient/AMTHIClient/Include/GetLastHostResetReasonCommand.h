/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (C) 2010-2019 Intel Corporation
 */
/*++

@file: GetLastHostResetReasonCommand.h

--*/

#ifndef __GET_LAST_HOST_RESET_REASON_COMMAND_H__
#define __GET_LAST_HOST_RESET_REASON_COMMAND_H__

#include "AMTHICommand.h"
#include "MEIparser.h"

namespace Intel
{
	namespace MEI_Client
	{
		namespace AMTHI_Client
		{
			typedef struct LAST_HOST_RESET_REASON_RESPONSE_t
			{
				uint32_t Reason;
				uint32_t RemoteControlTimeStamp;
				
				void parse (std::vector<uint8_t>::const_iterator& itr, const std::vector<uint8_t>::const_iterator end)
				{
					Intel::MEI_Client::parseData(*this,itr,end);
				}
			} LAST_HOST_RESET_REASON_RESPONSE;

			class GetLastHostResetReasonRequest;
			class GetLastHostResetReasonResponse;
			class GetLastHostResetReasonCommand : public AMTHICommand
			{
			public:

				GetLastHostResetReasonCommand();
				virtual ~GetLastHostResetReasonCommand() {}

				LAST_HOST_RESET_REASON_RESPONSE getResponse();

			private:
				virtual void parseResponse(const std::vector<uint8_t>& buffer);

				std::shared_ptr<AMTHICommandResponse<LAST_HOST_RESET_REASON_RESPONSE>> m_response;

				static const uint32_t RESPONSE_COMMAND_NUMBER = 0x0480004A;
			};

			class GetLastHostResetReasonRequest : public AMTHICommandRequest
			{
			public:
				GetLastHostResetReasonRequest() {}
				virtual ~GetLastHostResetReasonRequest() {}

			private:
				static const uint32_t REQUEST_COMMAND_NUMBER = 0x0400004A;
				virtual unsigned int requestHeaderCommandNumber()
				{
					//this is the command number (taken from the AMTHI document)
					return REQUEST_COMMAND_NUMBER;
				}

				virtual uint32_t requestDataSize()
				{
					return 0;
				}
				virtual std::vector<uint8_t> SerializeData();
			};
		} // namespace AMTHI_Client
	} // namespace MEI_Client
} // namespace Intel

#endif //__GET_LAST_HOST_RESET_REASON_COMMAND_H__