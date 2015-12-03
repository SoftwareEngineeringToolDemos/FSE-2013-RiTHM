package ca.uwaterloo.esg.rvtool.monitor;

import java.util.concurrent.atomic.AtomicInteger;

public class PropVar {
	String name;
	String type;
	String scope;
	String function;
	private static final AtomicInteger num = new AtomicInteger();
	
	public PropVar(String name, String type, String scope) {
		this(name,type,scope,"");
	}
	
	public PropVar(String name, String type, String scope, String function) {
		this.name = name;
		this.type = type;
		this.scope = scope;
		this.function = function;
		this.num.getAndIncrement();
	}
	
	public PropVar() {
		this.name = "";
		this.type = "";
		this.scope = "";
		this.function = "";
		this.num.getAndIncrement();
	}
}
