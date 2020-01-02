/*
 * MIT License
 *
 * Copyright (c) 2019-2020 Linas Nikiperavicius
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
#define OSSHS_PROTOCOL_CAN_INTERFACE_CONTROLLER_HPP

#include <osshs/protocol/interfaces/can/can_frame.hpp>
#include <osshs/protocol/interfaces/interface.hpp>

namespace osshs
{
	namespace protocol
	{
		namespace interfaces
		{
			namespace can
			{
				typedef std::function<void (std::unique_ptr<CanFrame>)> FrameReceivedCallback;

				template<typename CAN>
				class CanInterfaceController : private modm::NestedResumable<1>
				{
				public:
					CanInterfaceController(FrameReceivedCallback frameReceivedCallback = nullptr)
						: frameReceivedCallback(frameReceivedCallback)
					{
					}
					
					void
					initialize();

					void
					transmitFrame(std::unique_ptr<CanFrame> frame);

					bool
					run();
				private:
					FrameReceivedCallback frameReceivedCallback;
					std::queue<std::unique_ptr<CanFrame>> outgoingFrames;

					modm::ResumableResult<void>
					readFrame();

					modm::ResumableResult<void>
					writeFrame();
				};
			}
		}
	}
}

#include <osshs/protocol/interfaces/can/can_interface_controller_impl.hpp>

#endif  // OSSHS_PROTOCOL_CAN_INTERFACE_CONTROLLER_HPP
