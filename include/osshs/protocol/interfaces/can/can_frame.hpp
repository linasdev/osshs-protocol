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

#ifndef OSSHS_PROTOCOL_CAN_FRAME_HPP
#define OSSHS_PROTOCOL_CAN_FRAME_HPP

#include <memory>

namespace osshs
{
	namespace protocol
	{
		namespace interfaces
		{
			namespace can
			{
				class CanFrame
				{
				public:
					/**
					 * @brief Construct CAN frame from parameters and data.
					 * @param data Data.
					 * @param transmitterMac Transmitter device MAC address.
					 * @param frameCount Total number of frames in current packet.
					 * @param frameId Current frame id.
					 * @param error Whether or not this is an error frame.
					 */
					CanFrame(std::unique_ptr<const uint8_t[]> data, uint32_t transmitterMac, uint16_t frameCount = 1, uint16_t frameId = 0, bool error = false);

				private:
					uint32_t extendedIdentifier;
					std::unique_ptr<const uint8_t[]> data;
				};
			}
		}
	}
}

#endif  // OSSHS_PROTOCOL_CAN_FRAME_HPP
