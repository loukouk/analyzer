#include <stdlib.h>
#include <stdio.h>

//config filename
#define CONFIG "visualization.cfg"

int plot_data(uint64_t * xvals, uint64_t * yvals, int size)
{
	char buf[BUF_SIZE];

	//Open data file for the chart
	FILE * temp = fopen("chart.dat", "w");
	if (temp == NULL) {
		fprintf(stderr,"Could not open/create chart.dat file: %s.\n", strerror(errno));
		return 1;
	}

	//Write data to the data file
	for (int i=0; i < size; i++) {
		fprintf(temp, "%ld %ld\n", xvals[i], yvals[i]); //Write the data to a temporary file
	}
	fclose(temp);

	//Open the config file that holds chart options
	FILE * cfg = fopen(CONFIG, "r");
	if (cfg == NULL) {
		fprintf(stderr,"Could not open %s: %s.\n", CONFIG, strerror(errno));
		return 1;
	}
	
	//Open the GNUplot program and get it ready to feed it instructions
	FILE * gnuplotPipe = popen ("gnuplot", "w");
	if (gnuplotPipe == NULL) {
		fprintf(stderr,"Could not open GNUplot.\n");
		return 1;
	}
	
	//set output to be a PNG image file
	fprintf(gnuplotPipe, "%s\n", "set terminal png truecolor");
	//set output name
	fprintf(gnuplotPipe, "%s\n", "set output \'image.png\'");

	//This loops goes through every line of the config file
	//and feeds them into GNUplot
	for (int i = 0; !feof(cfg); i++) {
		memset(buf, '\0', BUF_SIZE); 	//clear buffer

		//get next line into the buffer
		if (fgets(buf, BUF_SIZE, cfg) == NULL && !feof(cfg)) {
			fprintf(stderr, "Could not read line %d in file %s", i, CONFIG);
			return 1;
		}

		//if end of file is reached, break out of the loop
		if (feof(cfg))
			break;

		//send command to GNUplot
		fprintf(gnuplotPipe, "%s\n", buf);
	}
	fprintf(gnuplotPipe, "%s\n", "plot \'chart.dat\' ls 2");

	//close GNUplot pipe
	if (pclose(gnuplotPipe) == -1) {
		fprintf(stderr, "Could not close pipe to GNUplot");
	}
	return 0;
}
