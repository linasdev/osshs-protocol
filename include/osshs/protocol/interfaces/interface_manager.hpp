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

#ifndef OSSHS_PROTOCOL_INTERFACE_MANAGER_HPP
#define OSSHS_PROTOCOL_INTERFACE_MANAGER_HPP

#include <vector>
#include <memory>
#include <osshs/protocol/interfaces/interface.hpp>
#include <osshs/protocol/interfaces/event_packet.hpp>
#include <osshs/events/event.hpp>

namespace osshs
{
	namespace protocol
	{
		namespace interfaces
		{
			class InterfaceManager
			{
			public:
				/**
				 * @brief Initialize the interface manager.
				 */
				static void
				initialize();

				/**
				 * @brief Register an interface.
				 * @param interface interface to register.
				 */
				static void
				registerInterface(Interface *interface);

				/**
				 * @brief Report event packet. Should be called from within interfaces.
				 * @param eventPacket event packet to report.
				 * @param sourceInterface pointer to the source interface.
				 */
				static void
				reportEventPacket(std::shared_ptr<EventPacket> eventPacket, Interface *sourceInterface = nullptr);

				/**
				 * @brief Report event. Should be called from System.
				 * @param event event to report.
				 */
				static void
				reportEvent(std::shared_ptr<events::Event> event);

				/**
				 * @brief Update all registered interfaces.
				 */
				static void
				run();
			private:
				static std::vector<Interface*> interfaces;
			};
		}
	}
}

#endif  // OSSHS_PROTOCOL_INTERFACE_MANAGER_HPP
