package ca.uwaterloo.esg.rvtool.TTRV.CfgParser;

import java.util.Vector;
public class Node {
	
	String varIntst;
	
	int label;
	String type;
	String fName;
	int line1;
	int line2;
	int weight;
	int criticalWeight = 0;
	String name;
	
	Vector<LLVMWeight> weightList = new Vector<LLVMWeight>();
	
	Vector<Node> children = new Vector<Node>();
	Vector<Node> parents = new Vector<Node>();
	Vector<Edge> edges = new Vector<Edge>();
	
	boolean found =false;
	boolean processed =false;
	
	int sampleR = -1;
	
	boolean dfsProcessed = false;
	boolean loop = false;
	boolean bfsProcessed = false;
	int bfsIndegree =0;
	
	

}
