package ca.uwaterloo.esg.rvtool.TTRV.Heuristics;

public class CFGNode {
	int label;
	int line1;
	int line2;
	String function;
	
	CFGNode( int label, int line1, int line2, String function ) {
		this.label = label;
		this.line1 = line1;
		this.line2 = line2;
		this.function = function;
	}
}
