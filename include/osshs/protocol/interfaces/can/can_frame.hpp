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

#include <modm/platform.hpp>
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
					 * @param data Up to 8 bytes of data (7 bytes if lastFrameId > 0).
					 * @param dataLen Length of data.
					 * @param transmitterMac Transmitter device MAC address.
					 * @param lastFrameId Last frame id inside current packet.
					 * @param frameId Current frame id.
					 * @param error Whether or not this is an error frame.
					 */
					CanFrame(const uint8_t *data, uint8_t dataLen, uint16_t transmitterMac,
						uint16_t lastFrameId = 0, uint16_t frameId = 0, bool error = false);

					/**
					 * @brief Construct CAN frame from a modm CAN message.
					 * @param message Modm CAN message.
					 */
					CanFrame(const modm::can::Message &message);

					/**
					 * @brief Convert frame to a modm CAN message.
					 * @return Pointer to converted message.
					 */
					std::unique_ptr<modm::can::Message>
					getMessage();

					/**
					 * @brief Get pointer to data.
					 * @return Pointer to data.
					 */
					const uint8_t *
					getData();

					/**
					 * @brief Get data length.
					 * @return Data length.
					 */
					uint8_t
					getDataLen();

					/**
					 * @brief Get extended CAN frame identifier.
					 * @return Extended CAN frame identifier.
					 */
					uint32_t
					getExtendedIdentifier();

					/**
					 * @brief Get transmitter device MAC address.
					 * @return Transmitter device MAC address.
					 */
					uint16_t
					getTransmitterMac();

					/**
					 * @brief Get current or last frame id.
					 * @note Returns last frame id if current frame id is zero, otherwise returns current frame id.
					 * @return Current or last frame id.
					 */
					uint16_t
					getFrameId();

					/**
					 * @brief Check if this frame is an error frame.
					 * @return Whether or not this frame is an error frame.
					 */
					bool
					isError();

					/**
					 * @brief Check if this frame is part of a multi frame packet.
					 * @return Whether or not this frame is part of a multi frame packet.
					 */
					bool
					isMultiFrame();
				private:
					std::unique_ptr<const uint8_t[]> data;
					uint8_t dataLen;
					uint16_t extendedIdentifier;
				};
			}
		}
	}
}

#endif  // OSSHS_PROTOCOL_CAN_FRAME_HPP
