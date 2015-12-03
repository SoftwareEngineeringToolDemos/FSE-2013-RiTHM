#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>

/* This example reads the configuration file 'example.cfg' and displays
 * some of its contents.
 */

int main(int argc, char **argv)
{
  config_t cfg;
  config_setting_t *setting;
  const char *str;
  FILE *fp, *mon_vars_file;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <spec_file> <output_spec>\n", argv[0]);
    return(EXIT_FAILURE);
  }

  config_init(&cfg);

  fp = fopen(argv[2],"w");
  if(!fp) {
	config_destroy(&cfg);
  	fprintf(stderr,"Cannot open input.spec for writing\n");
	return(EXIT_FAILURE);
  }

  mon_vars_file = fopen("mon_vars.out","w");
  if(!mon_vars_file) {
  	fclose(fp);
	config_destroy(&cfg);
	fprintf(stderr, "Cannot open the mon_var.out file for writing\n");
	return(EXIT_FAILURE);
  }

  /* Read the file. If there is an error, report it and exit. */
  if(! config_read_file(&cfg, argv[1]))
  {
    fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
            config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    fclose(fp);
    return(EXIT_FAILURE);
  }

  /* Output a list of all books in the inventory. */
  setting = config_lookup(&cfg, "program_variables");
  if(setting != NULL)
  {
    int count = config_setting_length(setting);
    int i;

    char *var, *type, *scope, *function=NULL; 

    for(i = 0; i < count; ++i)
    {
      config_setting_t *book = config_setting_get_elem(setting, i);

      if(!(config_setting_lookup_string(book, "name", &var)
           && config_setting_lookup_string(book, "type", &type)
           && config_setting_lookup_string(book, "scope", &scope)))
        continue;

      if(!(strcmp(scope, "local")) && !(config_setting_lookup_string(book, "function", &function))) {
		fprintf(stderr, "Scope defined as local but function not provided\n");
		config_destroy(&cfg);
 		fclose(fp);
		fclose(mon_vars_file);
		return(EXIT_FAILURE);
	}
	if(function) {
		fprintf(fp,"name=%s,type=%s,scope=%s,function=%s\n",var,type,scope,function);
		fprintf(mon_vars_file,"%s_%s\n",function,var);
	}
	else {
		fprintf(fp,"name=%s,type=%s,scope=%s\n",var,type,scope);
		fprintf(mon_vars_file,"%s\n",var);
	}
      printf("%-30s  %-30s  %-30s\n", var, type, scope);
    }
    putchar('\n');
  }

  config_destroy(&cfg);
  fclose(fp);
  fclose(mon_vars_file);
  return(EXIT_SUCCESS);
}

/* eof */
