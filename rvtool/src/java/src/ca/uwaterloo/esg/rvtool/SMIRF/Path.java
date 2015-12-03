package ca.uwaterloo.esg.rvtool.SMIRF;

import java.util.ArrayList;

public class Path {

	ArrayList<Node> vertices;
	int sum;
	
	public Path(){
		vertices = new ArrayList<Node>();
		sum = 0;
	}
	
	ArrayList<Node> getVertices(){
		return vertices;
	}
	
	void addVertex(Node vertex){
		if(!vertices.contains(vertex)){
			vertices.add(vertex);
			sum = sum + vertex.getExecutionTime();
		}
	}
	
	int getSum(){
		return sum;
	}
	
	public String toString(){
		String str="";
		boolean flag = true;
		for(Node v:vertices){
			if(flag){
				str = String.valueOf(v.getID());
				flag = false;
			}
			else{
				str = str + "->" + String.valueOf(v.getID());
			}
		}
		return str;
	}
	
	boolean isSame(Path temp){
		if(this.vertices.size() != temp.getVertices().size())
			return false;
		
		for(int i=0;i<vertices.size();i++){
			if(vertices.get(i).getID() != temp.getVertices().get(i).getID()){
				return false;
			}
		}
		
		return true;
	}
	
}
