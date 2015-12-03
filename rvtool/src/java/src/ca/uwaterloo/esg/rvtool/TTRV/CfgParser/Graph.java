package ca.uwaterloo.esg.rvtool.TTRV.CfgParser;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.StringReader;
import java.util.*;


public class Graph {
	
	
	/**	
	 � * This Function adds dummy nodes to the start and end of each function in the CFG graph	
	 � * @param	: dummies (Structure containing the dummy nodes), graph (Structure containing the CFG)
	 � * @return : void	
	*/	
	public void syncGraph(Vector<Dummy> dummies, Vector<Node> graph)
	{
		System.out.println("->>>>>>>>>>>>>> Syncing CFG graph with dummy nodes");

	    int label = -1;
		for(int i = 0; i < graph.size();i++)
		{
			String fName = graph.get(i).fName;
			boolean find = false;
			for(int j = 0; j < dummies.size(); j++)
			{
				if(dummies.get(j).fName.contentEquals(fName))
				{
					find = true;
					break;
				}
			}
			
			if(find == false)
			{
				Dummy dm = new Dummy();
				dm.fName = fName;
				dm.head.label = label;
				dm.head.fName = fName;
				label--;
				dm.tail.label = label;
				label--;
				dm.tail.fName = fName;
				dummies.add(dm);
			}
		}
		
		
		for(int i = 0; i < graph.size();i++)
		{
			if(graph.get(i).parents.size() == 0)
			{
				for(int j = 0; j < dummies.size(); j++)
				{
					if(dummies.get(j).fName.contentEquals(graph.get(i).fName))
					{
						graph.get(i).parents.add(dummies.get(j).head);
						dummies.get(j).head.children.add(graph.get(i));
						Edge ed = new Edge();
						ed.label =  graph.get(i).label;
						ed.line1 = graph.get(i).line1;
						ed.line2 = graph.get(i).line1;
						ed.weight = 1;
						//----- adding info about path (used for path aware)
						PathPort pp = new PathPort();
						pp.line1 = ed.line1;
						pp.line2 = ed.line2;
						ed.myPath.path.add(pp);
						ed.myPath.weight = ed.weight;
						dummies.get(j).head.edges.add(ed);
					}
				}
			}
			
			
			if(graph.get(i).children.size() > 0)
			{
				for(int j = 0; j < graph.get(i).children.size(); j++)
				{
					Edge ed = new Edge();
					ed.label =  graph.get(i).children.get(j).label;
					ed.line1 = graph.get(i).line1;
					ed.line2 = graph.get(i).line2;

					ed.weight = graph.get(i).weight;

					//----- adding info about path (used for path aware)
					PathPort pp = new PathPort();
					pp.line1 = ed.line1;
					pp.line2 = ed.line2;
					ed.myPath.path.add(pp);
					ed.myPath.weight = ed.weight;
					graph.get(i).edges.add(ed);
				}
			}
			else
			{

				for(int j = 0; j < dummies.size(); j++)
				{
					if(dummies.get(j).fName.contentEquals(graph.get(i).fName))
					{
						graph.get(i).children.add(dummies.get(j).tail);
						dummies.get(j).tail.parents.add(graph.get(i));
						Edge ed = new Edge();
						ed.label =  dummies.get(j).tail.label;
						ed.line1 = graph.get(i).line1;
						ed.line2 = graph.get(i).line2;
						ed.weight = graph.get(i).weight;
						
						//----- adding info about path (used for path aware)
						PathPort pp = new PathPort();
						pp.line1 = ed.line1;
						pp.line2 = ed.line2;
						ed.myPath.path.add(pp);
						ed.myPath.weight = ed.weight;

						graph.get(i).edges.add(ed);
					}
				}
			}
			
		}
	}
	
	
	/**	
	 � * This Function merges the CFG of different functions at function call locations	
	 � * @param	: dummies (Structure containing the dummy nodes), callFunctions (Structure containing the call locations), graph (Structure containing the CFG), label is the id of last node in graph
	 � * @return : void	
	*/	
	public void addingFuncalls(Vector<Dummy> dummies, Vector<CallFunc> callFunctions, Vector<Node> graph, int label)
	{
		System.out.println("->>>>>>>>>>>>>> Merging CFG of functions at function call locations");
		
		int initlabel = label;


		for(int i = 0; i < callFunctions.size(); i++)
		{
			if(callFunctions.get(i).links.size() > 0)
			{
				
				String fName;
				int line;
				Dummy dm = null;
				Node nd;
				
				for(int j = 0 ; j < callFunctions.get(i).links.size(); j++)
				{
					fName = callFunctions.get(i).links.get(j).funName;
					line =  callFunctions.get(i).links.get(j).line;
					
					System.out.println("The Search for "+fName);
					
					for(int k = 0; k < dummies.size(); k++)
					{
						System.out.println(dummies.get(k).fName);
						
						if(dummies.get(k).fName.contentEquals(fName))
						{
							dm = dummies.get(k);
							break;
						}
					}
					System.out.println("-------------------");
					
					if(dm != null)
					{
						for(int k = 0; k < graph.size(); k++)
						{
							if(graph.get(k).label < initlabel && graph.get(k).line1 <= line && graph.get(k).line2 >= line && graph.get(k).fName.equals(callFunctions.get(i).funName))
							{
								nd = graph.get(k);
								
								nd.children.add(dm.head);
								dm.head.parents.add(nd);
								Edge ed = new Edge();
								ed.label = dm.head.label;
								ed.line1 = nd.line1;
								ed.line2 = line;
								
								
								boolean go = true;
								for(int jj = 0; jj < nd.weightList.size(); jj++)
								{
									if(nd.weightList.get(jj).line <= line && go)
									{
										ed.weight = ed.weight + nd.weightList.get(jj).weight;
									}
									else
									{
										go = false;
									}
								}
								
								//--------------------- adding path info (for path-aware analysis)
								/*PathPort pp = new PathPort();
								pp.line1 = ed.line1;
								pp.line2 = ed.line2;
								ed.myPath.path.add(pp);
								ed.myPath.weight = ed.weight;*/
								
								nd.edges.add(ed);
								///----------------------------------------							
								

									Node newNode = new Node();
									if(line < nd.line2)
									{
										newNode.line1 = line +1;
									}
									else
									{
										newNode.line1 = line;
									}
									
									newNode.line2 = nd.line2;
									newNode.label = label;
									label++;
									
									newNode.fName = nd.fName;
									
									
									boolean goON = false;
									for(int jj = 0; jj < nd.weightList.size(); jj++)
									{
										if(nd.weightList.get(jj).line > line || goON == true)
										{
											newNode.weight = newNode.weight + nd.weightList.get(jj).weight;
											LLVMWeight lw = new LLVMWeight();
											lw.line = nd.weightList.get(jj).line;
											lw.weight = nd.weightList.get(jj).weight;
											newNode.weightList.add(lw);
											goON = true;
										}
									}
									

									
								
									
									for(int jj = 0; jj < nd.children.size();jj++)
									{
										if(nd.children.get(jj).label > 0)
										{
											newNode.children.add(nd.children.get(jj));
											nd.children.get(jj).parents.add(newNode);
											
											Edge edj = new Edge();
											edj.label = nd.children.get(jj).label;
											edj.line1 = newNode.line1;
											edj.line2 = newNode.line2;
											edj.weight = newNode.weight;
											
											//--------------------- adding path info (for path-aware analysis)
											/*PathPort pp2 = new PathPort();
											pp2.line1 = edj.line1;
											pp2.line2 = edj.line2;
											edj.myPath.path.add(pp2);
											edj.myPath.weight = edj.weight;*/
											
											newNode.edges.add(edj);
										}
										
									}

									
									for(int jj = 0; jj < nd.children.size();)
									{
										if(nd.children.get(jj).label > 0)
										{
											for(int je = 0; je < nd.edges.size();je++)
											{
												if(nd.edges.get(je).label == nd.children.get(jj).label)
												{
													nd.edges.remove(je);
												}
											}
											int index = nd.children.get(jj).parents.indexOf(nd);
											nd.children.get(jj).parents.remove(index);
											nd.children.remove(jj);
										}
										else
										{
											jj++;
										}
										
									}
									
									newNode.type = nd.type;
									
									nd.line2 = line;
									
									
								

									goON = false;
									for(int jj = 0; jj < nd.weightList.size();)
									{
										if(nd.weightList.get(jj).line > line || goON == true)
										{
											nd.weightList.remove(jj);
											goON = true;
										}
										else
										{
											jj++;
										}
									}
									
									nd.weight = nd.weight - newNode.weight;
									
									
									
									dm.tail.children.add(newNode);
									Edge ed1 = new Edge();
									ed1.label = newNode.label;
									ed1.line1 = dm.tail.line1;
									ed1.line2 = dm.tail.line2;
									ed1.weight = 1;
									//--------------------- adding path info (for path-aware analysis)
									/*PathPort pp1 = new PathPort();
									pp1.line1 = ed1.line1;
									pp1.line2 = ed1.line2;
									ed1.myPath.path.add(pp1);
									ed1.myPath.weight = ed1.weight;*/
									
									dm.tail.edges.add(ed1);
									newNode.parents.add(dm.tail);
									
									/*Edge edj = new Edge();
									edj.line1 = nd.line1;
									edj.line2 = line;
									edj.weight = nd.weight;
									edj.label = newNode.label;
									
									//--------------------- adding path info  (for path-aware analysis)
									PathPort pp3 = new PathPort();
									pp3.line1 = edj.line1;
									pp3.line2 = edj.line2;
									edj.myPath.path.add(pp3);
									edj.myPath.weight = edj.weight;
									
									nd.edges.add(edj);
									System.out.println(" The new edges ");
									for(int jj = 0; jj < nd.edges.size();jj++)
									{
										System.out.println(" edge "+ nd.edges.get(jj).label);
									}*/
									
									graph.add(newNode);

							}
						}
					}

					
				}
			}
		}
		
		
		Node linker= null;
		
		for(int i = 0; i < dummies.size(); i++)
		{
			if(dummies.get(i).head.parents.size() == 0 && !(dummies.get(i).fName.contentEquals("main")))
			{
				Dummy dm = dummies.get(i);
				
				for(int j = 0; j < dummies.size(); j++)
				{
					if(dummies.get(j).fName.contentEquals(dm.fName))
					{
						linker = dummies.get(j).tail.parents.get(0);
						linker.children.add(dm.head);
						dm.head.parents.add(linker);
						Edge ed = new Edge();
						ed.label = dm.head.label;
						ed.line1 = linker.line1;
						ed.line2 = linker.line2;
						if(ed.line1 >= ed.line2)
							ed.weight = ed.line1 -ed.line2 + 1;
						else
							ed.weight = ed.line2 -ed.line1 +1;
						
						//--------------------- adding path info (for path-aware analysis)
						PathPort pp3 = new PathPort();
						pp3.line1 = ed.line1;
						pp3.line2 = ed.line2;
						ed.myPath.path.add(pp3);
						ed.myPath.weight = ed.weight;
						linker.edges.add(ed);
						
						//-----------
						dm.tail.children.add(linker);
						Edge ed1 = new Edge();
						ed1.label = linker.label;
						ed1.line1 = dm.tail.line1;
						ed1.line2 = dm.tail.line2;
						if(ed1.line1 >= ed1.line2)
							ed1.weight = ed1.line1 -ed1.line2 + 1;
						else
							ed1.weight = ed1.line2 -ed1.line1 +1;
						
						//--------------------- adding path info (for path-aware analysis)
						PathPort pp4 = new PathPort();
						pp4.line1 = ed1.line1;
						pp4.line2 = ed1.line2;
						ed1.myPath.path.add(pp4);
						ed1.myPath.weight = ed1.weight;
						dm.tail.edges.add(ed1);
						linker.parents.add(dm.tail);
						
					}
				}
				
				
			}
		}
	}
	
	
	
	/**	
	 � * This Function removes Non-Critical nodes from the CFG graph	
	 � * @param	: graph (Structure containing the CFG), inputs (Structure containing critical instructions), label is the id of last node in graph, dummies (Structure containing the dummy nodes)
	 � * @return : void	
	*/
	public int updateGraph(Vector<Node> graph, Vector<Input> inputs, int label, Vector<Dummy> dummies)
	{
		System.out.println(">>>>>>>>>>>>>> Removing Non-Critical nodes from the CFG graph");


		for(int i = 0 ; i < graph.size();)
		{
			Node tempNode = graph.get(i);
			
			boolean find = false;
			for(int j =0; j < inputs.size(); j++)
			{
				for(int p = 0; p < inputs.get(j).lines.size();p++)
				{
					if(tempNode.line1 <= inputs.get(j).lines.get(p) && tempNode.line2 >= inputs.get(j).lines.get(p))
					{
						find = true;
						break;
					}
				}
				
				if(find == true)
					break;
				
			}
			
			
			
			if(find == false)
			{
				
				if(tempNode.parents.size() > 0)
				{
					
					for(int j = 0; j < tempNode.parents.size(); j++)
					{
						Node tempParent = tempNode.parents.get(j);

						
						//------------------------- Going through the node's children one by one
						for(int p = 0 ; p < tempNode.children.size();p++)
						{
							Node tempChild = tempNode.children.get(p);
							
							
							
							try{
								
								if(tempChild.label != tempNode.label || tempNode.label != tempParent.label)
								{

									
									if(j == 0)
									{
										int index = tempChild.parents.indexOf(tempNode);
										tempChild.parents.remove(index);
									}
									
								
									if(tempNode.label != tempParent.label && tempNode.label != tempChild.label)
									{

										
										if(!tempChild.parents.contains(tempParent))
										{
											tempChild.parents.add(tempParent);
										}
										else
										{
											//System.out.println("already has the parent "+tempParent.label);
										}
										
										if(!tempParent.children.contains(tempChild))
										{
											tempParent.children.add(tempChild);
											//----------------------------------------
											Edge edg = new Edge();

											
											Edge ed1 = null;
											Edge ed2 = null;
											
											for(int e = 0; e < tempParent.edges.size(); e++)
											{
												if(tempParent.edges.get(e).label == tempNode.label)
												{
													ed1 = tempParent.edges.get(e);
													break;
												}
											}
											
											for(int e = 0; e < tempNode.edges.size(); e++)
											{
												if(tempNode.edges.get(e).label == tempChild.label)
												{
													ed2 = tempNode.edges.get(e);
													break;
												}
											}
											
											int min = ed1.line1;
											int max = ed1.line2;
											
											//----------- figuring out the starting and ending lines
											
											if( ed1.line1 == 0 ||  ed1.line2 == 0)
											{
												if( ed2.line1 <=  ed2.line2)
												{
													min =  ed2.line1;
													max =  ed2.line2;
												}
												else
												{
													max =  ed2.line1;
													min =  ed2.line2;
												}
											}
											else
											{
												if(ed2.line1 == 0 ||  ed2.line2 == 0)
												{
													if( ed1.line1 <=  ed1.line2)
													{
														min =  ed1.line1;
														max =  ed1.line2;
													}
													else
													{
														max =  ed1.line1;
														min =  ed1.line2;
													}
												}
												else
												{
													if(ed1.line1 <= ed1.line2 && ed1.line1 <= ed2.line1 && ed1.line1 <= ed2.line2)
													{

														min = ed1.line1;
														
													}
													if(ed1.line2 <= ed1.line1 && ed1.line2 <= ed2.line1 && ed1.line2 <= ed2.line2)
													{

														min = ed1.line2;

													}
													if(ed2.line1 <= ed1.line1 && ed2.line1 <= ed1.line2 && ed2.line1 <= ed2.line2)
													{

														min = ed2.line1;

													}
													if(ed2.line2 <= ed2.line1 && ed2.line2 <= ed1.line1 && ed2.line2 <= ed1.line2)
													{
														min = ed2.line2;
													}
													//-------------------------------------
													if(ed1.line1 >= ed1.line2 && ed1.line1 >= ed2.line1 && ed1.line1 >= ed2.line2)
													{
														max = ed1.line1;
													}
													if(ed1.line2 >= ed1.line1 && ed1.line2 >= ed2.line1 && ed1.line2 >= ed2.line2)
													{
														max = ed1.line2;
													}
													if(ed2.line1 >= ed1.line1 && ed2.line1 >= ed1.line2 && ed2.line1 >= ed2.line2)
													{
														max = ed2.line1;
													}
													if(ed2.line2 >= ed2.line1 && ed2.line2 >= ed1.line1 && ed2.line2 >= ed1.line2)
													{
														max = ed2.line2;
													}
												}
												
											}

			
											edg.line1 = min;
											edg.line2 = max;
											
											//-------------- Adding path info (for Path-aware Analysis)
											for(int jj= 0; jj < ed1.myPath.path.size(); jj++)
											{
												PathPort pp = new PathPort();
												pp.line1 =  ed1.myPath.path.get(jj).line1;
												pp.line2 = ed1.myPath.path.get(jj).line2;
												edg.myPath.path.add(pp);
											}

											for(int jj= 0; jj < ed2.myPath.path.size(); jj++)
											{
												PathPort pp = new PathPort();
												pp.line1 =  ed2.myPath.path.get(jj).line1;
												pp.line2 = ed2.myPath.path.get(jj).line2;
												edg.myPath.path.add(pp);
											}
											
											edg.myPath.weight = ed1.myPath.weight+ed2.myPath.weight;
											
											
											//------------ adding side paths (for Path-aware Analysis)

											if(ed1.paths.size() > 0)
											{
												for(int jj = 0; jj < ed1.paths.size(); jj++)
												{
													Patth newPath = new Patth();
													Patth path = ed1.paths.get(jj);
													for(int jp = 0; jp < path.path.size(); jp++)
													{
														PathPort pp = new PathPort();
														pp.line1 = path.path.get(jp).line1;
														pp.line2 = path.path.get(jp).line2;
														newPath.path.add(pp);

													}
													newPath.weight = path.weight;
												
													if(ed2.paths.size() > 0)
													{

														for(int j2 = 0; j2 < ed2.paths.size();j2++)
														{
															Patth newPath1 = new Patth();
															newPath1.path = (Vector<PathPort>)newPath.path.clone();
															newPath1.weight = newPath.weight;
															
															Patth newPath2 = ed2.paths.get(j2);
															
															for(int jp = 0; jp < newPath2.path.size(); jp++)
															{
																PathPort pp = new PathPort();
																pp.line1 = newPath2.path.get(jp).line1;
																pp.line2 = newPath2.path.get(jp).line2;

																newPath1.path.add(pp);

															}
															newPath1.weight = newPath1.weight + newPath2.weight;

															edg.paths.add(newPath1);

														}
														
													}
													
													for(int jp = 0; jp < ed2.myPath.path.size(); jp++)
													{
														PathPort pp = new PathPort();
														pp.line1 = ed2.myPath.path.get(jp).line1;
														pp.line2 = ed2.myPath.path.get(jp).line2;
														newPath.path.add(pp);
													}
													newPath.weight = newPath.weight  + ed2.myPath.weight;

													edg.paths.add(newPath);
												}
											}
											
											//----------------- processing edge2
											if(ed2.paths.size() > 0)
											{
												for(int jj = 0; jj < ed2.paths.size(); jj++)
												{
													Patth newPath = new Patth();
													Patth path = ed2.paths.get(jj);
													for(int jp = 0; jp < path.path.size(); jp++)
													{
														PathPort pp = new PathPort();
														pp.line1 = path.path.get(jp).line1;
														pp.line2 = path.path.get(jp).line2;
														newPath.path.add(pp);

													}
													newPath.weight = path.weight;
												
													if(ed1.paths.size() > 0)
													{

														for(int j2 = 0; j2 < ed1.paths.size();j2++)
														{
															Patth newPath1 = new Patth();
															newPath1.weight = newPath.weight;
															
															Patth newPath2 = ed1.paths.get(j2);
															
															for(int jp = 0; jp < newPath2.path.size(); jp++)
															{
																PathPort pp = new PathPort();
																pp.line1 = newPath2.path.get(jp).line1;
																pp.line2 = newPath2.path.get(jp).line2;

																newPath1.path.add(pp);

															}
															for(int jp = 0; jp < newPath.path.size(); jp++)
															{
																PathPort pp = new PathPort();
																pp.line1 = newPath.path.get(jp).line1;
																pp.line2 = newPath.path.get(jp).line2;

																newPath1.path.add(pp);

															}
															newPath1.weight = newPath1.weight + newPath2.weight;

															edg.paths.add(newPath1);

														}
														
													}
													
													Patth finalPat = new Patth();
													for(int jp = 0; jp < ed1.myPath.path.size(); jp++)
													{
														PathPort pp = new PathPort();
														pp.line1 = ed1.myPath.path.get(jp).line1;
														pp.line2 = ed1.myPath.path.get(jp).line2;

														finalPat.path.add(pp);
													}
													for(int jp = 0; jp < newPath.path.size(); jp++)
													{
														PathPort pp = new PathPort();
														pp.line1 = newPath.path.get(jp).line1;
														pp.line2 = newPath.path.get(jp).line2;

														finalPat.path.add(pp);

													}
													finalPat.weight = newPath.weight  + ed1.myPath.weight;

													edg.paths.add(finalPat);
												}
											}
											
											
											
											edg.weight = ed1.weight + ed2.weight;
											
											edg.label = tempChild.label;
											
											tempParent.edges.add(edg);
										}
										else
										{
											
											Edge tempEdge = null;
											for(int je = 0; je < tempParent.edges.size(); je++)
											{
												if(tempParent.edges.get(je).label == tempChild.label)
												{
													tempEdge = tempParent.edges.get(je);
													break;
												}
											}
											
											//----------------------------------------
											Edge ed1 = null;
											Edge ed2 = null;
											
											
											
											for(int e1 = 0; e1 < tempParent.edges.size(); e1++)
											{
												if(tempParent.edges.get(e1).label == tempNode.label)
												{
													ed1 = tempParent.edges.get(e1);
													break;
												}
											}
											
											for(int e1 = 0; e1 < tempNode.edges.size(); e1++)
											{
												if(tempNode.edges.get(e1).label == tempChild.label)
												{
													ed2 = tempNode.edges.get(e1);
													break;
												}
											}
											
											int min = ed1.line1;
											int max = ed1.line2;
											
											
											//----------- figuring out the starting and ending lines
											
											if( ed1.line1 == 0 ||  ed1.line2 == 0)
											{
												if( ed2.line1 <=  ed2.line2)
												{
													min =  ed2.line1;
													max =  ed2.line2;
												}
												else
												{
													max =  ed2.line1;
													min =  ed2.line2;
												}
											}
											else
											{
												if(ed2.line1 == 0 ||  ed2.line2 == 0)
												{
													if( ed1.line1 <=  ed1.line2)
													{
														min =  ed1.line1;
														max =  ed1.line2;
													}
													else
													{
														max =  ed1.line1;
														min =  ed1.line2;
													}
												}
												else
												{
													if(ed1.line1 <= ed1.line2 && ed1.line1 <= ed2.line1 && ed1.line1 <= ed2.line2)
													{

														min = ed1.line1;
														
													}
													if(ed1.line2 <= ed1.line1 && ed1.line2 <= ed2.line1 && ed1.line2 <= ed2.line2)
													{

														min = ed1.line2;

													}
													if(ed2.line1 <= ed1.line1 && ed2.line1 <= ed1.line2 && ed2.line1 <= ed2.line2)
													{

														min = ed2.line1;

													}
													if(ed2.line2 <= ed2.line1 && ed2.line2 <= ed1.line1 && ed2.line2 <= ed1.line2)
													{
														min = ed2.line2;
													}
													//-------------------------------------
													if(ed1.line1 >= ed1.line2 && ed1.line1 >= ed2.line1 && ed1.line1 >= ed2.line2)
													{
														max = ed1.line1;
													}
													if(ed1.line2 >= ed1.line1 && ed1.line2 >= ed2.line1 && ed1.line2 >= ed2.line2)
													{
														max = ed1.line2;
													}
													if(ed2.line1 >= ed1.line1 && ed2.line1 >= ed1.line2 && ed2.line1 >= ed2.line2)
													{
														max = ed2.line1;
													}
													if(ed2.line2 >= ed2.line1 && ed2.line2 >= ed1.line1 && ed2.line2 >= ed1.line2)
													{
														max = ed2.line2;
													}
												}
												
											}

			
											
											//-------------- Adding path info
											
											
											
											//------------ adding side paths (for Path-aware Analysis)
											
											if(ed1.paths.size() > 0)
											{
												for(int jj = 0; jj < ed1.paths.size(); jj++)
												{
													Patth newPath = new Patth();
													Patth path = ed1.paths.get(jj);
													for(int jp = 0; jp < path.path.size(); jp++)
													{
														PathPort pp = new PathPort();
														pp.line1 = path.path.get(jp).line1;
														pp.line2 = path.path.get(jp).line2;
														newPath.path.add(pp);

													}
													newPath.weight = path.weight;
													if(ed2.paths.size() > 0)
													{

														for(int j2 = 0; j2 < ed2.paths.size();j2++)
														{
															Patth newPath1 = new Patth();
															newPath1.path = (Vector<PathPort>)newPath.path.clone();
															newPath1.weight = newPath.weight;
															
															Patth newPath2 = ed2.paths.get(j2);
															
															for(int jp = 0; jp < newPath2.path.size(); jp++)
															{
																PathPort pp = new PathPort();
																pp.line1 = newPath2.path.get(jp).line1;
																pp.line2 = newPath2.path.get(jp).line2;

																newPath1.path.add(pp);

															}
															newPath1.weight = newPath1.weight + newPath2.weight;

															tempEdge.paths.add(newPath1);

														}
														
													}
													
													for(int jp = 0; jp < ed2.myPath.path.size(); jp++)
													{
														PathPort pp = new PathPort();
														pp.line1 = ed2.myPath.path.get(jp).line1;
														pp.line2 = ed2.myPath.path.get(jp).line2;

														newPath.path.add(pp);
													}
													newPath.weight = newPath.weight  + ed2.myPath.weight;

													tempEdge.paths.add(newPath);
												}
											}
											
											//----------------- processing edge2
											if(ed2.paths.size() > 0)
											{
												for(int jj = 0; jj < ed2.paths.size(); jj++)
												{
													Patth newPath = new Patth();
													Patth path = ed2.paths.get(jj);
													for(int jp = 0; jp < path.path.size(); jp++)
													{
														PathPort pp = new PathPort();
														pp.line1 = path.path.get(jp).line1;
														pp.line2 = path.path.get(jp).line2;
														newPath.path.add(pp);

													}
													newPath.weight = path.weight;
													if(ed1.paths.size() > 0)
													{

														for(int j2 = 0; j2 < ed1.paths.size();j2++)
														{
															Patth newPath1 = new Patth();
															newPath1.weight = newPath.weight;
															
															Patth newPath2 = ed1.paths.get(j2);
															
															for(int jp = 0; jp < newPath2.path.size(); jp++)
															{
																PathPort pp = new PathPort();
																pp.line1 = newPath2.path.get(jp).line1;
																pp.line2 = newPath2.path.get(jp).line2;

																newPath1.path.add(pp);

															}
															for(int jp = 0; jp < newPath.path.size(); jp++)
															{
																PathPort pp = new PathPort();
																pp.line1 = newPath.path.get(jp).line1;
																pp.line2 = newPath.path.get(jp).line2;

																newPath1.path.add(pp);

															}
															newPath1.weight = newPath1.weight + newPath2.weight;

															tempEdge.paths.add(newPath1);

														}
														
													}
													
													Patth finalpat = new Patth();
													for(int jp = 0; jp < ed1.myPath.path.size(); jp++)
													{
														PathPort pp = new PathPort();
														pp.line1 = ed1.myPath.path.get(jp).line1;
														pp.line2 = ed1.myPath.path.get(jp).line2;

														finalpat.path.add(pp);
													}
													for(int jp = 0; jp < newPath.path.size(); jp++)
													{
														PathPort pp = new PathPort();
														pp.line1 = newPath.path.get(jp).line1;
														pp.line2 = newPath.path.get(jp).line2;

														finalpat.path.add(pp);

													}
													finalpat.weight = newPath.weight  + ed1.myPath.weight;

													tempEdge.paths.add(finalpat);
												}
											}
											
											/////--------------------------------

											
											
											
											
											if(tempEdge.weight > (ed1.weight + ed2.weight) )
											{
												tempEdge.line1 = min;
												tempEdge.line2 = max;
												tempEdge.weight = ed1.weight + ed2.weight;
												
												//------------ Swapping the main path (for Path-aware Analysis)
												Patth newPath = new Patth();
												
												for(int jj= 0; jj < tempEdge.myPath.path.size();jj++)
												{
													PathPort pp = new PathPort();
													pp.line1 = tempEdge.myPath.path.get(jj).line1;
													pp.line2 = tempEdge.myPath.path.get(jj).line2;

													newPath.path.add(pp);	
												}
												
												newPath.weight = tempEdge.myPath.weight;
												tempEdge.paths.add(newPath);
												tempEdge.myPath.path.clear();
												
												for(int jj= 0; jj < ed1.myPath.path.size(); jj++)
												{
													PathPort pp = new PathPort();
													pp.line1 =  ed1.myPath.path.get(jj).line1;
													pp.line2 = ed1.myPath.path.get(jj).line2;

													tempEdge.myPath.path.add(pp);
												}
												for(int jj= 0; jj < ed2.myPath.path.size(); jj++)
												{

													PathPort pp = new PathPort();
													pp.line1 =  ed2.myPath.path.get(jj).line1;
													pp.line2 = ed2.myPath.path.get(jj).line2;

													tempEdge.myPath.path.add(pp);
												}
												
												tempEdge.myPath.weight = ed1.myPath.weight+ed2.myPath.weight;

											}
											else
											{
												Patth newPath = new Patth();
												
												for(int jj= 0; jj < ed1.myPath.path.size(); jj++)
												{
													PathPort pp = new PathPort();
													pp.line1 =  ed1.myPath.path.get(jj).line1;
													pp.line2 = ed1.myPath.path.get(jj).line2;

													newPath.path.add(pp);
												}
												for(int jj= 0; jj < ed2.myPath.path.size(); jj++)
												{

													PathPort pp = new PathPort();
													pp.line1 =  ed2.myPath.path.get(jj).line1;
													pp.line2 = ed2.myPath.path.get(jj).line2;

													newPath.path.add(pp);
												}
												
												newPath.weight = ed1.myPath.weight+ed2.myPath.weight;
												tempEdge.paths.add(newPath);
											}
											
											
											
																								
										}
									
									}
									else
									{
										try
										{
											//System.out.println("	Node to be deleted is referring to itself ");
											
											if(tempNode.label == tempChild.label)
											{
												//System.out.println("=========== Child is equal to TempN ");
											}
											if(tempNode.label == tempParent.label)
											{
												//System.out.println(" Parnt is equal to tempN");
											}
									
									
										
										}
										catch(Exception e)
										{
											System.err.println("cant find node");
										}
									}
									
								

								}						
															
							}
							catch(Exception e)
							{
								System.err.println("cant find the parent node " + tempNode.label);
								return 0;
							}

							
						}

						
						
						for(int e1 = 0; e1 < tempParent.edges.size(); e1++)
						{
							if(tempParent.edges.get(e1).label == tempNode.label)
							{
								tempParent.edges.remove(e1);
								break;
							}
						}
						
						int indexNode = tempParent.children.indexOf(tempNode);
						tempParent.children.remove(indexNode);
						
					}
					
				}
				
				
				
				
				for(int qq = 0; qq < tempNode.children.size(); qq++)
				{
					Node tempChild = tempNode.children.get(qq);

					for(int ee = 0; ee < tempChild.edges.size(); ee++)
					{
						if(tempChild.edges.get(ee).label == tempNode.label)
						{
							tempChild.edges.remove(ee);
							break;
						}
					}
					for(int ee = 0; ee < tempChild.children.size(); ee++)
					{
						if(tempChild.children.get(ee).label == tempNode.label)
						{
							tempChild.children.remove(ee);
							break;
						}
					}
				}
				
				if(graph.contains(tempNode))
				{
					graph.remove(tempNode);
				}
				
		
			}
			else
			{
				i++;
			}
		}
		
		
		
		//------------- Cleaning up the dummies
		
		for(int i = 0; i < dummies.size(); i++)
		{
			Node head =  dummies.get(i).head;
			Node tail =  dummies.get(i).tail;
			if(head.children.contains(head))
			{
				int index = head.children.indexOf(head);
				head.children.remove(index);
			}
			if(head.parents.contains(head))
			{
				int index = head.parents.indexOf(head);
				head.parents.remove(index);
			}
			for(int j = 0; j< head.edges.size();)
			{
				if(head.edges.get(j).label == head.label)
				{
					head.edges.remove(j);
				}
				else
				{
					j++;
				}
			}
			
			//---------------------------------
			if(tail.children.contains(tail))
			{
				int index = tail.children.indexOf(tail);
				tail.children.remove(index);
			}
			if(tail.parents.contains(tail))
			{
				int index = tail.parents.indexOf(tail);
				tail.parents.remove(index);
			}
			for(int j = 0; j< tail.edges.size();)
			{
				if(tail.edges.get(j).label == tail.label)
				{
					tail.edges.remove(j);
				}
				else
				{
					j++;
				}
			}
			
			
		}
		
		return label;
	}
	
			
	/**	
	 � * This Function puts the critical instructions in separate nodes in the CFG graph	
	 � * @param	: graph (Structure containing the CFG), inputs (Structure containing critical instructions), label is the id of last node in graph
	 � * @return : void	
	*/
	public void fixingGraph(Vector<Node> graph, Vector<Input> inputs, int label)
	{
		System.out.println(">>>>>>>>>>>>>> Putting the critical instructions in separate nodes in the CFG graph");
	    
	    int idThreshold = label;
		
		for(int j = 0; j < graph.size() ;)
		{
			Node tempNode = graph.get(j);
			Vector<Integer> count = new Vector<Integer>();
				
			for(int i = 0; i < inputs.size() ; i++)
			{
				Input input = inputs.get(i);

				for(int k = 0 ; k < inputs.get(i).lines.size() ; k++)
				{
					if(tempNode.line1 <= input.lines.get(k) && tempNode.line2 >= input.lines.get(k))
					{
						count.add(input.lines.get(k));
					}
				}
			}
				
				
			for(int i = 0; i < count.size();i++)
			{
				int line = count.get(i);
				
				for(int ii = i+1; ii < count.size();)
				{
					if(count.get(ii) == line)
					{
						count.remove(ii);
					}
					else
					{
						ii++;
					}
				}
			}
			
			if(count.size() > 1)
			{
				
				SortLines(count);
					
				Node prevNode = null;
				for(int k = 0; k < count.size(); k++)
				{
					Node newNode = new Node();
					if(k == 0)
					{
						newNode.label = label;
						label++;
						for(int p = 0; p < tempNode.parents.size(); p++)
						{
							try{
								if(tempNode.parents.get(p).label != tempNode.label)
								{
									newNode.parents.add(tempNode.parents.get(p));
									tempNode.parents.get(p).children.add(newNode);
									tempNode.parents.get(p).children.remove(tempNode);
								}
								else
								{
									newNode.parents.add(newNode);
									newNode.children.add(newNode);
								}
									
									
								//-------------------- calculating the difference in weights
								int diffweight = 0;
								boolean goON = true;
								for(int je = 0; je < tempNode.weightList.size(); je++)
								{
									if(tempNode.weightList.get(je).line < count.get(k) && goON)
									{
										diffweight = tempNode.weightList.get(je).weight + diffweight;
									}
									else
									{
										goON = false;
									}
								}
								
								//---------------- editing the edges and the paths (the paths are for path-aware analysis)
								if(tempNode.parents.get(p).label != tempNode.label)
								{
									for(int ee = 0; ee < tempNode.parents.get(p).edges.size(); ee++)
									{
										if( tempNode.parents.get(p).edges.get(ee).label == tempNode.label)
										{
											tempNode.parents.get(p).edges.get(ee).label = newNode.label;
																								
											tempNode.parents.get(p).edges.get(ee).weight = tempNode.parents.get(p).edges.get(ee).weight + diffweight;
											
											tempNode.parents.get(p).edges.get(ee).myPath.weight = tempNode.parents.get(p).edges.get(ee).myPath.weight + diffweight;
												
											for(int jp = 0; jp < tempNode.parents.get(p).edges.get(ee).paths.size(); jp++)
											{
												tempNode.parents.get(p).edges.get(ee).paths.get(jp).weight = tempNode.parents.get(p).edges.get(ee).paths.get(jp).weight + diffweight;
											}
											
										}
									}
										
										
									
								}
								else
								{

									for(int ee = 0; ee < tempNode.parents.get(p).edges.size(); ee++)
									{
										if( tempNode.parents.get(p).edges.get(ee).label == tempNode.label)
										{
											Edge edg = new Edge();
												
											edg.label = newNode.label;
												
											edg.line1 = newNode.line1;
											edg.line2 = newNode.line2;
												
											
											edg.weight = tempNode.parents.get(p).edges.get(ee).weight + diffweight;
												
											edg.myPath.path = (Vector<PathPort>)tempNode.parents.get(p).edges.get(ee).myPath.path.clone();
											
											edg.myPath.weight = tempNode.parents.get(p).edges.get(ee).myPath.weight + diffweight;
												
											edg.paths = (Vector<Patth>)tempNode.parents.get(p).edges.get(ee).paths.clone();
												
											for(int jp = 0; jp < edg.paths.size(); jp++)
											{
												edg.paths.get(jp).weight = edg.paths.get(jp).weight + diffweight;
											}
												
											newNode.edges.add(edg);
											
										}
									}

								}

							}
							catch(Exception e){
								System.err.println("SOOMF: cant remove child in fix");
							}
								
								
						}
							
						newNode.line1 = tempNode.line1;
						newNode.type = tempNode.type;
						newNode.fName = tempNode.fName;
						newNode.line2 = count.get(k);
							
							
						boolean goON = true;
						for(int edjj = 0; edjj < tempNode.weightList.size(); edjj++)
						{
							if(goON && tempNode.weightList.get(edjj).line <newNode.line2)
							{
								newNode.weight = newNode.weight + tempNode.weightList.get(edjj).weight;
								LLVMWeight lw = new LLVMWeight();
								lw.line = tempNode.weightList.get(edjj).line;
								lw.weight = tempNode.weightList.get(edjj).weight;
								newNode.weightList.add(lw);
								if(tempNode.weightList.get(edjj).line == newNode.line2)
								{
									newNode.criticalWeight = tempNode.weightList.get(edjj).weight;
								}
							}
							else
							{
								goON = false;
							}
						}
							
							
						prevNode = newNode;
						graph.add(newNode);
							
					}
					else
					{
						if(k == count.size() -1)
						{
							newNode.label = label;
							label++;
							try
							{
								newNode.parents.add(prevNode);
								prevNode.children.add(newNode);
								newNode.line1 = prevNode.line2+1;
								newNode.type = tempNode.type;
								newNode.line2 = tempNode.line2;
								newNode.fName = tempNode.fName;
									
								boolean goON = false;
								boolean goON2 = false;
								for(int edjj = 0; edjj < graph.get(j).weightList.size(); edjj++)
								{
										
									if(goON || tempNode.weightList.get(edjj).line >newNode.line1)
									{
										newNode.weight = newNode.weight + tempNode.weightList.get(edjj).weight;
										LLVMWeight lw = new LLVMWeight();
										lw.line = tempNode.weightList.get(edjj).line;
										lw.weight = tempNode.weightList.get(edjj).weight;
										newNode.weightList.add(lw);
										goON = true;
										if(goON2 || tempNode.weightList.get(edjj).line == count.get(k))
										{
											newNode.criticalWeight = tempNode.weightList.get(edjj).weight + newNode.criticalWeight;
											goON2 = true;
										}
									}
										
								 }
								
								if(newNode.criticalWeight == 0)
								{
									newNode.criticalWeight = newNode.weight;
								}
									
								//-------------- adding the edges to prevNode
									
								Edge newEdge = new Edge();
								newEdge.label = newNode.label;
								newEdge.line1 = prevNode.line1;
								newEdge.line2 = prevNode.line2;
									
										
								newEdge.weight = prevNode.criticalWeight;
									
									
								int diff = 0;
								goON = true;
								for(int je =0; je < newNode.weightList.size(); je++)
								{
									if(goON && newNode.weightList.get(je).line < count.get(k))
									{
										diff = diff + newNode.weightList.get(je).weight;
									}
									else
									{
										goON = false;
									}
								}
									
								newEdge.weight = newEdge.weight + diff;
									
									
								//------------------- add path info
								PathPort pp = new PathPort();
								pp.line1 = newEdge.line1;
								pp.line2 = newEdge.line2;
								newEdge.myPath.path.add(pp);
								newEdge.myPath.weight = newEdge.weight;
								
								prevNode.edges.add(newEdge);
								//----------------
								
									
									
								for(int p = 0; p < tempNode.children.size(); p++)
								{
									if(tempNode.children.get(p).label != tempNode.label)
									{

										newNode.children.add(tempNode.children.get(p));
										tempNode.children.get(p).parents.add(newNode);
										tempNode.children.get(p).parents.remove(tempNode);
											
										//------------------ adding the edges to the newNode
										Edge newEdge1 = new Edge();
										newEdge1.label = tempNode.children.get(p).label;
										newEdge1.line1 = newNode.line1;
										newEdge1.line2 = newNode.line2;
											
										int negWeight = tempNode.weight - newNode.criticalWeight;
											
											
											
										for(int jp = 0 ; jp < tempNode.edges.size(); jp++)
										{
											if( tempNode.edges.get(jp).label == tempNode.children.get(p).label)
											{
												for(int je = 0; je < tempNode.edges.get(jp).myPath.path.size();je++)
												{
													PathPort pp1 = new PathPort();
													pp1.line1 = tempNode.edges.get(jp).myPath.path.get(je).line1;
													pp1.line2 = tempNode.edges.get(jp).myPath.path.get(je).line2;
													newEdge1.myPath.path.add(pp1);
													newEdge1.myPath.weight = tempNode.edges.get(jp).myPath.weight - negWeight;
														
												}
													
												for(int je = 0; je < tempNode.edges.get(jp).paths.size();je++)
												{
													Patth path = new Patth();
													for(int j2 = 0; j2 < tempNode.edges.get(jp).paths.get(je).path.size();j2++)
													{
														PathPort pp1 = new PathPort();
														pp1.line1 = tempNode.edges.get(jp).paths.get(je).path.get(j2).line1;
														pp1.line2 = tempNode.edges.get(jp).paths.get(je).path.get(j2).line2;
														path.path.add(pp1);
														path.weight = tempNode.edges.get(jp).paths.get(je).weight - negWeight;
													}
													newEdge1.paths.add(path);
														
												}
												newEdge1.weight = graph.get(j).edges.get(jp).weight - negWeight;
											}
										}
											
											
										newNode.edges.add(newEdge1);
											
										//------------------
									}
										
								}
									
								
								graph.add(newNode);									
									
							}
							catch(Exception e)
							{
								System.err.println("SOOMF: cant find previous node in fix");
							}
								
								
								
						}
						else
						{
							try
							{
								newNode.label = label;
								label++;
								newNode.parents.add(prevNode);
								prevNode.children.add(newNode);
								newNode.line1 = prevNode.line2 +1;
								newNode.type = tempNode.type;
								newNode.fName = tempNode.fName;
								newNode.line2 = count.get(k);
									
								for(int edjj = 0; edjj < tempNode.weightList.size(); edjj++)
								{
									if(tempNode.weightList.get(edjj).line >newNode.line1 && tempNode.weightList.get(edjj).line <newNode.line2)
									{
										newNode.weight = newNode.weight + tempNode.weightList.get(edjj).weight;
										LLVMWeight lw = new LLVMWeight();
										lw.line = tempNode.weightList.get(edjj).line;
										lw.weight = tempNode.weightList.get(edjj).weight;
										newNode.weightList.add(lw);
										if(tempNode.weightList.get(edjj).line == count.get(k))
										{
											newNode.criticalWeight = tempNode.weightList.get(edjj).weight;
										}
									}
								}
									
									

								//------------------ adding edge to prevNode
								Edge newEdge = new Edge();
								newEdge.label = newNode.label;
								newEdge.line1 = prevNode.line1;
								newEdge.line2 = prevNode.line2;
									
									
								///------newEdge.weight = prevNode.weight;
								newEdge.weight = prevNode.criticalWeight;
									
								//------------------- add path info
								PathPort pp = new PathPort();
								pp.line1 = newEdge.line1;
								pp.line2 = newEdge.line2;
								newEdge.myPath.path.add(pp);
								newEdge.myPath.weight = newEdge.weight;
									
								prevNode.edges.add(newEdge);		

								//-------------------------

								prevNode = newNode;
								
								graph.add(newNode);
									
									
									
							}
							catch(Exception e)
							{
								System.err.println(" cant find previous node in fix 2");
							}
								
						}
					}
				}
					
					
				graph.remove(j);
					
			}
			else
			{
				
				if(tempNode.label < idThreshold)
				{
					
					//-------------------- calculating the difference in weights
					int diffweight = 0;
					boolean goON = true;
					for(int je = 0; je < tempNode.weightList.size(); je++)
					{
						if(tempNode.weightList.get(je).line < count.get(0) && goON)
						{
							diffweight = tempNode.weightList.get(je).weight + diffweight;
						}
						else
						{
							goON = false;
						}
					}
					
					for(int p = 0; p < tempNode.parents.size(); p++)
					{
						try{											

							//---------------- editing the edges and the paths (the paths are for path-aware analysis)
							for(int ee = 0; ee < tempNode.parents.get(p).edges.size(); ee++)
							{
								if( tempNode.parents.get(p).edges.get(ee).label == tempNode.label)
								{										
											
									tempNode.parents.get(p).edges.get(ee).weight = tempNode.parents.get(p).edges.get(ee).weight + diffweight;
										
									tempNode.parents.get(p).edges.get(ee).myPath.weight = tempNode.parents.get(p).edges.get(ee).myPath.weight + diffweight;
											
									for(int jp = 0; jp < tempNode.parents.get(p).edges.get(ee).paths.size(); jp++)
									{
										tempNode.parents.get(p).edges.get(ee).paths.get(jp).weight = tempNode.parents.get(p).edges.get(ee).paths.get(jp).weight + diffweight;
									}
										
								}
							}
									

						}
						catch(Exception e){
							System.err.println("SOOMF: cant remove child in fix");
						}
							
							
					}
					
					

					try
					{		
						goON = false;
						boolean goON2 = false;
						
						if(tempNode.criticalWeight == 0)
						{
							for(int edjj = 0; edjj < graph.get(j).weightList.size(); edjj++)
							{
								if(goON2 || tempNode.weightList.get(edjj).line == count.get(0))
								{
									tempNode.criticalWeight = tempNode.weightList.get(edjj).weight + tempNode.criticalWeight;
									goON2 = true;
								}
									
							 }
						}
						
							
							
						//-------------- adding the edges to prevNode
							
							
						int diff = 0;
						goON = true;
						for(int je =0; je < tempNode.weightList.size(); je++)
						{
							if(goON && tempNode.weightList.get(je).line < count.get(0))
							{
								diff = diff + tempNode.weightList.get(je).weight;
							}
							else
							{
								goON = false;
							}
						}
													
							
						//----------------												
						for(int p = 0; p < tempNode.children.size(); p++)
						{

								for(int jp = 0 ; jp < tempNode.edges.size(); jp++)
								{
									if( tempNode.edges.get(jp).label == tempNode.children.get(p).label)
									{
										if(tempNode.edges.get(jp).weight >= diff)
										{
											tempNode.edges.get(jp).weight = tempNode.edges.get(jp).weight - diff;
										}
										if(tempNode.edges.get(jp).myPath.weight >= diff)
										{
											tempNode.edges.get(jp).myPath.weight = tempNode.edges.get(jp).myPath.weight - diff;												
										}
										
										
										for(int je = 0; je < tempNode.edges.get(jp).paths.size();je++)
										{
											if(tempNode.edges.get(jp).paths.get(je).weight >= diff)
											{
												tempNode.edges.get(jp).paths.get(je).weight = tempNode.edges.get(jp).paths.get(je).weight - diff;
											}
													
										}
									}
								}
									
								//------------------
							
								
						}							
							
					}
					catch(Exception e)
					{
						System.err.println("SOOMF: cant find previous node in fix");
					}
					
				}
				
				j++;

			}
		}
		
		
	}

	/**	
	 � * This Function writes the critical CFG to a file	
	 � * @param	: dir (directory of monitored program),filename (output file), dummies (Structure containing the dummy nodes),graph (Structure containing the CFG)
	 � * @return : void	
	*/
	public void newGraphOut(String dir,String filename, Vector<Dummy> dummies, Vector<Node> graph) throws  NumberFormatException, IOException
	{
		System.out.println(">>>>>>>>>>>>>> Writting the Critical CFG");
		
		BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/Graph/"+filename)); 
		BufferedWriter out1 = new BufferedWriter(new FileWriter(dir+"/Graph/"+filename+"_path"));
		
		for(int i = 0; i < graph.size(); i++)
		{
			out.write("v,"+graph.get(i).label+","+graph.get(i).fName + ","+graph.get(i).line1+","+graph.get(i).line2+"\n");
			out1.write("v,"+graph.get(i).label+","+graph.get(i).fName + ","+graph.get(i).line1+","+graph.get(i).line2+"\n");
		}
		
		for(int i = 0; i < dummies.size(); i++)
		{
			out.write("v,"+dummies.get(i).head.label+","+dummies.get(i).head.fName + ","+dummies.get(i).head.line1+","+dummies.get(i).head.line2+"\n");
			out1.write("v,"+dummies.get(i).head.label+","+dummies.get(i).head.fName + ","+dummies.get(i).head.line1+","+dummies.get(i).head.line2+"\n");

			out.write("v,"+dummies.get(i).tail.label+","+dummies.get(i).tail.fName + ","+dummies.get(i).tail.line1+","+dummies.get(i).tail.line2+"\n");
			out1.write("v,"+dummies.get(i).tail.label+","+dummies.get(i).tail.fName + ","+dummies.get(i).tail.line1+","+dummies.get(i).tail.line2+"\n");

		}
		
		for(int i = 0; i < graph.size(); i++)
		{
			for(int j = 0; j < graph.get(i).edges.size(); j++)
			{
				int weight = graph.get(i).edges.get(j).weight;
				out.write("e,"+graph.get(i).label+","+graph.get(i).edges.get(j).label+","+weight+"\n");
				out1.write("e,"+graph.get(i).label+","+graph.get(i).edges.get(j).label+","+weight+":");
				out1.write("MainPath > ");

				for(int jj = 0; jj < graph.get(i).edges.get(j).myPath.path.size();jj++)
				{
					out1.write(graph.get(i).edges.get(j).myPath.path.get(jj).line1 +","+graph.get(i).edges.get(j).myPath.path.get(jj).line2+",");
				}
				
				out1.write(":SidePaths > ");
				
				for(int jj = 0; jj < graph.get(i).edges.get(j).paths.size();jj++)
				{
					out1.write(jj+"=");
					for(int je =0; je < graph.get(i).edges.get(j).paths.get(jj).path.size();je++)
					{
						out1.write(graph.get(i).edges.get(j).paths.get(jj).path.get(je).line1+","+graph.get(i).edges.get(j).paths.get(jj).path.get(je).line2+",");
					}
					out1.write("weight="+graph.get(i).edges.get(j).paths.get(jj).weight+":");
				}
				out1.write("\n");
					

			}
		}
		for(int i = 0; i < dummies.size(); i++)
		{
			for(int j = 0; j < dummies.get(i).head.edges.size(); j++)
			{
				int weight = dummies.get(i).head.edges.get(j).weight;
				out.write("e,"+dummies.get(i).head.label+","+dummies.get(i).head.edges.get(j).label + ","+weight+"\n");
				out1.write("e,"+dummies.get(i).head.label+","+dummies.get(i).head.edges.get(j).label + ","+weight+":");
				out1.write("MainPath > ");

				for(int jj = 0; jj < dummies.get(i).head.edges.get(j).myPath.path.size();jj++)
				{
						out1.write(dummies.get(i).head.edges.get(j).myPath.path.get(jj).line1 +","+dummies.get(i).head.edges.get(j).myPath.path.get(jj).line2+",");
				}
				out1.write(":SidePaths > ");
				for(int jj = 0; jj < dummies.get(i).head.edges.get(j).paths.size();jj++)
				{
					out1.write(jj+"=");
					for(int je =0; je < dummies.get(i).head.edges.get(j).paths.get(jj).path.size();je++)
					{
						out1.write(dummies.get(i).head.edges.get(j).paths.get(jj).path.get(je).line1+","+dummies.get(i).head.edges.get(j).paths.get(jj).path.get(je).line2+",");
					}
					out1.write("weight="+dummies.get(i).head.edges.get(j).paths.get(jj).weight+":");
				}
				out1.write("\n");
			}
			for(int j = 0; j < dummies.get(i).tail.edges.size(); j++)
			{
				int weight = dummies.get(i).tail.edges.get(j).weight;
				out.write("e,"+dummies.get(i).tail.label+","+dummies.get(i).tail.edges.get(j).label + ","+weight+"\n");
				out1.write("e,"+dummies.get(i).tail.label+","+dummies.get(i).tail.edges.get(j).label + ","+weight+":");
				out1.write("MainPath > ");

				for(int jj = 0; jj < dummies.get(i).tail.edges.get(j).myPath.path.size();jj++)
				{
					out1.write(dummies.get(i).tail.edges.get(j).myPath.path.get(jj).line1 +","+dummies.get(i).tail.edges.get(j).myPath.path.get(jj).line2+",");
				}
				
				out1.write(":SidePaths > ");
				for(int jj = 0; jj < dummies.get(i).tail.edges.get(j).paths.size();jj++)
				{
					out1.write(jj+"=");
					for(int je =0; je < dummies.get(i).tail.edges.get(j).paths.get(jj).path.size();je++)
					{
						out1.write(dummies.get(i).tail.edges.get(j).paths.get(jj).path.get(je).line1+","+dummies.get(i).tail.edges.get(j).paths.get(jj).path.get(je).line2+",");
					}
					out1.write("weight="+dummies.get(i).tail.edges.get(j).paths.get(jj).weight+":");
				}
				out1.write("\n");
			}
		}
		out.close();
		out1.close();
	}

	
	/**	
	 � * This Function writes the critical CFG to a file	
	 � * @param	: dir (directory of monitored program),filename (input file)
	 � * @return : void	
	*/
	public void extractingLSP(String dir,String contents) throws  NumberFormatException, IOException
	{
		System.out.println(">>>>>>>>>>>>>> Finding LSP");
		int MIN = 100000;
		StringTokenizer tok;
		String line;
		int weight = 0;
		
		BufferedReader reader = new BufferedReader(new StringReader(contents));
		BufferedWriter out = new BufferedWriter(new FileWriter(dir+"/Graph/LSP")); 
		
		while ( (line = reader.readLine()) != null ) {				
			if ( line.equals("")) break;
			tok = new StringTokenizer(line,",");
			int j = 0;
			int node1 = 0;
			int node2 = 0;
			while (tok.hasMoreTokens())
			{
				if(j == 0)
				{
					String tempS = String.valueOf(tok.nextToken().trim());
					if(!tempS.contentEquals("e"))
					{
						break;
					}
				}
				if(j == 1)
				{
					node1 = Integer.valueOf(tok.nextToken().trim()).intValue();
				}
				if(j == 2)
				{
					node2 = Integer.valueOf(tok.nextToken().trim()).intValue();
				}

				if (j == 3)
				{
					weight = Integer.valueOf(tok.nextToken().trim()).intValue();
					if(node1 > 0 && node2 > 0)
					{
						if(weight == 0)
						{
							weight++;
						}
						if (weight < MIN)
						{
							MIN = weight;
						}
					}
					
				}
				j++;

				
			}
		}
		
		if(MIN == 100000)
		{
			out.write("No Sample \n");
		}
		else
		{
			out.write("LSP: "+ MIN + "\n");
		}
		
		out.close();
	}
	
	
	/**	
	 � * This Function finds the back edges in the Critical CFG to extract the loops
	 � * @param	: graph (Structure containing the CFG),dummies (Structure containing the dummy nodes), dfsLoops(Structure containing loop structures)
	 � * @return : void	
	*/
	public void findBackEdge(Vector<Node> graph, Vector<Dummy> dummies, Vector<DFSLoop> dfsLoops)
	{		
		Vector<Node> dfsPath = new Vector<Node>();

		System.out.println(">>>>>>>>>>>>>> Finding the Loop Structures");
		
		for(int i = 0; i < dummies.size(); i++)
		{
			if(dummies.get(i).head.parents.size() == 0)
			{
				//calling the DFS
				dfsPath.add(dummies.get(i).head);
				DFS(dummies.get(i).head, dfsLoops,dfsPath);
			}
			else
			{
				if(dummies.get(i).tail.parents.size() == 0)
				{
					//calling the DFS
					dfsPath.add(dummies.get(i).tail);
					DFS(dummies.get(i).tail, dfsLoops,dfsPath);

				}
			}
		}
		
		for(int i = 0; i < graph.size(); i++)
		{
			if(graph.get(i).parents.size() == 0)
			{
				// calling DFS
				dfsPath.add(graph.get(i));
				DFS(graph.get(i), dfsLoops,dfsPath);

			}
		}
	}
	
	
	
	/**	
	 � * This Function calculates the number of different weights an incoming edge can have
	 � * @param	: graph (Structure containing the CFG),dummies (Structure containing the dummy nodes), dfsLoops(Structure containing loop structures)
	 � * @return : void	
	*/
	public void bfsOutdegree(Vector<Node> graph,Vector<Dummy> dummies,Vector<DFSLoop> dfsLoops)
	{
		System.out.println(">>>>>>>>>>>>>> Finding indegree of each node");
		

		Vector<Node> process = new Vector<Node>();
		boolean found = false;
		
		for(int i = 0; i < dummies.size(); i++)
		{
			if(dummies.get(i).head.parents.size() == 0)
			{
				process.add(dummies.get(i).head);
			}
			else
			{
				if(dummies.get(i).tail.parents.size() == 0)
				{
					process.add(dummies.get(i).tail);

				}
			}
			
			while(process.size() > 0)
			{
				Node node = process.get(0);
				int origbfs = node.bfsIndegree;
				node.bfsIndegree = 0;
				node.bfsProcessed = true;
				
				
				if(node.parents.size() > 0)
				{
					for(int j= 0 ; j < node.parents.size(); j++)
					{
						Node parent = node.parents.get(j);
						
						if(node.loop == false)
						{
							node.bfsIndegree = node.bfsIndegree + parent.bfsIndegree;
						}
						else
						{
							
							found = false;
							

							if(parent.loop == true)
							{								

								for(int k = 0; k < dfsLoops.size(); k++)
								{
									if(dfsLoops.get(k).source.label == parent.label && dfsLoops.get(k).dest.label == node.label)
									{

										found = true;
										break;
									}
								}
							}
							
							
							if(found == false)
							{
								node.bfsIndegree = node.bfsIndegree + parent.bfsIndegree;
							}
						}
						
					}
				}
				else
				{
					node.bfsIndegree = 1;
					
				}
				
			

				for(int j= 0; j < node.children.size(); j++)
				{
					if(node.children.get(j).bfsProcessed == false)
					{
						process.add(node.children.get(j));

					}
					else
					{
						if(node.bfsIndegree != origbfs)
						{
							process.add(node.children.get(j));

						}
					}
				}
				
				process.remove(0);
				
			}

		}
		
		
		for(int i = 0; i < graph.size(); i++)
		{
			if(graph.get(i).parents.size() == 0)
			{
				
				process.add(graph.get(i));
				
				while(process.size() > 0)
				{
					Node n = process.get(0);
					int origbfs = n.bfsIndegree;
					n.bfsIndegree = 0;
					n.bfsProcessed = true;
					
					
					if(n.parents.size() > 0)
					{
						for(int j= 0 ; j < n.parents.size(); j++)
						{
							Node parent = n.parents.get(j);
							
							if(n.loop == false)
							{
								n.bfsIndegree = n.bfsIndegree + parent.bfsIndegree;
							}
							else
							{
								
								found = false;
								

								if(parent.loop == true)
								{								

									for(int k = 0; k < dfsLoops.size(); k++)
									{
										if(dfsLoops.get(k).source.label == parent.label && dfsLoops.get(k).dest.label == n.label)
										{

											found = true;
											break;
										}
									}
								}
								
								
								if(found == false)
								{
									n.bfsIndegree = n.bfsIndegree + parent.bfsIndegree;
								}
							}
							
						}
					}
					else
					{
						n.bfsIndegree = 1;
						
					}
					
				

					for(int j= 0; j < n.children.size(); j++)
					{
						if(n.children.get(j).bfsProcessed == false)
						{
							process.add(n.children.get(j));

						}
						else
						{
							if(n.bfsIndegree != origbfs)
							{
								process.add(n.children.get(j));

							}
						}
					}
					
					process.remove(0);
					
				}


			}
		}

	}
	
	
	
	
	private static void DFS(Node pNode, Vector<DFSLoop> dfsLoops, Vector<Node> dfsPath)
	{

	    
		if(pNode.children.size() > 0)
		{
			pNode.dfsProcessed = true;
			
			for(int i = 0; i < pNode.edges.size(); i++)
			{
				Edge edge = pNode.edges.get(i);
				 if(edge.dfsProcessed == false)
				 {
					 edge.dfsProcessed = true;
					 for(int j = 0; j < pNode.children.size(); j++)
					 {
						 if(pNode.children.get(j).label == edge.label)
						 {
							 
							 if(pNode.children.get(j).dfsProcessed == false)
							 {
								 dfsPath.add(pNode.children.get(j));
								 DFS(pNode.children.get(j), dfsLoops, dfsPath);

							 }
							 else
							 {
								 DFSLoop newLoop = new DFSLoop();
								 
								 newLoop.source = pNode;
								 newLoop.dest = pNode.children.get(j);
								 
								 
								 Vector<Integer> name = new Vector<Integer>();
								 
								 
								 boolean index = false;
								 for(int k = 0; k < dfsPath.size(); k++ )
								 {
									 if(dfsPath.get(k).label == pNode.children.get(j).label)
									 {
										 index = true;
										 pNode.children.get(j).loop = true;
										 pNode.loop = true;

									 }
									 
									 if(index == true)
									 {
										 newLoop.loop.add(dfsPath.get(k));
										 name.add(dfsPath.get(k).label);
									 }
								 }
								 
								 SortLines(name);
								 
								 for(int kk = 0; kk < name.size(); kk++)
								 {
									 newLoop.name = newLoop.name + Integer.toString(name.get(kk));
								 }
								 
								 dfsLoops.add(newLoop);
								 
							 }
						 }
					 }
				 }
			}
		}
		
		//------ make it unprocessed
		pNode.dfsProcessed = false;
		
		//------ make edges unporcessed
		for(int i = 0; i < pNode.edges.size(); i++)
		{
			pNode.edges.get(i).dfsProcessed = false;
		}
		
		//------ remove itself from the dfs path
		dfsPath.remove(dfsPath.size() - 1);
		
	}
	
	
	
	
	
	/**	
	 � * This Function sorts the code lines in ascending order	
	 � * @param	: countLines (Structure containing the code lines)
	 � * @return : void	
	*/
	private static void SortLines(Vector<Integer> countLines)
	{
		boolean swapped = false;
		do
		{
			swapped = false;
			for(int i =1; i < countLines.size(); i++)
			{
				if(countLines.get(i) < countLines.get(i-1))
				{
					int temp = countLines.get(i-1);
					countLines.remove(i-1);
					countLines.insertElementAt(temp, i);
					swapped = true;	
				}
			}
			
		}while(swapped == true);			
	}
	

}