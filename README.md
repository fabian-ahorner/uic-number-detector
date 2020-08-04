This is the source code written for my [master thesis](./doc/thesis.pdf): 

Usage
----------------

Build and run docker file (No UI)
```
docker build --tag uic-detector .
docker run uic-detector
```

The finished program provides a command line interface that can be used
to process a single image or to test a UIC detector on a specified
dataset. Thereby, the program provides a choice of different character
detectors, which can be configured by passing a selection of color
channels that should be used for the detection. Both functions can be
further configured with the following options:

-   “**–whitelist arg**” or “**-w arg**” Can be used to pass a list of
    comma separated uic numbers that will be used during the detection
    process.

-   “**–detector arg**” or “**-d arg**” With this option a specific UIC
    detector can be selected.

-   “**–channels arg**” or “**-c arg**” The argument passed defines the
    color channels that the detector should use. Note this option has no
    effect on a FASText3 detector as it does not use color channels. The
    channels can be passed as keys without a delimiter (e.g.: “-c RGBD”
    for red green blue and gradient channel). The possible channel are:

    -   **R** …Red channel

    -   **G** …Green channel

    -   **B** …Blue channel

    -   **H** …Hue channel

    -   **S** …Saturation channel

    -   **L** …Luminance channel

    -   **D** …Gradient channel

-   “**–ui**” or “**-u**” Enables the graphical user interface. Both,
    the single and the dataset mode offer a GUI that allows further
    analysis of the returned results. For this purpose the following
    images are shown after processing an input (Figure [fig:ui~c~li]):

    [b]<span>0.49</span> ![Original image shown with the use of an
    annotated dataset](doc/original.png "fig:")
    [fig:ui~c~li~o~riginal]

    [b]<span>0.49</span> ![The extracted UIC
    number](doc/uic.png "fig:") [fig:ui~c~li~u~ic]

    [b]<span>0.49</span> ![Outline of all digits in the extracted UIC
    number](doc/outline.png "fig:") [fig:ui~c~li~o~utline]

    [b]<span>0.49</span> ![All characters part of the extracted UIC
    number](doc/chars.png "fig:") [fig:ui~c~li~c~hars]

    [fig:ui~c~li]

    -   **UIC number** (Figure [fig:ui~c~li~u~ic])\
        This image shows the bounding box of the UIC number as a red
        rectangle and the bounding boxes of each line as blue
        rectangles. The detected digits are overlaid directly over the
        bounding box of the corresponding character which allows a user
        to identify missed characters.

    -   **UIC outline** (Figure [fig:ui~c~li~o~utline])\
        Figure [fig:ui~c~li~o~utline] shows the outline of all
        characters part of the detected UIC number. This can be helpful
        to see the quality of the returned characters.

    -   **UIC characters** (Figure [fig:ui~c~li~c~hars])\
        The last image that is shown to the user, shows the outlines of
        each detected character but adds a gap in between them. This can
        be useful to identify characters that are linked to each other,
        which can prevent the OCR software to identify them correctly.

### Single image mode

If the program was started with the path of a “JPG” or “PNG” it will
process the image and return the detected UIC number on the command
line. This function can be used in a production system where only the
result is of interest. If the GUI is enabled, three windows are opened
at the end of the detection process, allowing a in depth analysis of the
result (For examples see figure [fig:ui~c~li]). In addition to the
previously mentioned parameters, this mode also supports the following
options.

-   “**–print-rect**” or “**-r**” If enabled the program will return the
    detected UIC number and its bounding box. The bounding box is
    returned as a comma separated list (“x,y,width,height”). This can be
    helpful if an image of only the UIC number should be saved as a
    reference or should to presented to a human to identify missing
    digits.

-   “**–print-timings**” or “**-t**” With the print timing flag the
    program returns the runtime of each pipeline component.

### Dataset mode

If the program is started with a path to folder, the program reads the
dataset in that folder and tests the passed UIC detector with it. The
dataset needs to be provided in the format described in section
[sec:intro~d~ata]. In addition to the parameters the dataset mode shares
with the single image mode, it also allows the user to define which part
of the image should be used during the detection process. The area can
be specified with the “**–region** arg” option. The possible selections
are:

-   **original** …Full size image

-   **train** …Cropped version, containing the train wagon

-   **uic** …Selection roughly containing the UIC number

-   **digits** …Contains only the bounding box of the UIC number

```
0.00% :   0 218024572776_1 -> Overlap: 96.51% Edit Distance: 3
0.34% :   1 218024572776_2 -> Overlap: 95.23% Edit Distance: 1
0.68% :   2 218024574905_1 -> Overlap: 98.97% Edit Distance: 7
...
98.97% : 289 378053771747_2 -> Overlap: 99.10% Edit Distance: 0
99.32% : 290 378053771887_1 -> Overlap: 96.66% Edit Distance: 2
99.66% : 291 838027450652_1 -> Overlap: 0.00% Edit Distance: -1

Detector: FasTextDetector1C -> CustomLineDetector >> Channels -> SimpleUicDetector >> Scale Pyramid
Total Time: 61440
Mean Time: 210
48105 	 FasTextDetector1C
10883 	 TesseractSingleCharClassifier
 1829 	 FasTextDetector1C -> CustomLineDetector
   16 	 FasTextDetector1C -> CustomLineDetector >> Channels -> SimpleUicDetector

   Overlap	        Cnt	          %
      <30%	         31	     10.62%
      >30%	          3	      1.03%
      >50%	        258	     88.36%

EditDistance	       Cnt.	        %	  Cumulated
         0	         40	     13.70%	     13.70%
         1	         35	     11.99%	     25.68%
         2	         52	     17.81%	     43.49%
         3	         45	     15.41%	     58.90%
         4	         26	      8.90%	     67.81%
         5	         22	      7.53%	     75.34%
         6	          7	      2.40%	     77.74%
         7	          8	      2.74%	     80.48%
         8	         10	      3.42%	     83.90%
         9	         11	      3.77%	     87.67%
        10	          4	      1.37%	     89.04%
        11	          1	      0.34%	     89.38%
        12	          0	      0.00%	     89.38%
         -	         31	         -%	     10.62%
Area:     67.89%
CriticalErrors:          1  	      0.34%
Avg (overlapping only): 3.12644
Avg (all): 4.06849
```

When the dataset is evaluated the current process is shown on the
command line. An example of this can be seen on line 1-7 in listing
[lst:ui~e~val~r~esult] (Line 1-7). The first value shows the process as
a percent value, the second value is the index of the current entry.
Then the UIC number and the overlap of the extracted bounding box with
the ground truth are listed. The final value for each entry is the edit
distance of the extracted UIC number. If no number was found at all,
$-1$ is shown instead.\
Once the process is completed, the result is summarized as seen on lines
9-40 in listing [lst:ui~e~val~r~esult]. The shown information is:

1.  The used UIC detector.

2.  The total time needed to process the whole dataset in milliseconds.

3.  The average time one entry in the dataset required to be processed
    in milliseconds.

4.  The total times required by the subcomponents of the UIC detector in
    milliseconds. Note that all these values might not sum up to the
    total time previously stated, because not all stages such as
    spitting the image into color channels are tracked.

5.  Line 17-20 show how many UIC bounding boxes were overlapping less
    than 30%, 30-50% and more than 50% with the ground truth.

6.  Line 22-36 show the distribution of the edit distances of the
    results. The first value shows the edit distance. The next value,
    shows the number of entries with that result. The penultimate value
    shows the same value as a percentage and the last value shows the
    cumulated percentage value of entries with the same or less edits.\
    The last column shows the number of entries where no UIC number was
    found.

7.  The area is the area of the cumulated line chart of the edit
    distances divided by the maximal possible area of this chart (See
    equation [eq:results~s~core]).

8.  Critical Errors shows the number of UICs that were detected
    incorrectly, but have a correct check digit.

9.  Line 39 shows the average edit distance of all UIC numbers, with a
    bounding box that was overlapping at least 30% with the ground
    truth.

10. The last line shows the average edit distance counting not detected
    UIC numbers as an edit distance of 13.

If the application was started with the GUI option, the dataset is not
directly evaluated, but instead the dataset is loaded and can be browsed
by the user. The program can be controlled using the following keys:

-   **“A”** and **“D”** allow the selection of an UIC image. The images
    are sorted by their UIC number. The current image is shown as in
    figure [fig:ui~c~li~o~riginal]. The blue boxes represent the digit
    bounds that were calculated using the bounding boxes of the digit
    groups. The command line also shows the index of the current entry
    and its key. The key consists of the UIC number and an index if one
    UIC number exists multiple times.

-   **“W”** and **“S”** allow the selection of an UIC detector. The
    current detector is shown on the command line by printing all of its
    components.

-   **“O”**…Original, **“T”**…Train, **“U”**…Uic and **“I”** …dIgits,
    can be used to set the region that is passed to the UIC detector.

-   **Space** is used to evaluate the current entry. Thereby, the result
    is shown as described in section [sec:ui~u~ic]. In addition to that,
    the command line prints the run time of the major pipeline segments
    and the 5 best UIC results. Each result includes the detected
    number, its edit distance, the overlap of its bounding box with the
    ground truth and its score.

-   **“L”** enables and disables the use of a white list. Thereby all
    UIC numbers from the dataset are used. The white list is only used
    for evaluating the whole dataset, not a single entry.

-   **“E”** evaluates the dataset with the current UIC detector, region
    mode and optional white list. The result is shown on the command
    line and is in the same format as previously described in this
    section (See listing [lst:ui~e~val~r~esult]).

-   **“C”** calculates and shows the height in pixels of the smallest
    digit and the standard deviation (RMS) of all digits (Only using the
    area inside the digit groups). These values can be used to judge the
    quality of an image.

-   Any other key closes the program.

