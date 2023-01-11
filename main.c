#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

int main (int argc, char *argv[])
{
  bool output_declare = false;

  if (argc != 2)
    {
error:
      printf ("Expected argument --encoding or --declare\n");
      return 0;
    }
  if (strcmp (argv[1], "--encoding") == 0)
    ;
  else if (strcmp (argv[1], "--declare") == 0)
    output_declare = true;
  else
    goto error;

  FILE *file = fopen ("test.txt", "r");
  if (file == NULL)
    {
      printf ("unable to open the file\n");
      return 1;
    }

  char line[256];
  char line_low[256];
  while (fgets (line, sizeof (line), file))
    {
      char *p = line;

      /* Get opcode name.  */
      while (*p != ' ')
	p++;
      *p++ = '\0';

      if (output_declare)
	{
	  int k = 0;
	  while (line[k] != '\0')
	    {
	      if (line[k] == '_' || isdigit (line[k]))
		line_low[k] = line[k];
	      else
		line_low[k] = line[k] + 32;
	      k++;
	    }
	  line_low[k] = '\0';
	  printf ("DECLARE_INSN(%s, MATCH_%s, MASK_%s)\n", line_low, line, line);
	  continue;
	}

      /* Parse operands.  */
      unsigned int mask = 0;
      unsigned int match = 0;
      while (*p != '\0' && *p != '\n')
	{
	  if (!isdigit (*p))
	    {
	      p++;
	      continue;
	    }
	  /* start, end, value.  */
	  long value[3] = { 0, 0, 0 };
	  for (int i = 0; i < 3; i++)
	    {
	      while (!isdigit (*p))
		  p++;
	      int b = 10;
	      if (*p == '0')
		switch (*(p + 1))
		  {
		    case 'x': b = 16; break;
		    case 'b': b = 2; p += 2; break;
		    default: break;
		  }
	      value [i] = strtol (p, &p, b);
	    }
	  for (int j = value[0]; j >= value[1]; j--)
	    mask |= (1 << j);
	  match |= (value[2] << value[1]);
	}
	printf ("#define MATCH_%s 0x%08x\n", line, match);
	printf ("#define MASK_%s 0x%08x\n", line, mask);
    }

  fclose (file);
  return 0;
}
