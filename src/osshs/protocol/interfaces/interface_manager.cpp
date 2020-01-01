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

#include <osshs/protocol/interfaces/interface_manager.hpp>
#include <osshs/system.hpp>
#include <osshs/log/logger.hpp>

namespace osshs
{
	namespace protocol
	{
		namespace interfaces
		{
			std::vector<Interface*> InterfaceManager::interfaces;

			void
			InterfaceManager::initialize()
			{
				OSSHS_LOG_INFO("Initializing interface manager.");
			}

			void
			InterfaceManager::registerInterface(Interface *interface)
			{
				OSSHS_LOG_INFO("Registering interface.");

				interfaces.push_back(interface);
				interface->initialize();
			}

			void
			InterfaceManager::reportEventPacket(std::shared_ptr<EventPacket> eventPacket, Interface *sourceInterface)
			{
				OSSHS_LOG_DEBUG(
					"Handling event packet(multiTarget = %u, command = %u, transmitterMac = 0x%08x, receiverMac = 0x%08x, eventType = 0x%04x).",
					eventPacket->isMultiTarget(),
					eventPacket->isCommand(),
					eventPacket->getTransmitterMac(),
					eventPacket->getReceiverMac(),
					eventPacket->getEvent()->getType()
				);

				if (eventPacket->isMalformed())
				{
					OSSHS_LOG_WARNING("Discarding malformed event packet.");
					return;
				}

				for (Interface *interface : interfaces)
				{
					if (interface == sourceInterface)
						continue;
						
					interface->reportEventPacket(eventPacket);
				}

				System::reportEvent(eventPacket->getEvent());
			}

			void
			InterfaceManager::reportEvent(std::shared_ptr<events::Event> event)
			{
				OSSHS_LOG_DEBUG("Handling event(type = 0x%04x).", event->getType());

				std::shared_ptr<EventPacket> eventPacket(new (std::nothrow) EventPacket(
					event,
					0x00000000
				));

				if (eventPacket == nullptr)
				{
					OSSHS_LOG_ERROR("Failed to allocate memory for an event packet.");
					return;
				}

				if (eventPacket->isMalformed())
				{
					OSSHS_LOG_WARNING("Discarding malformed event packet.");
					return;
				}

				for (Interface *interface : interfaces)
				{
					interface->reportEventPacket(eventPacket);
				}
			}

			void
			InterfaceManager::run()
			{
				for (Interface *interface : interfaces)
				{
					interface->run();
				}
			}
		}
	}
}
