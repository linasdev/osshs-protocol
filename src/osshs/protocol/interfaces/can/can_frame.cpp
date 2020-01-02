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

#include <osshs/protocol/interfaces/can/can_frame.hpp>

namespace osshs
{
	namespace protocol
	{
		namespace interfaces
		{
			namespace can
			{
				CanFrame::CanFrame(const uint8_t *data, uint8_t dataLen, uint16_t transmitterMac,
						uint16_t lastFrameId = 0, uint16_t frameId = 0, bool error = false)
				{
					if (lastFrameId == 0)
					{
						uint8_t *buffer = new uint8_t[dataLen];
						std::copy(&data[0], &data[dataLen], &buffer[0]);

						this->data = std::unique_ptr<const uint8_t[]>(buffer);
						this->dataLen = dataLen;
					}
					else
					{
						uint8_t *buffer = new uint8_t[dataLen + 1];
						buffer[0] = frameId > 0 ? frameId & 0x0ff : lastFrameId & 0x0ff;
						std::copy(&data[0], &data[dataLen], &buffer[1]);
						
						this->data = std::unique_ptr<const uint8_t[]>(buffer);
						this->dataLen = dataLen + 1;
					}

					extendedIdentifier = 0x00000000;
					extendedIdentifier |= (static_cast<uint8_t>(!error)) << 28; // NOT_ERROR_FLAG
					extendedIdentifier |= (static_cast<uint8_t>(frameId == 0)) << 27; // START_FRAME_FLAG
					extendedIdentifier |= (static_cast<uint8_t>(lastFrameId > 0)) << 26; // MULTI_FRAME_FLAG
					extendedIdentifier |= (frameId > 0 ? frameId & 0xf00 : lastFrameId & 0xf00) << 16; // FRAME_COUNT / FRAME_ID
					extendedIdentifier |= transmitterMac; // TRANSMITTER_MAC
				}

				CanFrame::CanFrame(const modm::can::Message &message)
				{
					data = std::unique_ptr<const uint8_t[]>(message.data);
					dataLen = message.getLength();
					extendedIdentifier = message.getIdentifier();
				}

				std::unique_ptr<modm::can::Message>
				CanFrame::getMessage()
				{
					std::unique_ptr message(new modm::can::Message(extendedIdentifier, dataLen);
					message->setExtended(true);
					std::copy(&data[0], &data[dataLen], &message->data[0]);
					
					return message;
				}

				const uint8_t *
				CanFrame::getData()
				{
					if (isMultiFrame())
						return &data[1];

					return &data[0];
				}

				uint8_t
				CanFrame::getDataLen()
				{
					if (isMultiFrame())
						return dataLen - 1;

					return dataLen;
				}

				uint32_t
				CanFrame::getExtendedIdentifier()
				{
					return extendedIdentifier;
				}

				uint16_t
				CanFrame::getTransmitterMac()
				{
					return extendedIdentifier & 0xffff;
				}

				uint16_t
				CanFrame::getFrameId()
				{
					return ((extendedIdentifier >> 8) & 0x0f00) | data[0];
				}

				bool
				CanFrame::isError()
				{
					return (extendedIdentifier >> 28) & 0b1 == 0;
				}

				bool
				CanFrame::isMultiFrame()
				{
					return (extendedIdentifier >> 26) & 0b1 == 1;
				}
			}
		}
	}
}
