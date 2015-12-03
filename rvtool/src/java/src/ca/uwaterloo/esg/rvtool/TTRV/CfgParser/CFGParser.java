package ca.uwaterloo.esg.rvtool.TTRV.CfgParser;

import java.util.Scanner;
import java.util.Vector;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.File;


public class CFGParser {

	public static void main(String[] args) {
		
		MyParser mParse = new MyParser();
		Vector<Node> graph = new Vector<Node>();
		Vector<DFSLoop> dfsLoops = new Vector<DFSLoop>();
		ILParser lpsolve = new ILParser();
		Vector<Input> inputs = new Vector<Input>();
		Vector<Dummy> dummies = new Vector<Dummy>();
		Vector<CallFunc> callFunctions = new Vector<CallFunc>();

		Graph mygraph = new Graph();
		
		int label = 0;
		
		String dir = args[0];
		String Limit = args[1];
		
		boolean goLimit = false;

		
		int deleteLimit = 15;
		
		try {
			deleteLimit = Integer.parseInt(Limit);	
		}
		catch (Exception e) {
			goLimit = true;
			while (goLimit) {
				goLimit = false;
				System.out.println("Input Sampling period was not in proper numeric format, Please re-enter the Input Sampling Period (e.g. 15)>");
				try{
					BufferedReader bufferRead = new BufferedReader(new InputStreamReader(System.in));
					Limit = bufferRead.readLine();
					System.out.println(Limit);
					deleteLimit = Integer.parseInt(Limit);
				}
				catch (Exception e2) {
					//System.err.println("Input is still in incorrect format (must be numeric)!");
					goLimit = true;
					
				}
			}
			
		}
		
		System.out.println("Acceptable Input Sampling Period "+deleteLimit);



		
		
		System.out.println("///---------------------- Merging the CFGs ----------------------///");
		System.out.println("///--------------------------------------------------------------///");

		try{
			///---------------------- Extracting the function calls

			BufferedReader reader = new BufferedReader(new FileReader(dir+"/Pass/FunctionCalls.txt"));
			StringBuilder fileContents = new StringBuilder();
			String line;
			while ( (line = reader.readLine()) != null ) {
				if ( line.equals("")) break;
				fileContents.append(line+"\n");
			}
				
			mParse.funCalls(fileContents.toString(), callFunctions);	

		}
		catch (IOException e) {
			System.err.println("SOOMF: Could not process the function calls file");
			e.printStackTrace();
			System.exit(1);
		}
    					
		
		try{
			///---------------------- Reading the raw CFG

			File directory = new File(args[0]+"/CFG");
			File[] fileList = directory.listFiles();
			
			BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/CFG/rawCFG"));

			for (int i = 0; i < fileList.length; i++) {
				String dotFile= fileList[i].getName();
				
				FileInputStream fstream = new FileInputStream(dir+"/CFG/"+dotFile);
				DataInputStream in = new DataInputStream(fstream);
				BufferedReader br = new BufferedReader(new InputStreamReader(in)); 				
				String strLine;
				
				while ((strLine = br.readLine()) != null)   {
					if ( !(strLine.equals("")))
						out.write(strLine+"\n");

				}				
			}
			out.close();
			
			System.out.println(">>>>>>>>>>>>>> Finished Merging dot files");
			

			
			BufferedReader reader = new BufferedReader(new FileReader(dir+"/CFG/rawCFG"));
			StringBuilder fileContents = new StringBuilder();
			String line;
			while ( (line = reader.readLine()) != null ) {
				if ( line.equals("")) break;
				fileContents.append(line+"\n");
			}
						
			label = mParse.graphParse(fileContents.toString(),dir, graph);	
			


		}
		catch (IOException e) {
			System.err.println("SOOMF: Can not read files for raw CFGs");
			e.printStackTrace();
			System.exit(1);
		} 
		
		
		try{
			///---------------------- Adding edges to the graph

			BufferedReader reader = new BufferedReader(new FileReader(dir+"/Graph/"+"out_Edges"));
			StringBuilder fileContents = new StringBuilder();
			String line;
			while ( (line = reader.readLine()) != null ) {
				if ( line.equals("")) break;
				fileContents.append(line+"\n");
			}
			
			mParse.graphComplete(graph, fileContents.toString());
		}
		catch (IOException e) {
			System.err.println("SOOMF: Can not read file for Edges");
			e.printStackTrace();
			System.exit(1);
		}
		
		label = graph.size() +1;
		
		
		try{
			//---------------------- Reading the input file for variables of interest
			BufferedReader reader = new BufferedReader(new FileReader(dir+"/Pass/instLines.txt"));
			StringBuilder fileContents = new StringBuilder();
			String line;
			while ( (line = reader.readLine()) != null ) {
				if ( line.equals("")) break;
				fileContents.append(line+"\n");
			}
			
			mParse.inputParse(inputs,fileContents.toString());

		}
		catch (IOException e) {
			System.err.println("SOOMF: Can not read file critLines");
			e.printStackTrace();
			System.exit(1);
		}
		
		
		//---------------------- Adding dummy nodes to the graph to represent start and end of CFG graph

		mygraph.syncGraph(dummies, graph);
		
		
		//--------------------- Adding the function Calls

		mygraph.addingFuncalls(dummies, callFunctions, graph, label);

		
		//--------------------------------
		
		try{
			//---------------------- providng the CFG to SMIRF
			mParse.outputSMIRF(dir, graph, dummies);


		}
		catch (IOException e) {
			System.err.println("SOOMF: Can not write the file graph.txt");
			e.printStackTrace();
			System.exit(1);
		}
		
		
		
		
		System.out.println("///---------------------- Creating Critical CFG ----------------------///");
		System.out.println("///-------------------------------------------------------------------///");

		
		//--------------------- Removing non-critical nodes
		
		label = mygraph.updateGraph(graph, inputs, label, dummies);
		


		//--------------------- Separating  critical instructions
		label++;

		mygraph.fixingGraph(graph, inputs, label);
		
		//-----------------------------------------------------

		
		System.out.println("///---------------------- Calculating LSP ----------------------///");
		System.out.println("///--------------------------------------------------------------///");

		
		try{
			//---------------------- Writes out the critical CFG
			mygraph.newGraphOut(dir,"critical_CFG", dummies, graph);

		}
		catch (IOException e) {
			System.err.println("SOOMF: Can not write critical CFG to file");
			e.printStackTrace();
			System.exit(1);
		}

		// TODO: horrible hack to provide option to toggle the generation of the ILP model; this code needs to be refactored!
		if (args.length == 2 || (args.length == 3 && !args[2].equals("-ilp"))) {
			return;
		}
				
		try{
			BufferedReader reader = new BufferedReader(new FileReader(dir+"/Graph/critical_CFG"));
			StringBuilder fileContents = new StringBuilder();
			String line;
			while ( (line = reader.readLine()) != null ) {
				if ( line.equals("")) break;
				fileContents.append(line+"\n");
			}
			mygraph.extractingLSP(dir,fileContents.toString());

		}
		catch (IOException e) {
			System.err.println("SOOMF: Can not calcualte LSP");
			e.printStackTrace();
			System.exit(1);
		}
		
		
		System.out.println("///---------------------- Creating ILP History Model ----------------------///");
		System.out.println("///------------------------------------------------------------------------///");

		
		mygraph.findBackEdge(graph, dummies, dfsLoops);
		
		
		for(int i = 0; i < dfsLoops.size(); i++)
		{
			String loopName = dfsLoops.get(i).name;
			
			for(int ii = i+1; ii < dfsLoops.size(); )
			{
				if(loopName.contentEquals(dfsLoops.get(ii).name))
				{
					dfsLoops.remove(ii);
				}
				else
				{
					ii++;
				}
			}
		}
		
		
		mygraph.bfsOutdegree(graph, dummies, dfsLoops);
				
		lpsolve.ILP_lpsolve(dir,graph, "out_histModel", deleteLimit, dfsLoops, dummies);
			    	    
	}

}
