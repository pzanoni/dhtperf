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

//package dhtperf.pastry;

import java.io.IOException;
import java.net.*;
import java.util.Scanner;

import rice.Continuation;
import rice.environment.Environment;
import rice.p2p.commonapi.Id;
import rice.p2p.commonapi.NodeHandle;
import rice.p2p.past.*;
import rice.pastry.*;
import rice.pastry.commonapi.PastryIdFactory;
import rice.pastry.socket.SocketPastryNodeFactory;
import rice.pastry.standard.RandomNodeIdFactory;
import rice.persistence.*;

public class PastConsole {

    Past app_;
    Environment env_;
    PastryIdFactory pastryIdFactory_;

    public PastConsole(int bindport, InetSocketAddress bootaddress,
		       final Environment env) throws Exception {
	NodeIdFactory nodeIdFactory = new RandomNodeIdFactory(env);
	PastryNodeFactory pastryNodeFactory = new SocketPastryNodeFactory(
						nodeIdFactory, bindport, env);
	PastryNode node = pastryNodeFactory.newNode();
	pastryIdFactory_ = new rice.pastry.commonapi.PastryIdFactory(env);
	Storage stor = new MemoryStorage(pastryIdFactory_);
	app_ = new PastImpl(node,
			    new StorageManagerImpl(pastryIdFactory_, stor,
			       new LRUCache(new MemoryStorage(pastryIdFactory_),
					    512 * 1024,
					    node.getEnvironment())),
			   0, "");
	env_ = env;
	node.boot(bootaddress);
	synchronized(node) {
	    do {
		node.wait(500);

		if (node.joinFailed()) {
		    throw new IOException(
			"Could not join the FreePastry ring. Reason: " +
			node.joinFailedReason());
		}
	    } while (!node.isReady());
	}
	System.out.println("Finished creating new node " + node);
    }


    class PutContinuation implements Continuation<Boolean[], Exception> {

	public boolean done;

	public void receiveResult(Boolean[] results) {
	    int successes = 0;
	    for(int i = 0; i < results.length; i++) {
		if (results[i].booleanValue()) {
		    successes++;
		}
	    }
	    System.out.println(successes + " keys stored successfully");
	    done = true;
	}

	public void receiveException(Exception result) {
	    System.out.println("Error storing key");
	    done = true;
	}

	public void waitDone() {
	    System.out.println("Waiting...");
	    while (!done) {
		try {
		    Thread.sleep(10);
		} catch (final java.lang.InterruptedException e) {
		    // do nothing
		}
	    }
	    System.out.println("... wait finished.");
	}
    };

    class GetContinuation implements Continuation<PastContent, Exception> {

	public boolean done;

	public void receiveResult(PastContent result) {
	    if (result == null)
		System.out.println("Not found");
	    else
		System.out.println("Value: " + result);
	    done = true;
	}

	public void receiveException(Exception result) {
	    System.out.println("Error storing key");
	    done = true;
	}

	public void waitDone() {
	    System.out.println("Waiting...");
	    while (!done) {
		try {
		    Thread.sleep(10);
		} catch (final java.lang.InterruptedException e) {
		    // do nothing
		}
	    }
	    System.out.println("... wait finished.");
	}
    };

    private void Put(String key, String value) {
	final PastContent myContent = new
	    MyPastContent(pastryIdFactory_.buildId(key), value);

	PutContinuation cont = new PutContinuation();
	cont.done = false;
	app_.insert(myContent, cont);
	cont.waitDone();
    }

    private void Get(String key) {
	final Id id = pastryIdFactory_.buildId(key);

	GetContinuation cont = new GetContinuation();
	cont.done = false;
	app_.lookup(id, cont);
	cont.waitDone();
    }

    private static String JoinStringFromIndex(String[] s, String separator,
					      int from) {
	String ret = "";
	for(int i = from; i < s.length; i++) {
	    if (i != from)
		ret += separator;
	    ret += s[i];
	}
	return ret;
    }

    public void Run() throws Exception {
	Scanner scanner = new Scanner(System.in);
	String line;
	String[] words;

	while(true) {
	    System.out.println("pc$");

	    try {
		line = scanner.nextLine();
	    } catch (final java.util.NoSuchElementException e) {
		System.out.println("Bye!");
		System.exit(0);
		break; // --> silence warning
	    }

	    words = line.split("[ ]+");
	    if (words[0].equals("put") && words.length >= 3) {
		String key = words[1];
		String value = JoinStringFromIndex(words, " ", 2);
		Put(key, value);

	    } else if (words[0].equals("get") && words.length == 2) {
		String key = words[1];
		Get(key);

	    } else if (line.equals("exit")) {
		System.out.println("Bye!");
		System.exit(0);
	    } else {
		System.out.println("Unrecognized command: " + words[0]);
	    }
	}
    }

    public static void main(String[] args) throws Exception {
	Environment env = new Environment();
	env.getParameters().setString("nat_search_policy", "never");

	try {
	    int bindport = Integer.parseInt(args[0]);
	    InetSocketAddress bootaddress;
	    if (args[1].equals("bootstrap")) {
		bootaddress = null;
	    } else {
		InetAddress bootaddr = InetAddress.getByName(args[1]);
		int bootport = Integer.parseInt(args[2]);
		bootaddress = new InetSocketAddress(bootaddr, bootport);
	    }
	    PastConsole console = new PastConsole(bindport, bootaddress, env);
	    console.Run();
	} catch (Exception e) {
	    System.out.println("Usage: ");
	    System.out.println("java [-cp FreePastry-<version>.jar] " +
			       "dhtperf.pastry.PastConsole localbindport " +
			       "bootIp bootPort");
	    System.out.println("Example: java dhtperf.pastry.PastConsole " +
			       "9001 localhost 9000");
	    System.out.println("To bootstrap a new network, just pass " +
			       "'bootstrap' as the bootIp");
	    System.exit(1);
	}
    }
}

