package ca.uwaterloo.esg.rvtool.TTRV.FunctionCleaner;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.util.StringTokenizer;


public class FunctionCleaner {


	public static void main(String[] args) {

		try
		{
			  StringTokenizer tok;
  
			  String dir = args[0];
			  FileInputStream fstream = new FileInputStream(dir+"/LLVM/FunctionCalls.txt");
			  DataInputStream in = new DataInputStream(fstream);
			  BufferedReader br = new BufferedReader(new InputStreamReader(in));
			  
			  BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/Pass"+"/"+"FunctionCalls.txt")); 

			  String strLine;
			  String tokStrings;
			  
			  while ((strLine = br.readLine()) != null)   {
				  if(strLine.contains("@"))
					  
				  {
					  tok = new StringTokenizer(strLine,"::");
					  tokStrings = tok.nextToken().trim();
					  out.write(tokStrings+"::");
					  tokStrings = tok.nextToken().trim();
					  int index1 = tokStrings.indexOf("@");
					  int index2 = tokStrings.indexOf(" ", index1);
					  tokStrings = tokStrings.substring(index1+1, index2);
					  out.write(tokStrings+"\n");
				  }
				  else
				  {
					  out.write(strLine+"\n");
				  }
			  }
			  out.close();
		}
		catch(Exception e)
		{
			System.err.println("SOOMF: cant not open file for cleaning out the functioncalls");
			System.exit(0);
		}

	}

}
