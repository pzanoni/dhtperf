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

//package dhtperf.pastry

import rice.p2p.commonapi.Id;
//import rice.p2p.past.ContentHashPastContent;
import rice.p2p.past.*;

public class MyPastContent extends ContentHashPastContent {
    String content;
    public MyPastContent(Id id, String content) {
	super(id);
	this.content = content;
    }
    public String toString() {
	return content;
    }

    // This is a reimplementation of the original function of our parent class,
    // returning "null" instead of an exception in case the key is already
    // stored.
    public PastContent checkInsert(Id id, PastContent existingContent)
				   throws PastException {
	if (existingContent != null) {
	    return null;
	}

	if (!id.equals(getId())) {
	    throw new PastException("ContentHashPastContent:can't insert, " +
				    "content hash incorrect");
	}
	return this;
    }
}

