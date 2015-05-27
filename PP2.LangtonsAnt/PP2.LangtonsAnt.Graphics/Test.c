#include "aalib.h"
#include <stdio.h>
#include <stdlib.h>

aa_context *context;

int main(void)
{
	context = aa_autoinit(&aa_defparams);
	if (context == NULL) {
		fprintf(stderr, "Šupi\n");
		return EXIT_FAILURE;
	}
	
	aa_putpixel(context, 10, 10, 5);
	aa_fastrender(context, 1, 1, 15, 15);
	aa_flush(context);

	aa_close(context);
	return EXIT_SUCCESS;
}