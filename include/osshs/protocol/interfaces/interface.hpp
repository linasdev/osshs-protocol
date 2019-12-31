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

#ifndef OSSHS_PROTOCOL_INTERFACE_HPP
#define OSSHS_PROTOCOL_INTERFACE_HPP

#include <queue>
#include <modm/processing/protothread.hpp>
#include <osshs/protocol/interfaces/event_packet.hpp>

namespace osshs
{
	namespace protocol
	{
		namespace interfaces
		{
			class InterfaceManager;

			class Interface : public modm::pt::Protothread
			{
			public:
				Interface() = default;
			protected:
				std::queue<std::shared_ptr<EventPacket>> eventPacketQueue;

				/**
				 * @brief Run interface protothread.
				 */
				virtual bool
				run() = 0;

				/**
				 * @brief Report an event packet to be transmitted.
				 * @param eventPacket event packet to transmit.
				 */
				void
				reportEventPacket(std::shared_ptr<EventPacket> eventPacket);
			private:
				Interface(const Interface&) = delete;

				Interface&
				operator=(const Interface&) = delete;

				/**
				 * @brief Initialize the interface. Should only be called from InterfaceManager.
				 */
				virtual void
				initialize() = 0;

				friend InterfaceManager;
			};
		}
	}
}

#endif  // OSSHS_PROTOCOL_INTERFACE_HPP
