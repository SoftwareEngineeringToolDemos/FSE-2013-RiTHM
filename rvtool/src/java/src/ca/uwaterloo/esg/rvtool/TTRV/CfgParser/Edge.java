package ca.uwaterloo.esg.rvtool.TTRV.CfgParser;

import java.util.*;

public class Edge {
	
	int line1;
	int line2;
	int label;
	int weight;
	
	Vector<Patth> paths = new Vector<Patth>();
	Patth myPath = new Patth();
	
	boolean dfsProcessed = false;


}
