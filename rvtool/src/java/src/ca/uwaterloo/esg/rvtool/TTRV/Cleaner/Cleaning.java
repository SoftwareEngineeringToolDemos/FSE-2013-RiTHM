package ca.uwaterloo.esg.rvtool.TTRV.Cleaner;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.util.*;


public class Cleaning {

	public static void main(String[] args) {
		
		System.out.println(">>>>>>>>>>>>>>> Start Cleaning out the Final Result");
		
		try
		{
			  StringTokenizer tok;
  
			  String dir = args[0];
			  FileInputStream fstream = new FileInputStream(dir+"/Pass/"+"critLines.txt");
			  DataInputStream in = new DataInputStream(fstream);
			  BufferedReader br = new BufferedReader(new InputStreamReader(in));
			  
			  BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/Pass"+"/"+"instLines.txt")); 

			  String str_line;
			  Vector<String> line_rem = new Vector<String>(); 
			  String func = "";
			  
			  while ((str_line = br.readLine()) != null)   {
				  tok = new StringTokenizer(str_line,",");
				  String temp_func = tok.nextToken().trim();
				  if(func.contentEquals(temp_func))
				  {
					  boolean find = false;
					  for(int i = 0; i < line_rem.size(); i++)
					  {
						 if(line_rem.get(i).contentEquals(str_line))
						 {

							 find = true;
							 break;
						 }
					  }
					  
					  if(find == false)
					  {
							 line_rem.add(str_line);
					  }
				  }
				  else
				  {
					  func = temp_func;
					  for(int i = 0; i < line_rem.size(); i++)
					  {
						  out.write(line_rem.get(i)+"\n");
					  }
					  line_rem.clear();
					  line_rem.add(str_line);
				  }
				  
				  

			  }
			  for(int i = 0; i < line_rem.size(); i++)
			  {
				  out.write(line_rem.get(i)+"\n");
			  }
			  out.close();
				System.out.println(">>>>>>>>>>>>>>> Finished Cleaning out the Final Result");

		}
		catch(Exception e)
		{
			System.err.println("cant not open file for cleaning out the final result");
			System.exit(0);
		}

	}

}
