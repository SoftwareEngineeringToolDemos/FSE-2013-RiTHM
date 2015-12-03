package ca.uwaterloo.esg.rvtool.TTRV.CfgParser;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.util.*;


public class MyParser {
	
	
	public void outputSMIRF(String dir, Vector<Node> graph, Vector<Dummy> dummies) throws  NumberFormatException, IOException
	{
		System.out.println(">>>>>>>>>>>>>> Providing CFG to SMIRF");
		
		BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/Graph"+"/"+"graph.txt")); 
		
		for(int i = 0; i < graph.size(); i++)
		{
			out.write("v,"+graph.get(i).label+","+graph.get(i).weight+"\n");
		}
		for(int i = 0; i < dummies.size(); i++)
		{
			out.write("v,"+dummies.get(i).head.label+","+dummies.get(i).head.weight+"\n");

			out.write("v,"+dummies.get(i).tail.label+","+dummies.get(i).tail.weight+"\n");

		}
		for(int i = 0; i < graph.size(); i++)
		{
			for(int j = 0; j < graph.get(i).edges.size(); j++)
			{
				out.write("e,"+graph.get(i).label+","+graph.get(i).edges.get(j).label+"\n");
			}
		}
		
		out.close();
		
		System.out.println(">>>>>>>>>>>>>> Finished Providing CFG to SMIRF");


	}

	public void instrument(String dir, Vector<Input> inputs, Vector<Node> graph) throws  NumberFormatException, IOException
	{
		
		System.out.println(">>>>>>>>>>>>>> Creating Instrumentation for History");

		
		StringTokenizer tok;
		Vector<String> vertisIds = new Vector<String>();

		FileInputStream fstreamVar = new FileInputStream(dir+"/Graph/"+"vertices");
		DataInputStream inVar = new DataInputStream(fstreamVar);
		BufferedReader brVar = new BufferedReader(new InputStreamReader(inVar));
		
		FileInputStream fstream = new FileInputStream(dir+"/ILP/"+"out_ilp.txt");
		DataInputStream in = new DataInputStream(fstream);
		BufferedReader br = new BufferedReader(new InputStreamReader(in));
		  
		BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/ILP"+"/"+"instLines.txt")); 
		
		String strLine;
		String xName;
		String value;
		String id;
		
		while ((strLine = brVar.readLine()) != null)   
		{
			tok = new StringTokenizer(strLine,",");
			while(tok.hasMoreTokens())
			{
				id = tok.nextToken().trim();
				if(!(id.contains("x") || id.contains("v")))
				{
					vertisIds.add(id);
				}
			}
			  
		}

		
		while ((strLine = br.readLine()) != null)   {
			
			
			if(strLine.contains("x"))
			{
				tok = new StringTokenizer(strLine,"                              ");
				xName = tok.nextToken().trim();
				value = tok.nextToken().trim();
				
				if(value.contains("1"))
				{
					
					int index = xName.indexOf("x");
					xName = xName.substring(index+1);
					index = Integer.parseInt(xName);
					String instString = vertisIds.get(index);
									
					//----------------- extracting what to instrument
					int nodeId = Integer.parseInt(instString);
					

					
					for(int i  =0; i < graph.size(); i++)
					{
						if(graph.get(i).label == nodeId)
						{
							
							for(int j = 0; j < inputs.size(); j++)
							{
								for(int k = 0; k < inputs.get(j).lines.size(); k++)
								{
									int line = inputs.get(j).lines.get(k);
									if(graph.get(i).line1 <= line && graph.get(i).line2 >= line)
									{
										out.write(graph.get(i).fName+","+inputs.get(j).var+","+line+"\n");

									}
								}
							}
							break;
						}
					}
					
				}
			}
			  
		}
		
		out.close();
		System.out.println(">>>>>>>>>>>>>> Instrumentation for Instrumentor Complete");

	}
	
	
	/**	
	 � * This Function Parses the Function Call locations from a file to help Merge the CFGs of different functions	
	 � * @param	: contents (file contents as String), call_functions (Structure for saving call locations)
	 � * @return : void	
	*/	
	
	public void funCalls(String contents, Vector<CallFunc> callFunctions) throws  NumberFormatException, IOException
	{
		
		System.out.println(">>>>>>>>>>>>>> Parsing the function Call locations");
		
		StringTokenizer tok;
		BufferedReader reader = new BufferedReader(new StringReader(contents));
		
		String line;
		String funName = "";

		while ( (line = reader.readLine()) != null ) {	
			
			if ( line.equals("")) break;
			
			if(line.contains("->"))
			{
				tok = new StringTokenizer(line,"::");
				String str1 = String.valueOf(tok.nextToken().trim());
				String str2 = String.valueOf(tok.nextToken().trim());
				int ind = str1.indexOf(">");
				str1 = str1.substring(ind+1);
				int strLine = Integer.parseInt(str1);
				for(int i = 0; i < callFunctions.size(); i++)
				{
					if(callFunctions.get(i).funName.contentEquals(funName))
					{
						FunClass newFunc = new FunClass();
						newFunc.line = strLine;
						newFunc.funName = str2;
						callFunctions.get(i).links.add(newFunc);
						
					}
				}
				
			}
			else
			{
				funName = line;
				boolean find = false;
				for(int i = 0; i < callFunctions.size(); i++)
				{
					if(callFunctions.get(i).funName.contentEquals(line))
					{
						find = true;
						break;
					}
				}
				
				if(find == false)
				{
					CallFunc newCall = new CallFunc();
					newCall.funName = line;
					callFunctions.add(newCall);
				}
			}	
			
		}
		
		System.out.println(">>>>>>>>>>>>>> Finished Parsing the function Call locations");

	}
	
	
	
		
	/**	
	 � * This Function Parses the raw CFGs and creates the graph	
	 � * @param	: contents (file contents as String), graph (Structure containing the CFG)
	 � * @return : void	
	*/	
	public int graphParse(String contents, String dir, Vector<Node> graph) throws  NumberFormatException, IOException 
	{
		

		System.out.println(">>>>>>>>>>>>>> Reading the raw CFG files");

		StringTokenizer tok;
		BufferedReader reader = new BufferedReader(new StringReader(contents));
		String line;
		String fName = "";

		int label = 0;
		
		
		BufferedWriter outE = new BufferedWriter(new FileWriter(dir+"/Graph/out_Edges")); 
		BufferedWriter outN = new BufferedWriter(new FileWriter(dir+"/Graph/out_Nodes")); 

			
		while ( (line = reader.readLine()) != null ) {
			
				
			if(line.contains("digraph"))
			{
				StringTokenizer tokDg =  new StringTokenizer(line,"'");
				tokDg.nextToken().trim();
				fName = String.valueOf(tokDg.nextToken().trim());
				continue;
			}
				
			if(line.contains("->") == true)
			{
				StringTokenizer tokS =  new StringTokenizer(line,"->");
				outE.write("e,");
				String label1 = tokS.nextToken().trim();
				StringTokenizer tokLabel =  new StringTokenizer(label1,":");
				label1 = tokLabel.nextToken().trim();
				outE.write(label1);
				outE.write(",");
				String label2 = tokS.nextToken().trim();
				tokLabel =  new StringTokenizer(label2,";");
				label2 = tokLabel.nextToken().trim();
				outE.write(label2);
				outE.write("\n");
				continue;
					
			}

				
			if(line.contains("shape") == true)
			{
				tok = new StringTokenizer(line,"[");
				String name = tok.nextToken().trim();
				
				outN.write("v,");
				Node n = new Node();
				n.fName = fName;
				outN.write(n.fName+",");
				n.label = label;
				outN.write(n.label+",");
				n.name = name;
				outN.write(n.name+",");


				
				label++;

						
				String info = tok.nextToken().trim();
					
				StringTokenizer tokI =  new StringTokenizer(info,"\\");
				String first = tokI.nextToken().trim();
				StringTokenizer tokFirst =  new StringTokenizer(first,"{");
				tokFirst.nextToken().trim();
				String type = tokFirst.nextToken().trim();		
				n.type = type;
				outN.write(n.type+",");
					
					
				String second = tokI.nextToken().trim();
				second = second.substring(1);
				

				
				if(!second.contentEquals(" --"))
				{
					
					StringTokenizer tokSecond =  new StringTokenizer(second,"--");
					int number = 0;
					int line1 = 0;
					int line2 = 0;
					int tempLine = 0;
					while (tokSecond.hasMoreTokens())
					{
						if(number == 0)
						{
							line1 = Integer.valueOf(tokSecond.nextToken().trim()).intValue();
						}
						else
						{
							tempLine =  Integer.valueOf(tokSecond.nextToken().trim()).intValue();
							if(tempLine >= line2)
							{
								line2 = tempLine;

							}
						}
						number++;
							
					}
											
					if(line2 < line1)
					{
						n.line2 = line1;
						n.line1 = line2;
					}
					else
					{
						n.line2 = line2;
						n.line1 = line1;
					}
					outN.write(n.line1+",");
					outN.write(n.line2+",");

						
						
					String third = tokI.nextToken().trim();
					StringTokenizer tokThird =  new StringTokenizer(third,",");

					while (tokThird.hasMoreTokens())
					{
						String tempStr = tokThird.nextToken().trim();
						if(tempStr.contains("@") == true)
						{
							StringTokenizer tok_weight =  new StringTokenizer(tempStr,"@");
							LLVMWeight lw = new LLVMWeight();
							lw.line = Integer.valueOf(tok_weight.nextToken().trim()).intValue();
							lw.weight = Integer.valueOf(tok_weight.nextToken().trim()).intValue();
							if(lw.weight == 0)
							{
								lw.weight++;
							}
							n.weightList.add(lw);
								
						}
					}
						
					for(int j = 0; j < n.weightList.size(); j++)
					{
						n.weight = n.weightList.get(j).weight + n.weight;
					}
					outN.write(n.weight+"\n");						

				}
				else
				{
					
					n.line1 = 0;
					n.line2 = 0;
					
					
					outN.write(n.line1+",");
					outN.write(n.line2+",");
					
					LLVMWeight lw = new LLVMWeight();
					lw.line = 0;
					lw.weight = 1;
					
					n.weightList.add(lw);
					
					for(int j = 0; j < n.weightList.size(); j++)
					{
						n.weight = n.weightList.get(j).weight + n.weight;
					}
					outN.write(n.weight+"\n");						

				}
				
				graph.add(n);
					
			}

		}
			
			
		outE.close();
		outN.close();
		
		label = label + 1;
		
		System.out.println(graph.size());
		
		return label;
		
	}
	
	/**	
	 � * This Function Parses the out_Edges file and adds the edges to the nodes in the graph	
	 � * @param	: graph (Structure containing the CFG), contents (file contents as String)
	 � * @return : void	
	*/	
	public void graphComplete(Vector<Node> graph, String contents) throws  NumberFormatException, IOException
	{
		
		System.out.println(">>>>>>>>>>>>>> Adding edges to the graph");
		
		StringTokenizer tok;
		BufferedReader reader = new BufferedReader(new StringReader(contents));
		
		String line;
			
		Node Node1 = null;
		Node Node2 = null;
		
		int tokCounter = 0;

				
		while ( (line = reader.readLine()) != null ) {	
			
			tokCounter = 0;
			
			if ( line.equals("")) break;
			
			tok = new StringTokenizer(line,",");
			
			while (tok.hasMoreTokens())
			{
				String tempS = String.valueOf(tok.nextToken().trim());
				
				if(tokCounter > 0)
				{
					for(int i = 0; i < graph.size();i++)
					{
						
						if(tempS.contentEquals(graph.get(i).name))
						{
							if(tokCounter == 1)
							{
								Node1 = graph.get(i);
								
							}
							if(tokCounter == 2)
							{
								Node2 = graph.get(i);
							}
						}
					}
				}

				tokCounter++;
			}
			
			if(Node1 == null || Node2 == null)
			{
				System.err.println("SOOMF: Nodes for adding edges are missing");
				System.exit(-1);
			}
			else
			{
				Node1.children.add(Node2);
				Node2.parents.add(Node1);
			}
			
			Node1 = null;
			Node2 = null;
			
		}
		
	}
	
	
	/**	
	 � * This Function Parses the critLines file to find critical instructions	
	 � * @param	: inputs (Structure containing the variables of interest), contents (file contents as String)
	 � * @return : void	
	*/	
	public void inputParse(Vector<Input> inputs, String contents) throws  NumberFormatException, IOException
	{
		
		System.out.println(">>>>>>>>>>>>>> Reading variables of interest");
		
		StringTokenizer tok;
		BufferedReader reader = new BufferedReader(new StringReader(contents));
		String line;
		
		boolean find = false;
			

		while ( (line = reader.readLine()) != null ) {		
			if ( line.equals("")) break;
			tok = new StringTokenizer(line,",");
			String fName = tok.nextToken().trim();
			String varName = tok.nextToken().trim();
			
			for(int i = 0; i < inputs.size();i++)
			{
				if(inputs.get(i).fName.equals(fName) && inputs.get(i).var.equals(varName))
				{
					inputs.get(i).lines.add(Integer.valueOf(tok.nextToken().trim()).intValue());
					find = true;
					break;
				}
			}
			
			if(find == false)
			{
				Input input = new Input();
				input.var = varName;
				input.fName = fName;
				input.lines.add(Integer.valueOf(tok.nextToken().trim()).intValue());
				inputs.add(input);
			}
			
		}
		
		
	}
	
	
	
	
	
	
	

}
