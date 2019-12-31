/*
 * MIT License
 *
 * Copyright (c) 2019 Linas Nikiperavicius
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <osshs/protocol/interfaces/interface.hpp>
#include <osshs/log/logger.hpp>

namespace osshs
{
	namespace protocol
	{
		namespace interfaces
		{
			void
			Interface::reportEventPacket(std::shared_ptr<EventPacket> eventPacket)
			{
				OSSHS_LOG_DEBUG_STREAM << "Handling event packet"
					<< "(multi_target = "<< eventPacket->isMultiTarget()
					<< ", command = " << eventPacket->isCommand()
					<< ", transmitter_mac = " << eventPacket->getTransmitterMac()
					<< ", receiver_mac = " << eventPacket->getTransmitterMac()
					<< ", event_type = " << eventPacket->getEvent()->getType()
					<< ").\r\n";

				eventPacketQueue.push(eventPacket);
			}
		}
	}
}
