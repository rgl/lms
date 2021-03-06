/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (C) 2010-2019 Intel Corporation
 */
/*++

@file: CloseUserInitiatedConnectionCommand.cpp

--*/

#include "CloseUserInitiatedConnectionCommand.h"

namespace Intel {
	namespace MEI_Client {
		namespace AMTHI_Client {
			CloseUserInitiatedConnectionCommand::CloseUserInitiatedConnectionCommand()
			{
				std::shared_ptr<MEICommandRequest> tmp(new CloseUserInitiatedConnectionRequest());
				m_request = tmp;
				Transact();
			}

			void CloseUserInitiatedConnectionCommand::parseResponse(const std::vector<uint8_t>& buffer)
			{
				std::shared_ptr<AMTHICommandResponse<CloseUserInitiatedConnection_RESPONSE>> tmp(new AMTHICommandResponse<CloseUserInitiatedConnection_RESPONSE>(buffer, RESPONSE_COMMAND_NUMBER));
				m_response = tmp;
			}

			std::vector<uint8_t> CloseUserInitiatedConnectionRequest::SerializeData()
			{
				std::vector<uint8_t> output;
				return output;
			}
		} // namespace AMTHI_Client
	} // namespace MEI_Client
} // namespace Intel	