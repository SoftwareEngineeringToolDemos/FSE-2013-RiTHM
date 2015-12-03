package ca.uwaterloo.esg.rvtool.TTRV.Heuristics;

import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintStream;
import java.util.*;

import au.com.bytecode.opencsv.*;

public class CFG {
	/** working copy of adjacency matrix of CFG */
	int graph[][];
	/** original adjacency matrix of CFG */
	int orig_graph[][];
	/** number of nodes in the original CFG */
	int N;
	/** node label map */
	int nodelabels[];
	/** desired maximum sampling period */
	int desired_MSP;
	/** starting node index */
	int start;
	/** terminating node index */
	int end;

	/** pruned nodes */
	Vector<Integer> pruned_nodes;
	/** merge node stack */
	Vector<Integer> merge_stack;
	/** created selfloops */
	Vector<CFGEdge> newselfloops;
	/** initial CFG's selfloops */
	Vector<CFGEdge> selfloops;
	/** detailed nodes of CFG */
	Vector<CFGNode> detailednodes;
	/** debug stream */
	PrintStream fout;

	CFG(Vector<Integer> nodes, Vector<CFGEdge> edges, int period,
			Vector<CFGNode> dnodes) {
		N = nodes.size();
		graph = new int[N][N];
		orig_graph = new int[N][N];
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				graph[i][j] = 0;
				orig_graph[i][j] = 0;
			}
		}

		nodelabels = new int[N];
		for (int i = 0; i < N; i++) {
			nodelabels[i] = nodes.get(i);
		}

		selfloops = new Vector<CFGEdge>();
		constructCFG(edges);
		findStartAndEnd();

		desired_MSP = period;

		pruned_nodes = new Vector<Integer>();
		merge_stack = new Vector<Integer>();
		newselfloops = new Vector<CFGEdge>();
		detailednodes = (Vector<CFGNode>) dnodes.clone();
		fout = System.out;
	}

	private void findStartAndEnd() {
		boolean foundStart = false;
		boolean foundEnd = false;
		startNode: for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (orig_graph[j][i] > 0)
					continue startNode;
			}
			start = i;
			foundStart = true;
			break;
		}
		endNode: for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (orig_graph[i][j] > 0)
					continue endNode;
			}
			end = i;
			foundEnd = true;
			break;
		}

		if (!foundStart || !foundEnd) {
			System.out
					.println("WARNING: Could not find starting and ending node in the CFG... this may cause problems when running the heuristics.");
		}
	}

	private int findidx(int label) {
		for (int i = 0; i < N; i++) {
			if (nodelabels[i] == label)
				return i;
		}
		return -1;
	}

	/**
	 * Construct CFG from the list of edges/arcs. This function assumes that all
	 * of the node labels have already been acquired.
	 * 
	 * @param edges
	 *            the list of arcs/edges in the CFG
	 */
	private void constructCFG(Vector<CFGEdge> edges) {
		int n1, n2;
		CFGEdge edge;
		for (int i = 0; i < edges.size(); i++) {
			edge = edges.get(i);
			n1 = findidx(edge.x);
			n2 = findidx(edge.y);
			if (n1 == n2) { // ignore self loops in the heuristics
				selfloops.add(edge);
			} else {
				// graph[n1][n2] = edge.weight;
				orig_graph[n1][n2] = edge.weight;
			}
		}
	}

	private void collapseNode(int idx, boolean prune) {
		boolean incoming[] = new boolean[N];
		boolean outgoing[] = new boolean[N];

		for (int i = 0; i < N; i++) {
			incoming[i] = false;
			outgoing[i] = false;
		}

		// obtain all incoming and outgoing arcs from node to be collapsed
		for (int i = 0; i < N; i++) {
			if (graph[idx][i] > 0)
				outgoing[i] = true;
			if (graph[i][idx] > 0)
				incoming[i] = true;
		}

		// update weights of the newly formed arcs
		for (int i = 0; i < N; i++) {
			if (!incoming[i])
				continue;

			for (int j = 0; j < N; j++) {
				if (!outgoing[j])
					continue;
				if (graph[i][j] > 0) {
					graph[i][j] = Math.min(graph[i][idx] + graph[idx][j],
							graph[i][j]);
				} else {
					graph[i][j] = graph[i][idx] + graph[idx][j];
				}

				// prune self loops
				if (i == j) {
					newselfloops.add(new CFGEdge(nodelabels[i], nodelabels[i],
							graph[i][j]));
					graph[i][j] = 0;
				}
			}
		}

		// finally, remove node from CFG by setting the corresponding row and
		// column in the
		// adjacency matrix to zero
		for (int i = 0; i < N; i++) {
			graph[idx][i] = graph[i][idx] = 0;
		}

		// add the collapsed node to the stack
		if (!prune)
			merge_stack.add(idx);
	}

	/**
	 * After the CFG is constructed, remove all edges with a weight higher than
	 * the desired sampling period of the sampling-based monitor.
	 */
	private void pruneCFG() {
		boolean flag;
		boolean nonzero;
		do {
			flag = false;
			prune: for (int i = 0; i < N; i++) {
				// continue on to the next node if the current is the start or
				// end node of the CFG
				if (i == start || i == end)
					continue prune;
				nonzero = false;
				for (int j = 0; j < N; j++) {
					if (graph[i][j] > 0 || graph[j][i] > 0)
						nonzero = true;
					// if an existing incoming/outgoing arc of node i is less
					// than the desired MSP, stop current search and continue
					// with the next
					if ((graph[i][j] > 0 && graph[i][j] < desired_MSP)
							|| (graph[j][i] > 0 && graph[j][i] < desired_MSP)) {
						continue prune;
					}
				}

				// remove vertex where all incoming and outgoing arcs >= desired
				// sampling period
				// by collapsing the node
				if (nonzero) {
					collapseNode(i, true);
					pruned_nodes.add(i);
					flag = true;
				}
			}
		} while (flag);
	}

	/**
	 * Returns the maximum sampling period of the working copy of the CFG
	 * 
	 * @return the maximum sampling period
	 */
	private int getMSP() {
		int msp = Integer.MAX_VALUE;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (graph[i][j] > 0 && graph[i][j] < msp) {
					msp = graph[i][j];
				}
			}
		}
		return msp;
	}

	public void printNodeCollapseHistory(String type, String filepath,
			long t_exec) {
		int temp;
		System.out
				.println("=========================================================");
		System.out.println(type + " Results");
		System.out
				.println("=========================================================");
		System.out.println("Starting node: " + nodelabels[start]);
		System.out.println("Ending   node: " + nodelabels[end]);
		System.out.println("Desired   MSP: " + desired_MSP);
		System.out.println("Actual    MSP: " + getMSP());
		System.out.println("Pruned  nodes: " + pruned_nodes.size());
		System.out.println("Number of critical nodes to delete: "
				+ merge_stack.size());
		System.out.println("Total number of critical nodes in original CFG: "
				+ N);
		System.out.println("Execution time (ns): " + t_exec);
		for (int i = 0; i < merge_stack.size(); i++) {
			temp = merge_stack.get(i);
			System.out.println("v," + detailednodes.get(temp).label + ","
					+ detailednodes.get(temp).function + ","
					+ detailednodes.get(temp).line1 + ","
					+ detailednodes.get(temp).line2);
		}

		if (fout != null) {
			fout.print("=========================================================\n");
			fout.print(type + " Results\n");
			fout.print("=========================================================\n");
			fout.print("Starting node: " + nodelabels[start] + "\n");
			fout.print("Ending   node: " + nodelabels[end] + "\n");
			fout.print("Desired   MSP: " + desired_MSP + "\n");
			fout.print("Actual    MSP: " + getMSP() + "\n");
			fout.print("Pruned  nodes: " + pruned_nodes.size() + "\n");
			fout.print("Number of critical nodes to delete: "
					+ merge_stack.size() + "\n");
			fout.print("Total number of critical nodes in original CFG: "
					+ N + "\n");
			fout.print("Execution time (ns): " + t_exec + "\n");
			for (int i = 0; i < merge_stack.size(); i++) {
				temp = merge_stack.get(i);
				fout.print("v," + detailednodes.get(temp).label + ","
						+ detailednodes.get(temp).function + ","
						+ detailednodes.get(temp).line1 + ","
						+ detailednodes.get(temp).line2 + "\n");
			}
		}
	}

	public void printCurrentCFG() {
		System.out
				.println("---------------------------------------------------------");
		System.out.println("Current CFG");
		System.out
				.println("---------------------------------------------------------");
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (graph[i][j] > 0) {
					System.out.println(nodelabels[i] + " -> " + nodelabels[j]
							+ "  [" + graph[i][j] + "]");
				}
			}
		}

		if (fout != null) {
			fout.print("---------------------------------------------------------\n");
			fout.print("Current CFG\n");
			fout.print("---------------------------------------------------------\n");
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					if (graph[i][j] > 0) {
						fout.print(nodelabels[i] + " -> " + nodelabels[j]
								+ "  [" + graph[i][j] + "]\n");
					}
				}
			}
		}
	}

	private void resetCFG() {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				graph[i][j] = orig_graph[i][j];
			}
		}
	}

	/**
	 * Greedy Heuristic
	 */
	private void runHeuristic1() {
		long t_begin = System.nanoTime();
		int msp;
		int tempcount, maxcount = 0;
		int maxidx;

		resetCFG();
		pruned_nodes.clear();
		merge_stack.clear();

		// collapse nodes that already satisfy the MSP, i.e., all nodes with
		// edges >= desired sampling period
		pruneCFG();

		while (true) {
			msp = getMSP();

			// we are done when all arcs in the working copy of the CFG are >=
			// desired sampling period
			if ((getMSP() >= desired_MSP) || (merge_stack.size() >= (N - 2)))
				break;

			maxidx = -1;
			maxcount = 0;

			// determine greedily the node to collapse by taking the node with
			// the smallest incoming and outgoing arc weight sum
			greedynode: for (int i = 0; i < N; i++) {
				if (i == start || i == end)
					continue greedynode;
				tempcount = 0;
				for (int j = 0; j < N; j++) {
					if (graph[i][j] == msp) {
						tempcount++;
					}
					if (graph[j][i] == msp) {
						tempcount++;
					}
				}

				if (tempcount > maxcount) {
					maxcount = tempcount;
					maxidx = i;
				}
			}

			// maxidx corresponds to the node to collapse
			if (maxidx < 0) {
				System.out
						.println("No more nodes can be collapsed; cannot continue running greedy heuristic. Terminating...");
				return;
			}
			collapseNode(maxidx, false);
		}
//		printNodeCollapseHistory("Greedy Heuristic (== MSP)", null,
//				System.nanoTime() - t_begin);
//		printCurrentCFG();
		if (getMSP() < desired_MSP)
			System.out.println(">>>>> ERROR: Desired sampling period of "
					+ desired_MSP
					+ " could not be obtained. The MSP in this case is "
					+ getMSP());
	}

	/**
	 * Greedy Heuristic
	 */
	private void runHeuristic1b() {
		long t_begin = System.nanoTime();
		int tempcount, maxcount = 0;
		int maxidx;

		resetCFG();
		pruned_nodes.clear();
		merge_stack.clear();

		// collapse nodes that already satisfy the MSP, i.e., all nodes with
		// edges >= desired sampling period
		pruneCFG();

		while (true) {
			// we are done when all arcs in the working copy of the CFG are >=
			// desired sampling period
			if ((getMSP() >= desired_MSP) || (merge_stack.size() >= (N - 2)))
				break;

			maxidx = -1;
			maxcount = 0;

			// determine greedily the node to collapse by taking the node with
			// the smallest incoming and outgoing arc weight sum
			greedynode: for (int i = 0; i < N; i++) {
				if (i == start || i == end)
					continue greedynode;
				tempcount = 0;
				for (int j = 0; j < N; j++) {
					if (graph[i][j] > 0 && graph[i][j] <= desired_MSP) {
						tempcount++;
					}
					if (graph[j][i] > 0 && graph[j][i] <= desired_MSP) {
						tempcount++;
					}
				}

				if (tempcount > maxcount) {
					maxcount = tempcount;
					maxidx = i;
				}
			}

			// maxidx corresponds to the node to collapse
			if (maxidx < 0) {
				System.out
						.println("No more nodes can be collapsed; cannot continue running greedy heuristic. Terminating...");
				return;
			}
			collapseNode(maxidx, false);
		}
//		printNodeCollapseHistory("Greedy Heuristic (<= desired MSP)", null,
//				System.nanoTime() - t_begin);
//		printCurrentCFG();
		if (getMSP() < desired_MSP)
			System.out.println(">>>>> ERROR: Desired sampling period of "
					+ desired_MSP
					+ " could not be obtained. The MSP in this case is "
					+ getMSP());
	}

	/**
	 * Vertex cover-based heuristic (arbitrary edge removal)
	 */
	private void runHeuristic2() {
		long t_begin = System.nanoTime();
		int initialvc_count;
		Vector<Integer> vertexcover = new Vector<Integer>();
		Vector<CFGEdge> edges = new Vector<CFGEdge>();

		int temp;
		int x, y;
		int iter = 0;

		Random rand = new Random();

		resetCFG();
		merge_stack.clear();
		pruned_nodes.clear();

		while (true) {
			iter++;
			// we are done when all arcs in the working copy of the CFG are >=
			// desired sampling period
			if ((getMSP() >= desired_MSP) || (merge_stack.size() >= (N - 2)))
				break;

			// reset all temporary vectors used to construct the vertex cover
			vertexcover.clear();
			edges.clear();

			// construct list of arcs in current CFG
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					if (graph[i][j] > 0) {
						edges.add(new CFGEdge(i, j, graph[i][j]));
					}
				}
			}

			// include all nodes that satisfy the required sampling period into
			// the vertex cover
			initialvc_count = 0;
			initialpartialset: for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					if ((graph[i][j] > 0 && graph[i][j] < desired_MSP)
							|| (graph[j][i] > 0 && graph[j][i] < desired_MSP))
						continue initialpartialset;
				}

				// remove all incoming and outgoing arcs to and from vertex i
				for (int k = 0; k < edges.size(); k++) {
					if (edges.elementAt(k).x == i || edges.elementAt(k).y == i) {
						edges.remove(k);
						k--;
					}
				}
				vertexcover.add(i);
				initialvc_count++;
			}

			// with the remaining nodes, construct a 2-approximate minimum
			// (non-weighted) vertex cover with the current partial cover
			// pseudocode (see CLRS (3e) 35.1)
			while (!edges.isEmpty()) {
				temp = rand.nextInt(edges.size());
				x = edges.elementAt(temp).x;
				y = edges.elementAt(temp).y;
				vertexcover.add(x);
				vertexcover.add(y);
				for (int i = 0; i < edges.size(); i++) {
					if (edges.elementAt(i).x == x || edges.elementAt(i).y == y
							|| edges.elementAt(i).y == x
							|| edges.elementAt(i).x == y) {
						edges.remove(i);
						i--;
					}
				}
			}

			// collapse nodes in the vertex cover
			System.out.println("VC Size: " + vertexcover.size());
			for (int i = initialvc_count; i < vertexcover.size(); i++) {
				collapseNode(vertexcover.get(i), false);
			}
		}
//		printNodeCollapseHistory("Vertex Cover", null, System.nanoTime()
//				- t_begin);
//		printCurrentCFG();
		if (getMSP() < desired_MSP)
			System.out.println(">>>>> ERROR: Desired sampling period of "
					+ desired_MSP
					+ " could not be obtained. The MSP in this case is "
					+ getMSP());
		System.out.println("Iterations: " + iter);
	}

	/**
	 * Vertex cover-based heuristic (select edges by least weight)
	 */
	private void runHeuristic2b() {
		long t_begin = System.nanoTime();
		int initialvc_count;
		Vector<Integer> vertexcover = new Vector<Integer>();
		Vector<CFGEdge> edges = new Vector<CFGEdge>();
		Vector<CFGEdge> minedges = new Vector<CFGEdge>();

		int temp;
		int x, y;
		int msp;

		int iter = 0;

		Random rand = new Random();

		resetCFG();
		merge_stack.clear();
		pruned_nodes.clear();

		while (true) {
			iter++;
			// we are done when all arcs in the working copy of the CFG are >=
			// desired sampling period
			if ((getMSP() >= desired_MSP) || (merge_stack.size() >= (N - 2)))
				break;

			// reset all temporary vectors used to construct the vertex cover
			vertexcover.clear();
			edges.clear();

			// construct list of arcs in current CFG
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					if (graph[i][j] > 0) {
						edges.add(new CFGEdge(i, j, graph[i][j]));
					}
				}
			}

			// include all nodes that satisfy the required sampling period into
			// the vertex cover
			initialvc_count = 0;
			initialpartialset: for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					if ((graph[i][j] > 0 && graph[i][j] < desired_MSP)
							|| (graph[j][i] > 0 && graph[j][i] < desired_MSP))
						continue initialpartialset;
				}

				// remove all incoming and outgoing arcs to and from vertex i
				for (int k = 0; k < edges.size(); k++) {
					if (edges.elementAt(k).x == i || edges.elementAt(k).y == i) {
						edges.remove(k);
						k--;
					}
				}
				vertexcover.add(i);
				initialvc_count++;
			}

			// weighted vertex cover approximation
			while (!edges.isEmpty()) {
				minedges.clear();
				msp = Integer.MAX_VALUE;
				for (int i = 0; i < edges.size(); i++) {
					if (edges.get(i).weight < msp)
						msp = edges.get(i).weight;
				}

				for (int i = 0; i < edges.size(); i++) {
					if (edges.get(i).weight == msp) {
						minedges.add(new CFGEdge(edges.get(i).x,
								edges.get(i).y, edges.get(i).weight));
					}
				}

				temp = rand.nextInt(minedges.size());
				x = minedges.get(temp).x;
				y = minedges.get(temp).y;

				vertexcover.add(x);
				vertexcover.add(y);
				for (int i = 0; i < edges.size(); i++) {
					if (edges.elementAt(i).x == x || edges.elementAt(i).y == y
							|| edges.elementAt(i).y == x
							|| edges.elementAt(i).x == y) {
						edges.remove(i);
						i--;
					}
				}
			}

			// collapse nodes in the vertex cover
			System.out.println("VC Size: " + vertexcover.size());
			for (int i = initialvc_count; i < vertexcover.size(); i++) {
				collapseNode(vertexcover.get(i), false);
			}
		}
//		printNodeCollapseHistory("Vertex Cover (quasi-weighted)", null,
//				System.nanoTime() - t_begin);
//		printCurrentCFG();
		if (getMSP() < desired_MSP)
			System.out.println(">>>>> ERROR: Desired sampling period of "
					+ desired_MSP
					+ " could not be obtained. The MSP in this case is "
					+ getMSP());
		System.out.println("Iterations: " + iter);
	}

	/**
	 * Vertex cover-based heuristic (select edges by least weight, subset of
	 * edges)
	 */
	private void runHeuristic2c(boolean print) {
		long t_begin = System.nanoTime();
		Vector<Integer> vertexcover = new Vector<Integer>();
		Vector<CFGEdge> edges = new Vector<CFGEdge>();

		int temp;
		int x, y;

		int iter = 0;

		Random rand = new Random();

		resetCFG();
		merge_stack.clear();
		pruned_nodes.clear();
		pruneCFG();

		while (true) {
			// we are done when all arcs in the working copy of the CFG are >=
			// desired sampling period
			if ((getMSP() >= desired_MSP) || (merge_stack.size() >= (N - 2)))
				break;

			iter++;

			// reset all temporary vectors used to construct the vertex cover
			vertexcover.clear();
			edges.clear();
			// Runtime.getRuntime().gc();

			// construct list of arcs in current CFG that are less than the
			// desired MSP
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					if (graph[i][j] > 0 && graph[i][j] < desired_MSP) {
						edges.add(new CFGEdge(i, j, graph[i][j]));
					}
				}
			}

			// vertex cover approximation
			while (!edges.isEmpty()) {
				temp = rand.nextInt(edges.size());
				x = edges.get(temp).x;
				y = edges.get(temp).y;

				vertexcover.add(x);
				vertexcover.add(y);
				for (int i = 0; i < edges.size(); i++) {
					if (edges.elementAt(i).x == x || edges.elementAt(i).y == y
							|| edges.elementAt(i).y == x
							|| edges.elementAt(i).x == y) {
						edges.remove(i);
						i--;
					}
				}
			}

			// collapse nodes in the vertex cover
			for (int i = 0; i < vertexcover.size(); i++) {
				collapseNode(vertexcover.get(i), false);
			}
		}
		if (print) {
//			printNodeCollapseHistory("Vertex Cover (subset arcs)", null,
//					System.nanoTime() - t_begin);
//			 printCurrentCFG();
		}
		if (getMSP() < desired_MSP)
			System.out.println(">>>>> ERROR: Desired sampling period of "
					+ desired_MSP
					+ " could not be obtained. The MSP in this case is "
					+ getMSP());
	}
	
	public Vector<Integer> runApproxMinVCHeuristic(final int nrepeat) {
		long t_begin = System.nanoTime();
		int mincollapsed = Integer.MAX_VALUE;
		int graph[][] = new int[this.N][this.N];
		Vector<Integer> pruned_nodes = new Vector<Integer>();
		Vector<Integer> merge_stack = new Vector<Integer>();
		Vector<CFGEdge> newselfloops = new Vector<CFGEdge>();

		for (int i = 0; i < nrepeat; i++) {
			this.runHeuristic2c(false);
			if (this.merge_stack.size() < mincollapsed) {
				mincollapsed = this.merge_stack.size();
				pruned_nodes.clear();
				merge_stack.clear();
				newselfloops.clear();

				pruned_nodes = (Vector<Integer>) this.pruned_nodes.clone();
				merge_stack = (Vector<Integer>) this.merge_stack.clone();
				newselfloops = (Vector<CFGEdge>) this.newselfloops.clone();
				for (int j = 0; j < this.N; j++) {
					for (int k = 0; k < this.N; k++) {
						graph[j][k] = this.graph[j][k];
					}
				}
			}
		}
		long t_end = System.nanoTime();

		this.pruned_nodes.clear();
		this.merge_stack.clear();
		this.newselfloops.clear();

		this.pruned_nodes = (Vector<Integer>) pruned_nodes.clone();
		this.merge_stack = (Vector<Integer>) merge_stack.clone();
		this.newselfloops = (Vector<CFGEdge>) newselfloops.clone();
		for (int j = 0; j < this.N; j++) {
			for (int k = 0; k < this.N; k++) {
				this.graph[j][k] = graph[j][k];
			}
		}

		this.printNodeCollapseHistory("Vertex Cover (subset arcs)", null,
				t_end - t_begin);
		return this.merge_stack;
	}
	
	public Vector<Integer> runGreedyHeuristic() {
		this.runHeuristic1b();
		return this.merge_stack;
	}
	
	public enum HeuristicType { GREEDY, VERTEX_COVER, NONE }
	
	public static void PrintUsage() {
		System.err.println("TTRVHeuristics <heuristic type (heu1|heu2)> <input CFG filepath> <desired sampling period> [<results output filepath>] [VC iterations]");
	}

	/**
	 * @param args
	 *            - must be in the following order: heuristic input_cfg desired_SP
	 *            [output_file] where the output file is optional
	 */
	public static void main(String[] args) {
		if (args.length < 3 || args.length > 5) {
			System.err.println("ERROR: Invalid number of arguments");
			PrintUsage();
			return;
		}
		
		HeuristicType hType = HeuristicType.NONE;
		if ( args[0].compareToIgnoreCase("heu1") == 0 ) {
			hType = HeuristicType.GREEDY;
		} else if ( args[0].compareToIgnoreCase("heu2") == 0 ) {
			hType = HeuristicType.VERTEX_COVER;
		} else {
			System.err.println("ERROR: Invalid heuristic type. Must either be \"heu1\" (greedy) or \"heu2\" (vertex cover)");
		}
		assert hType != HeuristicType.NONE;

		String inputfile = args[1];
		int desired_period = Integer.parseInt(args[2]);
		String outputfile = (args.length >= 4) ? args[3] : "h_result.txt";
		final int nrepeat = (args.length == 5) ? Integer.parseInt(args[4]) : 500;

		Vector<Integer> nodes = new Vector<Integer>();
		Vector<CFGEdge> edges = new Vector<CFGEdge>();
		Vector<CFGNode> detailednodes = new Vector<CFGNode>();

		try {
			FileReader freader = new FileReader(inputfile);
			FileWriter fwriter = new FileWriter(outputfile);
			BufferedWriter out = new BufferedWriter(fwriter);
			CSVReader reader = new CSVReader(freader);
			String[] nextLine;
			while ((nextLine = reader.readNext()) != null) {
				if (nextLine[0].compareToIgnoreCase("v") == 0
						&& nextLine.length == 5) {
					nodes.add(Integer.parseInt(nextLine[1]));
					detailednodes.add(new CFGNode(
							Integer.parseInt(nextLine[1]), Integer
									.parseInt(nextLine[3]), Integer
									.parseInt(nextLine[4]), nextLine[2]));
				} else if (nextLine[0].compareToIgnoreCase("e") == 0
						&& nextLine.length == 4) {
					edges.add(new CFGEdge(Integer.parseInt(nextLine[1]),
							Integer.parseInt(nextLine[2]), Integer
									.parseInt(nextLine[3])));
				}
			}
			CFG cfg = new CFG(nodes, edges, desired_period, detailednodes);
			
			Vector<Integer> result = new Vector<Integer>();
			if (hType == HeuristicType.GREEDY) {
				result = cfg.runGreedyHeuristic();
			} else if (hType == HeuristicType.VERTEX_COVER) {
				result = cfg.runApproxMinVCHeuristic(nrepeat);
			}
			
			for (Integer id : result) {
				out.write("x" + id + "\n");
			}
			
			out.close();
			reader.close();
		} catch (FileNotFoundException e) {
			System.err.println("FileNotFoundException: " + e.getMessage());
			e.printStackTrace();
		} catch (IOException e) {
			System.err.println("IOException: " + e.getMessage());
			e.printStackTrace();
		}
	}

}
