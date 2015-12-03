package ca.uwaterloo.esg.rvtool.SMIRF;

import java.util.*;

public class Naive {
	
	private ArrayList<Node> vertices_bfs;
	private int root;
	
	Naive(int root,ArrayList<Node> vertices)
	{
		this.root = root;
		vertices_bfs = vertices;
	}
	
	ArrayList<Node> getVertices()
	{
		return vertices_bfs;
	}
	
	int getRoot()
	{
		return root;
	}
}
