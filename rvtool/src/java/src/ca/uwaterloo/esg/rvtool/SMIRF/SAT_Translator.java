package ca.uwaterloo.esg.rvtool.SMIRF;

import java.io.*;
import java.util.*;
import java.util.*;


public class SAT_Translator {

	/**
	 * @param args
	 */
	
	private static ArrayList<Node> vertices = new ArrayList<Node>();
	private static ArrayList<Node> vertices_naive_approach = new ArrayList<Node>();
	private static ArrayList<Node> vertices_bfs;
	private static ArrayList<Naive> naive_nodes;
	private static boolean 			sat=false,greedy=false;
	private static boolean 			DEBUG = true;	//Prints additional messages useful for debugging
	private static int				verbose = 1;	//Set the verbosity level
	static HashMap<Node,ArrayList<Node>> incidentVertices;
	static HashMap<Node, Node> backEdges;
	static int sampling_period;
	static ArrayList<String> loop_constraints;
	static String extracted_filename;
	static ArrayList<Node> leavesG;
	static ArrayList<Node> starting_vertices;
	static ArrayList<Path> loops;
	static ArrayList<String> additional_constraints;
	static ArrayList<Node> circular_naive_approach;
	static FileWriter fw=null;
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		incidentVertices = new HashMap<Node,ArrayList<Node>>();
		backEdges = new HashMap<Node,Node>();
		leavesG = new ArrayList<Node>();
		starting_vertices = new ArrayList<Node>();
		loops = new ArrayList<Path>();
		additional_constraints = new ArrayList<String>();
		circular_naive_approach = new ArrayList<Node>();
		
		FileReader fr;
		BufferedReader br;
		StringTokenizer token_comma;
		String line,first_token;
		int id, execution_time,vertex_1_id,vertex_2_id,closing_bracket_counter=0;
		Node vertex_1,vertex_2;
		
		BFS_Tree bfs_tree;
		StringBuilder SAT_expression = new StringBuilder("");
		StringBuilder temp;
		StringBuilder debugbuilder;
		StringBuffer buffer = null;
		ArrayList<String> clauses = new ArrayList<String>();
		int artificial_nodes_counter=100000;
		ArrayList<Node> vertices_leaves;

		if(args.length<4){
			System.err.println("Insufficient Arguments");
			System.exit(1);
		}
		
		String workingDir = args[3] + "/";
		
		if(args[2].equalsIgnoreCase("-sat"))
			sat = true;
		else
			greedy = true;
		
		sampling_period = Integer.parseInt(args[1]);
		
		StringTokenizer token_filename = new StringTokenizer(args[0],"/");
		token_filename.nextToken();
		String filename = token_filename.nextToken();
		token_filename = new StringTokenizer(filename,".");
		filename = token_filename.nextToken();
		extracted_filename = filename;
		loop_constraints = new ArrayList<String>();
		
		if(DEBUG && verbose >= 3) {
			System.out.println("Reading CFG");
		}
		
		try
		{
			fr = new FileReader(args[0]);
			br = new BufferedReader(fr);
			
			while((line = br.readLine()) != null)
			{
				 token_comma = new StringTokenizer(line,", ");
				 first_token = token_comma.nextToken();
				 if(first_token.equalsIgnoreCase("v"))
				 { 
					id = Integer.parseInt(token_comma.nextToken());
					execution_time = Integer.parseInt(token_comma.nextToken());
					Node vertex = new Node(id,execution_time);
					if(!vertices.contains(vertex))
						vertices.add(vertex);
				 }
				 if(first_token.equalsIgnoreCase("e"))
				 {
					 vertex_1_id = Integer.parseInt(token_comma.nextToken().trim());
					 vertex_2_id = Integer.parseInt(token_comma.nextToken().trim());
					 vertex_1 = getVertex(vertex_1_id);
					 vertex_2 = getVertex(vertex_2_id);
					 //if(!vertex_1.getChildren().contains(vertex_2))
					 boolean duplicate = false;
					 for(Node child:vertex_1.getChildren()) {
						 if(child.getID() == vertex_2.getID()) {
							 duplicate = true;
						 }
					 }
					 if(!duplicate)
					 	vertex_1.getChildren().add(vertex_2);
					 
					 if(incidentVertices.containsKey(vertex_2)){
						ArrayList<Node> incidentEdges = incidentVertices.get(vertex_2);
						if(!incidentEdges.contains(vertex_1))
							incidentEdges.add(vertex_1);
					 }
					 else{
						ArrayList<Node> entry = new ArrayList<Node>();
						entry.add(vertex_1);
						incidentVertices.put(vertex_2, entry);
					 }
				 }
			}
			
			for(Node key:vertices){
				if(!incidentVertices.containsKey(key)){
					starting_vertices.add(key);
				}
			}
			
			if(DEBUG && verbose >= 3) {
				System.out.println("Done Reading CFG");
			}
			
			StringBuffer temp_buffer;
			FileWriter writer;
			StringBuffer temp_buffer_orig=null;
			if(DEBUG){	
				temp_buffer_orig = new StringBuffer();
				temp_buffer_orig.append("digraph{\n");
				for(int i=0;i<vertices.size();i++)
				{
					if(verbose > 5){
						System.out.println(vertices.get(i).getID());
						System.out.println("Children");
					}
				
					//Adding the leaves of the graph to leavesG (If DEBUG is false, then please make sure that adding leaves is exectued for sure)
					if(vertices.get(i).getChildren().isEmpty()){
						if(verbose > 3){
							System.out.println("Leaf: "+vertices.get(i).getID());
						}
					leavesG.add(vertices.get(i));
					}
				
					for(int j=0;j<vertices.get(i).getChildren().size();j++)
					{
						if(!vertices.get(i).getChildren().get(j).getArtificial()) {
							debugbuilder = new StringBuilder("");
							System.gc();
							debugbuilder.append(vertices.get(i).getID());
							debugbuilder.append("->");
							debugbuilder.append(vertices.get(i).getChildren().get(j).getID());
							debugbuilder.append("[label=");
							debugbuilder.append(vertices.get(i).getExecutionTime());
							debugbuilder.append("]\n");
							temp_buffer_orig.append(debugbuilder.toString());							
						}
						else {
							debugbuilder = new StringBuilder("");
							System.gc();
							debugbuilder.append(vertices.get(i).getID());
							debugbuilder.append("->");
							debugbuilder.append(vertices.get(i).getChildren().get(j).getID());
							debugbuilder.append(" [color=blue,style=dotted,label=");
							debugbuilder.append(vertices.get(i).getExecutionTime());
							debugbuilder.append("];\n");
							temp_buffer_orig.append(debugbuilder.toString());							
						}
						if(verbose > 5){
							System.out.println(vertices.get(i).getChildren().get(j).getID());
							System.out.println(vertices.get(i).getChildren().get(j).getArtificial());
						}
					}
					if(verbose > 5){
						System.out.println("$$$$$$$$$$$$");
					}
				}
				temp_buffer_orig.append("}\n");
				writer = new FileWriter(new File(workingDir+filename+"_cfg_orig.dot"));
				writer.write(temp_buffer_orig.toString());
				writer.close();
			}
			
			if(DEBUG && verbose >= 3) {
				System.out.println("Identifying back edges");
			}
			
			identifyBackEdges();

			if(DEBUG && verbose >= 3) {
				System.out.println("Done identifying back edges");
			}
			
			//Adding the first vertex by default
			vertices_naive_approach.add(vertices.get(0));
			
			//Starting to create the SAT file if sat option is enabled
			if(sat) {
				if(DEBUG && verbose >= 3) {
					System.out.println("Creating SAT file");
				}

				System.out.println(vertices.size());
				fw = new FileWriter("SAT_expression.ys");
				//buffer = new StringBuffer();
				
				for(int i=0;i<vertices.size();i++)
				{
					fw.write("(define "+ vertices.get(i).getXID() +"::bool)\n");
					//buffer.append("(define "+ vertices.get(i).getXID() +"::bool)\n");
				}
				
				fw.write("\n");
				fw.write("(define-type v (subtype (n::int) (and (>= n 0) (<= n 1))))\n");
				//buffer.append("\n");
				//buffer.append("(define-type v (subtype (n::int) (and (>= n 0) (<= n 1))))\n");
				for(int i=0;i<vertices.size();i++) {
					fw.write("(define y"+ vertices.get(i).getID() +"::v)\n");
					//buffer.append("(define y"+ vertices.get(i).getID() +"::v)\n");
				}
				
				for(Node start:starting_vertices) {
					fw.write("(assert (= "+start.getXID()+" true))\n");
					//buffer.append("(assert (= "+start.getXID()+" true))\n");
				}
			}

			
			int number_of_artificial_nodes = 0;
			Node previous_node;

			if(DEBUG && verbose >= 3) {
				System.out.println("Adding aritficial nodes");
			}
			
			//Add additional vertices so that all weights of the edges are 1
			for(int i=0;i<vertices.size();i++)
			{
				number_of_artificial_nodes = vertices.get(i).getExecutionTime()-1;
				previous_node = vertices.get(i);
					
				for(int j=0;j<number_of_artificial_nodes;j++)
				{
					Node artificial_node = new Node(artificial_nodes_counter++,1);
					artificial_node.setArtificial(true);
					vertices.add(artificial_node);
					for(int l=0;l<previous_node.getChildren().size();l++)
					{
						artificial_node.getChildren().add(previous_node.getChildren().get(l));
					}
					previous_node.initializeChildren();
					previous_node.getChildren().add(artificial_node);
					previous_node = artificial_node;
				}
			}
			if(DEBUG && verbose > 5){
				temp_buffer = new StringBuffer();
				temp_buffer.append("digraph{\n");
				for(int i=0;i<vertices.size();i++)
				{
					if(verbose > 5){
						System.out.println(vertices.get(i).getID());
						System.out.println("Children");
					}
					for(int j=0;j<vertices.get(i).getChildren().size();j++)
					{
						if(!vertices.get(i).getChildren().get(j).getArtificial()) {
							debugbuilder = new StringBuilder("");
							System.gc();
							debugbuilder.append(vertices.get(i).getID());
							debugbuilder.append("->");
							debugbuilder.append(vertices.get(i).getChildren().get(j).getID());
							debugbuilder.append("\n");
							temp_buffer.append(debugbuilder.toString());							
						}
						else {
							debugbuilder = new StringBuilder("");
							System.gc();
							debugbuilder.append(vertices.get(i).getID());
							debugbuilder.append("->");
							debugbuilder.append(vertices.get(i).getChildren().get(j).getID());
							debugbuilder.append(" [color=blue,style=dotted];\n");
							temp_buffer.append(debugbuilder.toString());							
						}
						if(verbose > 5){
							System.out.println(vertices.get(i).getChildren().get(j).getID());
							System.out.println(vertices.get(i).getChildren().get(j).getArtificial());
						}
					}
					if(verbose > 5){
						System.out.println("$$$$$$$$$$$$");
					}
				}
				temp_buffer.append("}\n");
				writer = new FileWriter(new File(workingDir+filename+"_cfg_additional_nodes.dot"));
				writer.write(temp_buffer.toString());
				writer.close();
			}
			
			if(DEBUG && verbose >= 3) {
				System.out.println("Done adding artificial nodes");
			}
			
			//Generate the SAT expression
			if(DEBUG && verbose >= 3 && sat) {
				System.out.println("Generating SAT expression");
			}
			//SAT_expression = new StringBuilder("");
			//SAT_expression.append("(assert ");
				
			naive_nodes = new ArrayList<Naive>();
			
			for(int i=0;i<vertices.size();i++)
			{	
				if(vertices.get(i).getArtificial()) {
					continue;
				}
				
				if(DEBUG && verbose >=3) {
					System.out.println("Vertex:"+vertices.get(i).getID());
				}
				
				//Initialize BFS Children
				for(int j=0;j<vertices.size();j++)
					vertices.get(j).initializeBfsChildren();
					
				//Compute the BFS tree
				if(DEBUG && verbose >= 3) {
					System.out.println("Creating BFS tree");
				}

				bfs_tree = new BFS_Tree(vertices,vertices.get(i),sampling_period);
				vertices_bfs = bfs_tree.getDFSTree();
				if(DEBUG && verbose >= 3) {
					System.out.println("Done creating BFS Tree");
				}

				if(DEBUG && verbose > 5){
					temp_buffer = new StringBuffer();
					temp_buffer.append("digraph{\n");
					
					for(int j=0;j<vertices_bfs.size();j++)
					{
						for(int l=0;l<vertices_bfs.get(j).getBfsChildren().size();l++)
						{
							if(!vertices_bfs.get(j).getBfsChildren().get(l).getArtificial()) {
								debugbuilder = new StringBuilder("");
								System.gc();
								debugbuilder.append(vertices_bfs.get(j).getID());
								debugbuilder.append("->");
								debugbuilder.append(vertices_bfs.get(j).getBfsChildren().get(l).getID());
								debugbuilder.append("\n");
								temp_buffer.append(debugbuilder.toString());								
							}
							else {
								debugbuilder = new StringBuilder("");
								System.gc();
								debugbuilder.append(vertices_bfs.get(j).getID());
								debugbuilder.append("->");
								debugbuilder.append(vertices_bfs.get(j).getBfsChildren().get(l).getID());
								debugbuilder.append(" [color=red,style=dotted];\n");
								temp_buffer.append(debugbuilder.toString());
							}
						}
					}
					temp_buffer.append("}\n");
					writer = new FileWriter(new File(workingDir+filename+"_"+vertices.get(i).getID()+"_cfg_bfs_additional_nodes.dot"));
					writer.write(temp_buffer.toString());
					writer.close();
				}
					
				//Remove all the artificial nodes
				//Iterative method of removing nodes
				if(DEBUG && verbose >= 3) {
					System.out.println("Removing artificial nodes");
				}

				Node removedNode;
				while(!checkArtificial())
				{
					for(int j=0;j<vertices_bfs.size();j++)
					{
						if(vertices_bfs.get(j).getArtificial())
						{
							vertices_bfs.remove(j);
							j=0;
							continue;
						}
						for(int l=0;l<vertices_bfs.get(j).getBfsChildren().size();l++)
						{
							if(vertices_bfs.get(j).getBfsChildren().get(l).getArtificial())
							{
								for(int m=0;m<vertices_bfs.get(j).getBfsChildren().get(l).getBfsChildren().size();m++)
								{
									vertices_bfs.get(j).getBfsChildren().add(vertices_bfs.get(j).getBfsChildren().get(l).getBfsChildren().get(m));
								}								
								vertices_bfs.get(j).getBfsChildren().remove(l);
							}							
						}
					}
				}
					
				for(int j=0;j<vertices_bfs.size();j++)
				{
					for(int l=0;l<vertices_bfs.get(j).getBfsChildren().size();l++)
					{
						if(!vertices_bfs.contains(vertices_bfs.get(j).getBfsChildren().get(l)))
						{
							vertices_bfs.add(vertices_bfs.get(j).getBfsChildren().get(l));
						}
					}						
				}
				if(DEBUG && verbose >= 3) {
					System.out.println("Done removing artificial nodes");
				}

					
				//System.out.println("Done Removing Artificial Nodes.");
				//Recursively method of removing nodes
				/*if(!vertices_bfs.isEmpty())
					removeArtificialNodes(vertices_bfs.get(0));*/
					
				if(DEBUG){
					temp_buffer = new StringBuffer();
					temp_buffer.append("digraph{\n");

					for(int j=0;j<vertices_bfs.size();j++)
					{
						for(int l=0;l<vertices_bfs.get(j).getBfsChildren().size();l++)
						{
							if(!vertices_bfs.get(j).getBfsChildren().get(l).getArtificial()) {
								debugbuilder = new StringBuilder("");
								System.gc();
								debugbuilder.append(vertices_bfs.get(j).getID());
								debugbuilder.append("->");
								debugbuilder.append(vertices_bfs.get(j).getBfsChildren().get(l).getID());
								debugbuilder.append("\n");
								temp_buffer.append(debugbuilder.toString());
							}
							else {
								debugbuilder = new StringBuilder("");
								System.gc();
								debugbuilder.append(vertices_bfs.get(j).getID());
								debugbuilder.append("->");
								debugbuilder.append(vertices_bfs.get(j).getBfsChildren().get(l).getID());
								debugbuilder.append(" [color=red,style=dotted];\n");
								temp_buffer.append(debugbuilder.toString());								
							}
						}
						if(vertices_bfs.get(j).getBfsChildren().size()==0)
						{
							debugbuilder = new StringBuilder("");
							System.gc();
							debugbuilder.append(vertices_bfs.get(j).getID());
							debugbuilder.append(";\n");
							temp_buffer.append(debugbuilder.toString());								
						}
					}
					temp_buffer.append("}\n");
					writer = new FileWriter(new File(workingDir+filename+"_"+vertices.get(i).getID()+"_cfg_bfs_orig.dot"));
					writer.write(temp_buffer.toString());
					writer.close();
				}
					
				if(greedy){
					vertices_leaves = new ArrayList<Node>();
					for(int j=0;j<vertices_bfs.size();j++)
					{						
						if(vertices_bfs.get(j).getBfsChildren().isEmpty())
						{
							vertices_leaves.add(vertices_bfs.get(j));
						}
					}
					Naive naive_node = new Naive(vertices.get(i).getID(),vertices_leaves);
					naive_nodes.add(naive_node);
				}
					
				//Solving by SAT solver
				if(sat || greedy) {
					//Suppose if the one of the vertices in the starting_vertices list has all the leaves of the graph in it then we can directly make the SAT
					//expression since the instrumented vertices would be the root and leaves and one or two nodes of the loops
					if(DEBUG && verbose >= 3) {
						System.out.println("Checking special case");
					}

					boolean leaves = false;
					if(i==0){
						for(Node leaf:leavesG){
							if(!vertices_bfs.contains(leaf)){
								leaves = false;
								break;
							}
							else
								leaves = true;
						}
					}

					//If leaves is true then all the vertices in vertices has to have a color value of 2
					if(leaves){
						for(Node vertex:vertices){
							if(!vertex.getArtificial() && vertex.getColor()!=2){
								leaves = false;
								break;
							}
								
						}
					}
					
					if(leaves){
						/*
						 * Note: If reached inside this function, then that means that the sampling period is greater than the height of the graph
						 * This case is handled in the same way for both SAT and greedy approach
						 * For SAT, there will be only SAT_Expression.ys generated
						 * For greedy, there will be SAT_Expression.ys and sat_solution generated.
						 */
						if(DEBUG && verbose >= 3) {
							System.out.println("Special Case");
						}

						//Removing the artificial nodes from the vertices
						//NOTE: If a node has artificial children then that has not been removed here
						for(int random_index=0;random_index<vertices.size();random_index++)
						{
							if(vertices.get(random_index).getArtificial())
							{
								vertices.remove(random_index);
								random_index=0;
							}
						}
						
						//If all the leaves of the graph are present in the dfs tree of the starting node then we can directly instrument
//						if(sat)
//							System.out.println(vertices.size());
							
//						fw = new FileWriter("SAT_expression.ys");
//						buffer = new StringBuffer();
//						
//						for(Node v:vertices)
//						{
//							buffer.append("(define "+ v.getXID() +"::bool)\n");
//						}
//						
//						buffer.append("\n");
//						buffer.append("(define-type v (subtype (n::int) (and (>= n 0) (<= n 1))))\n");
//						for(Node v:vertices)
//							buffer.append("(define y"+ v.getID() +"::v)\n");
						
						for(Node start:starting_vertices){
//							buffer.append("(assert (= "+start.getXID()+" true))\n");
							//For greedy approach
							if(!vertices_naive_approach.contains(start))
								vertices_naive_approach.add(start);
						}
						
						//Adding all the leaves
						for(Node v:leavesG){
							if(sat)
								fw.write("(assert (= "+v.getXID()+" true))\n");
							//buffer.append("(assert (= "+v.getXID()+" true))\n");
							
							//For greedy approach
							if(!vertices_naive_approach.contains(v))
								vertices_naive_approach.add(v);
						}
												
						for(Path p:loops){
							if(p.getSum() < sampling_period){
								//fSuppose A,B,C,D are present on the loop, then x_A or x_B or x_C or x_D
								String loop_string="";
								for(int loop_index=0;loop_index<p.getVertices().size()-1;loop_index++){
									loop_string = loop_string+"(or "+p.getVertices().get(loop_index).getXID()+" ";
								}
								loop_string = loop_string+p.getVertices().get(p.getVertices().size()-1).getXID();
								//For greedy approach
								if(!vertices_naive_approach.contains(p.getVertices().get(p.getVertices().size()-1)))
									vertices_naive_approach.add(p.getVertices().get(p.getVertices().size()-1));
								
								for(int loop_index=0;loop_index<p.getVertices().size()-1;loop_index++)
									loop_string=loop_string+")";
								if(sat)
									fw.write("(assert "+loop_string+")\n");
								//buffer.append("(assert "+loop_string+")\n");								
							}
							if(p.getSum() > sampling_period){
								if(sat) {
									fw.write("(assert "+p.getVertices().get(0).getXID()+")\n");
									fw.write("(assert "+p.getVertices().get(p.getVertices().size()-1).getXID()+")\n");
								}
								
								//For greedy approach
								if(!vertices_naive_approach.contains(p.getVertices().get(0)))
									vertices_naive_approach.add(p.getVertices().get(0));
								if(!vertices_naive_approach.contains(p.getVertices().get(p.getVertices().size()-1)))
									vertices_naive_approach.add(p.getVertices().get(p.getVertices().size()-1));
							}
							if(p.getSum() == sampling_period){
								if(sat)
									fw.write("(assert "+p.getVertices().get(p.getVertices().size()-1).getXID()+")\n");
								
								//For greedy approach
								if(!vertices_naive_approach.contains(p.getVertices().get(p.getVertices().size()-1)))
									vertices_naive_approach.add(p.getVertices().get(p.getVertices().size()-1));
							}
						}

						//Adding this so that self loops are taken care
						for(Node selfloops: backEdges.keySet()){
							if(selfloops.getID() == backEdges.get(selfloops).getID()){
								if(sat)
									fw.write("(assert "+selfloops.getXID()+")\n");
								
								//For greedy approach
								if(!vertices_naive_approach.contains(selfloops))
									vertices_naive_approach.add(selfloops);
							}
						}
						
						//Taking care of additional constraints now
						for(String str:additional_constraints) {
							if(sat)
								fw.write(str);
							//buffer.append(str);
						}
						
						for(Node v:vertices) {
							if(sat)
								fw.write("(assert (= (= "+v.getXID()+" true) (= y"+v.getID()+" 1)))\n");
						}
						
						temp = new StringBuilder("");
						temp.append("(assert (<= ");
						for(int random_index=0;random_index<vertices.size()-1;random_index++)
						{
							temp.append("(+ ");
							temp.append("y");
							temp.append(vertices.get(random_index).getID());
							temp.append(" ");
						}						
						temp.append("y");
						temp.append(vertices.get(vertices.size()-1).getID());
						temp.append(" ");
						
						for(int random_index=0;random_index<vertices.size()-1;random_index++) {
							temp.append(")");
						}
						temp.append("1))\n");
						if(sat) {
							fw.write(temp.toString());
							fw.write("(check)\n");
							fw.close();
						}
						if(DEBUG && verbose > 2){
							System.out.println("Done writing the special case SAT file");
						}
						
						if(greedy){
							//Adding the constraints that if A has an edge to B and B has an edge back to A, then one of them have to instrumented
							for(Node circular:circular_naive_approach){
								if(!vertices_naive_approach.contains(circular))
									vertices_naive_approach.add(circular);
							}
							
							System.out.println(vertices_naive_approach.size());
							//Printing the naive approach solution into sat_solution file. This is done so that it can be fit into the toolchain
							StringBuffer sat_solution_buffer = new StringBuffer();
							FileWriter sat_writer = new FileWriter(new File("sat_solution"));
							for(int yindex=0;yindex<vertices_naive_approach.size();yindex++)
							{
								sat_solution_buffer.append("(= y"+vertices_naive_approach.get(yindex).getID()+" 1)\n");
							}
							sat_writer.write(sat_solution_buffer.toString());
							sat_writer.close();
							
							//Creating solution in terms of Dot file
							if(DEBUG){
								StringBuffer solution_buffer = new StringBuffer();
								solution_buffer.append("digraph{\n");
								for(Node naive_v:vertices_naive_approach){
									solution_buffer.append(naive_v.getID()+"[color=red]\n");
								}
								StringTokenizer token_orig = new StringTokenizer(temp_buffer_orig.toString(),"\n");
								token_orig.nextToken();	//ignoring the first token since it is digraph{\n
								while(token_orig.hasMoreTokens()){
									solution_buffer.append(token_orig.nextToken()+"\n");
								}
								sat_writer = new FileWriter(new File(workingDir+filename+"_soln.dot"));
								sat_writer.write(solution_buffer.toString());
								sat_writer.close();
							}
						}						
						System.exit(0);
					}
						
					if(sat && !vertices_bfs.isEmpty())
					{
						vertices.get(i).setRoot(true);
						if(DEBUG && verbose >= 3) {
							System.out.println("Computing clauses for SAT expression and adding it to the set of clauses");
						}

						//clauses.add(SAT_String_Generator(vertices,vertices.get(i),sampling_period));
//						clauses.add(SAT_String_Generator(vertices_bfs.get(0)));
						fw.write("(assert ");						
						SAT_String_GeneratorFile(vertices_bfs.get(0));
						fw.write(")\n");
						vertices.get(i).setRoot(false);
						if(DEBUG && verbose >= 3) {
							System.out.println("Done computing clauses");
						}

					}
				}					
			}
			if(greedy){
				Queue<Naive> q = new LinkedList<Naive>();
				Naive temp_naive,temp_naive1;
				q.add(naive_nodes.get(0));
				ArrayList<Node> temp_vertices;
				
				while(!q.isEmpty())
				{
					temp_naive = q.remove();
					if(DEBUG && verbose > 3){
						System.out.println("Root: "+temp_naive.getRoot());
					}
					temp_vertices = temp_naive.getVertices();
					if(DEBUG && verbose > 3){
						System.out.println("Leaves: "+temp_vertices.toString());
					}
					for(int j=0;j<temp_vertices.size();j++)
					{
						if(!vertices_naive_approach.contains(temp_vertices.get(j)))
						{
							temp_naive1 = new Naive(temp_vertices.get(j).getID(), getNaiveNode(temp_vertices.get(j).getID()));
							q.add(temp_naive1);
							vertices_naive_approach.add(temp_vertices.get(j));
						}
					}
				}
			}
				
			//Naive approach output
			if(DEBUG && greedy){
				if(verbose > 3){
					System.out.println("Naive Approach Solution");
					for(int i=0;i<vertices_naive_approach.size();i++)
					{
						System.out.println(vertices_naive_approach.get(i).getID());
					}					
				}
			}
			
//			if(sat){
//				for(int i=0;i<clauses.size()-1;i++)
//				{
//					SAT_expression.append("( and ");
//					SAT_expression.append(clauses.get(i));
//					SAT_expression.append(" ");
//				}
//				
//				//last iteration
//				SAT_expression.append(clauses.get(clauses.size()-1));
//				
//				for(int i=0;i<clauses.size();i++) {
//					SAT_expression.append(" )");
//				}
//			}
				
			if(greedy){
				System.out.println(vertices_naive_approach.size());
				//Printing the naive approach solution into sat_solution file. This is done so that it can be fit into the toolchain
				StringBuffer sat_solution_buffer = new StringBuffer();
				FileWriter sat_writer = new FileWriter(new File("sat_solution"));
				for(int i=0;i<vertices_naive_approach.size();i++)
				{
					sat_solution_buffer.append("(= y"+vertices_naive_approach.get(i).getID()+" 1)\n");
				}
				sat_writer.write(sat_solution_buffer.toString());
				sat_writer.close();
				
				//Creating solution in terms of Dot file
				if(DEBUG){
					StringBuffer solution_buffer = new StringBuffer();
					solution_buffer.append("digraph{\n");
					for(Node naive_v:vertices_naive_approach){
						solution_buffer.append(naive_v.getID()+"[color=red]\n");
					}
					StringTokenizer token_orig = new StringTokenizer(temp_buffer_orig.toString(),"\n");
					token_orig.nextToken();	//ignoring the first token since it is digraph{\n
					while(token_orig.hasMoreTokens()){
						solution_buffer.append(token_orig.nextToken()+"\n");
					}
					sat_writer = new FileWriter(new File(workingDir+filename+"_soln.dot"));
					sat_writer.write(solution_buffer.toString());
					sat_writer.close();
				}
			}
				
			//Removing the artificial nodes from the vertices
			//NOTE: If a node has artificial children then that has not been removed here
			for(int i=0;i<vertices.size();i++)
			{
				if(vertices.get(i).getArtificial()) {
					vertices.remove(i);
					i=0;
				}
			}
				
			//Printing out the number of vertices so that an upper bound can be kept for finding min-SAT
			if(sat){
//				if(DEBUG && verbose >= 3) {
//					System.out.println("Creating SAT file");
//				}

//				System.out.println(vertices.size());
//				fw = new FileWriter("SAT_expression.ys");
//				buffer = new StringBuffer();
				
//				for(int i=0;i<vertices.size();i++)
//				{
//					buffer.append("(define "+ vertices.get(i).getXID() +"::bool)\n");
//				}
//				
//				buffer.append("\n");
//				buffer.append("(define-type v (subtype (n::int) (and (>= n 0) (<= n 1))))\n");
//				for(int i=0;i<vertices.size();i++)
//					buffer.append("(define y"+ vertices.get(i).getID() +"::v)\n");
//				
//				for(Node start:starting_vertices)
//					buffer.append("(assert (= "+start.getXID()+" true))\n");
				
//				for(int i=0;i<clauses.size();i++)
//				{
//					buffer.append("(assert "+clauses.get(i)+")\n");
//				}
				
				for(int i=0;i<vertices.size();i++)
				{
					fw.write("(assert (= (= "+vertices.get(i).getXID()+" true) (= y"+vertices.get(i).getID()+" 1)))\n");
					//buffer.append("(assert (= (= "+vertices.get(i).getXID()+" true) (= y"+vertices.get(i).getID()+" 1)))\n");
				}
								
				//Adding this so that self loops are taken care
				for(Node selfloops: backEdges.keySet()){
					if(selfloops.getID() == backEdges.get(selfloops).getID()) {
						fw.write("(assert "+selfloops.getXID()+")\n");
						//buffer.append("(assert "+selfloops.getXID()+")\n");		
					}
				}
				
				//Taking care of additional constraints now
				for(String str:additional_constraints) {
					fw.write(str);
					//buffer.append(str);
				}
				
				temp = new StringBuilder("");
				temp.append("(assert (<= ");
				for(int i=0;i<vertices.size()-1;i++)
				{
					temp.append("(+ ");
					temp.append("y");
					temp.append(vertices.get(i).getID());
					temp.append(" ");
				}
				temp.append("y");
				temp.append(vertices.get(vertices.size()-1).getID());
				temp.append(" ");
				for(int i=0;i<vertices.size()-1;i++) {
					temp.append(")");
				}
				temp.append("1))\n");
				
				fw.write(temp.toString());
				fw.write("(check)\n");
				//buffer.append(temp.toString());
				//buffer.append("(check)\n");
				//fw.write(buffer.toString());
				fw.close();
				if(DEBUG && verbose >= 3) {
					System.out.println("Done creating SAT file");
				}

			}
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}

	public static ArrayList<Node> getNaiveNode(int id)
	{
		for(int i=0;i<naive_nodes.size();i++)
		{
			if(naive_nodes.get(i).getRoot()==id) {
				return naive_nodes.get(i).getVertices();
			}
		}
		return null;
	}
	
	public static Node getVertex(int id)
	{
		for(int i=0;i<vertices.size();i++)
		{			
			if(vertices.get(i).getID()==id) {
				return vertices.get(i);
			}
		}
		return null;
	}

	public static boolean checkArtificialVertices()
	{
		boolean removedAllArtificial=true;
		for(int i=0;i<vertices.size();i++)
		{
			for(int j=0;j<vertices.get(i).getChildren().size();j++)
			{
				if(vertices.get(i).getChildren().get(j).getArtificial()) {
					removedAllArtificial = false;
					break;
				}
			}
		}
		return removedAllArtificial;
	}
	
	public static boolean checkArtificial()
	{
		boolean removedAllArtificial=true;
		for(int i=0;i<vertices_bfs.size();i++)
		{
			for(int j=0;j<vertices_bfs.get(i).getBfsChildren().size();j++)
			{
				if(vertices_bfs.get(i).getBfsChildren().get(j).getArtificial()) {
					removedAllArtificial = false;
					break;
				}
			}
		}
		return removedAllArtificial;
	}
	
	/*public static void removeArtificialNodes(Node v)
	{
		Node removedNode;
		for(int i=0;i<v.getBfsChildren().size();i++)
		{
			if(!v.getBfsChildren().get(i).getArtificial())
			{
				removeArtificialNodes(v.getBfsChildren().get(i));
			}
			else
			{
				for(int m=0;m<v.getBfsChildren().get(i).getBfsChildren().size();m++)
				{
					v.getBfsChildren().add(v.getBfsChildren().get(i).getBfsChildren().get(m));
				}
				removedNode = v.getBfsChildren().get(i);
				v.getBfsChildren().remove(i);
				vertices_bfs.remove(removedNode);
				removeArtificialNodes(v);
			}
		}
	}*/

	
//	public static String SAT_String_Generator(ArrayList<Node> vertices,Node vertex, int sampling_period)
//	{
//		String str="";
//		ArrayList<String> clauses = new ArrayList<String>();
//		
//		if(sampling_period==0)
//		{	
//			str = str +" "+ vertex.getXID();
//			
//			return str;
//		}
//				
//		if(vertex.getBfsChildren().isEmpty())
//		{
//			return vertex.getXID();
//		}
//	
//		if(!vertex.getRoot())
//			str = "( or " + vertex.getXID() + " ";
//		else
//			str = "( => "+vertex.getXID()+" ";
//		
//		
//		for(int i=0;i< vertex.getBfsChildren().size();i++)
//		{
//			clauses.add(SAT_String_Generator(vertices,vertex.getBfsChildren().get(i),sampling_period-1));
//		}
//		for(int i=0;i<clauses.size()-1;i++)
//		{
//			str = str + "( and " + clauses.get(i)+" ";
//		}
//		str = str + clauses.get(clauses.size()-1);
//		
//		for(int i=0;i<clauses.size();i++)
//			str = str + " )";
//		
//		return str;
//	}
	
	public static String SAT_String_Generator(Node vertex)
	{
		//String str="";
		StringBuilder str;
		
		ArrayList<String> clauses = new ArrayList<String>();
						
		if(vertex.getBfsChildren().isEmpty())
		{
			return vertex.getXID();
		}
	
		if(!vertex.getRoot()) {
			str = new StringBuilder("( or " + vertex.getXID() + " ");
			//str = "( or " + vertex.getXID() + " ";
		}
		else {
			str = new StringBuilder("( => "+vertex.getXID()+" ");
			if(DEBUG && verbose >=3) {
				System.out.println("Root: "+vertex.getID());
			}
			//str = "( => "+vertex.getXID()+" ";
		}
		
		
		for(int i=0;i< vertex.getBfsChildren().size();i++)
		{
			clauses.add(SAT_String_Generator(vertex.getBfsChildren().get(i)));
		}
		
		for(int i=0;i<clauses.size()-1;i++)
		{
			str.append("( and " + clauses.get(i)+" ");
			if(DEBUG && verbose >=3) {
				System.out.println("Vertex "+vertex.getID()+" Clause "+i+" -> "+clauses.get(i));
			}

			//str = str + "( and " + clauses.get(i)+" ";
		}

		str.append(clauses.get(clauses.size()-1));
		if(DEBUG && verbose >=3) {
			System.out.println("Vertex "+vertex.getID()+" Clause "+(clauses.size()-1)+" -> "+clauses.get(clauses.size()-1));
		}

		//str = str + clauses.get(clauses.size()-1);
		
		for(int i=0;i<clauses.size();i++) {
			str.append(" )");
			//str = str + " )";
		}
		
		if(DEBUG && verbose >=3) {
			System.out.println("SAT String: "+str.toString());
		}
		return str.toString();
	}

	public static void SAT_String_GeneratorFile(Node vertex)
	{		
		try {
		if(vertex.getBfsChildren().isEmpty())
		{
			fw.write(vertex.getXID());
			return;
		}
		
		if(!vertex.getRoot()) {
			fw.write("( or " + vertex.getXID() + " ");
		}
		else {
			fw.write("( => "+vertex.getXID()+" ");
			if(DEBUG && verbose >=3) {
				System.out.println("Root: "+vertex.getID());
			}
		}
		
		for(int i=0;i< vertex.getBfsChildren().size()-1;i++)
		{
			fw.write("( and ");
			SAT_String_GeneratorFile(vertex.getBfsChildren().get(i));
			fw.write(" ");
		}
				
		SAT_String_GeneratorFile(vertex.getBfsChildren().get(vertex.getBfsChildren().size()-1));

		for(int i=0;i<vertex.getBfsChildren().size();i++) {
			fw.write(" )");
		}
		
		return;
		
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	
	/*
	 * This function generates the loop bfs tree
	 */
	public static ArrayList<String> loop_bfs(Node root, Node vertex, int present_sp){
		ArrayList<ArrayList<String>> clauses = new ArrayList<ArrayList<String>>();
		ArrayList<String> constraints = new ArrayList<String>();
						
		if(vertex.getChildren().isEmpty())
		{
			constraints.add(vertex.getYID());
			return constraints;
		}
		
		present_sp +=vertex.getExecutionTime();
		vertex.setLoopBFSVisited(true);
		
		
		if(vertex.getID() == root.getID()){
			Node tmp = backEdges.get(root);
			if(tmp.getExecutionTime()+present_sp <= sampling_period && !tmp.getLoopBFSVisited())
				clauses.add(loop_bfs(root, tmp,present_sp));
		}
		else{
			for(int i=0;i< vertex.getChildren().size();i++)
			{
				if(vertex.getChildren().get(i).getExecutionTime()+present_sp <= sampling_period && !vertex.getChildren().get(i).getLoopBFSVisited())
					clauses.add(loop_bfs(root, vertex.getChildren().get(i),present_sp));
			}
		}
		
		if(clauses.isEmpty()){
			constraints.add(vertex.getYID());
			return constraints;
		}
		else{
			for(int i=0;i<clauses.size();i++)
			{
				for(int j=0;j<clauses.get(i).size();j++)
					constraints.add("(+ "+vertex.getYID()+" "+clauses.get(i).get(j)+")");
			}
		}
				
		return constraints;
	}
	
	/*
	 * Using a DFS traversal, back edges are removed in the graph.
	 */
	public static void addLoopConstraints(String workingDir){
		Stack<Node> s = new Stack<Node>();		
		s.push(vertices.get(0));

		while(!s.isEmpty()){
			Node tmp = s.pop();
			if(tmp.getColor()==0){
				tmp.setColor(1);
				s.push(tmp);
				for(Node inciV:tmp.getChildren()){
					if(inciV.getColor()==1){
						backEdges.put(tmp, inciV);
						//If both tmp and inciV have each other as children then one of them have to instrumented, reason being that it could cycle through these two vertices
						if(inciV.getChildren().contains(tmp)){
							loop_constraints.add("(assert (>= (+ "+tmp.getYID()+" "+inciV.getYID()+") 1))\n");
						}
						
						tmp.setRoot(true);
						for(Node v:vertices){
							v.setLoopBFSVisited(false);
						}
						for(String temp_string:loop_bfs(tmp,tmp, 0))
							loop_constraints.add("(assert (>= "+temp_string+" 1))\n");
						try {
							FileWriter fw = new FileWriter(workingDir+extracted_filename+"_loop_"+tmp.getID()+".dot");
							StringBuffer buffer = new StringBuffer();
							
							buffer.append("digraph{\n");
							for(int i=0;i<vertices.size();i++){
								if(vertices.get(i).getLoopBFSVisited())
									buffer.append(vertices.get(i).getID()+"[color=blue]");
								for(int j=0;j<vertices.get(i).getChildren().size();j++){
									buffer.append(vertices.get(i).getID()+"->"+vertices.get(i).getChildren().get(j).getID()+"[label="+vertices.get(i).getExecutionTime()+"]"+"\n");
								}
							}
							buffer.append("}\n");
							fw.write(buffer.toString());
							fw.close();
							
							} catch (IOException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}		
						tmp.setRoot(false);
						
					}
					if(inciV.getColor()==0){
						s.push(inciV);
					}
				}
			}
			else if(tmp.getColor()==1){
				tmp.setColor(2);
			}
		}
		
	}

	/*
	 * Using a DFS traversal, back edges are removed in the graph.
	 */
	public static void identifyBackEdges(){
		Stack<Node> s = new Stack<Node>();
		
		for(Node all_vertex:vertices) {
			if(all_vertex.getColor() == 0) {
				s.push(vertices.get(0));
				vertices.get(0).setD(0);
				vertices.get(0).setPredecessor(null);
			}

		while(!s.isEmpty()){
			Node tmp = s.pop();
			if(tmp.getColor()==0){
				tmp.setColor(1);
				s.push(tmp);
				for(Node inciV:tmp.getChildren()){
					if(inciV.getColor()==1){
						backEdges.put(tmp, inciV);
						if(inciV.getChildren().contains(tmp)){
							StringBuilder string_holder = new StringBuilder("");
							string_holder.append("(assert (or ");
							string_holder.append(tmp.getXID());
							string_holder.append(" ");
							string_holder.append(inciV.getXID());
							string_holder.append("))\n");
							additional_constraints.add(string_holder.toString());
							//For greedy approach
							if(!circular_naive_approach.contains(inciV))
								circular_naive_approach.add(inciV);
						}
						
						Node dummy = tmp;
						Path p = new Path();
						p.addVertex(dummy);
						
						while(dummy!=null && dummy.getID() != inciV.getID()){
							dummy = dummy.getPredecessor();
							p.addVertex(dummy);
						}
						if(dummy == null){
							System.out.println("Could not find loop start..Something wrong");
							System.exit(1);
						}
						
						if(dummy.getID() == inciV.getID()){
							p.addVertex(dummy);
							loops.add(p);
							if(DEBUG && verbose > 4){
								System.out.println(p.toString());
								System.out.println(tmp.getID()+":Loop Sum: "+p.getSum());
							}
						}
					}
					if(inciV.getColor()==0){
						inciV.setD(tmp.getD()+tmp.getExecutionTime());
						inciV.setPredecessor(tmp);
						s.push(inciV);
					}
				}
			}
			else if(tmp.getColor()==1){
				tmp.setColor(2);
			}
		}
		
	}
	}
}
