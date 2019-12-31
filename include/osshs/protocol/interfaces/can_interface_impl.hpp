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

#ifndef OSSHS_PROTOCOL_CAN_INTERFACE_HPP
	#error "Don't include this file directly, use 'can_interface.hpp' instead!"
#endif

#include <modm/platform.hpp>
#include <osshs/resource_lock.hpp>
#include <osshs/interfaces/interface_manager.hpp>
#include <osshs/log/logger.hpp>

namespace osshs
{
	namespace protocol
	{
		namespace interfaces
		{
				template<typename Can>
				bool
				CanInterface<Can>::run()
				{
					PT_BEGIN();

					do
					{
						PT_WAIT_UNTIL(Can::isMessageAvailable() | !eventPacketQueue.empty());

						if (Can::isMessageAvailable())
						{
							PT_CALL(readEventPacket());
						}
						else
						{
							currentEventPacket = eventPacketQueue.front();
							eventPacketQueue.pop();

							PT_CALL(writeEventPacket(currentEventPacket));

							currentEventPacket.reset();
						}

						PT_YIELD();
					}
					while (true);

					PT_END();
				}

				template<typename Can>
				void
				CanInterface<Can>::initialize()
				{
					OSSHS_LOG_INFO("Initializing CAN interface.");

					modm::platform::CanFilter::setFilter(
						0,
						modm::platform::CanFilter::FIFO0,
						modm::platform::CanFilter::ExtendedIdentifier(0),
						modm::platform::CanFilter::ExtendedFilterMask(0)
					);
				}

				template<typename Can>
				uint32_t
				CanInterface<Can>::generateCurrentFrameIdentifier()
				{
					constexpr uint32_t temp_mac = 0x00000000;
					uint32_t identifier = temp_mac & 0xffff;

					identifier |= (0b1 << 28);
					identifier |= currentFrameId ? (0b0 << 27) : (0b1 << 27);
					identifier |= (currentFrameCount > 1) ? (0b1 << 26) : (0b0 << 26);
					identifier |= (0b0 << 25);
					identifier |= (0b0 << 24);
					identifier |= (0b0 << 23);
					identifier |= (0b0 << 22);
					identifier |= (0b0 << 21);
					identifier |= (0b0 << 20);

					identifier |= (((currentFrameId ? currentFrameId : currentFrameCount) & 0xf00) << 8);

					return identifier;
			}

			template<typename Can>
			modm::ResumableResult<void>
			CanInterface<Can>::readEventPacket()
			{
				RF_BEGIN();

				OSSHS_LOG_DEBUG("Reading event packet.");

				RF_WAIT_UNTIL(ResourceLock<Can>::tryLock());

				{
					modm::can::Message frame;
					if (Can::getMessage(frame))
					{
						if (frame.getIdentifier() & (0b1 << 26))
						{
							uint16_t frameCount = (frame.getIdentifier() >> 8) & 0xf00;
							frameCount |= frame.data[0];

							uint16_t bufferLength = frame.data[1] | (frame.data[2] << 8);
							uint8_t *buffer = new (std::nothrow) uint8_t[bufferLength];

							if (buffer == nullptr)
							{
								OSSHS_LOG_ERROR_STREAM << "Failed to allocate memory for a buffer"
									<< "(buffer_length = " << bufferLength
									<< ").\r\n";

								RF_RETURN();
							}

							for (uint16_t frameId = 0; frameId < frameCount; frameId++)
							{
								if (frameId)
								{
									while (!Can::isMessageAvailable());
									Can::getMessage(frame);
								}

								std::copy(&frame.data[1], &frame.data[8], &buffer[frameId * 7]);
							}

							std::shared_ptr<EventPacket> eventPacket(new (std::nothrow) EventPacket(
								std::unique_ptr<const uint8_t[]>(buffer),
								&InterfaceManager::reportEvent
							));

							if (eventPacket == nullptr)
							{
								OSSHS_LOG_ERROR("Failed to allocate memory for an event packet.");
								RF_RETURN();
							}

							if (eventPacket->isMalformed())
							{
								OSSHS_LOG_WARNING("Discarding malformed event packet.");
								RF_RETURN();
							}

							InterfaceManager::reportEventPacket(eventPacket, this);
						}
						else
						{
							uint16_t bufferLength = frame.data[0] | (frame.data[1] << 8);
							uint8_t *buffer = new (std::nothrow) uint8_t[bufferLength];

							if (buffer == nullptr)
							{
								OSSHS_LOG_ERROR_STREAM << "Failed to allocate memory for a buffer"
									<< "(buffer_length = " << bufferLength
									<< ").\r\n";

								RF_RETURN();
							}

							std::copy(&frame.data[0], &frame.data[bufferLength], &buffer[0]);

							std::shared_ptr<EventPacket> eventPacket(new (std::nothrow) EventPacket(
								std::unique_ptr<const uint8_t[]>(buffer),
								&InterfaceManager::reportEvent
							));

							if (eventPacket == nullptr)
							{
								OSSHS_LOG_ERROR("Failed to allocate memory for an event packet.");
								RF_RETURN();
							}

							if (eventPacket->isMalformed())
							{
								OSSHS_LOG_WARNING("Discarding malformed event packet.");
								RF_RETURN();
							}
							
							InterfaceManager::reportEventPacket(eventPacket, this);
						}
					}
					else
					{
						OSSHS_LOG_WARNING("Could not read CAN frame.");
					}
				}

				ResourceLock<Can>::unlock();

				RF_END();
			}

			template<typename Can>
			modm::ResumableResult<void>
			CanInterface<Can>::writeEventPacket(std::shared_ptr<EventPacket> eventPacket)
			{
				RF_BEGIN();

				OSSHS_LOG_DEBUG_STREAM << "Writing event packet"
					<< "(multi_target = "<< eventPacket->isMultiTarget()
					<< ", command = " << eventPacket->isCommand()
					<< ", transmitter_mac = " << eventPacket->getTransmitterMac()
					<< ", receiver_mac = " << eventPacket->getTransmitterMac()
					<< ", event_type = " << eventPacket->getEvent()->getType()
					<< ").\r\n";

				RF_WAIT_UNTIL(ResourceLock<Can>::tryLock());

				currentBuffer = eventPacket->serialize();

				if (currentBuffer == nullptr)
				{
					OSSHS_LOG_WARNING("Failed to serialize event packet.");
					RF_RETURN();
				}

				currentBufferLength = currentBuffer[0] | (currentBuffer[1] << 8);
				if (!((currentBufferLength - 1) / 8))
				{
					currentFrameCount = 1;
				}
				else
				{
					currentFrameCount = 1 + ((currentBufferLength - 1) / 7);
				}
				currentFrameId = 0;

				if (currentFrameCount == 1)
				{
					modm::can::Message frame(generateCurrentFrameIdentifier(), currentBufferLength);
					frame.setExtended(true);

					std::copy(&currentBuffer[0], &currentBuffer[currentBufferLength], &frame.data[0]);

					Can::sendMessage(frame);
				}
				else
				{
					for (; currentFrameId < currentFrameCount; currentFrameId++)
					{
						uint16_t len = 8;
						if (currentFrameId == currentFrameCount - 1)
						{
							len = currentBufferLength - currentFrameId * 7 + 1;
						}

						modm::can::Message frame(generateCurrentFrameIdentifier(), len);
						frame.setExtended(true);
						frame.data[0] = currentFrameId ? currentFrameId & 0xff : currentFrameCount & 0xff;

						std::copy(&currentBuffer[0], &currentBuffer[currentFrameId * 7], &frame.data[1]);

						Can::sendMessage(frame);
					}
				}

				ResourceLock<Can>::unlock();

				RF_END();
			}
		}
	}
}
