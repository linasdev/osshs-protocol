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

#ifndef OSSHS_PROTOCOL_UART_INTERFACE_HPP
	#error "Don't include this file directly, use 'uart_interface.hpp' instead!"
#endif

#include <modm/platform.hpp>
#include <osshs/resource_lock.hpp>
#include <osshs/log/logger.hpp>

namespace osshs
{
	namespace protocol
	{
		namespace interfaces
		{
			template<typename Uart>
			bool
			UartInterface<Uart>::run()
			{
				PT_BEGIN();

				do
				{
					PT_WAIT_WHILE(eventPacketQueue.empty());

					currentEventPacket = eventPacketQueue.front();
					eventPacketQueue.pop();

					PT_CALL(writeEventPacket(currentEventPacket));

					currentEventPacket.reset();

					PT_YIELD();
				}
				while (true);

				PT_END();
			}

			template<typename Uart>
			void
			UartInterface<Uart>::initialize()
			{
				OSSHS_LOG_INFO("Initializing UART interface.");
			}

			template<typename Uart>
			modm::ResumableResult<void>
			UartInterface<Uart>::writeEventPacket(std::shared_ptr<EventPacket> eventPacket)
			{
				RF_BEGIN();

				OSSHS_LOG_DEBUG_STREAM << "Writing event packet"
					<< "(multi_target = "<< eventPacket->isMultiTarget()
					<< ", command = " << eventPacket->isCommand()
					<< ", transmitter_mac = " << eventPacket->getTransmitterMac()
					<< ", receiver_mac = " << eventPacket->getTransmitterMac()
					<< ", event_type = " << eventPacket->getEvent()->getType()
					<< ").\r\n";

				RF_WAIT_UNTIL(ResourceLock<Uart>::tryLock());

				{
					std::unique_ptr<const uint8_t[]> buffer = eventPacket->serialize();

					if (buffer == nullptr)
					{
						OSSHS_LOG_WARNING("Failed to serialize event packet.");
						RF_RETURN();
					}

					uint16_t bufferLength = buffer[0] | (buffer[1] << 8);

					Uart::writeBlocking(buffer.get(), bufferLength);
				}

				ResourceLock<Uart>::unlock();

				RF_END();
			}
		}
	}
}
