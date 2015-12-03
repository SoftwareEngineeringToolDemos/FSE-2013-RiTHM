package ca.uwaterloo.esg.rvtool.TTRV.ExtractAlias;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.util.StringTokenizer;


public class Extractor {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		System.out.println(">>>>>>>>>>> Start processing Aliases");
		
		try
		{
			  StringTokenizer tok;
  
			  String dir = args[0];
			  FileInputStream fstream = new FileInputStream(dir+"/Alias/"+"alias.txt");
			  DataInputStream in = new DataInputStream(fstream);
			  BufferedReader br = new BufferedReader(new InputStreamReader(in));
			  
			  FileInputStream fstream1 = new FileInputStream(dir+"/Alias/"+"func.txt");
			  DataInputStream in1 = new DataInputStream(fstream1);
			  BufferedReader br1 = new BufferedReader(new InputStreamReader(in1));
			  
			  BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/LLVM"+"/"+"alias.txt")); 

			  String str_line;
			  String str_line1="";

			  int counter = 0;
			  
			  while ((str_line = br.readLine()) != null)   {
				  if(str_line.contains("Alias Set Tracker"))
				  {
					  str_line1 = br1.readLine();
				  }
				  if(str_line.contains("AliasSet"))
				  {
					  int index1 = str_line.indexOf(",");
					  int index2 = str_line.indexOf("]");
					  int num_alias = Integer.parseInt(str_line.substring(index1+1, index2).trim());

					  if(num_alias > 1)
					  {
						  out.write("[g"+counter+"] \n");
						  out.write(str_line1+"\n");
						  counter++;
						  str_line = str_line.substring(index2+1).trim();

						  index1 = str_line.indexOf(",");

						  String alias = str_line.substring(0,index1).trim();

						  out.write(num_alias+"\n"+alias+"\n");
						  //--------------------------------------
						  str_line = str_line.substring(index1+1);
						  tok = new StringTokenizer(str_line,",");
						  
						  while(tok.hasMoreTokens())
						  {
							  String token = tok.nextToken().trim();
							  if(token.contains("%"))
							  {
								  int index3 = token.indexOf("%");
								  token = token.substring(index3+1).trim();
								  out.write(token+"\n");
							  }
							  else{
								  if(token.contains("@"))
								  {
									  int index3 = token.indexOf("@");
									  token = token.substring(index3+1).trim();
									  out.write(token+"\n");
								  }
							  }
						  }
						  out.write("\n");

						  
						  
					  }
				  }
			  }
			  out.close();
				System.out.println(">>>>>>>>>>> Finished processing Aliases");

		}
		catch(Exception e)
		{
			System.err.println("cant not open file to parse aliases");
			System.exit(0);
		}

	}

}
