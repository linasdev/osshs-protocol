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

#ifndef OSSHS_PROTOCOL_CAN_INTERFACE_CONTROLLER_HPP
	#error "Don't include this file directly, use 'can_interface_controller.hpp' instead!"
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
			namespace can
			{
				template<typename CAN>
				void
				CanInterfaceController<CAN>::initialize()
				{
					modm::platform::CanFilter::setFilter(
						0,
						modm::platform::CanFilter::FIFO0,
						modm::platform::CanFilter::ExtendedIdentifier(0),
						modm::platform::CanFilter::ExtendedFilterMask(0)
					);

					OSSHS_LOG_INFO("Initializing CAN interface controller succeeded.");
				}

				template<typename CAN>
				void
				CanInterfaceController<CAN>::transmitFrame(std::unique_ptr<CanFrame> frame)
				{
					outgoingFrames.push(frame);
				}

				template<typename CAN>
				bool
				CanInterfaceController<CAN>::run()
				{
					PT_BEGIN();

					do
					{
						PT_WAIT_UNTIL(CAN::isMessageAvailable() | !outgoingFrames.empty());

						if (CAN::isMessageAvailable())
						{
							PT_CALL(readFrame());
						}
						else
						{
							PT_CALL(writeFrame());
						}

						PT_YIELD();
					}
					while (true);

					PT_END();
				}

				template<typename CAN>
				modm::ResumableResult<void>
				CanInterfaceController<CAN>::readFrame()
				{
					RF_BEGIN();

					RF_WAIT_UNTIL(ResourceLock<CAN>::tryLock());

					{
						modm::can::Message frame;
						if (CAN::getMessage(frame) && frameReceivedCallback != nullptr)
						{
								frameReceivedCallback(std::unique_ptr<CanFrame>(new CanFrame(frame)));
						}
						else
						{
							OSSHS_LOG_WARNING("Reading CAN frame failed.");
						}
					}

					ResourceLock<CAN>::unlock();

					RF_END();
				}

				template<typename CAN>
				modm::ResumableResult<void>
				CanInterfaceController<CAN>::writeFrame()
				{
					RF_BEGIN();

					RF_WAIT_UNTIL(ResourceLock<CAN>::tryLock());

					CAN::sendMessage(outgoingFrames.front());
					outgoingFrames.pop();

					ResourceLock<CAN>::unlock();

					RF_END();
				}
			}
		}
	}
}
