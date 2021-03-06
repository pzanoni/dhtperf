/*
 * dhtperf: DHT performance evaluation tool
 * Copyright (C) 2011 Paulo Zanoni <przanoni@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DHTPERF_SERVICE_PROTOCOL_H
#define DHTPERF_SERVICE_PROTOCOL_H

#define DEFAULT_PORT 1811

#define MAX_MSG_SIZE 255
#define MSG_PREFIX "dhtperf"
#define MSG_PING    MSG_PREFIX "ping"
#define MSG_START   MSG_PREFIX "start"
#define MSG_STOP    MSG_PREFIX "stop"
#define MSG_ACK     MSG_PREFIX "ack"
#define MSG_UPDATE  MSG_PREFIX "update"
#define MSG_VERSION MSG_PREFIX "version"

#endif
