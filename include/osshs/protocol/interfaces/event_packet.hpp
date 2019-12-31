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

#ifndef OSSHS_PROTOCOL_EVENT_PACKET_HPP
#define OSSHS_PROTOCOL_EVENT_PACKET_HPP

#include <memory>
#include <osshs/events/event.hpp>

namespace osshs
{
	namespace protocol
	{
		namespace interfaces
		{
			class EventPacket
			{
			public:
				static constexpr uint32_t NULL_MAC = static_cast<uint32_t>(-1);

				/**
				 * @brief Construct event packet from serialized data.
				 * @param data serialized event packet.
				 * @param callback callback for underlying event.
				 */
				EventPacket(std::unique_ptr<const uint8_t[]> data, events::EventCallback callback = nullptr);

				/**
				 * @brief Construct event packet.
				 * @param event underlying event.
				 * @param transmitterMac transmitter mac.
				 * @param receiverMac receiver mac or NULL_MAC if event packet is multi target.
				 * @param command whether or not this event packet is a command.
				 */
				EventPacket(std::shared_ptr<events::Event> event, uint32_t transmitterMac, uint32_t receiverMac = NULL_MAC, bool command = false)
					: multiTarget(receiverMac == NULL_MAC), command(command), transmitterMac(transmitterMac), receiverMac(receiverMac), event(event)
				{
				}

				/**
				 * @brief Multi target getter.
				 * @return Whether or not this event packet is multi target.
				 */
				bool
				isMultiTarget() const;

				/**
				 * @brief Command getter.
				 * @return Whether or not this event packet is a command.
				 */
				bool
				isCommand() const;

				/**
				 * @brief Transmitter mac getter.
				 * @return Transmitter mac.
				 */
				uint32_t
				getTransmitterMac() const;

				/**
				 * @brief Receiver mac getter.
				 * @return Receiver mac.
				 */
				uint32_t
				getReceiverMac() const;

				/**
				 * @brief Event getter.
				 * @return Event contained in this event packet.
				 */
				std::shared_ptr<events::Event>
				getEvent() const;

				/**
				 * @brief Check whether this event packet is malformed.
				 * @return Whether or not this event packet is malformed.
				 */
				bool
				isMalformed() const;

				/**
				 * @brief Serialize this event packet.
				 * @return Serialized event packet or nullptr if serialization failed.
				 */
				std::unique_ptr<const uint8_t[]>
				serialize() const;
			private:
				bool multiTarget;
				bool command;
				uint32_t transmitterMac;
				uint32_t receiverMac;
				std::shared_ptr<events::Event> event;
			};
		}
	}
}
#endif  // OSSHS_PROTOCOL_EVENT_PACKET_HPP
