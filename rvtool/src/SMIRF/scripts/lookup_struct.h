#define MAX_LINE_LENGTH 100
#define OUTPUT_FILE     "instrumentation.txt"

typedef struct {
	char *v;
	char *functionName;
	char  *sequenceNumber;
	char *nodeName;
	char *nodeType;
	char *start_line_num;
	char *end_line_num;
	char *weight;
} lookup_table_t;
