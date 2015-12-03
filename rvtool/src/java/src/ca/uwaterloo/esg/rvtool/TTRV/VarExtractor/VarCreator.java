package ca.uwaterloo.esg.rvtool.TTRV.VarExtractor;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.util.StringTokenizer;


public class VarCreator {

	public static void main(String[] args) {

		
		System.out.println(">>>>>>>>>>>>>>> Start Extracting Variables of Interest");
		
		try
		{
			  StringTokenizer tok;
  
			  String globalDir = args[0];
			  String fileName = args[1];
			  FileInputStream fstream = new FileInputStream(globalDir+"/"+fileName);
			  DataInputStream in = new DataInputStream(fstream);
			  BufferedReader br = new BufferedReader(new InputStreamReader(in));
			  
			  String dir = args[2];
			  BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/"+"critInst.txt")); 

			  String str_line;
			  String varString ="";
			  String scope;
			  String name;
			  String func="";
			  
			  int counter = 0;
			  
			  
			  while ((str_line = br.readLine()) != null)   {
				  if(counter > 0)
				  {
					  tok = new StringTokenizer(varString,",");
					  
					  name = tok.nextToken().trim();
					  tok.nextToken().trim();
					  scope = tok.nextToken().trim();
					  
					  int index = name.indexOf("\"");
					  name = name.substring(index+1);
					  index = name.indexOf("\"");
					  name = name.substring(0, index);
					  
					  if(tok.hasMoreTokens())
					  {
						  func = tok.nextToken().trim();
						  index = func.indexOf("\"");
						  func = func.substring(index+1);
						  index = func.indexOf("\"");
						  func = func.substring(0, index);

					  }

					  
					  if(scope.contains("local"))
					  {

						  out.write(func+","+name+"\n");
					  }
					  else
					  {
						  
						  out.write("globalFUNC,"+name+"\n");
					  }
					  varString = str_line;
					  counter++;
				  }
				  else
				  {
					  varString = str_line;
					  counter++;
				  }
				  
			  }
			  
			  tok = new StringTokenizer(varString,",");
			  
			  name = tok.nextToken().trim();
			  tok.nextToken().trim();
			  scope = tok.nextToken().trim();
			  
			  int index = name.indexOf("\"");
			  name = name.substring(index+1);
			  index = name.indexOf("\"");
			  name = name.substring(0, index);
			  
			  if(tok.hasMoreTokens())
			  {
				  func = tok.nextToken().trim();
				  index = func.indexOf("\"");
				  func = func.substring(index+1);
				  index = func.indexOf("\"");
				  func = func.substring(0, index);

			  }
			  
			  if(scope.contains("local"))
			  {

				  out.write(func+","+name);
			  }
			  else
			  {
				  
				  out.write("globalFUNC,"+name);
			  }
			
			  out.close();
				System.out.println(">>>>>>>>>>>>>>> Finished Extracting Variables of Interest");

		}
		catch(Exception e)
		{
			System.err.println("SOOMF: cant not open file for variables.txt");
			e.printStackTrace();

			System.exit(0);
		}

	

	}

}
