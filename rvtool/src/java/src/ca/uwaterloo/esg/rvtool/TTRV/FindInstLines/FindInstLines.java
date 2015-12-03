package ca.uwaterloo.esg.rvtool.TTRV.FindInstLines;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.util.StringTokenizer;
import java.util.Vector;


public class FindInstLines {

	public static void main(String[] args) {

		Vector<Node> graph = new Vector<Node>();
		Vector<Input> inputs = new Vector<Input>();

		
		System.out.println(">>>>>>>>>>>>>>> Start Extracting the Instrumentation Points");
		
		try
		{
			  StringTokenizer tok;
  
			  String dir = args[0];
			  FileInputStream fstream = new FileInputStream(dir+"/Graph/"+"critical_CFG");
			  DataInputStream in = new DataInputStream(fstream);
			  BufferedReader br = new BufferedReader(new InputStreamReader(in));
			  
			  String type = args[1];
			  
			  String heuristicsInputPath = "";
			  
			  if(type.contentEquals("heu1") || type.contentEquals("heu2") || type.contentEquals("smirf"))
			  {
				  heuristicsInputPath = args[2];
			  }
			  

			  String str_line;
			  
			  while ((str_line = br.readLine()) != null)   {
				  
				  Node newNode = new Node();
				  
				  tok = new StringTokenizer(str_line,",");
				  
				  if(tok.nextToken().trim().contentEquals("v"))
				  {
					  newNode.label = Integer.parseInt(tok.nextToken().trim());
					  newNode.fName = tok.nextToken().trim();
					  newNode.line1 = Integer.parseInt(tok.nextToken().trim());
					  newNode.line2 = Integer.parseInt(tok.nextToken().trim());
					  
					  graph.add(newNode);
					  
				  }
				  

			  }
			  
			  
			 try{
				 //---------------------- Reading the input file for variables of interest
				 BufferedReader reader = new BufferedReader(new FileReader(dir+"/Pass/instLines.txt"));
				 StringBuilder fileContents = new StringBuilder();
				 String line;
				 while ( (line = reader.readLine()) != null ) {
					 if ( line.equals("")) break;
					 fileContents.append(line+"\n");
				 }
					
				 inputParse(inputs,fileContents.toString());

			 }
			 catch (IOException e) {
				 System.err.println("SOOMF: Can not read file critLines");
				 e.printStackTrace();
				 System.exit(1);
			 }
			 
			 try{
				 //---------------------- Extracting instrumentation points
				 instrument(dir,inputs,graph,type,heuristicsInputPath);

			 }
			 catch (IOException e) {
				 System.err.println("SOOMF: Can not write");
				 e.printStackTrace();
				 System.exit(1);
			 }
			  
			 System.out.println(">>>>>>>>>>>>>>> Finished Extracting the Instrumentation Points");

		}
		catch(Exception e)
		{
			System.err.println("SOOMF: cant not open file for graph.txt");
			System.exit(0);
		}

	

	

	}
	
	
	/**	
	 � * This Function Parses the critLines file to find critical instructions	
	 � * @param	: inputs (Structure containing the variables of interest), contents (file contents as String)
	 � * @return : void	
	*/	
	private static void inputParse(Vector<Input> inputs, String contents) throws  NumberFormatException, IOException
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
	
	
	/**	
	 � * This Function extracts the instrumentation lines to provide to the instrumentor
	 � * @param	: inputs (Structure containing the variables of interest), contents (file contents as String)
	 � * @return : void	
	*/	
	private static void instrument(String dir, Vector<Input> inputs, Vector<Node> graph, String type, String heuristicPath) throws  NumberFormatException, IOException
	{
		
		System.out.println(">>>>>>>>>>>>>> Creating Instrumentation for History");
		

		String strLine;

		
		if(type.contentEquals("ilp"))
		{
			StringTokenizer tok;
			Vector<String> vertisIds = new Vector<String>();

			FileInputStream fstreamVar = new FileInputStream(dir+"/Graph/"+"vertices");
			DataInputStream inVar = new DataInputStream(fstreamVar);
			BufferedReader brVar = new BufferedReader(new InputStreamReader(inVar));
			
			FileInputStream fstream = new FileInputStream(dir+"/ILP/"+"out_ilp.txt");
			DataInputStream in = new DataInputStream(fstream);
			BufferedReader br = new BufferedReader(new InputStreamReader(in));
			
			BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/ILP"+"/"+"instLines.txt")); 

			  
			
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

		}
		
		
		if(type.contentEquals("heu1") || type.contentEquals("heu2") || type.contentEquals("smirf"))
		{
			
			BufferedWriter out;
			if (type.contentEquals("heu1")) {
				out = new BufferedWriter(new FileWriter(dir+"/Heu/"+"instLines_Heu1.txt")); 
			} else if (type.contentEquals("heu2")) {
				out = new BufferedWriter(new FileWriter(dir+"/Heu/"+"instLines_Heu2.txt"));
			} else {
				out = new BufferedWriter(new FileWriter(dir+"/SMIRF/"+"instLines_SMIRF.txt"));
			}

			FileInputStream fstream = new FileInputStream(heuristicPath);
			DataInputStream in = new DataInputStream(fstream);
			BufferedReader br = new BufferedReader(new InputStreamReader(in));
			
			while ((strLine = br.readLine()) != null)   {
				
				
				if(strLine.contains("x"))
				{
					int index = strLine.indexOf("x");
					strLine = strLine.substring(index+1);
					index = Integer.parseInt(strLine.trim());
					
					for(int i  =0; i < graph.size(); i++)
					{
						if(graph.get(i).label == index)
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
			
			out.close();		
		}

		
		
		System.out.println(">>>>>>>>>>>>>> Instrumentation for Instrumentor Complete");

	}

}
