package ca.uwaterloo.esg.rvtool.SMIRF;

import java.util.ArrayList;


public class Node {
	private int id;
	private ArrayList<Node> children;
	private int execution_time;
	
	//BFS required details
	private int color;
	private int d;
	private boolean root;
	private ArrayList<Node> bfs_children;
	private boolean artificial;
	boolean loop_bfs_visited;
	Node predecessor;
	
	Node(int id,int execution_time)
	{
		this.id = id;
		this.execution_time = execution_time;
		children = new ArrayList<Node>();
		this.color = 0;
		d = Integer.MAX_VALUE;
		root = false;
		bfs_children = new ArrayList<Node>();
		artificial = false;
		loop_bfs_visited = false;
		predecessor = null;
	}
	
	Node getPredecessor(){
		return predecessor;
	}
	
	void setPredecessor(Node tmp){
		predecessor = tmp;
	}
	
	void setLoopBFSVisited(boolean value){
		loop_bfs_visited = value;
	}
	
	boolean getLoopBFSVisited(){
		return loop_bfs_visited;
	}
	
	int getID()
	{
		return id;
	}
	
	int getExecutionTime()
	{
		return execution_time;
	}
	
	ArrayList<Node> getChildren()
	{
		return children;
	}
	
	void setChildren(ArrayList<Node> n)
	{
		children = (ArrayList<Node>)n.clone();
	}
	
	int getColor()
	{
		return color;
	}
	
	void setColor(int color)
	{
		this.color = color;
	}
	
	int getD()
	{
		return d;
	}
	
	void setD(int d)
	{
		this.d = d;
	}
	
	Node getClone()
	{
		Node n = new Node(this.getID(),this.getExecutionTime());
		n.setChildren(this.getChildren());
		n.setColor(this.getColor());
		n.setD(this.getD());
		return n;
	}
	
	boolean getRoot()
	{
		return root;
	}
	
	void setRoot(boolean value)
	{
		root = value;
	}
	
	String getYID()
	{
		return (String)("y"+id);
	}
	
	String getXID()
	{
		return (String)("x"+id);
	}
	
	ArrayList<Node> getBfsChildren()
	{
		return bfs_children;
	}
	
	void initializeBfsChildren()
	{
		bfs_children = new ArrayList<Node>();
	}
	
	boolean getArtificial()
	{
		return artificial;
	}
	
	void setArtificial(boolean value)
	{
		artificial = value;
	}
	
	void initializeChildren()
	{
		children = new ArrayList<Node>();
	}
	
	public String toString()
	{
		//return id + "," + artificial;
		return String.valueOf(id);
	}
	
}
